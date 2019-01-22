// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_IE_WEB_BROWSER_IE_H_
#define CONTENT_COMMON_IE_WEB_BROWSER_IE_H_

// atl.h need be in front of the COM headers
#include "base/win/atl.h"

#include <Exdisp.h>
#include <Mshtml.h>

#include <list>
#include <map>
#include <string>
#include <vector>

#include "content/common/IE/doc_host_ui_handler_ie.h"
#include "content/common/IE/event_ie.h"
#include "content/common/IE/common_ie.h"

namespace ie {

class Event;

struct PrivateDnsIp {
  PrivateDnsIp();
  ~PrivateDnsIp();

  std::wstring host;
  std::list<std::wstring> ip_list;
};

class EventHandler;
class DevToolsHost;

class WebBrowser : public IOleClientSite,
                   public IOleInPlaceSite,
                   public IStorage {
 public:
  WebBrowser(HWND parent_handle,
             EventHandler* delegate,
             int browser_emu,
             bool is_new);
  virtual ~WebBrowser();

  static WebBrowser* GetWebBrowser();

  virtual void SetRect(const RECT& rect);

  // ----- Control methods -----

  void GoBack();

  void GoForward();

  void Refresh();

  void Navigate(const std::wstring& url);

  IDispatch* GetIeDispatch();

  void Show();
  void Hide();

  HRESULT ExcuteJavaScript(std::wstring& function_name,
                           std::vector<variant_t>& params,
                           variant_t* result);
  HRESULT ExcuteJavaScript(std::wstring& code, variant_t* result);
  int GetDocumentMode();
  bool SetDocumentMode(DocumentMode mode);
  bool SetBrowserCookie(const std::wstring& json, const int flag);
  std::vector<std::wstring> SpliteCookieKeyVal(const std::wstring& cookie);
  HRESULT SetUserAgent(const std::wstring& user_agent);  // param user_agent
                                                         // like "User-Agent:
                                                         // MyUserAgent\r\n"
  bool EnableSubClass(HWND window_handle);
  void DisableSubClass(HWND window_handle);
  bool DownLoadFile(const std::wstring& url,
                    const int status,
                    const VARIANT& var);
  void UpdateCookie(const std::wstring& json);
  void FinishDownload(bool is_cancel);
  void SetFunctionControl(const std::wstring& json);
  bool AutoLoginToSite(const std::wstring& frame_xpath,
                       const std::wstring& uname_xpath,
                       const std::wstring& uname_value,
                       const std::wstring& password_xpath,
                       const std::wstring& password_value,
                       const std::wstring& login_button_xpath);
  void NotifyCommand(const std::wstring& command);
  void TryGetLoginInfo(CComPtr<IHTMLElement> click_element,
                       bool enter_key_pressed);
  void SetListenLoginXPath(const std::wstring& url,
                           const std::wstring& frame_xpath,
                           const std::wstring& name_xpath,
                           const std::wstring& password_xpath,
                           const std::wstring& login_button_xpath);
  void ConfirmLoginSuccess();
  bool IsDevToolsOpened();
  void ShowDevTools(bool show);

  // ----- IUnknown -----

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                           void** ppv_object) override;

  ULONG STDMETHODCALLTYPE AddRef() override;

  ULONG STDMETHODCALLTYPE Release() override;

  // ---------- IOleWindow ----------

  HRESULT STDMETHODCALLTYPE
  GetWindow(__RPC__deref_out_opt HWND* handle) override;

  HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL enter_mode) override;

  // ---------- IOleInPlaceSite ----------

  HRESULT STDMETHODCALLTYPE CanInPlaceActivate() override;

  HRESULT STDMETHODCALLTYPE OnInPlaceActivate() override;

  HRESULT STDMETHODCALLTYPE OnUIActivate() override;

  HRESULT STDMETHODCALLTYPE
  GetWindowContext(__RPC__deref_out_opt IOleInPlaceFrame** frame,
                   __RPC__deref_out_opt IOleInPlaceUIWindow** document,
                   __RPC__out LPRECT position_rect,
                   __RPC__out LPRECT clip_rect,
                   __RPC__inout LPOLEINPLACEFRAMEINFO frame_info) override;

  HRESULT STDMETHODCALLTYPE Scroll(SIZE scroll_extant) override;

  HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL undoable) override;

  virtual HWND GetControlWindow();

  HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate() override;

  HRESULT STDMETHODCALLTYPE DiscardUndoState() override;

  HRESULT STDMETHODCALLTYPE DeactivateAndUndo() override;

  HRESULT STDMETHODCALLTYPE
  OnPosRectChange(__RPC__in LPCRECT position_rect) override;

  // ---------- IOleClientSite ----------

  HRESULT STDMETHODCALLTYPE SaveObject() override;

  HRESULT STDMETHODCALLTYPE
  GetMoniker(DWORD assign,
             DWORD which_moniker,
             __RPC__deref_out_opt IMoniker** moniker) override;

  HRESULT STDMETHODCALLTYPE
  GetContainer(__RPC__deref_out_opt IOleContainer** container) override;

  HRESULT STDMETHODCALLTYPE ShowObject() override;
  HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL show) override;

  HRESULT STDMETHODCALLTYPE RequestNewObjectLayout() override;

  // ----- IStorage -----

  HRESULT STDMETHODCALLTYPE
  CreateStream(__RPC__in_string const OLECHAR* name,
               DWORD mode,
               DWORD reserved1,
               DWORD reserved2,
               __RPC__deref_out_opt IStream** stream) override;

  HRESULT STDMETHODCALLTYPE OpenStream(const OLECHAR* name,
                                       void* reserved1,
                                       DWORD mode,
                                       DWORD reserved2,
                                       IStream** stream) override;

  HRESULT STDMETHODCALLTYPE
  CreateStorage(__RPC__in_string const OLECHAR* name,
                DWORD mode,
                DWORD reserved1,
                DWORD reserved2,
                __RPC__deref_out_opt IStorage** storage) override;

  HRESULT STDMETHODCALLTYPE
  OpenStorage(__RPC__in_opt_string const OLECHAR* name,
              __RPC__in_opt IStorage* priority,
              DWORD mode,
              __RPC__deref_opt_in_opt SNB exclude,
              DWORD reserved,
              __RPC__deref_out_opt IStorage** storage) override;

  HRESULT STDMETHODCALLTYPE CopyTo(DWORD ciid_exclude,
                                   const IID* rgiid_exclude,
                                   __RPC__in_opt SNB exclude,
                                   IStorage* storage_dest) override;

  HRESULT STDMETHODCALLTYPE
  MoveElementTo(__RPC__in_string const OLECHAR* name,
                __RPC__in_opt IStorage* storage_dest,
                __RPC__in_string const OLECHAR* new_name,
                DWORD flags) override;

  HRESULT STDMETHODCALLTYPE Commit(DWORD commit_flags) override;

  HRESULT STDMETHODCALLTYPE Revert() override;

  HRESULT STDMETHODCALLTYPE EnumElements(DWORD reserved1,
                                         void* reserved2,
                                         DWORD reserved3,
                                         IEnumSTATSTG** statstg) override;

  HRESULT STDMETHODCALLTYPE
  DestroyElement(__RPC__in_string const OLECHAR* name) override;

  HRESULT STDMETHODCALLTYPE
  RenameElement(__RPC__in_string const OLECHAR* old_name,
                __RPC__in_string const OLECHAR* new_name) override;

  HRESULT STDMETHODCALLTYPE
  SetElementTimes(__RPC__in_opt_string const OLECHAR* name,
                  __RPC__in_opt const FILETIME* creation_time,
                  __RPC__in_opt const FILETIME* access_time,
                  __RPC__in_opt const FILETIME* modification_time) override;

  HRESULT STDMETHODCALLTYPE SetClass(__RPC__in REFCLSID class_id) override;
  HRESULT STDMETHODCALLTYPE SetStateBits(DWORD state_bits, DWORD mask) override;

  HRESULT STDMETHODCALLTYPE Stat(__RPC__out STATSTG* pstatstg,
                                 DWORD state_flag) override;

 protected:
  bool CreateBrowser(bool is_new);

  RECT PixelToHiMetric(const RECT& rect);

  bool OnBrowserWndActive();

  void MaybeCloseOnFinishDownload();

  static LRESULT CALLBACK SubClassWndProc(HWND window_handle,
                                          UINT message,
                                          WPARAM w_param,
                                          LPARAM l_param);

  static LRESULT CALLBACK SubClassControlWndProc(HWND window_handle,
                                                 UINT message,
                                                 WPARAM w_param,
                                                 LPARAM l_param);

  static bool PreTranslateMsg(LPMSG message);

  static LRESULT CALLBACK MessageProc(int code, WPARAM w_param, LPARAM l_param);

  static HWND GetMainFrameHwnd(HWND window_handle);

  static std::list<std::wstring> JsonStrToIpList(const std::wstring& json_str);

  void AddUAString(const std::wstring& user_agent);

  CComPtr<IHTMLDOMNode> GetRootDomNode();

  CComPtr<IHTMLDOMNode> GetSubFrameDocDomNode(
      CComPtr<IHTMLDOMNode> parent_document_dom,
      const std::wstring& frame_xpath);

  CComPtr<IHTMLDOMNode> GetDomNodeFromXPath(CComPtr<IHTMLDOMNode> document,
                                            const std::wstring& xpath);

  void ClearListenLoginXpath();

 private:
  volatile long com_ref_count_;
  IWebBrowser2* web_browser2_;
  Event* event_handler_;
  DocHostUIHandler* doc_host_hander_;
  ServiceProvider* server_provider_;
  DevToolsHost* dev_tools_host_;
  IConnectionPoint* connection_point_;
  DWORD cookie_;
  HWND parent_handle_;
  HWND control_handle_;
  IOleInPlaceObject* ole_in_place_object_;
  IOleObject* ole_object_;
  RECT rect_object_;
  EventHandler* delegate_;
  HWND ie_browser_handle_;
  HMODULE module_handle_;
  HMODULE win_inet_handle_;
  HMODULE url_mon_handle_;
  typedef HRESULT(__stdcall* UrlSetSessionOptionFun)(DWORD,
                                                     LPVOID,
                                                     DWORD,
                                                     DWORD);
  UrlSetSessionOptionFun fpUrlSetSessionOption;
  typedef HRESULT(
      __stdcall* UrlMkGetSessionOptionFun)(DWORD, LPVOID, DWORD, DWORD*, DWORD);
  UrlMkGetSessionOptionFun fpUrlGetSessionOption;
  bool is_downloading_;
  ie::Emulation browser_emulation_;
  std::wstring old_ua_;
  bool print_enabled_;
  bool mouse_right_button_enabled_;
  bool cut_copy_enabled_;
  bool save_file_enabled_;
  std::map<std::wstring, DWORD> auto_login_times_;

  static LONG_PTR old_window_proc_;
  static LONG_PTR old_control_proc_;
  static HHOOK next_hook_;
  static WebBrowser* self_;
  static IOleInPlaceActiveObject* ole_in_place_active_object_;

  std::wstring ua_add_string_;
  std::wstring login_info_frame_xpath_;
  std::wstring login_info_name_xpath_;
  std::wstring login_info_password_xpath_;
  std::wstring login_info_button_xpath_;
  std::wstring login_info_name_;
  std::wstring login_info_password_;
  std::wstring login_info_url_;
};

}  // namespace ie

#endif  // CONTENT_COMMON_IE_WEB_BROWSER_IE_H_
