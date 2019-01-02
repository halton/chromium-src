// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/IE/browser_process_ie.h"

#include <ShlObj.h>
#include <Shobjidl.h>
#include <commdlg.h>

#include "base/message_loop/message_loop_current.h"
#include "base/run_loop.h"
#include "base/strings/utf_string_conversions.h"
#include "content/common/IE/web_browser_ie.h"
#include "content/public/browser/browser_thread.h"
#include "url/gurl.h"

namespace ie {

const static char kRegisterObjectEventName[] = "Register_Object_Event_Name";

DWORD BrowserProcess::register_ = 0;

BrowserProcess::BrowserProcess()
    : ie_browser_(nullptr),
      host_app_(nullptr),
      ie_handle_(nullptr),
      should_be_show_(VARIANT_FALSE) {
}

BrowserProcess::~BrowserProcess() {
}

STDMETHODIMP BrowserProcess::CreateBrowser(long window_handle,
                                           int browser_emulation,
                                           VARIANT_BOOL is_new_window) {
  HWND handle = static_cast<HWND>(LongToPtr(window_handle));
  ie_browser_ = new WebBrowser(handle, this, browser_emulation,
                               is_new_window == VARIANT_TRUE);
  return S_OK;
}

STDMETHODIMP BrowserProcess::get_WebBrowser(IDispatch** pVal) {
  if (pVal == NULL || ie_browser_ == NULL)
    return E_INVALIDARG;
  *pVal = ie_browser_->GetIeDispatch();
  return S_OK;
}

STDMETHODIMP BrowserProcess::CloseBrowser() {
  if (ie_browser_) {
    ie_browser_->DisableSubClass(ie_handle_);
    delete ie_browser_;
    ie_browser_ = NULL;
  }
  NotifyHostBrowserWillClose();
  if (host_app_) {
    host_app_->Release();
    host_app_ = NULL;
  }
  return S_OK;
}

STDMETHODIMP BrowserProcess::SetRect(int left, int top, int right, int bottom) {
  if (ie_browser_) {
    RECT rc = {left, top, right, bottom};
    ie_browser_->SetRect(rc);
    return S_OK;
  }
  return E_NOTIMPL;
}

STDMETHODIMP BrowserProcess::putref_HostApp(IDispatch* app) {
  host_app_ = app;
  host_app_->AddRef();  //因为是putref 所以调用AddRef 加引用，保证以后可用
  return S_OK;
}

STDMETHODIMP BrowserProcess::ExcuteJS(BSTR src, VARIANT* result) {
  if (result == NULL || ie_browser_ == NULL)
    return E_FAIL;
  std::wstring code = src;
  variant_t ret;
  HRESULT hr = ie_browser_->ExcuteJavaScript(code, &ret);
  *result = ret;
  return hr;
}

STDMETHODIMP BrowserProcess::Show(VARIANT_BOOL bShow) {
  if (ie_browser_ == NULL)
    return E_FAIL;
  should_be_show_ = bShow;
  if (bShow == VARIANT_TRUE) {
    if (IsWindow(ie_handle_)) {
      ie_browser_->Show();
    }
  } else
    ie_browser_->Hide();
  return S_OK;
}

STDMETHODIMP BrowserProcess::GetDocumentMode(int* mode) {
  if (ie_browser_) {
    *mode = ie_browser_->GetDocumentMode();
    return S_OK;
  }
  return E_FAIL;
}

STDMETHODIMP BrowserProcess::SetDocumentMode(int mode) {
  if (ie_browser_) {
    DocumentMode temp = static_cast<DocumentMode>(mode);
    bool b = ie_browser_->SetDocumentMode(temp);
    if (b)
      return S_OK;
  }
  return E_FAIL;
}

STDMETHODIMP BrowserProcess::SetCookie(BSTR json, int flag) {
  if (ie_browser_) {
    bool ret = false;
    ret = ie_browser_->SetBrowserCookie(json, flag);
    if (ret)
      return S_OK;
  }
  return E_FAIL;
}

STDMETHODIMP BrowserProcess::SetUserAgent(BSTR user_agent) {
  if (ie_browser_) {
    HRESULT hr = ie_browser_->SetUserAgent(user_agent);
    return hr;
  }
  return E_FAIL;
}

STDMETHODIMP BrowserProcess::FinishDownload(VARIANT_BOOL is_cancel) {
  bool cancel = false;
  if (is_cancel == VARIANT_TRUE)
    cancel = true;
  if (ie_browser_)
    ie_browser_->FinishDownload(cancel);
  return S_OK;
}

STDMETHODIMP BrowserProcess::SetFunctionControl(BSTR json_string) {
  if (ie_browser_ == NULL)
    return E_FAIL;
  ie_browser_->SetFunctionControl(json_string);
  return S_OK;
}

STDMETHODIMP BrowserProcess::AutoLogin(BSTR frame_xpath,
                                       BSTR username_xpath,
                                       BSTR username_value,
                                       BSTR password_xpath,
                                       BSTR password_value,
                                       BSTR login_button_xpath) {
  if (ie_browser_ == NULL)
    return E_FAIL;
  ie_browser_->AutoLoginToSite(frame_xpath, username_xpath, username_value,
                               password_xpath, password_value,
                               login_button_xpath);
  return S_OK;
}

STDMETHODIMP BrowserProcess::SetListenLoginXPath(BSTR url,
                                                 BSTR frame_xpath,
                                                 BSTR username_xpath,
                                                 BSTR password_xpath,
                                                 BSTR login_button_xpath) {
  if (ie_browser_ == NULL)
    return E_FAIL;

  ie_browser_->SetListenLoginXPath(url, frame_xpath, username_xpath,
                                   password_xpath, login_button_xpath);
  return S_OK;
}

STDMETHODIMP BrowserProcess::NotifyCommand(BSTR command) {
  if (ie_browser_ == NULL)
    return E_FAIL;
  ie_browser_->NotifyCommand(command);
  return S_OK;
}

STDMETHODIMP BrowserProcess::GetDevToolsOpenStatus(VARIANT_BOOL* is_open) {
  if (ie_browser_ == NULL || is_open == NULL)
    return E_FAIL;
  bool open = ie_browser_->IsDevToolsOpened();
  VARIANT_BOOL retOpen = VARIANT_FALSE;
  if (open)
    retOpen = VARIANT_TRUE;
  *is_open = retOpen;
  return S_OK;
}

STDMETHODIMP BrowserProcess::ShowDevTools(VARIANT_BOOL show) {
  if (ie_browser_ == NULL)
    return E_FAIL;
  bool isShow = false;
  if (show == VARIANT_TRUE)
    isShow = true;
  ie_browser_->ShowDevTools(isShow);
  return S_OK;
}

bool BrowserProcess::OnBeforeNavigate(const std::wstring& url, bool click_url) {
  if (host_app_) {
    std::vector<variant_t> params;
    params.push_back(url.c_str());
    if (click_url)
      params.push_back(VARIANT_TRUE);
    else
      params.push_back(VARIANT_FALSE);
    VARIANT_BOOL cancel = VARIANT_FALSE;
    variant_t v;
    v.pboolVal = &cancel;
    v.vt = VT_BOOL | VT_BYREF;
    params.push_back(v);
    variant_t ret;
    HRESULT hr = DispatchInvoke(host_app_, L"OnBeforeNavigate", DISPATCH_METHOD,
                                params, ret);
    if (SUCCEEDED(hr) && cancel != VARIANT_FALSE)
      return true;
  }
  return false;
}

void BrowserProcess::OnFinishNavigate(const std::wstring& url,
                                      const std::wstring& favicon_urls) {
  if (host_app_) {
    std::vector<variant_t> params;
    params.push_back(url.c_str());
    params.push_back(favicon_urls.c_str());
    variant_t ret;
    DispatchInvoke(host_app_, L"OnFinishNavigate", DISPATCH_METHOD, params,
                   ret);
  }
  if (ie_browser_ == NULL)
    return;
  ie_browser_->ConfirmLoginSuccess();
  HWND hwnd = 0;
  hwnd = ie_browser_->GetControlWindow();
  if (IsWindow(hwnd)) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    ::MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left - 1,
                 rect.bottom - rect.top - 1, FALSE);
    ::MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left,
                 rect.bottom - rect.top, TRUE);
  }
}

void BrowserProcess::OnTitleChange(const std::wstring& title) {
  if (host_app_) {
    std::vector<variant_t> params;
    params.push_back(title.c_str());
    variant_t ret;
    DispatchInvoke(host_app_, L"OnTitleChange", DISPATCH_METHOD, params, ret);
  }
}

bool BrowserProcess::OnLoadUrlNewWnd(const std::wstring& url,
                                     long flag,
                                     IDispatch** dispatch) {
  if (host_app_) {
    std::vector<variant_t> params;
    variant_t argUrl;
    argUrl.vt = VT_BSTR;
    argUrl.bstrVal = bstr_t(url.c_str());
    params.push_back(argUrl);
    variant_t argFlag;
    argFlag.vt = VT_I4;
    argFlag.intVal = flag;
    params.push_back(argFlag);
    VARIANT_BOOL cancel = VARIANT_FALSE;
    variant_t v;
    v.pboolVal = &cancel;
    v.vt = VT_BOOL | VT_BYREF;
    params.push_back(v);
    IDispatch* idisp = NULL;
    variant_t argDisp;
    argDisp.vt = VT_BYREF | VT_DISPATCH;
    argDisp.ppdispVal = &idisp;
    params.push_back(argDisp);
    variant_t ret;
    HRESULT hr = DispatchInvoke(host_app_, L"OnLoadUrlNewWnd", DISPATCH_METHOD,
                                params, ret);
    if (SUCCEEDED(hr)) {
      if (idisp) {
        idisp->AddRef();
        *dispatch = idisp;
      }
    }
    if (cancel == VARIANT_TRUE)
      return true;
  }
  return false;
}

void BrowserProcess::OnBrowserClosing() {
  if (host_app_) {
    std::vector<variant_t> params;
    variant_t ret;
    HRESULT hr = E_FAIL;
    hr = DispatchInvoke(host_app_, L"OnBrowserClosing", DISPATCH_METHOD, params,
                        ret);
  }
}

void BrowserProcess::SetIEHwnd(HWND hwnd) {
  ie_handle_ = hwnd;
  wchar_t wndClassName[256] = {0};
  GetClassName(ie_handle_, wndClassName,
               255);  //如果打开的是FILE://协议的url，IE不会创建Internet
                      // Explorer_Server子窗口，所以不能subclass
  if (ie_browser_ &&
      std::wstring(wndClassName) == L"Internet Explorer_Server") {
    ie_browser_->EnableSubClass(ie_handle_);
  }
  Show(should_be_show_);
  OnIEServerCreated(hwnd);
}

bool BrowserProcess::OnShowContxtMenu(int id, POINT pt) {
  if (host_app_) {
    std::vector<variant_t> params;
    params.push_back(id);
    params.push_back(pt.x);
    params.push_back(pt.y);
    variant_t ret;
    DispatchInvoke(host_app_, L"OnShowContextMenu", DISPATCH_METHOD, params,
                   ret);
    return true;
  }
  return false;
}

bool BrowserProcess::OnDownLoadFile(const std::wstring& url,
                                    int status,
                                    VARIANT variant) {
  if (host_app_) {
    std::vector<variant_t> params;
    params.push_back(url.c_str());
    params.push_back(status);
    variant_t var;
    var.vt = VT_BYREF | VT_VARIANT;
    var.pvarVal = &variant;
    params.push_back(var);
    variant_t ret;
    HRESULT hr = DispatchInvoke(host_app_, L"OnDownLoadFile", DISPATCH_METHOD,
                                params, ret);
    if (SUCCEEDED(hr))
      return true;
  }
  return false;
}

void BrowserProcess::GetGoBackOrForwardState(bool* go_back, bool* go_forward) {
  if (host_app_) {
    std::vector<variant_t> params;
    variant_t temp;
    temp.vt = VT_BOOL | VT_BYREF;
    VARIANT_BOOL back = VARIANT_FALSE;
    temp.pboolVal = &back;
    params.push_back(temp);

    VARIANT_BOOL forward = VARIANT_FALSE;
    temp.pboolVal = &forward;
    params.push_back(temp);
    variant_t ret;
    HRESULT hr = E_FAIL;
    hr = DispatchInvoke(host_app_, L"OnCanGoBackOrForward", DISPATCH_METHOD,
                        params, ret);
    if (SUCCEEDED(hr)) {
      if (back == VARIANT_TRUE)
        *go_back = true;
      if (forward == VARIANT_TRUE)
        *go_forward = true;
    }
  }
}

void BrowserProcess::GoBack() {
  if (host_app_) {
    std::vector<variant_t> params;
    variant_t ret;
    HRESULT hr = E_FAIL;
    hr = DispatchInvoke(host_app_, L"OnGoBack", DISPATCH_METHOD, params, ret);
  }
}

void BrowserProcess::GoForward() {
  if (host_app_) {
    std::vector<variant_t> params;
    variant_t ret;
    HRESULT hr = E_FAIL;
    hr =
        DispatchInvoke(host_app_, L"OnGoForward", DISPATCH_METHOD, params, ret);
  }
}

void BrowserProcess::OnRequestChangePos(int left,
                                        int top,
                                        int width,
                                        int height) {
  if (host_app_) {
    std::vector<variant_t> params;
    params.push_back(left);
    params.push_back(top);
    params.push_back(width);
    params.push_back(height);
    variant_t ret;
    HRESULT hr = E_FAIL;
    hr = DispatchInvoke(host_app_, L"OnRequestChangePos", DISPATCH_METHOD,
                        params, ret);
  }
}

void BrowserProcess::OnGetMainWndPos(RECT* rect) {
  int left = 0;
  int top = 0;
  int width = 0;
  int height = 0;
  ::SetRect(rect, 0, 0, 0, 0);
  if (host_app_) {
    std::vector<variant_t> params;
    variant_t temp;
    temp.vt = VT_I4 | VT_BYREF;
    temp.pintVal = &left;
    params.push_back(temp);

    temp.pintVal = &top;
    params.push_back(temp);

    temp.pintVal = &width;
    params.push_back(temp);

    temp.pintVal = &height;
    params.push_back(temp);
    variant_t ret;
    HRESULT hr = E_FAIL;
    hr = DispatchInvoke(host_app_, L"OnGetMainWndPos", DISPATCH_METHOD, params,
                        ret);
    if (SUCCEEDED(hr))
      ::SetRect(rect, left, top, left + width, top + height);
  }
}

void BrowserProcess::OnIEServerCreated(HWND hwnd) {
  if (host_app_) {
    std::vector<variant_t> params;
    params.push_back((int)hwnd);
    variant_t ret;
    HRESULT hr = E_FAIL;
    hr = DispatchInvoke(host_app_, L"OnIEServerWndCreated", DISPATCH_METHOD,
                        params, ret);
  }
}

void BrowserProcess::OnUpdateCookie(const std::wstring& cookie) {
  if (host_app_) {
    std::vector<variant_t> params;
    params.push_back(cookie.c_str());
    variant_t ret;
    HRESULT hr = E_FAIL;
    hr = DispatchInvoke(host_app_, L"OnUpdateCookie", DISPATCH_METHOD, params,
                        ret);
  }
}

void BrowserProcess::OnClickElement(CComPtr<IHTMLElement> click_element,
                                    bool by_enter_key) {
  if (ie_browser_)
    ie_browser_->TryGetLoginInfo(click_element, by_enter_key);
}

void BrowserProcess::OnGetLoginInfo(const std::wstring& url,
                                    const std::wstring& username,
                                    const std::wstring& password) {
  if (host_app_) {
    std::vector<variant_t> params;
    params.push_back(url.c_str());
    params.push_back(username.c_str());
    params.push_back(password.c_str());
    variant_t ret;
    HRESULT hr = E_FAIL;
    hr = DispatchInvoke(host_app_, L"OnGetLoginInfo", DISPATCH_METHOD, params,
                        ret);
  }
}

void BrowserProcess::OnRequestAcceleratorFromKeyboard(int key_code,
                                                      int modifiers) {
  if (host_app_) {
    std::vector<variant_t> params;
    params.push_back(key_code);
    params.push_back(modifiers);
    variant_t ret;
    HRESULT hr = E_FAIL;
    hr = DispatchInvoke(host_app_, L"OnRequestAcceleratorFromKeyboard",
                        DISPATCH_METHOD, params, ret);
  }
}

void BrowserProcess::OnRequestAcceleratorFromMouseWheel(int delta,
                                                        int point_x,
                                                        int point_y,
                                                        int modifiers) {
  if (host_app_) {
    std::vector<variant_t> params;
    params.push_back(delta);
    params.push_back(point_x);
    params.push_back(point_y);
    params.push_back(modifiers);
    variant_t ret;
    HRESULT hr = E_FAIL;
    hr = DispatchInvoke(host_app_, L"OnRequestAcceleratorFromMouseWheel",
                        DISPATCH_METHOD, params, ret);
  }
}

void BrowserProcess::OnWindowDomainAuthenticate(std::wstring* username,
                                                std::wstring* password) {
  if (host_app_ && username && password) {
    *username = L"";
    *password = L"";
    std::vector<variant_t> params;
    bstr_t uNameStr = L"";
    variant_t arg1;
    arg1.vt = VT_BYREF | VT_BSTR;
    arg1.pbstrVal = uNameStr.GetAddress();
    bstr_t uPwdStr = L"";
    variant_t arg2;
    arg2.vt = VT_BYREF | VT_BSTR;
    arg2.pbstrVal = uPwdStr.GetAddress();
    params.push_back(arg1);
    params.push_back(arg2);
    variant_t ret;
    HRESULT hr = E_FAIL;
    hr = DispatchInvoke(host_app_, L"OnWinDomainAuthenticate", DISPATCH_METHOD,
                        params, ret);
    if (SUCCEEDED(hr) && uNameStr.length() > 0 && uPwdStr.length() > 0) {
      *username = uNameStr;
      *password = uPwdStr;
    }
  }
}

void BrowserProcess::OnQueryPrivateDNS(const std::wstring& host,
                                       std::wstring* ip_list_json_string) {
  if (host_app_ && host.empty() == false && ip_list_json_string) {
    *ip_list_json_string = L"";
    std::vector<variant_t> params;
    bstr_t hostArg = host.c_str();
    params.push_back(hostArg);
    bstr_t ipListArg = L"";
    variant_t arg2;
    arg2.vt = VT_BYREF | VT_BSTR;
    arg2.pbstrVal = ipListArg.GetAddress();
    params.push_back(arg2);
    variant_t ret;
    HRESULT hr = E_FAIL;
    hr = DispatchInvoke(host_app_, L"OnQueryPrivateDNS", DISPATCH_METHOD,
                        params, ret);
    if (SUCCEEDED(hr) && ipListArg.GetBSTR()) {
      *ip_list_json_string = ipListArg.GetBSTR();
    }
  }
}
bool BrowserProcess::RegisterClassObject() {
  // lamda used for notify browser process before leaving the function
  auto open_event_functor = [=]() {
    HANDLE register_object_event =
        OpenEventA(EVENT_ALL_ACCESS, FALSE, kRegisterObjectEventName);
    // in case of open error[almost impossible]
    // try to open event once per second
    while (!register_object_event) {
      Sleep(1000);
      register_object_event =
          OpenEventA(EVENT_ALL_ACCESS, FALSE, kRegisterObjectEventName);
    }
    SetEvent(register_object_event);
  };

  CComPtr<IUnknown> cf;
  HRESULT hr = AtlComModuleGetClassObject(
      &_AtlComModule, CLSID_BrowserContainer, IID_IUnknown, (void**)&cf);
  if (FAILED(hr)) {
    open_event_functor();
    return false;
  }
  hr = CoRegisterClassObject(CLSID_BrowserContainer, cf, CLSCTX_LOCAL_SERVER,
                             REGCLS_SINGLEUSE, &register_);
  open_event_functor();
  return SUCCEEDED(hr);
}

bool BrowserProcess::UnregisterClassObject() {
  if (register_ != 0) {
    HRESULT hr = CoRevokeClassObject(register_);
    if (SUCCEEDED(hr))
      return true;
  }
  return false;
}

HRESULT BrowserProcess::DispatchInvoke(IDispatch* dispatch,
                                       const wchar_t* name,
                                       int type,
                                       std::vector<variant_t>& params,
                                       variant_t& ret) {
  HRESULT hr = E_INVALIDARG;
  if (dispatch == NULL)
    return hr;

  DISPID dispID = 0;
  OLECHAR* szMember = (WCHAR*)name;
  hr = dispatch->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_SYSTEM_DEFAULT,
                               &dispID);
  if (FAILED(hr))
    return hr;

  DISPPARAMS args;
  args.cArgs = params.size();
  args.cNamedArgs = 0;
  args.rgdispidNamedArgs = NULL;
  args.rgvarg = NULL;
  if (args.cArgs > 0) {
    args.rgvarg = new VARIANTARG[args.cArgs];
    int i = params.size() - 1;
    int j = 0;
    for (; i >= 0; i--) {
      args.rgvarg[j] = params[i];
      j++;
    }
  }
  hr = dispatch->Invoke(dispID, IID_NULL, LOCALE_USER_DEFAULT, type, &args,
                        &ret, NULL, NULL);
  if (args.cArgs) {
    delete[] args.rgvarg;
  }
  return hr;
}

void BrowserProcess::NotifyHostBrowserWillClose() {
  if (host_app_) {
    std::vector<variant_t> params;
    variant_t ret;
    DispatchInvoke(host_app_, L"OnBeforeBrowserClose", DISPATCH_METHOD, params,
                   ret);
  }
}
}  // namespace ie
