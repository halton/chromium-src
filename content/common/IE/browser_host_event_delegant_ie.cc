// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/IE/browser_host_event_delegant_ie.h"

#include <MsHtmHst.h>
#include <Shobjidl.h>
#include <exdisp.h>

#include "base/json/json_reader.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "base/win/win_util.h"
#include "base/win/windows_version.h"
#include "content/browser/frame_host/navigation_controller_impl.h"
#include "content/browser/web_contents/web_contents_ie.h"
#include "content/common/IE/IEInterface.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/download_manager.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/common/context_menu_params.h"
#include "content/public/common/favicon_url.h"
#include "ui/aura/window.h"
#include "ui/events/event.h"
#include "ui/events/keycodes/keyboard_code_conversion_win.h"
#include "url/gurl.h"

#pragma warning(disable : 4302)
#pragma warning(disable : 4189)

namespace FOR_INCLUDE {
#include "content/common/IE/IEInterface_i.c"
}

namespace {

void split(std::wstring string,
           std::wstring delim,
           std::vector<std::wstring>& result) {
  size_t last = 0;
  size_t index = string.find_first_of(delim, last);
  while (index != std::wstring::npos) {
    std::wstring temp = string.substr(last, index - last);
    if (temp != L"") {
      result.push_back(temp);
    }

    last = index + 1;
    index = string.find_first_of(delim, last);
  }

  if (string.size() - last > 0) {
    result.push_back(string.substr(last, string.size() - last));
  }
}

}  // namespace

namespace ie {

const static char kRegisterObjectEventName[] = "Register_Object_Event_Name";
// initialize once
HANDLE BrowserHostEventDelegant::register_object_event_ =
    CreateEventA(nullptr, true, false, kRegisterObjectEventName);

BrowserHostEventDelegant::BrowserHostEventDelegant()
    : main_window_(NULL),
      web_browser2_(NULL),
      host_window_(NULL),
      ie_browser_handle_(NULL),
      is_loading_(false),
      page_title_(L""),
      ie_content_(NULL),
      browser_container_(NULL),
      weak_factory_(this),
      is_download_(false),
      function_control_json_(L""),
      is_disable_screen_capture_(false),
      get_window_display_affinity_function_(NULL),
      set_window_display_affinity_function_(NULL),
      delegant_weak_factory_(this),
      update_dl_data_weak_factory_(this) {
  set_window_display_affinity_function_ =
      (SetWindowDisplayAffinityPtr)GetProcAddress(
          GetModuleHandleA("user32.dll"), "SetWindowDisplayAffinity");
  get_window_display_affinity_function_ =
      (GetWindowDisplayAffinityPtr)GetProcAddress(
          GetModuleHandleA("user32.dll"), "GetWindowDisplayAffinity");
}

BrowserHostEventDelegant::~BrowserHostEventDelegant() {
  if (IsWindow(host_window_)) {
    ::DestroyWindow(host_window_);
  }
}

STDMETHODIMP
BrowserHostEventDelegant::OnBeforeNavigate(BSTR url,
                                           VARIANT_BOOL navigate_new_url,
                                           VARIANT_BOOL* cancel) {
  *cancel = VARIANT_FALSE;
  is_loading_ = true;

  if (ie_content_) {
    bool click_url = false;
    if (navigate_new_url == VARIANT_TRUE)
      click_url = true;
    bool ret = false;
    ie_content_->OnBeforeNavigate(url, click_url, &ret);
    if (ret)
      *cancel = VARIANT_TRUE;
  }
  return S_OK;
}

STDMETHODIMP
BrowserHostEventDelegant::OnFinishNavigate(BSTR url, BSTR favicon_urls) {
  // variant_t ret;
  // ExcuteJavaScript(L"myHello(4,6);", &ret);

  // 解决IE屏蔽截屏bug，参看CreateHostWindow函数中的注释
  if (base::win::GetVersion() >= base::win::VERSION_WIN7 &&
      set_window_display_affinity_function_) {
    HWND mainHwnd = GetParent(host_window_);
    if (is_disable_screen_capture_ && IsWindow(mainHwnd)) {
      BOOL ret = FALSE;
      ret = set_window_display_affinity_function_(mainHwnd, WDA_MONITOR);
    }
  }

  is_loading_ = false;
  if (ie_content_) {
    GURL gurl = GURL(std::wstring(url));

    std::vector<std::wstring> favicon_ursl;
    split(std::wstring(favicon_urls), L"\t", favicon_ursl);
    std::vector<GURL> faviconGUrls;
    for (auto it = favicon_ursl.begin(); it != favicon_ursl.end(); it++) {
      faviconGUrls.push_back(GURL(*it));
    }

    ie_content_->OnFinishNavigate(gurl, faviconGUrls);
    ie_content_->NotifyNavigationStateChanged(content::INVALIDATE_TYPE_ALL);
  }
  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnTitleChange(BSTR title) {
  page_title_ = std::wstring(title, SysStringLen(title));
  if (ie_content_) {
    ie_content_->NotifyNavigationStateChanged(content::INVALIDATE_TYPE_ALL);
  }
  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnLoadUrlNewWnd(BSTR url,
                                                       int flag,
                                                       VARIANT_BOOL* cancel,
                                                       IDispatch** dispatch) {
  HRESULT result = E_FAIL;
  if (web_browser2_ == NULL)
    return result;

  if (ie_content_) {
    GURL gurl = GURL(std::wstring(url));
    bool stop = false;
    ie_content_->OnLoadUrlInNewContent(gurl, flag, &stop, dispatch);
    if (stop)
      *cancel = VARIANT_TRUE;
    result = S_OK;
  }

  return result;
}

STDMETHODIMP BrowserHostEventDelegant::OnBrowserClosing() {
  if (ie_content_) {
    // 先让函数返回完成COM
    // rpc调用在执行关闭过程，否则BrowserHostEventDelegant的引用计数不对无法析构
    content::BrowserThread::PostTask(
        content::BrowserThread::UI, FROM_HERE,
        base::Bind(&BrowserHostEventDelegant::OnJSWindowClose,
                   delegant_weak_factory_.GetWeakPtr()));
  }

  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnBeforeBrowserClose() {
  if (web_browser2_) {
    web_browser2_->Release();
    web_browser2_ = NULL;
  }

  if (browser_container_) {
    browser_container_->Release();
    browser_container_ = NULL;
  }
  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnShowContextMenu(int id, int x, int y) {
  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnDownLoadFile(BSTR url,
                                                      int status,
                                                      VARIANT* variant) {
  std::wstring temp = url;
  if (ie_content_) {
    if (status == (int)ie::BEGINDOWNLOAD && variant && variant->vt == VT_BSTR) {
      is_download_ = true;
      download_data_callback_.Reset();
      std::wstring response_header = variant->bstrVal;
      ie_content_->OnBeforeDownloadFile(url, response_header);
    } else if (status == (int)DOWNLOADINGDATA && variant &&
               variant->vt == VT_ARRAY) {
      SAFEARRAY* safe_array = variant->parray;
      HRESULT result = E_FAIL;
      long low = 0;
      long high = 0;
      // 维数索引从1开始，数组下界
      result = SafeArrayGetLBound(safe_array, 1, &low);
      // 维数索引从1开始，数组上界
      result = SafeArrayGetUBound(safe_array, 1, &high);
      long size = high - low + 1;
      char* data = NULL;
      result = SafeArrayAccessData(safe_array, (void**)&data);
      std::string buffer = "";
      buffer.append(data, size);
      SafeArrayUnaccessData(safe_array);
      content::BrowserThread::PostTask(
          content::BrowserThread::IO, FROM_HERE,
          base::Bind(&BrowserHostEventDelegant::OnJSWindowClose,
                     delegant_weak_factory_.GetWeakPtr()));
    } else if (status == (int)ENDDOWNLOAD || status == (int)DOWNLOADERROR) {
      std::string buffer = "";
      content::BrowserThread::PostTask(
          content::BrowserThread::IO, FROM_HERE,
          base::Bind(&BrowserHostEventDelegant::OnJSWindowClose,
                     delegant_weak_factory_.GetWeakPtr()));
    }
  }

  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnCanGoBackOrForward(
    VARIANT_BOOL* back,
    VARIANT_BOOL* forward) {
  if (ie_content_) {
    *back = VARIANT_FALSE;
    *forward = VARIANT_FALSE;
    if (ie_content_->GetController().CanGoBack())
      *back = VARIANT_TRUE;
    if (ie_content_->GetController().CanGoForward())
      *forward = VARIANT_TRUE;
  }

  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnGoBack() {
  if (ie_content_)
    ie_content_->GetController().GoBack();

  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnGoForward() {
  if (ie_content_)
    ie_content_->GetController().GoForward();

  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnRequestChangePos(int left,
                                                          int top,
                                                          int width,
                                                          int height) {
  if (ie_content_) {
    gfx::Rect rect;
    rect.SetRect(left, top, width, height);
    ie_content_->RequestSetBounds(rect);
  }

  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnGetMainWndPos(int* left,
                                                       int* top,
                                                       int* width,
                                                       int* height) {
  *left = 0;
  *top = 0;
  *width = 0;
  *height = 0;

  if (ie_content_ && IsWindow(host_window_)) {
    HWND parent = GetParent(host_window_);
    if (IsWindow(parent)) {
      RECT rect;
      ::GetWindowRect(parent, &rect);
      *left = rect.left;
      *top = rect.top;
      *width = rect.right - rect.left;
      *height = rect.bottom - rect.top;
    }
  }

  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnIEServerWndCreated(int window_handle) {
  ie_browser_handle_ = reinterpret_cast<HWND>(window_handle);
  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnUpdateCookie(BSTR cookie) {
  std::wstring json_string = cookie;
  std::string temp = base::UTF16ToUTF8(json_string);

  std::unique_ptr<base::Value> root = base::JSONReader().ReadToValue(temp);
  if (root == NULL)
    return E_FAIL;

  if (root->is_dict() && ie_content_) {
    base::DictionaryValue* root_dict = NULL;
    root->GetAsDictionary(&root_dict);
    std::wstring url = L"";
    root_dict->GetString("url", &url);

    std::vector<std::wstring> cookie_vector;
    base::ListValue* list = NULL;
    root_dict->GetList("cookies", &list);
    base::ListValue::const_iterator iter = list->begin();
    for (; iter != list->end(); iter++) {
      std::wstring buffer = L"";
      iter->GetAsString(&buffer);
      cookie_vector.push_back(buffer);
    }
    ie_content_->OnUpdateCookie(GURL(url), cookie_vector);
  }
  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnGetLoginInfo(BSTR url,
                                                      BSTR username,
                                                      BSTR password) {
  if (url == NULL || username == NULL || password == NULL)
    return E_FAIL;

  std::wstring uname = username;
  std::wstring upwd = password;
  if (ie_content_)
    ie_content_->OnGetLoginInfo(url, username, password);

  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnRequestAcceleratorFromKeyboard(
    int key_code,
    int modifiers) {
  if (ie_content_) {
    ui::KeyboardCode code = ui::KeyboardCodeForWindowsKeyCode(key_code);
    ui::KeyEvent key_event(ui::ET_KEY_PRESSED, code, 0);
    content::NativeWebKeyboardEvent event(key_event);
    event.SetModifiers(modifiers);
    ie_content_->HandleKeyboardEvent(event);
  }
  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnRequestAcceleratorFromMouseWheel(
    int delta,
    int ponit_x,
    int ponit_y,
    int modifiers) {
  if (ie_content_) {
    blink::WebMouseWheelEvent event;
    event.wheel_ticks_y = delta / 120;
    event.SetModifiers(modifiers);
    event.delta_x = ponit_x;
    event.delta_y = ponit_y;
    ie_content_->HandleWheelEvent(event);
  }
  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnWinDomainAuthenticate(BSTR* username,
                                                               BSTR* password) {
  if (username == NULL || password == NULL)
    return E_INVALIDARG;

  std::wstring name = L"";
  std::wstring pwd = L"";
  if (ie_content_) {
    ie_content_->GetDelegate()->DidGetWindowsDomainUserInfo(&name, &pwd);
  }

  *username = ::SysAllocString(name.c_str());
  *password = ::SysAllocString(pwd.c_str());
  return S_OK;
}

STDMETHODIMP BrowserHostEventDelegant::OnQueryPrivateDNS(BSTR host,
                                                         BSTR* ip_list) {
  if (host == NULL || ip_list == NULL)
    return E_INVALIDARG;

  std::wstring json_string = L"";
  if (ie_content_) {
    ie_content_->OnQueryPrivateDns(host, &json_string);
    if (json_string.empty() == false) {
      *ip_list = ::SysAllocString(json_string.c_str());
    }
  }
  return S_OK;
}

void BrowserHostEventDelegant::SetHostHWND(HWND window_handle) {
  main_window_ = window_handle;
}

bool BrowserHostEventDelegant::CreateBrowser(int browser_emulation,
                                             bool is_new) {
  HWND parent_hwnd = GetParent(main_window_);
  HWND window_handle = FindWindowExW(parent_hwnd, main_window_,
                                     L"Intermediate Software Window", nullptr);

  DCHECK(!!window_handle);

  if (IsWindow(host_window_) == FALSE) {
    host_window_ = CreateHostWindow(parent_hwnd);
    // host_window_ = CreateHostWindow(NULL);
    MoveWindow(host_window_, -300, -300, 300, 300, TRUE);
  }

  IClassFactory* class_factory = NULL;
  HRESULT hr;

  const static DWORD kRegisterEventWaitMillionSecond = 5000;

  DWORD ret = WaitForSingleObject(register_object_event_,
                                  kRegisterEventWaitMillionSecond);
  switch (ret) {
    case WAIT_ABANDONED:
    case WAIT_TIMEOUT:
    case WAIT_FAILED:
      LOG(ERROR) << "trident WaitForSingleObject error code  "
                 << GetLastError();
      break;
  }

  while (class_factory == NULL) {
    DLOG(INFO) << "trident CreateHostWindow begin to create";
    Sleep(500);
    hr = CoGetClassObject(CLSID_BrowserContainer, CLSCTX_LOCAL_SERVER, NULL,
                          IID_IClassFactory, (LPVOID*)&class_factory);
  }

  if (class_factory) {
    HRESULT result = class_factory->CreateInstance(NULL, IID_IDispatch,
                                                   (PVOID*)&browser_container_);

    if (browser_container_ == NULL)
      return false;

    std::vector<variant_t> params;
    variant_t param = (long)host_window_;
    params.push_back(param);
    params.push_back(browser_emulation);
    variant_t param_new;
    param_new.vt = VT_BOOL;
    if (is_new)
      param_new.boolVal = VARIANT_TRUE;
    else
      param_new.boolVal = VARIANT_FALSE;
    params.push_back(param_new);
    variant_t ret1;
    result = BrowserHostEventDelegant::DispatchInvoke(
        browser_container_, L"CreateBrowser", DISPATCH_METHOD, params, ret1);

    web_browser2_ = GetIWebBrowser2();
    if (web_browser2_)
      web_browser2_->AddRef();
    SetHostIDispatch(this);
    class_factory->Release();

    if (function_control_json_.empty() == false)
      SendFunctionControl(function_control_json_);
  }

  return true;
}

void BrowserHostEventDelegant::CloseBrowser() {
  if (browser_container_) {
    std::vector<variant_t> params;
    variant_t ret;
    DispatchInvoke(browser_container_, L"CloseBrowser", DISPATCH_METHOD, params,
                   ret);
  }
}

bool BrowserHostEventDelegant::LoadUrl(const std::wstring& url) {
  if (web_browser2_ == NULL)
    return false;

  HRESULT result = S_OK;
  std::vector<variant_t> params;
  variant_t ret;

  params.push_back(L"navigate");
  DispatchInvoke(browser_container_, L"NotifyCommand", DISPATCH_METHOD, params,
                 ret);

  params.clear();
  ret.Clear();
  params.push_back(url.c_str());
  variant_t flags = navNoHistory;
  params.push_back(flags);
  variant_t targetFrameName;
  params.push_back(targetFrameName);
  variant_t postData;
  params.push_back(postData);
  variant_t headers;
  params.push_back(headers);
  result =
      DispatchInvoke(web_browser2_, L"Navigate2", DISPATCH_METHOD, params, ret);
  if (SUCCEEDED(result)) {
    page_title_ = L"";
    if (ie_content_)
      ie_content_->NotifyNavigationStateChanged(content::INVALIDATE_TYPE_ALL);
    return true;
  }
  return false;
}

void BrowserHostEventDelegant::Stop() {
  if (web_browser2_ == NULL)
    return;

  is_loading_ = false;
  std::vector<variant_t> params;
  variant_t ret;
  DispatchInvoke(web_browser2_, L"Stop", DISPATCH_METHOD, params, ret);
}

void BrowserHostEventDelegant::Refresh() {
  if (web_browser2_ == NULL)
    return;

  std::vector<variant_t> params;
  params.push_back(L"refresh");
  variant_t ret;
  DispatchInvoke(browser_container_, L"NotifyCommand", DISPATCH_METHOD, params,
                 ret);

  params.clear();
  ret.Clear();
  DispatchInvoke(web_browser2_, L"Refresh", DISPATCH_METHOD, params, ret);
}

void BrowserHostEventDelegant::Find() {
  if (web_browser2_ == NULL)
    return;

  std::vector<variant_t> params;
  params.push_back((int)OLECMDID_FIND);
  params.push_back((int)OLECMDEXECOPT_DODEFAULT);
  params.push_back(variant_t());
  params.push_back(variant_t());
  variant_t ret;
  DispatchInvoke(web_browser2_, L"ExecWB", DISPATCH_METHOD, params, ret);
}

void BrowserHostEventDelegant::Print() {
  if (web_browser2_ == NULL)
    return;

  std::vector<variant_t> params;
  params.push_back((int)OLECMDID_PRINTPREVIEW);
  params.push_back((int)OLECMDEXECOPT_DODEFAULT);
  params.push_back(variant_t());
  params.push_back(variant_t());
  variant_t ret;
  DispatchInvoke(web_browser2_, L"ExecWB", DISPATCH_METHOD, params, ret);
}

void BrowserHostEventDelegant::Cut() {
  if (web_browser2_ == NULL)
    return;

  std::vector<variant_t> params;
  params.push_back((int)OLECMDID_CUT);
  params.push_back((int)OLECMDEXECOPT_DODEFAULT);
  params.push_back(variant_t());
  params.push_back(variant_t());
  variant_t ret;
  DispatchInvoke(web_browser2_, L"ExecWB", DISPATCH_METHOD, params, ret);
}

void BrowserHostEventDelegant::Copy() {
  if (web_browser2_ == NULL)
    return;

  std::vector<variant_t> params;
  params.push_back((int)OLECMDID_COPY);
  params.push_back((int)OLECMDEXECOPT_DODEFAULT);
  params.push_back(variant_t());
  params.push_back(variant_t());
  variant_t ret;
  DispatchInvoke(web_browser2_, L"ExecWB", DISPATCH_METHOD, params, ret);
}

void BrowserHostEventDelegant::Paste() {
  if (web_browser2_ == NULL)
    return;

  std::vector<variant_t> params;
  params.push_back((int)OLECMDID_PASTE);
  params.push_back((int)OLECMDEXECOPT_DODEFAULT);
  params.push_back(variant_t());
  params.push_back(variant_t());
  variant_t ret;
  DispatchInvoke(web_browser2_, L"ExecWB", DISPATCH_METHOD, params, ret);
}

void BrowserHostEventDelegant::Show(bool is_show) {
  if (IsWindow(host_window_)) {
    if (is_show) {
      ShowWindow(host_window_, SW_SHOW);
      std::vector<variant_t> params;
      VARIANT arg;
      arg.vt = VT_BOOL;
      arg.boolVal = VARIANT_TRUE;
      params.push_back(arg);
      variant_t ret;
      DispatchInvoke(browser_container_, L"Show", DISPATCH_METHOD, params, ret);
    } else
      ShowWindow(host_window_, SW_HIDE);
  }
}

bool BrowserHostEventDelegant::ExcuteJavaScript(std::wstring code,
                                                variant_t* result) {
  if (result == NULL || browser_container_ == NULL)
    return false;

  std::vector<variant_t> params;
  params.push_back(code.c_str());
  variant_t arg;
  arg.vt = VT_VARIANT | VT_BYREF;
  arg.pvarVal = result;
  params.push_back(arg);

  variant_t ret;
  return SUCCEEDED(DispatchInvoke(browser_container_, L"ExcuteJS",
                                  DISPATCH_METHOD, params, ret));
}

void BrowserHostEventDelegant::SetBrowserRect(RECT rect) {
  if (::IsWindow(host_window_)) {
    ::SetWindowPos(host_window_, HWND_TOP, rect.left, rect.top,
                   rect.right - rect.left, rect.bottom - rect.top,
                   SWP_SHOWWINDOW);
  }

  if (browser_container_) {
    RECT rect_tmp;
    rect_tmp.left = 0;
    rect_tmp.top = 0;
    rect_tmp.right = rect.right - rect.left;
    rect_tmp.bottom = rect.bottom - rect.top;
    std::vector<variant_t> params;
    params.push_back(rect_tmp.left);
    params.push_back(rect_tmp.top);
    params.push_back(rect_tmp.right);
    params.push_back(rect_tmp.bottom);
    variant_t ret;
    DispatchInvoke(browser_container_, L"SetRect", DISPATCH_METHOD, params,
                   ret);
  }
}

IDispatch* BrowserHostEventDelegant::GetIWebBrowser2() {
  IDispatch* dispatch = NULL;
  std::vector<variant_t> params;
  variant_t param;
  param.vt = VT_DISPATCH | VT_BYREF;
  param.ppdispVal = &dispatch;
  params.push_back(param);
  variant_t ret;
  DispatchInvoke(browser_container_, L"WebBrowser", DISPATCH_PROPERTYGET,
                 params, ret);
  return dispatch;
}

void BrowserHostEventDelegant::SetHostIDispatch(IDispatch* host) {
  DISPID disp_id = 0;
  OLECHAR* member = const_cast<OLECHAR*>(L"HostApp");
  HRESULT result = browser_container_->GetIDsOfNames(
      IID_NULL, &member, 1, LOCALE_SYSTEM_DEFAULT, &disp_id);

  DISPPARAMS disp_params = {NULL, NULL, 1, 1};
  variant_t host_var = (IDispatch*)host;
  disp_params.rgvarg = &host_var;
  DISPID disp_id_put = DISPID_PROPERTYPUT;
  disp_params.rgdispidNamedArgs = &disp_id_put;
  result = browser_container_->Invoke(disp_id, IID_NULL, LOCALE_USER_DEFAULT,
                                      DISPATCH_PROPERTYPUTREF, &disp_params,
                                      NULL, NULL, NULL);
}

HWND BrowserHostEventDelegant::CreateHostWindow(HWND parent_window) {
  HINSTANCE instance = ::GetModuleHandle(NULL);
  RegisterWndClass(instance);
  HWND window_handle =
      CreateWindowW(L"YSPHostWnd", L"",
                    WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                    0, 0, 0, 0, parent_window, nullptr, instance, nullptr);

  // 为了解决设置SetWindowDisplayAffinity(WDA_MONITOR)用来屏蔽截屏后,
  // IE在发生DISPID_NEWWINDOW3时无法创建IE子窗口的问题，如果设置了WDA_MONITOR，
  // 先关闭屏蔽截屏，在IE窗口创建完成后(OnIEServerWndCreated函数中)再打开屏蔽截屏
  if (base::win::GetVersion() >= base::win::VERSION_WIN7 &&
      get_window_display_affinity_function_ &&
      set_window_display_affinity_function_) {
    BOOL ret = FALSE;
    DWORD affinity = 0;
    ret = get_window_display_affinity_function_(parent_window, &affinity);
    if (ret == TRUE && affinity == WDA_MONITOR) {
      is_disable_screen_capture_ = true;
      ret = set_window_display_affinity_function_(parent_window, WDA_NONE);
    }
  }

  SetWindowLong(window_handle, GWLP_USERDATA, (LONG)this);
  ::ShowWindow(window_handle, SW_SHOW);
  return window_handle;
}

void BrowserHostEventDelegant::RegisterWndClass(HINSTANCE instance) {
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = instance;
  wcex.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = L"YSPHostWnd";
  wcex.lpszClassName = L"YSPHostWnd";
  wcex.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));

  RegisterClassExW(&wcex);
}

void BrowserHostEventDelegant::UpdateDownloadData(const int& status,
                                                  const std::string& data) {
  if (download_data_callback_.is_null())
    return;

  download_data_callback_.Run(status, data.c_str(), data.size());
}

LRESULT BrowserHostEventDelegant::WndProc(HWND window_handle,
                                          UINT message,
                                          WPARAM w_param,
                                          LPARAM l_param) {
  switch (message) {
    case WM_PAINT: {
      PAINTSTRUCT paint;
      BeginPaint(window_handle, &paint);
      // TODO: 在此处添加使用 hdc 的任何绘图代码...
      RECT rect;
      ::GetClientRect(window_handle, &rect);
      HBRUSH brush = ::CreateSolidBrush(RGB(255, 255, 255));
      ::FillRect(paint.hdc, &rect, brush);
      ::DeleteObject(brush);
      EndPaint(window_handle, &paint);
      break;
    }
    case WM_ERASEBKGND:
      break;
    case WM_SIZE: {
      break;
    }
    case WM_DESTROY: {
      SetWindowLong(window_handle, GWLP_USERDATA, 0);
      break;
    }
    default:
      return DefWindowProc(window_handle, message, w_param, l_param);
  }
  return 0;
}

void BrowserHostEventDelegant::OnJSWindowClose() {
  if (ie_content_ && ie_content_->GetRenderViewHost())
    ie_content_->Close(ie_content_->GetRenderViewHost());
}

HRESULT BrowserHostEventDelegant::DispatchInvoke(IDispatch* dispatch,
                                                 const wchar_t* name,
                                                 int type,
                                                 std::vector<variant_t>& params,
                                                 variant_t& ret) {
  HRESULT result = E_INVALIDARG;
  if (dispatch == NULL)
    return result;

  DISPID disp_id = 0;
  OLECHAR* member = (WCHAR*)name;
  result = dispatch->GetIDsOfNames(IID_NULL, &member, 1, LOCALE_SYSTEM_DEFAULT,
                                   &disp_id);
  if (FAILED(result))
    return result;

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
  result = dispatch->Invoke(disp_id, IID_NULL, LOCALE_USER_DEFAULT, type, &args,
                            &ret, NULL, NULL);

  if (args.cArgs) {
    delete[] args.rgvarg;
  }
  return result;
}
std::wstring BrowserHostEventDelegant::GetTitle() {
  // TODO: 在此处插入 return 语句
  return page_title_;
}

bool BrowserHostEventDelegant::IsLoading() {
  return is_loading_;
}

void BrowserHostEventDelegant::SetWebContent(
    content::WebContentsIE* web_contents) {
  ie_content_ = web_contents;
}

int BrowserHostEventDelegant::GetDocMode() {
  if (browser_container_) {
    std::vector<variant_t> params;
    int mode = 0;
    variant_t param;
    param.vt = VT_I4 | VT_BYREF;
    param.pintVal = &mode;
    params.push_back(param);
    variant_t ret;
    HRESULT result = DispatchInvoke(browser_container_, L"GetDocumentMode",
                                    DISPATCH_METHOD, params, ret);
    if (SUCCEEDED(result)) {
      return mode;
    }
  }

  return 0;
}

bool BrowserHostEventDelegant::SetDocMode(DocumentMode mode) {
  if (browser_container_) {
    std::vector<variant_t> params;
    variant_t param;
    param.vt = VT_I4;
    param.intVal = (int)mode;
    params.push_back(param);
    variant_t ret;
    HRESULT result = DispatchInvoke(browser_container_, L"SetDocumentMode",
                                    DISPATCH_METHOD, params, ret);
    if (SUCCEEDED(result))
      return true;
  }

  return false;
}

bool BrowserHostEventDelegant::GetCookie(std::wstring& cookies) {
  if (browser_container_) {
    std::vector<variant_t> params;
    BSTR buffer = NULL;
    variant_t arg_cookie;
    arg_cookie.vt = VT_BSTR | VT_BYREF;
    arg_cookie.pbstrVal = &buffer;
    params.push_back(arg_cookie);
    variant_t ret;
    HRESULT result = DispatchInvoke(browser_container_, L"GetCookie",
                                    DISPATCH_METHOD, params, ret);
    if (FAILED(result))
      return false;
    cookies = buffer;
    ::SysFreeString(buffer);
  }

  return true;
}

bool BrowserHostEventDelegant::SetCookie(const std::wstring& cookies,
                                         const int flag) {
  if (browser_container_) {
    std::vector<variant_t> params;
    params.push_back(cookies.c_str());
    params.push_back(flag);
    variant_t ret;
    DispatchInvoke(browser_container_, L"SetCookie", DISPATCH_METHOD, params,
                   ret);
  }
  return true;
}

bool BrowserHostEventDelegant::SetUserAgent(const std::wstring& user_agent) {
  if (browser_container_) {
    std::vector<variant_t> params;
    params.push_back(user_agent.c_str());
    variant_t ret;
    HRESULT result = DispatchInvoke(browser_container_, L"SetUserAgent",
                                    DISPATCH_METHOD, params, ret);
    if (SUCCEEDED(result))
      return true;
  }
  return false;
}

IDispatch* BrowserHostEventDelegant::GetBrowserIDispatch() {
  return web_browser2_;
}

bool BrowserHostEventDelegant::SetParentHWND(HWND window_handle) {
  if (::IsWindow(host_window_) && ::IsWindow(window_handle)) {
    SetHostHWND(window_handle);
    SetParent(host_window_, window_handle);
    return true;
  }
  return false;
}

bool BrowserHostEventDelegant::SendFunctionControl(const std::wstring& json) {
  if (browser_container_) {
    std::vector<variant_t> params;
    params.push_back(json.c_str());
    variant_t ret;
    HRESULT result = DispatchInvoke(browser_container_, L"SetFunctionControl",
                                    DISPATCH_METHOD, params, ret);
    if (SUCCEEDED(result)) {
      function_control_json_.clear();
      return true;
    }
  } else {
    // 设置的时候有可能IE子进程还未创建完成，如果设置失败就将json串
    // 暂存在IE子进程调用CreateBrowser函数时再设置
    function_control_json_ = json;
  }

  return false;
}

void BrowserHostEventDelegant::AutoLogin(const std::wstring& frame_xpath,
                                         const std::wstring& uname_xpath,
                                         const std::wstring& uname_value,
                                         const std::wstring& password_xpath,
                                         const std::wstring& password_value,
                                         const std::wstring& button_xpath) {
  if (!browser_container_)
    return;

  std::vector<variant_t> params;
  params.push_back(frame_xpath.c_str());
  params.push_back(uname_xpath.c_str());
  params.push_back(uname_value.c_str());
  params.push_back(password_xpath.c_str());
  params.push_back(password_value.c_str());
  params.push_back(button_xpath.c_str());

  variant_t ret;
  DispatchInvoke(browser_container_, L"AutoLogin", DISPATCH_METHOD, params,
                 ret);
}

void BrowserHostEventDelegant::SetListenLoginXPath(
    const std::wstring& url,
    const std::wstring& frame_xpath,
    const std::wstring& uname_xpath,
    const std::wstring& password_xpath,
    const std::wstring& button_xpath) {
  if (!browser_container_)
    return;

  std::vector<variant_t> params;
  params.push_back(url.c_str());
  params.push_back(frame_xpath.c_str());
  params.push_back(uname_xpath.c_str());
  params.push_back(password_xpath.c_str());
  params.push_back(button_xpath.c_str());

  variant_t ret;
  DispatchInvoke(browser_container_, L"SetListenLoginXPath", DISPATCH_METHOD,
                 params, ret);
}

bool BrowserHostEventDelegant::IsDevToolsOpened() {
  if (browser_container_ == NULL)
    return false;

  VARIANT_BOOL is_open = VARIANT_FALSE;
  std::vector<variant_t> params;
  variant_t var;
  var.vt = VT_BYREF | VT_BOOL;
  var.pboolVal = &is_open;
  params.push_back(var);
  variant_t ret;
  HRESULT result = E_FAIL;
  result = DispatchInvoke(browser_container_, L"GetDevToolsOpenStatus",
                          DISPATCH_METHOD, params, ret);

  return SUCCEEDED(result) && is_open == VARIANT_TRUE;
}

void BrowserHostEventDelegant::ShowDevTools(bool show) {
  if (!browser_container_)
    return;

  VARIANT_BOOL is_show = VARIANT_FALSE;
  if (show)
    is_show = VARIANT_TRUE;
  std::vector<variant_t> params;
  params.push_back(is_show);

  variant_t ret;
  DispatchInvoke(browser_container_, L"ShowDevTools", DISPATCH_METHOD, params,
                 ret);
}

void BrowserHostEventDelegant::SetZoomPercent(int percent) {
  if (web_browser2_ == NULL)
    return;

  CComPtr<IWebBrowser2> web_browser2 = NULL;
  web_browser2_->QueryInterface(IID_IWebBrowser2, (void**)&web_browser2);
  if (web_browser2 == NULL)
    return;

  CComVariant zoom = percent;
  if (base::win::GetSystemIEVersion() >= 7)
    web_browser2->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DODEFAULT, &zoom,
                         NULL);
}

bool BrowserHostEventDelegant::IsCanZoom() {
  return base::win::GetSystemIEVersion() >= 7;
}

void BrowserHostEventDelegant::RegisterCallback(
    const DataReceivedCallback& sink_callback) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  download_data_callback_ = sink_callback;
  ie_content_->GetBrowserContext()
      ->GetDownloadManager(ie_content_->GetBrowserContext())
      ->RegisterCallbackSucceeded();
}

void BrowserHostEventDelegant::FinishDownload(bool is_cancel) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  is_download_ = false;
  if (!browser_container_)
    return;

  std::vector<variant_t> params;
  VARIANT_BOOL cancel = VARIANT_FALSE;
  if (is_cancel)
    cancel = VARIANT_TRUE;
  params.push_back(cancel);

  variant_t ret;
  DispatchInvoke(browser_container_, L"FinishDownload", DISPATCH_METHOD, params,
                 ret);
}

base::WeakPtr<IEDownloader> BrowserHostEventDelegant::GetWeakPtr() {
  return weak_factory_.GetWeakPtr();
}

bool BrowserHostEventDelegant::IsDownloading() {
  return is_download_;
}

}  // namespace ie
