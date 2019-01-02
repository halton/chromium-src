// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/IE/doc_host_ui_handler_ie.h"

#include <ExDisp.h>
#include <ShlGuid.h>
#include <Strsafe.h>
#include <WinInet.h>
#include <mshtmcid.h>
#include <mshtml.h>
#include <vector>

#include "base/bind.h"
#include "base/message_loop/message_loop.h"
#include "base/strings/utf_string_conversions.h"
#include "content/common/IE/atl_include.h"
#include "content/common/IE/event_handler_ie.h"
#include "content/common/IE/javascript_handler_ie.h"
#include "content/common/IE/version_ie.h"
#include "content/common/IE/web_browser_ie.h"
#include "content/public/browser/browser_thread.h"
#include "url/gurl.h"

namespace ie {

DocHostUIHandler::DocHostUIHandler(EventHandler* handler, WebBrowser* browser)
    : event_handler_(handler),
      javascript_handler_(NULL),
      web_browser2_(browser),
      mouse_right_button_enabled_(true),
      cut_copy_enabled_(true),
      com_ref_count_(0),
      print_enabled_(true),
      save_file_enabled_(true) {}

DocHostUIHandler::~DocHostUIHandler() {
  if (javascript_handler_) {
    delete javascript_handler_;
  }
}

HRESULT DocHostUIHandler::QueryInterface(REFIID riid, void** ppv_object) {
  if (riid == __uuidof(IUnknown)) {
    (*ppv_object) = this;
  } else if (riid == __uuidof(IDocHostUIHandler)) {
    (*ppv_object) = static_cast<IDocHostUIHandler*>(this);
  } else if (riid == __uuidof(IOleCommandTarget)) {
    (*ppv_object) = static_cast<IOleCommandTarget*>(this);
  } else {
    return E_NOINTERFACE;
  }

  AddRef();
  return S_OK;
}

ULONG DocHostUIHandler::AddRef() {
  InterlockedIncrement(&com_ref_count_);
  return com_ref_count_;
}

ULONG DocHostUIHandler::Release() {
  InterlockedDecrement(&com_ref_count_);
  return com_ref_count_;
}

HRESULT DocHostUIHandler::ShowContextMenu(DWORD id,
                                          POINT* ppt,
                                          IUnknown* cmd_reserved,
                                          IDispatch* dispatch_reserved) {
  if (dispatch_reserved == NULL)
    return E_NOTIMPL;

  if (mouse_right_button_enabled_ == false)
    return S_OK;

  HRESULT result;
  IOleCommandTarget* command_target = NULL;
  if (cmd_reserved == NULL)
    return E_NOTIMPL;
  cmd_reserved->QueryInterface(IID_IOleCommandTarget, (void**)&command_target);
  if (command_target == NULL)
    return E_NOTIMPL;

  HMENU menu_handle = BuildMenu(id, command_target);
  command_target->Release();
  if (menu_handle == NULL)
    return E_NOTIMPL;

  int selection = 0;
  IOleWindow* ole_window = NULL;
  result = cmd_reserved->QueryInterface(IID_IOleWindow, (void**)&ole_window);
  HWND hwnd = NULL;
  result = ole_window->GetWindow(&hwnd);
  ole_window->Release();

  selection = TrackPopupMenu(menu_handle,
                             TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                             ppt->x, ppt->y, 0, hwnd, NULL);
  DestroyMenu(menu_handle);

  switch (selection) {
    case IDM_COPYSHORTCUT: {
      IHTMLElement* element;
      HRESULT result1 =
          dispatch_reserved->QueryInterface(IID_IHTMLElement, (void**)&element);
      if (element == NULL)
        return E_NOTIMPL;
      IHTMLAnchorElement* anchor = NULL;
      result1 =
          element->QueryInterface(IID_IHTMLAnchorElement, (void**)&anchor);
      if (anchor == NULL)
        return E_NOTIMPL;
      BSTR bstr;
      anchor->get_href(&bstr);
      std::wstring url = bstr;
      SysFreeString(bstr);
      anchor->Release();
      element->Release();
      if (selection == IDM_COPYSHORTCUT)
        CopyToClipboard(url);
    } break;
    case IDM_GOBACKWARD:
      event_handler_->GoBack();
      break;
    case IDM_GOFORWARD:
      event_handler_->GoForward();
      break;
    case IDM_REFRESH:
      if (web_browser2_)
        web_browser2_->NotifyCommand(L"refresh");
      SendMessage(hwnd, WM_COMMAND, selection, NULL);
      break;
    case 0:
      break;
    default:
      SendMessage(hwnd, WM_COMMAND, selection, NULL);
      break;
  }
  return S_OK;
}

HRESULT DocHostUIHandler::GetHostInfo(DOCHOSTUIINFO* doc_host_ui_info) {
  doc_host_ui_info->dwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_THEME |
                              DOCHOSTUIFLAG_NO3DOUTERBORDER;
  return S_OK;
}

HRESULT DocHostUIHandler::ShowUI(DWORD id,
                                 IOleInPlaceActiveObject* active_object,
                                 IOleCommandTarget* command_target,
                                 IOleInPlaceFrame* ole_in_place_frame,
                                 IOleInPlaceUIWindow* ole_in_place_ui_window) {
  return S_OK;
}

HRESULT DocHostUIHandler::HideUI() {
  return S_OK;
}

HRESULT DocHostUIHandler::UpdateUI() {
  return S_OK;
}

HRESULT DocHostUIHandler::EnableModeless(BOOL is_enable) {
  return S_OK;
}

HRESULT DocHostUIHandler::OnDocWindowActivate(BOOL is_activate) {
  return S_OK;
}

HRESULT DocHostUIHandler::OnFrameWindowActivate(BOOL is_activate) {
  return S_OK;
}

HRESULT DocHostUIHandler::ResizeBorder(LPCRECT prc_border,
                                       IOleInPlaceUIWindow* ui_window,
                                       BOOL rame_window) {
  return S_OK;
}

HRESULT DocHostUIHandler::TranslateAccelerator(LPMSG message,
                                               const GUID* guid_cmd_group,
                                               DWORD cmd_id) {
  return S_FALSE;
}

HRESULT DocHostUIHandler::GetOptionKeyPath(LPOLESTR* key, DWORD dw) {
  HRESULT result = E_NOTIMPL;
  // WCHAR* szKey = L"Software\\AllMobilize\\Redcore\\Trident";

  // size_t cbLength;
  // result = StringCbLengthW(szKey, 1280, &cbLength);

  // if (key)
  //{
  //  *key = (LPOLESTR)CoTaskMemAlloc(cbLength + sizeof(WCHAR));
  //  if (*key)
  //    result = StringCbCopyW(*key, cbLength + sizeof(WCHAR), szKey);
  //}

  return result;
}

HRESULT DocHostUIHandler::GetDropTarget(IDropTarget* drop_target_in,
                                        IDropTarget** drop_target_out) {
  return E_NOTIMPL;
}

HRESULT DocHostUIHandler::GetExternal(IDispatch** dispatch_out) {
  // if (javascript_handler_ == NULL)
  //  javascript_handler_ = new JavaScriptHandler;
  //*dispatch_out = javascript_handler_;

  // if (org_doc_host_)
  // {
  //   IDispatch* dispatch = NULL;
  //   org_doc_host_->GetExternal(&dispatch);
  //   *dispatch_out = dispatch;
  // }
  return S_OK;
}

HRESULT DocHostUIHandler::TranslateUrl(DWORD translate,
                                       LPWSTR url_in,
                                       LPWSTR* url_out) {
  return S_FALSE;
}

HRESULT DocHostUIHandler::FilterDataObject(IDataObject* data_object_in,
                                           IDataObject** data_object_out) {
  return S_FALSE;
}
HRESULT DocHostUIHandler::QueryStatus(const GUID* guid_cmd_group,
                                      ULONG cCmds,
                                      OLECMD prg_cmds[],
                                      OLECMDTEXT* pCmdText) {
  return E_NOTIMPL;
}
HRESULT DocHostUIHandler::Exec(const GUID* guid_cmd_group,
                               DWORD cmd_id,
                               DWORD cmd_exec_opt,
                               VARIANT* va_in,
                               VARIANT* va_out) {
  if (cmd_id == OLECMDID_SHOWSCRIPTERROR) {
    //获取js脚本错误信息并log输出
    IHTMLDocument2* ole_in_place_ui_window = NULL;
    IHTMLWindow2* window2 = NULL;
    IHTMLEventObj* event_object = NULL;
    BSTR names[5] = {
        SysAllocString(L"errorLine"), SysAllocString(L"errorCharacter"),
        SysAllocString(L"errorCode"), SysAllocString(L"errorMessage"),
        SysAllocString(L"errorUrl")};
    DISPID disp_ids[5];
    VARIANT event_infos[5];

    HRESULT result = E_FAIL;
    result = va_in->punkVal->QueryInterface(IID_IHTMLDocument2,
                                            (void**)&ole_in_place_ui_window);
    result = ole_in_place_ui_window->get_parentWindow(&window2);
    ole_in_place_ui_window->Release();
    result = window2->get_event(&event_object);

    if (event_object == NULL)
      return result;
    std::wstring error_message = L"";
    int i = 0;
    for (i = 0; i < 5; i++) {
      result = event_object->GetIDsOfNames(IID_NULL, &names[i], 1,
                                           LOCALE_SYSTEM_DEFAULT, &disp_ids[i]);

      DISPPARAMS params;
      params.cArgs = 0;
      params.cNamedArgs = 0;
      result = event_object->Invoke(disp_ids[i], IID_NULL,
                                    LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET,
                                    &params, &event_infos[i], NULL, NULL);

      std::wstring buffer = L"";
      if (event_infos[i].vt == VT_I4) {
        wchar_t temp[512] = {0};
        _itow_s(event_infos[i].intVal, temp, 10);
        buffer = temp;
      } else if (event_infos[i].vt == VT_BSTR &&
                 event_infos[i].bstrVal != NULL) {
        buffer = event_infos[i].bstrVal;
      }
      error_message = error_message + names[i] + L": " + buffer + L" \r\n";
      VariantClear(&event_infos[i]);
      SysFreeString(names[i]);
    }
    LOG(WARNING) << "IE Javascript error : " << error_message;
    window2->Release();
    event_object->Release();

    // 拦截js错误弹出框, 继续运行脚本.
    (*va_out).vt = VT_BOOL;
    (*va_out).boolVal = VARIANT_TRUE;
    return S_OK;
  }
  return E_NOTIMPL;
}

void DocHostUIHandler::EnableMouseRightButton(bool enable) {
  LOG(INFO) << "EnableMouseRightButton : " << enable;
  mouse_right_button_enabled_ = enable;
}

void DocHostUIHandler::EnableCutCopy(bool enable) {
  cut_copy_enabled_ = enable;
}

void DocHostUIHandler::EnablePrint(bool enable) {
  print_enabled_ = enable;
}

void DocHostUIHandler::EnableSaveFile(bool enable) {
  save_file_enabled_ = enable;
}

void DocHostUIHandler::SetOrgDocHost(IDocHostUIHandler* doc_host) {
  org_doc_host_ = doc_host;
}

HMENU DocHostUIHandler::BuildMenu(DWORD id, IOleCommandTarget* command_target) {
  HMENU menu_handle = CreatePopupMenu();
  std::vector<std::pair<OLECMD, std::wstring>> menu_item_array;
  switch (id) {
    case CONTEXT_MENU_DEFAULT: {
      if (event_handler_) {
        bool go_back = false;
        bool go_forward = false;
        event_handler_->GetGoBackOrForwardState(&go_back, &go_forward);
        if (go_back)
          AppendMenu(menu_handle, MF_STRING, IDM_GOBACKWARD, L"返回");
        else
          AppendMenu(menu_handle, MF_STRING | MF_DISABLED | MF_GRAYED,
                     IDM_GOBACKWARD, L"返回");
        if (go_forward)
          AppendMenu(menu_handle, MF_STRING, IDM_GOFORWARD, L"前进");
        else
          AppendMenu(menu_handle, MF_STRING | MF_DISABLED | MF_GRAYED,
                     IDM_GOFORWARD, L"前进");
      }
      OLECMD cmd;
      cmd.cmdID = IDM_REFRESH;
      cmd.cmdf = 0;
      menu_item_array.push_back(std::make_pair(cmd, L"重新加载"));

      if (save_file_enabled_) {
        cmd.cmdID = IDM_SAVEAS;
        menu_item_array.push_back(std::make_pair(cmd, L"另存为..."));
      }

      if (print_enabled_) {
        cmd.cmdID = IDM_PRINTPREVIEW;
        menu_item_array.push_back(std::make_pair(cmd, L"打印..."));
      }
    } break;
    case CONTEXT_MENU_IMAGE: {
      OLECMD cmd;
      cmd.cmdf = 0;

      if (save_file_enabled_) {
        cmd.cmdID = IDM_SAVEPICTURE;
        menu_item_array.push_back(std::make_pair(cmd, L"图片另存为"));
      }

      if (cut_copy_enabled_) {
        cmd.cmdID = IDM_COPY;
        menu_item_array.push_back(std::make_pair(cmd, L"复制图片"));
      }

      cmd.cmdID = IDM_FOLLOWLINKN;
      menu_item_array.push_back(std::make_pair(cmd, L"在新标签页中打开链接"));
    } break;
    case CONTEXT_MENU_TEXTSELECT: {
      OLECMD cmd;
      cmd.cmdf = 0;
      if (cut_copy_enabled_) {
        cmd.cmdID = IDM_CUT;
        menu_item_array.push_back(std::make_pair(cmd, L"剪切"));

        cmd.cmdID = IDM_COPY;
        menu_item_array.push_back(std::make_pair(cmd, L"复制"));
      }

      cmd.cmdID = IDM_PASTE;
      menu_item_array.push_back(std::make_pair(cmd, L"粘贴"));

      cmd.cmdID = IDM_SELECTALL;
      menu_item_array.push_back(std::make_pair(cmd, L"全选"));
    } break;
    case CONTEXT_MENU_ANCHOR: {
      OLECMD cmd;
      cmd.cmdID = IDM_FOLLOWLINKN;
      cmd.cmdf = 0;
      menu_item_array.push_back(std::make_pair(cmd, L"在新标签页中打开链接"));

      if (cut_copy_enabled_) {
        cmd.cmdID = IDM_COPYSHORTCUT;
        menu_item_array.push_back(std::make_pair(cmd, L"复制链接地址"));
      }

      if (save_file_enabled_) {
        cmd.cmdID = IDM_SAVETARGET;
        menu_item_array.push_back(std::make_pair(cmd, L"链接另存为"));
      }
    } break;
    case CONTEXT_MENU_CONTROL: {
      OLECMD cmd;
      cmd.cmdID = IDM_UNDO;
      cmd.cmdf = 0;
      menu_item_array.push_back(std::make_pair(cmd, L"撤销"));

      cmd.cmdID = MF_SEPARATOR;
      menu_item_array.push_back(std::make_pair(cmd, L""));

      if (cut_copy_enabled_) {
        cmd.cmdID = IDM_CUT;
        menu_item_array.push_back(std::make_pair(cmd, L"剪切"));

        cmd.cmdID = IDM_COPY;
        menu_item_array.push_back(std::make_pair(cmd, L"复制"));
      }

      cmd.cmdID = IDM_PASTE;
      menu_item_array.push_back(std::make_pair(cmd, L"粘贴"));

      cmd.cmdID = IDM_DELETE;
      menu_item_array.push_back(std::make_pair(cmd, L"删除"));

      cmd.cmdID = IDM_SELECTALL;
      menu_item_array.push_back(std::make_pair(cmd, L"全选"));
    } break;
  }

  size_t i = 0;
  for (i = 0; i < menu_item_array.size(); i++) {
    if (menu_item_array[i].first.cmdID == MF_SEPARATOR) {
      AppendMenu(menu_handle, MF_SEPARATOR, NULL, NULL);
      continue;
    }
    HRESULT result = command_target->QueryStatus(
        &CGID_MSHTML, 1, &(menu_item_array[i].first), NULL);
    if (SUCCEEDED(result)) {
      if ((menu_item_array[i].first.cmdf & OLECMDF_ENABLED) == OLECMDF_ENABLED)
        AppendMenu(menu_handle, MF_STRING, menu_item_array[i].first.cmdID,
                   menu_item_array[i].second.c_str());
      else if ((menu_item_array[i].first.cmdf & OLECMDF_SUPPORTED) ==
               OLECMDF_SUPPORTED)
        AppendMenu(menu_handle, MF_STRING | MF_DISABLED | MF_GRAYED,
                   menu_item_array[i].first.cmdID,
                   menu_item_array[i].second.c_str());
    }
  }
  return menu_handle;
}

void DocHostUIHandler::CopyToClipboard(const std::wstring& content) {
  if (!OpenClipboard(NULL))
    return;

  EmptyClipboard();

  HANDLE data_handle =
      GlobalAlloc(GMEM_MOVEABLE, sizeof(wchar_t) * (content.size() + 1));
  wchar_t* data = (wchar_t*)GlobalLock(data_handle);
  memcpy(data, content.c_str(), sizeof(wchar_t) * (content.size()));
  data[content.size()] = 0;
  GlobalUnlock(data_handle);

  SetClipboardData(CF_UNICODETEXT, data_handle);
  CloseClipboard();
}

ServiceProvider::ServiceProvider(EventHandler* handler)
    : com_ref_count_(0),
      download_manager_(NULL),
      delegate_(handler),
      html_window_service_(NULL),
      domain_authenticate_(NULL),
      web_browser2_(NULL),
      new_window_manager_(NULL) {}

ServiceProvider::~ServiceProvider() {
  if (download_manager_)
    delete download_manager_;

  if (html_window_service_)
    delete html_window_service_;

  if (domain_authenticate_)
    delete domain_authenticate_;

  if (new_window_manager_)
    delete new_window_manager_;

  if (web_browser2_)
    web_browser2_->Release();
}

void ServiceProvider::SetWebBrowser(IWebBrowser2* web_browser2) {
  if (web_browser2_) {
    web_browser2_->Release();
    web_browser2_ = NULL;
  }
  web_browser2_ = web_browser2;
  web_browser2_->AddRef();
}

ULONG ServiceProvider::AddRef() {
  InterlockedIncrement(&com_ref_count_);
  return com_ref_count_;
}

ULONG ServiceProvider::Release() {
  InterlockedDecrement(&com_ref_count_);
  return com_ref_count_;
}

HRESULT ServiceProvider::QueryInterface(REFIID riid, void** ppv_object) {
  if (riid == __uuidof(IUnknown)) {
    (*ppv_object) = this;
  } else if (riid == __uuidof(IServiceProvider)) {
    (*ppv_object) = static_cast<IServiceProvider*>(this);
  } else {
    return E_NOINTERFACE;
  }

  AddRef();
  return S_OK;
}

HRESULT ServiceProvider::QueryService(
    /* [in] */ _In_ REFGUID guid_service,
    /* [in] */ _In_ REFIID riid,
    /* [out] */ _Outptr_ void __RPC_FAR* __RPC_FAR* ppv_object) {
  HRESULT result = E_NOINTERFACE;

  if (guid_service == SID_SDownloadManager && riid == IID_IDownloadManager) {
    if (download_manager_ == NULL)
      download_manager_ = new DownLoadManager;
    result =
        download_manager_->QueryInterface(IID_IDownloadManager, ppv_object);
  }

  if (guid_service == SID_SHTMLOMWindowServices &&
      riid == IID_IHTMLOMWindowServices) {
    if (html_window_service_ == NULL)
      html_window_service_ = new HTMLOMWindowServices(delegate_);
    result = html_window_service_->QueryInterface(IID_IHTMLOMWindowServices,
                                                  ppv_object);
  }

  if (riid == IID_IAuthenticate) {
    if (domain_authenticate_ == NULL)
      domain_authenticate_ = new DomainAuthenticate(delegate_, web_browser2_);
    result =
        domain_authenticate_->QueryInterface(IID_IAuthenticate, ppv_object);
  }

  if (guid_service == SID_SNewWindowManager && riid == IID_INewWindowManager) {
    if (new_window_manager_ == NULL)
      new_window_manager_ = new NewWindowManager(delegate_);
    result =
        new_window_manager_->QueryInterface(IID_INewWindowManager, ppv_object);
  }

  return result;
}

const LPOLESTR oBindCtxBSCBObjectParam = const_cast<LPOLESTR>(L"_BSCB_Holder_");

DownLoadManager::DownLoadManager()
    : com_ref_count_(0),
      binding_(NULL),
      download_url(L""),
      stream_(NULL),
      is_download_canceled_(false),
      is_downloading_(false),
      previsous_bind_status_callback_(NULL),
      bind_context_(NULL),
      is_abort_(false) {}

DownLoadManager::~DownLoadManager() {}

void DownLoadManager::CancelDownload() {
  is_download_canceled_ = true;
  is_downloading_ = false;
}

HRESULT DownLoadManager::QueryInterface(REFIID riid, void** ppv_object) {
  if (riid == __uuidof(IUnknown)) {
    (*ppv_object) = this;
  } else if (riid == __uuidof(IDownloadManager)) {
    (*ppv_object) = static_cast<IDownloadManager*>(this);
  } else if (riid == __uuidof(IBindStatusCallback)) {
    (*ppv_object) = static_cast<IBindStatusCallback*>(this);
  } else if (riid == __uuidof(IBindStatusCallbackEx)) {
    (*ppv_object) = static_cast<IBindStatusCallbackEx*>(this);
  } else {
    return E_NOINTERFACE;
  }

  AddRef();
  return S_OK;
}

ULONG DownLoadManager::AddRef() {
  InterlockedIncrement(&com_ref_count_);
  return com_ref_count_;
}

ULONG DownLoadManager::Release() {
  InterlockedDecrement(&com_ref_count_);
  return com_ref_count_;
}

HRESULT DownLoadManager::Download(IMoniker* pmk,
                                  IBindCtx* pbc,
                                  DWORD bind_verb,
                                  LONG grf_bindf,
                                  BINDINFO* bind_info,
                                  LPCOLESTR headers,
                                  LPCOLESTR redir,
                                  UINT ui_cp) {
  // return E_FAIL;  //IE下载使用IE自己的下载工具

  //接管IE下载UI展现和数据保存成文件功能，使用Chrome界面做UI展现，下载的数据由Chrome主进程保存成文件
  if (is_downloading_)
    return S_OK;

  LPOLESTR name = NULL;
  name = (LPOLESTR)CoTaskMemAlloc(sizeof(wchar_t) * 10240);
  pmk->GetDisplayName(pbc, NULL, &name);
  download_url = name;
  CoTaskMemFree(name);

  HRESULT result;
  bind_context_ = pbc;
  bind_context_->AddRef();
  previsous_bind_status_callback_ = NULL;
  result = RegisterBindStatusCallback(
      pbc, reinterpret_cast<IBindStatusCallback*>(this),
      &previsous_bind_status_callback_, 0L);

  if ((FAILED(result)) && (previsous_bind_status_callback_)) {
    // RevokeObjectParam for current BSCB, so we can register our BSCB
    result = pbc->RevokeObjectParam(oBindCtxBSCBObjectParam);
    if (SUCCEEDED(result)) {
      // Attempt register again, should succeed now
      result = RegisterBindStatusCallback(
          pbc, reinterpret_cast<IBindStatusCallback*>(this), 0, 0L);
    }
  }

  if (previsous_bind_status_callback_)
    previsous_bind_status_callback_->AddRef();

  is_downloading_ = true;
  IStream* pstm = NULL;
  result = pmk->BindToStorage(pbc, 0, IID_IStream, (void**)&pstm);
  if (pstm)
    pstm->Release();

  return S_OK;
}

STDMETHODIMP DownLoadManager::GetBindInfoEx(DWORD* grf_bindf,
                                            BINDINFO* bind_info,
                                            DWORD* grf_bindf2,
                                            DWORD* pdw_reserved) {
  *grf_bindf = BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE | BINDF_PULLDATA |
               BINDF_GETNEWESTVERSION | BINDF_NOWRITECACHE;
  *grf_bindf2 = BINDF2_READ_DATA_GREATER_THAN_4GB;

  DWORD cbSize = bind_info->cbSize = sizeof(BINDINFO);
  memset(bind_info, 0, cbSize);
  bind_info->cbSize = cbSize;
  bind_info->dwBindVerb = BINDVERB_GET;
  bind_info->szExtraInfo = NULL;
  memset(&bind_info->stgmedData, 0, TYMED_NULL);
  bind_info->grfBindInfoF = 0;
  bind_info->szCustomVerb = NULL;
  // Reserved. Must be set to 0
  bind_info->dwOptions = 0;
  bind_info->dwOptionsFlags = 0;
  bind_info->dwReserved = 0;
  return S_OK;
}

STDMETHODIMP DownLoadManager::OnStartBinding(DWORD reserved,
                                             IBinding* binding) {
  binding_ = binding;
  if (binding_ == NULL) {
    return E_FAIL;
  }
  binding_->AddRef();

  CComPtr<IWinInetHttpInfo> win_inet_http_info;
  HRESULT result_temp = binding_->QueryInterface(
      IID_IWinInetHttpInfo, reinterpret_cast<void**>(&win_inet_http_info));
  if (win_inet_http_info == NULL)
    return E_FAIL;

  DWORD size = 0;
  DWORD flags = 0;
  result_temp = win_inet_http_info->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, 0,
                                              &size, &flags, 0);
  if (SUCCEEDED(result_temp)) {
    char* buffer = new char[size + 1];
    memset(buffer, 0, sizeof(char) * (size + 1));
    result_temp = win_inet_http_info->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF,
                                                buffer, &size, &flags, 0);
    std::wstring response_header = L"";
    response_header = base::ASCIIToUTF16(buffer);
    is_download_canceled_ = false;
    is_abort_ = false;
    if (WebBrowser::GetWebBrowser()) {
      variant_t var;
      var = response_header.c_str();
      WebBrowser::GetWebBrowser()->DownLoadFile(download_url,
                                                (int)ie::BEGINDOWNLOAD, var);
    }
    delete[] buffer;
  }
  if (previsous_bind_status_callback_) {
    previsous_bind_status_callback_->OnStopBinding(HTTP_STATUS_OK, NULL);
  }
  return S_OK;
}

STDMETHODIMP DownLoadManager::GetPriority(LONG* priority) {
  return E_NOTIMPL;
}

STDMETHODIMP DownLoadManager::OnLowResource(DWORD reserved) {
  return E_NOTIMPL;
}

STDMETHODIMP DownLoadManager::OnProgress(ULONG progress,
                                         ULONG progress_max,
                                         ULONG status_code,
                                         LPCWSTR status_text) {
  if (status_code == BINDSTATUS_BEGINDOWNLOADDATA) {
    std::wstring ss = L"";
    ss = status_text;

    CComPtr<IWinInetHttpInfo> win_inet_http_info;
    if (binding_ == NULL) {
      return E_FAIL;
    }

    HRESULT result_temp = binding_->QueryInterface(
        IID_IWinInetHttpInfo, reinterpret_cast<void**>(&win_inet_http_info));
    DWORD size = 0;
    DWORD flags = 0;
    result_temp = win_inet_http_info->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, 0,
                                                &size, &flags, 0);
    if (SUCCEEDED(result_temp)) {
      char* buffer = new char[size + 1];
      memset(buffer, 0, sizeof(char) * (size + 1));
      result_temp = win_inet_http_info->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF,
                                                  buffer, &size, &flags, 0);
      std::string header = "";
      header = buffer;
      delete[] buffer;
    }
  } else if (status_code == BINDSTATUS_REDIRECTING) {
    std::wstring ss = L"";
    ss = status_text;
  } else if (status_code == BINDSTATUS_ENDDOWNLOADDATA) {
    // TODO
  }

  if (previsous_bind_status_callback_) {
    // Need to do this otherwise a filedownload dlg wil be displayed
    // as we are downloading the file.
    // Interestingly, according to MSDN,
    // BINDSTATUS_CONTENTDISPOSITIONATTACH is declared obsolete????
    if (status_code == BINDSTATUS_CONTENTDISPOSITIONATTACH)
      return S_OK;
    previsous_bind_status_callback_->OnProgress(progress, progress_max,
                                                status_code, status_text);
  }

  return S_OK;
}

STDMETHODIMP DownLoadManager::OnStopBinding(HRESULT result, LPCWSTR error) {
  if (result != S_OK && result != E_ABORT) {
    if (WebBrowser::GetWebBrowser()) {
      variant_t var;
      WebBrowser::GetWebBrowser()->DownLoadFile(download_url,
                                                (int)ie::DOWNLOADERROR, var);
      is_download_canceled_ = true;
    }
  }

  if (result == E_ABORT)
    is_abort_ = true;

  return S_OK;
}

STDMETHODIMP DownLoadManager::GetBindInfo(DWORD* grf_bindf,
                                          BINDINFO* bind_info) {
  *grf_bindf = BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE | BINDF_PULLDATA |
               BINDF_GETNEWESTVERSION | BINDF_NOWRITECACHE;

  DWORD cbSize = bind_info->cbSize = sizeof(BINDINFO);
  memset(bind_info, 0, cbSize);
  bind_info->cbSize = cbSize;
  bind_info->dwBindVerb = BINDVERB_GET;
  bind_info->szExtraInfo = NULL;
  memset(&bind_info->stgmedData, 0, TYMED_NULL);
  bind_info->grfBindInfoF = 0;
  bind_info->szCustomVerb = NULL;
  // Reserved. Must be set to 0
  bind_info->dwOptions = 0;
  bind_info->dwOptionsFlags = 0;
  bind_info->dwReserved = 0;
  return S_OK;
}

STDMETHODIMP DownLoadManager::OnDataAvailable(DWORD bscf,
                                              DWORD size,
                                              FORMATETC* format_etc,
                                              STGMEDIUM* stgmedium) {
  if (is_download_canceled_ == true) {
    if (binding_) {
      // 必须在OnDataAvailable方法中调用abort
      // 取消下载，在其他地方调用会失败，造成无法再继续下载的问题，如果没有机会调用abort
      // 就只能让IE继续网络传输，我们能做的就是不再往文件中写入。
      binding_->Abort();
    }
    return E_ABORT;
  }

  if (stgmedium == NULL || stgmedium->pstm == NULL)
    return E_INVALIDARG;

  if (BSCF_FIRSTDATANOTIFICATION & bscf) {
    stream_ = stgmedium->pstm;
    if (stream_)
      stream_->AddRef();
    // 由于有些IE版本在没有CONTENT-LENGTH的数据时，IE只会调用一次OnDataAvailable函数，
    // 之后再也不会调用，只能一直使用IStream::Read，直到下载完成
    // 为了不过多占用UI线程资源，采用定时器读取下载数据
    // base::TimeDelta delay = base::TimeDelta::FromMilliseconds(50);
    // comment jus for compiling
    // base::MessageLoopForUI::current()->PostDelayedTask(FROM_HERE,
    //   base::Bind(&DownLoadManager::ReadDownloadData, base::Unretained(this)),
    //   delay);
  }

  return S_OK;
}

STDMETHODIMP DownLoadManager::OnObjectAvailable(REFIID riid, IUnknown* punk) {
  return E_NOTIMPL;
}

void DownLoadManager::ReadDownloadData() {
  if (stream_ == NULL)
    return;

  DWORD actual_read = 0;
  HRESULT result = S_OK;
  const int buffer_size = 16 * 1024;
  do {
    std::string data_received = "";
    char* buff = new char[buffer_size];
    memset(buff, 0, sizeof(char) * (buffer_size));
    result = stream_->Read(buff, buffer_size, &actual_read);
    if (actual_read > 0 && WebBrowser::GetWebBrowser() &&
        is_download_canceled_ == false) {
      data_received.append(buff, actual_read);
      SAFEARRAYBOUND Bound[1];
      Bound[0].lLbound = 0;
      Bound[0].cElements = data_received.size();
      SAFEARRAY* safe_array = SafeArrayCreate(VT_UI1, 1, Bound);
      unsigned char* data = NULL;
      HRESULT result1 = SafeArrayAccessData(safe_array, (void**)&data);
      memcpy(data, data_received.c_str(), data_received.size());
      result1 = SafeArrayUnaccessData(safe_array);
      variant_t var;
      var.vt = VT_ARRAY;
      var.parray = safe_array;
      WebBrowser::GetWebBrowser()->DownLoadFile(download_url,
                                                (int)ie::DOWNLOADINGDATA, var);
      SafeArrayDestroy(safe_array);
    }
    delete[] buff;
  } while (result == S_OK);

  if (is_download_canceled_ == true && is_abort_ == true) {
    ReleaseObject();
    return;
  }

  // 由于有些IE版本在http response
  // 头中CONTENT-LENGTH没有的情况下不返回BSCF_LASTDATANOTIFICATION状态，
  // 所以无法通过BSCF_LASTDATANOTIFICATION判断下载完成，
  // 改为使用IStream::Read返回值为S_FALSE且没有读取到任何数据，判断为下载结束
  if (result == S_FALSE && actual_read == 0) {
    if (WebBrowser::GetWebBrowser()) {
      variant_t var;
      WebBrowser::GetWebBrowser()->DownLoadFile(download_url,
                                                (int)ie::ENDDOWNLOAD, var);
    }
    is_downloading_ = false;
    ReleaseObject();
  } else {
    // base::TimeDelta delay = base::TimeDelta::FromMilliseconds(50);
    // base::MessageLoopForUI::current()->PostDelayedTask(FROM_HERE,
    //   base::Bind(&DownLoadManager::ReadDownloadData, base::Unretained(this)),
    //   delay);
  }
}

void DownLoadManager::ReleaseObject() {
  if (bind_context_ && previsous_bind_status_callback_) {
    bind_context_->RegisterObjectParam(
        oBindCtxBSCBObjectParam,
        reinterpret_cast<IUnknown*>(previsous_bind_status_callback_));
  }

  if (bind_context_) {
    bind_context_->Release();
    bind_context_ = NULL;
  }

  if (stream_) {
    stream_->Release();
    stream_ = NULL;
  }

  if (previsous_bind_status_callback_) {
    previsous_bind_status_callback_->Release();
    previsous_bind_status_callback_ = NULL;
  }

  if (binding_) {
    binding_->Release();
    binding_ = NULL;
  }

  is_downloading_ = false;
}

STDMETHODIMP DownLoadManager::BeginningTransaction(LPCWSTR url,
                                                   LPCWSTR headers,
                                                   DWORD reserved,
                                                   LPWSTR* additional_headers) {
  return E_NOTIMPL;
}

STDMETHODIMP DownLoadManager::OnResponse(DWORD response_code,
                                         LPCWSTR response_headers,
                                         LPCWSTR request_headers,
                                         LPWSTR* additional_request_headers) {
  return E_NOTIMPL;
}

// HTMLOMWindowServices
HTMLOMWindowServices::HTMLOMWindowServices(EventHandler* handler)
    : com_ref_count_(0), delegate_(handler) {}

HTMLOMWindowServices::~HTMLOMWindowServices() {}

HRESULT HTMLOMWindowServices::QueryInterface(REFIID riid, void** ppv_object) {
  if (riid == __uuidof(IUnknown)) {
    (*ppv_object) = this;
  } else if (riid == __uuidof(IHTMLOMWindowServices)) {
    (*ppv_object) = static_cast<IHTMLOMWindowServices*>(this);
  } else {
    return E_NOINTERFACE;
  }

  AddRef();
  return S_OK;
}

ULONG HTMLOMWindowServices::AddRef() {
  InterlockedIncrement(&com_ref_count_);
  return com_ref_count_;
}

ULONG HTMLOMWindowServices::Release() {
  InterlockedDecrement(&com_ref_count_);
  return com_ref_count_;
}

HRESULT HTMLOMWindowServices::moveTo(LONG x, LONG y) {
  if (delegate_) {
    RECT rc;
    delegate_->OnGetMainWndPos(&rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    delegate_->OnRequestChangePos(x, y, width, height);
  }

  return S_OK;
}
HRESULT HTMLOMWindowServices::moveBy(LONG x, LONG y) {
  if (delegate_) {
    RECT rc;
    delegate_->OnGetMainWndPos(&rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    delegate_->OnRequestChangePos(rc.left + x, rc.top + y, width, height);
  }

  return S_OK;
}
HRESULT HTMLOMWindowServices::resizeTo(LONG x, LONG y) {
  if (delegate_) {
    RECT rc;
    delegate_->OnGetMainWndPos(&rc);
    delegate_->OnRequestChangePos(rc.left, rc.top, x, y);
  }

  return S_OK;
}
HRESULT HTMLOMWindowServices::resizeBy(LONG x, LONG y) {
  if (delegate_) {
    RECT rc;
    delegate_->OnGetMainWndPos(&rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    delegate_->OnRequestChangePos(rc.left, rc.top, width + x, height + y);
  }

  return S_OK;
}

// DomainAuthenticate
DomainAuthenticate::DomainAuthenticate(EventHandler* handler,
                                       IWebBrowser2* web_browser2)
    : com_ref_count_(0), delegate_(handler), web_browser2_(web_browser2) {
  if (web_browser2_)
    web_browser2_->AddRef();
}

DomainAuthenticate::~DomainAuthenticate() {
  if (web_browser2_)
    web_browser2_->Release();
}

HRESULT DomainAuthenticate::QueryInterface(REFIID riid, void** ppv_object) {
  if (riid == __uuidof(IUnknown)) {
    (*ppv_object) = this;
  } else if (riid == __uuidof(IAuthenticate)) {
    (*ppv_object) = static_cast<IAuthenticate*>(this);
  } else {
    return E_NOINTERFACE;
  }

  AddRef();
  return S_OK;
}

ULONG DomainAuthenticate::AddRef() {
  InterlockedIncrement(&com_ref_count_);
  return com_ref_count_;
}

ULONG DomainAuthenticate::Release() {
  InterlockedDecrement(&com_ref_count_);
  return com_ref_count_;
}

HRESULT DomainAuthenticate::Authenticate(HWND* handle,
                                         LPWSTR* username,
                                         LPWSTR* password) {
  if (handle == NULL || username == NULL || password == NULL)
    return E_INVALIDARG;

  if (web_browser2_ == NULL)
    return E_ACCESSDENIED;

  *handle = NULL;

  std::wstring name = L"";
  std::wstring pwd = L"";
  if (delegate_)
    delegate_->OnWindowDomainAuthenticate(&name, &pwd);

  if (name.empty() == false && pwd.empty() == false) {
    int lenName = (name.size() + 1) * sizeof(wchar_t);
    int lenPwd = (pwd.size() + 1) * sizeof(wchar_t);
    *username = (LPWSTR)CoTaskMemAlloc(lenName);
    memset(*username, 0, lenName);
    memcpy_s(*username, lenName, name.c_str(), name.size() * sizeof(wchar_t));

    *password = (LPWSTR)CoTaskMemAlloc(lenPwd);
    memset(*password, 0, lenPwd);
    memcpy_s(*password, lenPwd, pwd.c_str(), pwd.size() * sizeof(wchar_t));
  } else {
    IOleObject* ole_object = NULL;
    web_browser2_->QueryInterface(IID_IOleObject, (void**)&ole_object);
    if (ole_object == NULL)
      return S_FALSE;
    IOleClientSite* ole_client_site = NULL;
    ole_object->GetClientSite(&ole_client_site);
    if (ole_client_site == NULL)
      return S_FALSE;
    IOleInPlaceSite* ole_in_place_site = NULL;
    ole_client_site->QueryInterface(IID_IOleInPlaceSite,
                                    (void**)&ole_in_place_site);
    if (ole_in_place_site == NULL)
      return S_FALSE;

    ole_in_place_site->GetWindow(handle);
    ole_in_place_site->Release();
    ole_client_site->Release();
    ole_object->Release();
  }

  return S_OK;
}

// NewWindowManager

IWebBrowser2* GetIWebBrowserFromHwnd(HWND hwnd) {
  return NULL;
}

NewWindowManager::NewWindowManager(EventHandler* event_handler)
    : com_ref_count_(0) {}

NewWindowManager::~NewWindowManager() {}

HRESULT NewWindowManager::QueryInterface(REFIID riid, void** ppv_object) {
  if (riid == __uuidof(IUnknown)) {
    (*ppv_object) = this;
  } else if (riid == __uuidof(INewWindowManager)) {
    (*ppv_object) = static_cast<INewWindowManager*>(this);
  } else {
    return E_NOINTERFACE;
  }

  AddRef();
  return S_OK;
}

ULONG NewWindowManager::AddRef() {
  InterlockedIncrement(&com_ref_count_);
  return com_ref_count_;
}

ULONG NewWindowManager::Release() {
  InterlockedDecrement(&com_ref_count_);
  return com_ref_count_;
}

HRESULT NewWindowManager::EvaluateNewWindow(LPCWSTR url,
                                            LPCWSTR name,
                                            LPCWSTR url_context,
                                            LPCWSTR features,
                                            BOOL is_eplace,
                                            DWORD flags,
                                            DWORD user_action_time) {
  // 是在showModelessDialog或者showModeDialog打开的弹出窗口中，又新开的页面
  if ((flags & 0x00000040) == 0x00000040) {
    long flag = ie::NewWindowFlag::TRANSITION;
    if (((flags & 0x40000) == 0x40000 && (flags & 0x20000) == 0) ||
        (flags & 0x10000) == 0x10000)
      flag = ie::NewWindowFlag::POPUP;
    else if ((flags & 0x00000006) == 0x00000006 ||
             (flags & 0x00010086) == 0x00010086 ||
             (flags & 0x00000002) == 0x00000002)
      flag = ie::NewWindowFlag::TRANSITION;

    flag = flag | ie::NewWindowFlag::FROMDIALOG;
    // if (delegate_)
    // {
    //   CComPtr<IDispatch> dispatch;
    //   bool cancel=delegate_->OnLoadUrlNewWnd(url, flag, &dispatch);
    //   if (cancel)
    //     return S_FALSE;
    // }
  }
  return E_FAIL;
}

}  // namespace ie
