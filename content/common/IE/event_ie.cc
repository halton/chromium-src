// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/IE/event_ie.h"

#include <Exdisp.h>
#include <MsHtmHst.h>
#include <MsHtmdid.h>
#include <Mshtml.h>
#include <WinUser.h>
#include <Wininet.h>
#include <initguid.h>
#include <objbase.h>
#include <oleauto.h>
#include <EXDISPID.H>
#include <stack>
#include <string>

#include "base/strings/utf_string_conversions.h"
#include "content/common/IE/atl_include.h"
#include "content/common/IE/doc_host_ui_handler_ie.h"
#include "content/common/IE/event_handler_ie.h"
#include "url/gurl.h"

// define GWL_HWNDPARENT here just for compiling
#define GWL_HWNDPARENT (-8)

DEFINE_GUID(CGID_MshtmlPriv,
            0xa11452bc,
            0xe055,
            0x4e56,
            0xa1,
            0x51,
            0x7b,
            0x16,
            0xdb,
            0xb4,
            0x54,
            0x4e);

enum {
  MSHTMLPRIV_TRUSTFORFIRSTDOWNLOAD,
  MSHTMLPRIV_BROWSERNAVSTATECHANGED,
  MSHTMLPRIV_SETSESSIONDOCUMENTMODE,
  MSHTMLPRIV_GETSESSIONDOCUMENTMODE
};

namespace ie {

static HWND s_last_window_handle = 0;

BOOL CALLBACK EnumChildProc(HWND child_handle, LPARAM l_param) {
  if (child_handle == NULL)
    return FALSE;

  if (s_last_window_handle == child_handle)
    return FALSE;

  s_last_window_handle = child_handle;
  wchar_t name[256] = {0};
  ::GetClassName(child_handle, name, 255);
  // IE访问FILE://协议的URL，IE子窗口类名为SysListView32，也需要处理
  if (std::wstring(name) == L"Internet Explorer_Server" ||
      std::wstring(name) == L"SysListView32") {
    Event* event = (Event*)l_param;
    event->SetIEHwnd(child_handle);
    return FALSE;
  }
  return TRUE;
}

Event::Event(EventHandler* delegate, DocHostUIHandler* doc_host)
    : com_ref_count_(0),
      web_browser_(NULL),
      html_doc2_(NULL),
      html_doc6_(NULL),
      javascript_dispath_(NULL),
      html_dispatch_(NULL),
      delegate_(delegate),
      doc_host_ui_handler_ie_(doc_host),
      // forward(VARIANT_FALSE),
      // back(VARIANT_FALSE),
      child_handle_(NULL),
      parent_handle_(NULL),
      ie_handle_(NULL),
      is_find_ie_(false),
      listen_click_doc2_(NULL),
      cookie_(0),
      document_event_(new DocumentEventIe(this)),
      refresh_flag_(false) {}

Event::~Event() {
  if (cookie_) {
    if (listen_click_doc2_) {
      IConnectionPointContainer* container = NULL;
      listen_click_doc2_->QueryInterface(IID_IConnectionPointContainer,
                                         (void**)&container);
      if (container) {
        IConnectionPoint* point = NULL;
        container->FindConnectionPoint(DIID_HTMLDocumentEvents2, &point);
        if (point && cookie_ != 0)
          point->Unadvise(cookie_);
        container->Release();
      }
    }
  }

  if (document_event_)
    delete document_event_;

  if (web_browser_)
    web_browser_->Release();
}

int Event::GetDocMode() {
  if (html_doc6_) {
    VARIANT mode;
    if (SUCCEEDED(html_doc6_->get_documentMode(&mode))) {
      int ret = (int)mode.fltVal;
      return ret;
    }
  }
  return 0;
}

bool Event::SetDocMode(DocumentMode mode) {
  if (!html_doc2_)
    return false;

  CComPtr<IOleCommandTarget> ole_target = NULL;
  HRESULT result =
      html_doc2_->QueryInterface(IID_IOleCommandTarget, (void**)&ole_target);
  if (FAILED(result) || ole_target == NULL)
    return false;

  NotifyRefresh(true);
  VARIANT param;
  param.vt = VT_I4;
  param.intVal = mode;
  result = ole_target->Exec(&CGID_MshtmlPriv, MSHTMLPRIV_SETSESSIONDOCUMENTMODE,
                            OLECMDEXECOPT_DODEFAULT, &param, NULL);
  if (FAILED(result))
    return false;

  result = ole_target->Exec(NULL, OLECMDID_REFRESH, OLECMDEXECOPT_DODEFAULT,
                            NULL, NULL);
  return SUCCEEDED(result);
}

HRESULT Event::QueryInterface(REFIID riid, void** ppv_object) {
  if (riid == __uuidof(IUnknown)) {
    (*ppv_object) = static_cast<IUnknown*>(this);
  } else if (riid == __uuidof(IDispatch)) {
    (*ppv_object) = static_cast<IDispatch*>(this);
  } else if (riid == __uuidof(DWebBrowserEvents2)) {
    (*ppv_object) = static_cast<DWebBrowserEvents2*>(this);
  } else {
    return E_NOINTERFACE;
  }

  AddRef();
  return S_OK;
}

ULONG Event::AddRef() {
  InterlockedIncrement(&com_ref_count_);
  return com_ref_count_;
}

ULONG Event::Release() {
  InterlockedDecrement(&com_ref_count_);
  return com_ref_count_;
}

HRESULT Event::GetTypeInfoCount(UINT* pctinfo) {
  *pctinfo = 0;
  return S_OK;
}

HRESULT Event::GetTypeInfo(UINT t_Info, LCID lcid, ITypeInfo** ppt_Info) {
  *ppt_Info = NULL;
  return E_NOTIMPL;
}

HRESULT Event::GetIDsOfNames(REFIID riid,
                             LPOLESTR* rgszNames,
                             UINT cNames,
                             LCID lcid,
                             DISPID* rgDispId) {
  return S_OK;
}

HRESULT Event::Invoke(DISPID disp_id,
                      REFIID riid,
                      LCID lcid,
                      WORD flags,
                      DISPPARAMS* disp_params,
                      VARIANT* var_result,
                      EXCEPINFO* excep_Info,
                      UINT* arg_err) {
  switch (disp_id) {
    case DISPID_NAVIGATECOMPLETE2: {
      if (disp_params->cArgs >= 2 && disp_params->rgvarg[1].vt == VT_DISPATCH)
        SetCustomDoc(disp_params->rgvarg[1].pdispVal);

      if (disp_params->rgvarg[0].vt == (VT_VARIANT | VT_BYREF)) {
        variant_t variant = *(disp_params->rgvarg[0].pvarVal);
        if (variant.vt == VT_BSTR) {
          bstr_t url = variant.bstrVal;
          bool find_url = false;
          std::list<std::wstring>::iterator iter =
              before_navigate_url_list_.begin();
          for (; iter != before_navigate_url_list_.end(); iter++) {
            if (*iter == url.GetBSTR()) {
              before_navigate_url_list_.erase(iter);
              find_url = true;
              break;
            }
          }
          if (find_url == false && delegate_) {
            delegate_->OnBeforeNavigate(url.GetBSTR(), false);
          }
        }
      }
      break;
    }
    case DISPID_BEFORENAVIGATE2: {
      if (disp_params->rgvarg[5].vt == (VT_VARIANT | VT_BYREF)) {
        // if (pDispParams->rgvarg[4].vt == (VT_VARIANT | VT_BYREF))
        //{
        //  VARIANT* vv=NULL;
        //  int ff = 0;
        //  vv = (VARIANT*)pDispParams->rgvarg[4].byref;
        //  ff = vv->intVal;
        //  if (ff == 64)
        //  {
        //    int a = 10;
        //    a++;
        //  }
        //}

        VARIANT* var_temp = (VARIANT*)disp_params->rgvarg[5].byref;
        bstr_t url = var_temp->bstrVal;
        before_navigate_url_list_.push_back(url.GetBSTR());
        GURL gurl(url.GetBSTR());
        if (delegate_) {
          bool cancel = false;
          bool navigate_new_url = false;
          if (GURL(click_url_) ==
              gurl)  //用户是否点击URL，通知主进程是否需要更新前进后退列表
            navigate_new_url = true;
          cancel = delegate_->OnBeforeNavigate(url.GetBSTR(), navigate_new_url);
          if (cancel) {
            ((VARIANT*)disp_params->rgvarg[0].byref)->vt = VT_BOOL;
            ((VARIANT*)disp_params->rgvarg[0].byref)->boolVal = VARIANT_TRUE;
          }
        }
        click_url_ = L"";
      }
      break;
    }
    case DISPID_DOWNLOADCOMPLETE: {
      // if (html_doc2_ == NULL)
      // //将自己实现的IDocHostUIHandler接口替换IE内核创建的，这样能够获取IE内核创建IDocHostUIHandler接口中的GetExternal接口
      // {
      //   CComPtr<IDispatch> dispatch;
      //   if (web_browser_)
      //     web_browser_->get_Document(&dispatch);
      //   if (dispatch)
      //   {
      //     dispatch->QueryInterface(IID_IHTMLDocument2, (void**)&html_doc2_);
      //     if (html_doc2_)
      //     {
      //       CComPtr<IOleObject> pOleObj;
      //       html_doc2_->QueryInterface(IID_IOleObject, (void**)&pOleObj);
      //       CComPtr<IOleClientSite> pClientSite;
      //       pOleObj->GetClientSite(&pClientSite);
      //       if (pClientSite)
      //       {
      //         pClientSite->QueryInterface(IID_IDocHostUIHandler,
      //         (void**)&org_doc_host_);

      //         doc_host_ui_handler_ie_->SetOrgDocHost(org_doc_host_);
      //         CComPtr<ICustomDoc> custom_doc;
      //         html_doc2_->QueryInterface(IID_ICustomDoc,
      //         (void**)&custom_doc);
      //         custom_doc->SetUIHandler(doc_host_ui_handler_ie_);
      //       }
      //     }
      //   }
      // }

      if (refresh_flag_ == false)
        break;
      if (cookie_ == 0 && web_browser_) {
        CComPtr<IDispatch> dispatch;
        web_browser_->get_Document(&dispatch);
        if (dispatch == NULL)
          break;
        CComPtr<IHTMLDocument2> document2;
        dispatch->QueryInterface(IID_IHTMLDocument2, (void**)&document2);
        if (document2 == NULL)
          break;
        ListenDocmentEvent(document2);
      }
    } break;
    case DISPID_DOCUMENTCOMPLETE: {
      if (disp_params->rgvarg[1].vt == VT_DISPATCH) {
        IDispatch* dispatch = disp_params->rgvarg[1].pdispVal;
        if (dispatch == web_browser_) {
          if (disp_params->rgvarg[0].vt !=
              (VT_VARIANT | VT_BYREF))  // Delegate handler
            return S_OK;
          VARIANT* var_temp = (VARIANT*)disp_params->rgvarg[0].byref;
          bstr_t url = var_temp->bstrVal;
          if (url.GetBSTR())
            OnFinishLoad(url.GetBSTR());
        }
      }
    } break;
    case DISPID_TITLECHANGE: {
      if (disp_params->rgvarg[0].vt == VT_BSTR) {
        //为了解决chrome49中
        // IE无法直接在Chrome里创建子窗口的问题，先让IE建立桌面的子窗口，待窗口创建完成后再将IE设为Chrome的子窗口。
        HWND parent_handle = ::GetParent(child_handle_);
        if (::IsWindow(child_handle_) && ::IsWindow(parent_handle_) &&
            parent_handle != parent_handle_) {
          DWORD err = 0;
          HWND main_window = GetParent(parent_handle_);
          if (SetWindowLong(child_handle_, GWL_HWNDPARENT, (LONG)main_window) ==
              0) {
            err = GetLastError();
          }

          HWND hwnd = ::SetParent(child_handle_, parent_handle_);
          long style = GetWindowLong(child_handle_, GWL_STYLE);
          style = style & ~WS_POPUP & ~WS_MAXIMIZEBOX;
          style = style | WS_CHILD;
          SetWindowLong(child_handle_, GWL_STYLE, style);

          ::SetForegroundWindow(child_handle_);
          if (hwnd == NULL) {
            err = GetLastError();
          }
        }
        if (is_find_ie_ == false) {
          HWND window_found = child_handle_;
          ie_handle_ = NULL;
          s_last_window_handle = 0;
          BOOL ret =
              ::EnumChildWindows(window_found, EnumChildProc, (LPARAM)this);
          if (::IsWindow(ie_handle_)) {
            is_find_ie_ = true;
            ShowWindow(child_handle_, SW_SHOW);
            if (ie_handle_ && delegate_)
              delegate_->SetIEHwnd(ie_handle_);
          }
          if (ret == FALSE) {
            break;
          }
        }

        bstr_t title = disp_params->rgvarg[0].bstrVal;
        if (delegate_) {
          delegate_->OnTitleChange(title.GetBSTR());
        }
      }
      break;
    }
    case DISPID_NEWWINDOW3: {
      if (disp_params->rgvarg[0].vt == (VT_BSTR)) {
        bstr_t url = disp_params->rgvarg[0].bstrVal;
        // nwmf == 0x00000006 (NWMF_USERINITED | NWMF_FIRST_USERINITED)
        // 是点击链接打开新窗口 nwmf == 0x00010086
        // 是从右键菜单中选择在新窗口打开 nwmf == 0x00040004 目前已知是弹出窗口
        long flag = ie::NewWindowFlag::TRANSITION;
        long nwmf = disp_params->rgvarg[2].intVal;
        if (((nwmf & 0x40000) == 0x40000 && (nwmf & 0x20000) == 0) ||
            (nwmf & 0x10000) == 0x10000)
          flag = ie::NewWindowFlag::POPUP;
        else if ((nwmf & 0x00000006) == 0x00000006 ||
                 (nwmf & 0x00010086) == 0x00010086 ||
                 (nwmf & 0x00000002) == 0x00000002)
          flag = ie::NewWindowFlag::TRANSITION;
        // comment unused part by webb.
        // VARIANT vt = pDispParams->rgvarg[4];
        IDispatch* dispatch = NULL;
        if (delegate_) {
          bool cancel =
              delegate_->OnLoadUrlNewWnd(url.GetBSTR(), flag, &dispatch);
          if (dispatch) {
            *(disp_params->rgvarg[4].ppdispVal) = dispatch;
          }
          if (cancel) {
            disp_params->rgvarg[3].vt = VT_BOOL | VT_BYREF;
            *(disp_params->rgvarg[3].pboolVal) = VARIANT_TRUE;
          }
        }
      }
      break;
    }
    case DISPID_WINDOWSETLEFT: {
      if (delegate_ && disp_params->rgvarg[0].vt == VT_I4) {
        int left = disp_params->rgvarg[0].intVal;
        RECT rc;
        delegate_->OnGetMainWndPos(&rc);
        int width = rc.right - rc.left;
        delegate_->OnRequestChangePos(left, rc.top, width, rc.bottom - rc.top);
      }
      break;
    }
    case DISPID_WINDOWSETTOP: {
      if (delegate_ && disp_params->rgvarg[0].vt == VT_I4) {
        int top = disp_params->rgvarg[0].intVal;
        RECT rc;
        delegate_->OnGetMainWndPos(&rc);
        int height = rc.bottom - rc.top;
        delegate_->OnRequestChangePos(rc.left, top, rc.right - rc.left, height);
      }
      break;
    }
    case DISPID_WINDOWSETWIDTH: {
      if (delegate_ && disp_params->rgvarg[0].vt == VT_I4) {
        int width = disp_params->rgvarg[0].intVal;
        RECT rc;

        GetWindowRect(parent_handle_, &rc);
        int x = rc.right - rc.left;

        delegate_->OnGetMainWndPos(&rc);
        // rc.right = rc.right + x;
        int newWidth = rc.right - rc.left - x + width;
        // delegate_->OnRequestChangePos(rc.left, rc.top, width, rc.bottom -
        // rc.top);
        delegate_->OnRequestChangePos(rc.left, rc.top, newWidth,
                                      rc.bottom - rc.top);
      }
      break;
    }
    case DISPID_WINDOWSETHEIGHT: {
      if (delegate_ && disp_params->rgvarg[0].vt == VT_I4) {
        int height = disp_params->rgvarg[0].intVal;
        RECT rc;

        GetWindowRect(parent_handle_, &rc);
        int y = rc.bottom - rc.top;

        delegate_->OnGetMainWndPos(&rc);
        int newHeight = rc.bottom - rc.top - y + height;
        // rc.bottom = rc.bottom - rc.top + y;
        // delegate_->OnRequestChangePos(rc.left, rc.top, rc.right-rc.left,
        // height);
        delegate_->OnRequestChangePos(rc.left, rc.top, rc.right - rc.left,
                                      newHeight);
      }
      break;
    }
    case DISPID_WINDOWCLOSING: {
      // IE
      // js执行window.close()方法时，会触发此事件，但此时不能通知主进程IE关闭了自己，
      //因为IE在通知此事件后可能还会有关闭IE页面提示、后续新开页面事件等，
      //所以将通知主进程IE关闭的消息改在通过拦截WM_DESTROY消息时通知主进程，具体参看WebBrowser::SubClassControlWndProc
      //方法中的WM_DESTROY消息处理
      break;
    }
    case DISPID_CLIENTTOHOSTWINDOW: {
      // int* y = pDispParams->rgvarg[0].pintVal;
      // int* x = pDispParams->rgvarg[1].pintVal;
      // POINT pt;
      // pt.x = *x;
      // pt.y = *y;
      // HWND hwnd = GetParent(child_handle_);
      // RECT rt;
      // GetWindowRect(hwnd, &rt);
      // GetWindowRect(child_handle_, &rt);
      // GetWindowRect(parent_handle_, &rt);
      // int a = 10;
      // a++;
      break;
    }
  }
  return S_OK;
}

void Event::SetWebBrowser(IWebBrowser2* web) {
  web_browser_ = web;
  web_browser_->AddRef();
  if (document_event_)
    document_event_->SetWebBrowser(web);
}

HRESULT Event::ExcuteJs(const std::wstring& function_name,
                        std::vector<variant_t> params,
                        variant_t* result) {
  if (javascript_dispath_ == NULL)
    return E_UNEXPECTED;

  bstr_t name = function_name.c_str();
  long dispatch_id = 0;
  HRESULT result_tmp = javascript_dispath_->GetIDsOfNames(
      IID_NULL, &name.GetBSTR(), 1, LOCALE_SYSTEM_DEFAULT, &dispatch_id);
  if (FAILED(result_tmp))
    return E_INVALIDARG;

  DISPPARAMS disp_params;
  memset(&disp_params, 0, sizeof(DISPPARAMS));
  disp_params.cArgs = params.size();
  disp_params.rgvarg = new VARIANT[disp_params.cArgs];
  disp_params.cNamedArgs = 0;

  int j = 0;
  for (int i = disp_params.cArgs - 1; i >= 0; i--) {
    disp_params.rgvarg[j] = params[i];
    j++;
  }

  VARIANT ret;
  EXCEPINFO exec_info;
  memset(&exec_info, 0, sizeof(EXCEPINFO));
  UINT arg_error = 0;
  result_tmp = javascript_dispath_->Invoke(
      dispatch_id, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD,
      &disp_params, &ret, &exec_info, &arg_error);
  if (disp_params.rgvarg)
    delete[] disp_params.rgvarg;

  return result_tmp;
}

HRESULT Event::ExcuteJs(const std::wstring& code, variant_t* result) {
  if (html_dispatch_ == NULL)
    return E_UNEXPECTED;

  LPOLESTR ss = const_cast<LPOLESTR>(L"eval");
  DISPID id = -1;
  HRESULT result_tmp = html_dispatch_->GetIDsOfNames(
      IID_NULL, &ss, 1, LOCALE_SYSTEM_DEFAULT, &id);
  if (FAILED(result_tmp))
    return result_tmp;

  DISPPARAMS disp_params;
  memset(&disp_params, 0, sizeof(DISPPARAMS));
  disp_params.cArgs = 1;
  disp_params.cNamedArgs = 0;
  disp_params.rgvarg = new VARIANT[disp_params.cArgs];
  disp_params.rgvarg[0].vt = VT_BSTR;
  bstr_t src = code.c_str();
  disp_params.rgvarg[0].bstrVal = src;
  EXCEPINFO exec_info;
  memset(&exec_info, 0, sizeof(EXCEPINFO));
  UINT arg_error = 0;
  result_tmp = html_dispatch_->Invoke(id, IID_NULL, LOCALE_SYSTEM_DEFAULT,
                                      DISPATCH_METHOD, &disp_params, result,
                                      &exec_info, &arg_error);
  delete[] disp_params.rgvarg;
  return result_tmp;
}

void Event::SetChildHWND(HWND window_handle) {
  child_handle_ = window_handle;
}

void Event::SetParentHwnd(HWND window_handle) {
  parent_handle_ = window_handle;
}

void Event::SetIEHwnd(HWND hwnd) {
  ie_handle_ = hwnd;
}

void Event::SetClickUrl(const std::wstring& url) {
  click_url_ = url;
}

void Event::NotifyRefresh(bool is_start) {
  refresh_flag_ = is_start;

  if (document_event_)
    document_event_->SetRefresh(is_start);
}

void Event::OnFinishLoad(const std::wstring& url) {
  if (web_browser_ == NULL)
    return;

  CComPtr<IDispatch> dispatch;
  // Get IHtmlDoc interface
  HRESULT result = web_browser_->get_Document(&dispatch);
  if (SUCCEEDED(result) && dispatch) {
    result = dispatch->QueryInterface(IID_IHTMLDocument2, (void**)&html_doc2_);
    if (html_doc2_) {
      result =
          html_doc2_->QueryInterface(IID_IHTMLDocument6, (void**)&html_doc6_);
      result = html_doc2_->get_Script((IDispatch**)&javascript_dispath_);
      // vector<variant_t> args;
      // variant_t args1;
      // args1 = 1;
      // args.push_back(args1);
      // variant_t args2;
      // args2 = L"哈哈";
      // args.push_back(args2);
      // variant_t ret;
      // ExcuteJs(L"myHello", args, &ret);

      click_url_ = L"";

      GURL gurl(url);
      if (gurl.SchemeIsHTTPOrHTTPS()) {
        IHTMLWindow2* parent_window = NULL;
        html_doc2_->get_parentWindow(&parent_window);
        parent_window->QueryInterface(IID_IDispatch, (void**)&html_dispatch_);
        parent_window->Release();
        ListenDocmentEvent(html_doc2_);
      }
    }
    NotifyRefresh(false);

    std::vector<std::string> favicon_urls;
    GetFaviconUrls(base::UTF16ToASCII(url), favicon_urls);

    std::string favicon_urls_string;
    for (auto it = favicon_urls.begin(); it != favicon_urls.end(); it++) {
      favicon_urls_string += *it;
      favicon_urls_string += '\t';
    }

    if (delegate_)
      delegate_->OnFinishNavigate(url, base::ASCIIToUTF16(favicon_urls_string));
  }
}

void Event::OnClickHtmlElement(CComPtr<IHTMLElement> click_element) {
  if (delegate_)
    delegate_->OnClickElement(click_element, false);
}

void Event::OnEnterKeyPressedHtmlElement(CComPtr<IHTMLElement> click_element) {
  if (delegate_)
    delegate_->OnClickElement(click_element, true);
}

void Event::ListenDocmentEvent(IHTMLDocument2* document2) {
  if (document_event_ == NULL || document2 == NULL)
    return;

  if (listen_click_doc2_) {
    IConnectionPointContainerPtr connection_point_container = NULL;
    listen_click_doc2_->QueryInterface(IID_IConnectionPointContainer,
                                       (void**)&connection_point_container);
    if (connection_point_container) {
      IConnectionPoint* connection_point = NULL;
      connection_point_container->FindConnectionPoint(DIID_HTMLDocumentEvents2,
                                                      &connection_point);
      if (connection_point && cookie_ != 0)
        connection_point->Unadvise(cookie_);
      cookie_ = 0;
      connection_point_container->Release();
    }
    listen_click_doc2_ = NULL;
  }

  IConnectionPointContainerPtr container = NULL;
  document2->QueryInterface(IID_IConnectionPointContainer, (void**)&container);
  if (container) {
    IConnectionPoint* point = NULL;
    container->FindConnectionPoint(DIID_HTMLDocumentEvents2, &point);
    point->Advise(document_event_, &cookie_);
    container->Release();
  }
  listen_click_doc2_ = document2;
}

void Event::ResetDocHostUIHandler() {
  if (html_doc2_ && org_doc_host_ && doc_host_ui_handler_ie_) {
    doc_host_ui_handler_ie_->SetOrgDocHost(NULL);
    CComPtr<ICustomDoc> custom_doc;
    html_doc2_->QueryInterface(IID_ICustomDoc, (void**)&custom_doc);
    custom_doc->SetUIHandler(org_doc_host_);
    org_doc_host_ = NULL;
  }
}

void Event::GetFaviconUrls(const std::string& page_url,
                           std::vector<std::string>& favicon_urls) {
  favicon_urls.clear();
  GURL page_gurl = GURL(page_url);
  do {
    HRESULT result = S_OK;
    if (web_browser_ == NULL)
      break;

    CComPtr<IDispatch> doc = NULL;
    result = web_browser_->get_Document(&doc);
    if (!SUCCEEDED(result) || doc == NULL)
      break;

    CComQIPtr<IHTMLDocument3> doc3(doc);
    if (doc3 == NULL)
      break;

    CComPtr<IHTMLElementCollection> link_elements = NULL;
    result = doc3->getElementsByTagName(const_cast<BSTR>(L"link"), &link_elements);
    if (!SUCCEEDED(result) || link_elements == NULL)
      break;

    long count = 0;
    result = link_elements->get_length(&count);
    DLOG(INFO) << "GetFaviconUrls: count = " << count;
    if (!SUCCEEDED(result) || count <= 0)
      break;

    for (long i = 0; i < count; i++) {
      CComVariant variant_name(i);
      variant_name.ChangeType(VT_UINT);
      CComVariant variant_index;
      CComPtr<IDispatch> element_dispatch;
      result =
          link_elements->item(variant_name, variant_index, &element_dispatch);
      if (!SUCCEEDED(result) || element_dispatch == NULL)
        continue;

      CComQIPtr<IHTMLLinkElement, &IID_IHTMLLinkElement> link_element(
          element_dispatch);
      if (link_element == NULL)
        continue;

      CComBSTR rel = NULL;
      link_element->get_rel(&rel);
      if (rel == NULL)
        continue;
      rel.ToLower();
      if (rel != "icon" && rel != "shortcut icon")
        continue;

      CComBSTR href = NULL;
      link_element->get_href(&href);
      if (href == NULL)
        continue;

      std::string href_string = base::UTF16ToASCII(std::wstring(href));
      if (href_string.size() < 2)
        continue;
      if (href_string[0] == '/' && href_string[1] == '/') {
        href_string = page_gurl.scheme() + ":" + href_string;
      }

      std::string favicon_url;
      GURL temp = GURL(href_string);
      if (temp.is_valid()) {
        favicon_url = href_string;
      } else {
        favicon_url = page_gurl.scheme() + "://";
        favicon_url += page_gurl.host();
        if (page_gurl.has_port()) {
          favicon_url += ":";
          favicon_url += page_gurl.port();
        }
        favicon_url += href_string;
      }
      favicon_urls.push_back(favicon_url);
    }
  } while (0);

  if (favicon_urls.size() == 0) {
    std::string favicon_url = page_gurl.scheme() + "://";
    favicon_url += page_gurl.host();
    if (page_gurl.has_port()) {
      favicon_url += ":";
      favicon_url += page_gurl.port();
    }
    favicon_url += "/favicon.ico";
    favicon_urls.push_back(favicon_url);
  }
}

void Event::SetCustomDoc(LPDISPATCH lp_disp) {
  if (lp_disp == NULL)
    return;

  IWebBrowser2* web_browser2 = NULL;
  HRESULT hr = lp_disp->QueryInterface(IID_IWebBrowser2, (void**)&web_browser2);

  if (SUCCEEDED(hr) && web_browser2) {
    IDispatch* doc = NULL;
    hr = web_browser2->get_Document(&doc);

    if (SUCCEEDED(hr) && doc) {
      ICustomDoc* cust_doc = NULL;
      hr = doc->QueryInterface(IID_ICustomDoc, (void**)&cust_doc);
      if (SUCCEEDED(hr) && cust_doc != NULL) {
        cust_doc->SetUIHandler(doc_host_ui_handler_ie_);
        cust_doc->Release();
      }

      doc->Release();
    }

    web_browser2->Release();
  }
}

DocumentEventIe::DocumentEventIe(Event* event)
    : com_ref_count_(0),
      event_ie_(event),
      web_browser2_(NULL),
      refresh_flag_(false) {}

DocumentEventIe::~DocumentEventIe() {
  if (web_browser2_)
    web_browser2_->Release();
}

void DocumentEventIe::SetWebBrowser(IWebBrowser2* web_browser2) {
  if (web_browser2_)
    web_browser2_->Release();

  web_browser2_ = web_browser2;
  web_browser2_->AddRef();
}

void DocumentEventIe::SetRefresh(bool is_refresh) {
  refresh_flag_ = is_refresh;
}

HRESULT DocumentEventIe::QueryInterface(REFIID riid, void** ppv_object) {
  if (riid == __uuidof(IUnknown)) {
    (*ppv_object) = static_cast<IUnknown*>(this);
  } else if (riid == __uuidof(IDispatch)) {
    (*ppv_object) = static_cast<IDispatch*>(this);
  } else if (riid == __uuidof(HTMLDocumentEvents2)) {
    (*ppv_object) = static_cast<HTMLDocumentEvents2*>(this);
  } else {
    return E_NOINTERFACE;
  }

  AddRef();
  return S_OK;
}

ULONG DocumentEventIe::AddRef() {
  InterlockedIncrement(&com_ref_count_);
  return com_ref_count_;
}

ULONG DocumentEventIe::Release() {
  InterlockedDecrement(&com_ref_count_);
  return com_ref_count_;
}

HRESULT DocumentEventIe::GetTypeInfoCount(UINT* pctinfo) {
  *pctinfo = 0;
  return S_OK;
}

HRESULT DocumentEventIe::GetTypeInfo(UINT t_Info,
                                     LCID lcid,
                                     ITypeInfo** ppt_info) {
  *ppt_info = NULL;
  return E_NOTIMPL;
}

HRESULT DocumentEventIe::GetIDsOfNames(REFIID riid,
                                       LPOLESTR* rgszNames,
                                       UINT cNames,
                                       LCID lcid,
                                       DISPID* rgDispId) {
  return S_OK;
}

HRESULT
DocumentEventIe::Invoke(DISPID disp_id,
                        REFIID riid,
                        LCID lcid,
                        WORD flags,
                        DISPPARAMS* disp_params,
                        VARIANT* var_result,
                        EXCEPINFO* excep_Info,
                        UINT* arg_err) {
  if (disp_id == DISPID_HTMLDOCUMENTEVENTS2_ONCLICK) {
    IHTMLEventObj* event_obj = NULL;
    IDispatch* dispatch = disp_params->rgvarg[0].pdispVal;
    dispatch->QueryInterface(IID_IHTMLEventObj, (void**)&event_obj);
    IHTMLElement* element = NULL;
    event_obj->get_srcElement(&element);
    event_ie_->OnClickHtmlElement(element);
    while (element) {
      _bstr_t tag_name;
      element->get_tagName(&tag_name.GetBSTR());
      if (_bstr_t("a") == tag_name || _bstr_t("A") == tag_name) {
        // 已找到 "a" 标签，在这里写相应代码
        _variant_t href_variant;
        element->getAttribute(const_cast<BSTR>(L"href"), 0, &href_variant);
        // event_obj->put_returnValue(_variant_t(VARIANT_TRUE, VT_BOOL));
        if (event_ie_ && href_variant.vt == VT_BSTR && href_variant.bstrVal) {
          event_ie_->SetClickUrl(href_variant.bstrVal);
        }
        element->Release();
        break;
      }
      IHTMLElement* element_temp = element;
      element->get_parentElement(&element);
      element_temp->Release();
    }
    event_obj->Release();
  } else if (disp_id == DISPID_HTMLDOCUMENTEVENTS2_ONKEYPRESS) {
    IHTMLEventObj* event_obj = NULL;
    IDispatch* dispatch = disp_params->rgvarg[0].pdispVal;
    dispatch->QueryInterface(IID_IHTMLEventObj, (void**)&event_obj);
    if (event_obj) {
      long key_code = 0;
      event_obj->get_keyCode(&key_code);
      if (key_code == 13 || key_code == 32) {
        IHTMLElement* element = NULL;
        event_obj->get_srcElement(&element);
        event_ie_->OnEnterKeyPressedHtmlElement(element);
      }
      event_obj->Release();
    }
  } else if (disp_id == DISPID_HTMLDOCUMENTEVENTS2_ONREADYSTATECHANGE) {
    // 解决刷新没有DISPID_DOCUMENTCOMPLETE事件问题，在此消息中判断readyState判断是否刷新完成
    if (web_browser2_ && refresh_flag_ && event_ie_) {
      CComPtr<IDispatch> dispatch = NULL;
      web_browser2_->get_Document(&dispatch);
      if (dispatch) {
        CComPtr<IHTMLDocument2> document2 = NULL;
        dispatch->QueryInterface(IID_IHTMLDocument2, (void**)&document2);
        bstr_t state;
        document2->get_readyState(state.GetAddress());
        if (std::wstring(state.GetBSTR()) == L"complete") {
          bstr_t url = L"";
          document2->get_URL(url.GetAddress());
          if (url.GetBSTR())
            event_ie_->OnFinishLoad(url.GetBSTR());
        }
      }
    }
  }

  return S_OK;
}

}  // namespace ie
