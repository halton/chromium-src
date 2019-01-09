// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_IE_BROWSER_PROCESS_H_
#define CONTENT_COMMON_IE_BROWSER_PROCESS_H_

// atl.h need be in front of the COM headers
#include "base/win/atl.h"

#include <comdef.h>

#include <string>
#include <vector>

#include "content/common/IE/IEInterface.h"
#include "content/common/IE/event_handler_ie.h"

namespace ie {

class WebBrowser;

class ATL_NO_VTABLE BrowserProcess
    : public CComObjectRootEx<CComSingleThreadModel>,
      public CComCoClass<BrowserProcess, &CLSID_BrowserContainer>,
      public IDispatchImpl<IBrowserContainer,
                           &IID_IBrowserContainer,
                           &LIBID_IEBrowserLib,
                           0xFFFF,
                           0xFFFF>,
      public EventHandler {
 public:
  BrowserProcess();
  ~BrowserProcess() override;

  DECLARE_NO_REGISTRY()

  BEGIN_COM_MAP(BrowserProcess)
  COM_INTERFACE_ENTRY(IBrowserContainer)
  COM_INTERFACE_ENTRY(IDispatch)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct() { return S_OK; }

  void FinalRelease() {}

  //---IDispatch---

  STDMETHOD(CreateBrowser)
  (long window_handle, int browser_emulation, VARIANT_BOOL is_new_window);
  STDMETHOD(get_WebBrowser)(IDispatch** pVal);
  STDMETHOD(CloseBrowser)();
  STDMETHOD(SetRect)(int left, int top, int right, int bottom);
  STDMETHOD(putref_HostApp)(IDispatch* app);
  STDMETHOD(ExcuteJS)(BSTR src, VARIANT* result);
  STDMETHOD(Show)(VARIANT_BOOL bShow);
  STDMETHOD(GetDocumentMode)(int* mode);
  STDMETHOD(SetDocumentMode)(int mode);
  STDMETHOD(SetCookie)(BSTR json, int flag);
  STDMETHOD(SetUserAgent)(BSTR user_agent);
  STDMETHOD(FinishDownload)(VARIANT_BOOL is_cancel);
  STDMETHOD(SetFunctionControl)(BSTR json_string);
  STDMETHOD(AutoLogin)
  (BSTR frame_xpath,
   BSTR username_xpath,
   BSTR username_value,
   BSTR password_xpath,
   BSTR password_value,
   BSTR login_button_xpath);
  STDMETHOD(SetListenLoginXPath)
  (BSTR url,
   BSTR frame_xpath,
   BSTR username_xpath,
   BSTR password_xpath,
   BSTR login_button_xpath);
  STDMETHOD(NotifyCommand)(BSTR command);
  STDMETHOD(GetDevToolsOpenStatus)(VARIANT_BOOL* is_open);
  STDMETHOD(ShowDevTools)(VARIANT_BOOL show);

  //----EventHandler---
  bool OnBeforeNavigate(const std::wstring& url, bool click_url) override;
  void OnFinishNavigate(const std::wstring& url,
                        const std::wstring& favicon_urls) override;
  void OnTitleChange(const std::wstring& title) override;
  bool OnLoadUrlNewWnd(const std::wstring& url,
                       long flag,
                       IDispatch** dispatch) override;
  void OnBrowserClosing() override;
  void SetIEHwnd(HWND hwnd) override;
  bool OnShowContxtMenu(int id, POINT pt) override;
  bool OnDownLoadFile(const std::wstring& url,
                      int status,
                      VARIANT variant) override;
  void GetGoBackOrForwardState(bool* go_back, bool* go_forward) override;
  void GoBack() override;
  void GoForward() override;
  void OnRequestChangePos(int left, int top, int width, int height) override;
  void OnGetMainWndPos(RECT* rect) override;
  void OnIEServerCreated(HWND hwnd) override;
  void OnUpdateCookie(const std::wstring& cookie) override;
  void OnClickElement(CComPtr<IHTMLElement> click_element,
                      bool by_enter_key) override;
  void OnGetLoginInfo(const std::wstring& url,
                      const std::wstring& username,
                      const std::wstring& password) override;
  void OnRequestAcceleratorFromKeyboard(int key_code, int modifiers) override;
  void OnRequestAcceleratorFromMouseWheel(int delta,
                                          int point_x,
                                          int point_y,
                                          int modifiers) override;
  void OnWindowDomainAuthenticate(std::wstring* username,
                                  std::wstring* password) override;
  void OnQueryPrivateDNS(const std::wstring& host,
                         std::wstring* ip_list_json_string) override;

 public:
  static bool RegisterClassObject();
  static bool UnregisterClassObject();
  HRESULT DispatchInvoke(IDispatch* dispatch,
                         const wchar_t* name,
                         int type,
                         std::vector<variant_t>& params,
                         variant_t& ret);
  void NotifyHostBrowserWillClose();

 private:
  static DWORD register_;
  WebBrowser* ie_browser_;
  IDispatch* host_app_;
  HWND ie_handle_;
  VARIANT_BOOL should_be_show_;
};

// TODO (halton): Below macro will cause duplicate symbols of
// CComObjectRootEx<CComMultiThreadModel> when component build,
// See details at http://jira.yunshipei.info/browse/RDC-6622
// Comment out below code and will check root cause in futhure.

// OBJECT_ENTRY_AUTO(__uuidof(BrowserContainer), BrowserProcess)

}  // namespace ie

#endif  // CONTENT_COMMON_IE_BROWSER_PROCESS_H_
