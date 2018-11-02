// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_IE_BROWSER_HOST_EVENT_DELEGENT_IE_H_
#define CONTENT_COMMON_IE_BROWSER_HOST_EVENT_DELEGENT_IE_H_

#include <comdef.h>
#include <map>
#include <string>
#include <vector>

#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "components/download/public/common/download_file_ie.h"
#include "content/common/IE/IEInterface_h.h"
#include "content/common/IE/atl_include.h"
#include "content/common/IE/event_ie.h"

namespace content {
class WebContentsIE;
}

namespace ie {

class ATL_NO_VTABLE BrowserHostEventDelegant
    : public CComObjectRootEx<CComSingleThreadModel>,
      public CComCoClass<BrowserHostEventDelegant, &CLSID_BrowserHost>,
      public IDispatchImpl<IBrowserEventHandler,
                           &IID_IBrowserEventHandler,
                           &LIBID_IEBrowserLib,
                           /*wMajor =*/0xFFFF,
                           /*wMinor =*/0xFFFF>,
      public IEDownloader {
 public:
  BrowserHostEventDelegant();
  ~BrowserHostEventDelegant() override;

  DECLARE_NO_REGISTRY()

  BEGIN_COM_MAP(BrowserHostEventDelegant)
  COM_INTERFACE_ENTRY(IBrowserEventHandler)
  COM_INTERFACE_ENTRY(IDispatch)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct() { return S_OK; }

  void FinalRelease() {}

  STDMETHOD(OnBeforeNavigate)
  (BSTR url, VARIANT_BOOL navigate_new_url, VARIANT_BOOL* cancel) override;
  STDMETHOD(OnFinishNavigate)(BSTR url, BSTR favicon_urls) override;
  STDMETHOD(OnTitleChange)(BSTR title) override;
  STDMETHOD(OnLoadUrlNewWnd)
  (BSTR url, int flag, VARIANT_BOOL* cancel, IDispatch** dispatch) override;
  STDMETHOD(OnBrowserClosing)() override;
  STDMETHOD(OnBeforeBrowserClose)() override;
  STDMETHOD(OnShowContextMenu)(int id, int x, int y) override;
  STDMETHOD(OnDownLoadFile)(BSTR url, int status, VARIANT* variant) override;
  STDMETHOD(OnCanGoBackOrForward)
  (VARIANT_BOOL* back, VARIANT_BOOL* forward) override;
  STDMETHOD(OnGoBack)() override;
  STDMETHOD(OnGoForward)() override;
  STDMETHOD(OnRequestChangePos)(int left, int top, int width, int height);
  STDMETHOD(OnGetMainWndPos)(int* left, int* top, int* width, int* height);
  STDMETHOD(OnIEServerWndCreated)(int window_handle);
  STDMETHOD(OnUpdateCookie)(BSTR cookie);
  STDMETHOD(OnGetLoginInfo)(BSTR url, BSTR username, BSTR password);
  STDMETHOD(OnRequestAcceleratorFromKeyboard)(int key_code, int modifiers);
  STDMETHOD(OnRequestAcceleratorFromMouseWheel)
  (int delta, int ponit_x, int ponit_y, int modifiers);
  STDMETHOD(OnWinDomainAuthenticate)(BSTR* username, BSTR* password);
  STDMETHOD(OnQueryPrivateDNS)(BSTR host, BSTR* ip_list);

  void SetHostHWND(HWND window_handle);
  bool CreateBrowser(int browser_emulation, bool is_new);
  void CloseBrowser();
  bool LoadUrl(const std::wstring& url);
  void Stop();
  void Refresh();
  void Find();
  void Print();
  void Cut();
  void Copy();
  void Paste();
  void Show(bool is_show);
  bool ExcuteJavaScript(std::wstring code, variant_t* result);
  void SetBrowserRect(RECT rt);
  static HRESULT DispatchInvoke(IDispatch* dispatch,
                                const wchar_t* name,
                                int type,
                                std::vector<variant_t>& params,
                                variant_t& ret);
  std::wstring GetTitle();
  bool IsLoading();
  void SetWebContent(content::WebContentsIE* web_contents);
  int GetDocMode();
  bool SetDocMode(DocumentMode mode);
  bool GetCookie(std::wstring& cookies);
  bool SetCookie(const std::wstring& cookies, const int flag);
  bool SetUserAgent(const std::wstring& user_agent);
  IDispatch* GetBrowserIDispatch();
  bool SetParentHWND(HWND window_handle);
  bool SendFunctionControl(const std::wstring& json);
  void AutoLogin(const std::wstring& frame_xpath,
                 const std::wstring& uname_xpath,
                 const std::wstring& uname_value,
                 const std::wstring& password_xpath,
                 const std::wstring& password_value,
                 const std::wstring& button_xpath);
  void SetListenLoginXPath(const std::wstring& url,
                           const std::wstring& frame_xpath,
                           const std::wstring& uname_xpath,
                           const std::wstring& password_xpath,
                           const std::wstring& button_xpath);
  bool IsDevToolsOpened();
  void ShowDevTools(bool show);
  void SetZoomPercent(int percent);
  bool IsCanZoom();

  // IEDownloader
  void RegisterCallback(const DataReceivedCallback& sink_callback) override;
  void FinishDownload(bool is_cancel) override;
  base::WeakPtr<IEDownloader> GetWeakPtr() override;
  bool IsDownloading() override;

 private:
  IDispatch* GetIWebBrowser2();
  void SetHostIDispatch(IDispatch* host);

  HWND CreateHostWindow(HWND parent_window);
  void RegisterWndClass(HINSTANCE instance);
  static LRESULT CALLBACK WndProc(HWND window_handle,
                                  UINT message,
                                  WPARAM w_param,
                                  LPARAM l_param);
  void OnJSWindowClose();
  void UpdateDownloadData(const int& status, const std::string& data);

 private:
  HWND main_window_;
  IDispatch* web_browser2_;
  HWND host_window_;
  HWND ie_browser_handle_;
  bool is_loading_;
  std::wstring page_title_;
  content::WebContentsIE* ie_content_;
  IDispatch* browser_container_;
  base::WeakPtrFactory<IEDownloader> weak_factory_;
  bool is_download_;
  std::wstring function_control_json_;
  bool is_disable_screen_capture_;
  DataReceivedCallback download_data_callback_;
  typedef BOOL(WINAPI* SetWindowDisplayAffinityPtr)(HWND, DWORD);
  typedef BOOL(WINAPI* GetWindowDisplayAffinityPtr)(HWND, DWORD*);
  GetWindowDisplayAffinityPtr get_window_display_affinity_function_;
  SetWindowDisplayAffinityPtr set_window_display_affinity_function_;
  base::WeakPtrFactory<BrowserHostEventDelegant> delegant_weak_factory_;
  base::WeakPtrFactory<BrowserHostEventDelegant> update_dl_data_weak_factory_;
};

OBJECT_ENTRY_AUTO(__uuidof(BrowserHost), BrowserHostEventDelegant)

}  // namespace ie

#endif  // CONTENT_COMMON_IE_BROWSER_HOST_EVENT_DELEGENT_IE_H_
