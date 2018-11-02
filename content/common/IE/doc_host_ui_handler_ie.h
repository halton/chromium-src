// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_IE_DOC_HOST_UI_HANDLER_IE_H_
#define CONTENT_COMMON_IE_DOC_HOST_UI_HANDLER_IE_H_

#include <DocObj.h>
#include <DownloadMgr.h>
#include <ExDisp.h>
#include <MsHtmHst.h>
#include <Servprov.h>
#include <ShObjIdl.h>
#include <string>

#include "content/common/IE/atl_include.h"

namespace ie {

class JavaScriptHandler;
class EventHandler;
class WebBrowser;

class DocHostUIHandler : public IDocHostUIHandler, public IOleCommandTarget {
 public:
  DocHostUIHandler(EventHandler* handler, WebBrowser* browser);
  virtual ~DocHostUIHandler();

  //------IUnknown------
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                           void** ppv_object) override;

  ULONG STDMETHODCALLTYPE AddRef() override;

  ULONG STDMETHODCALLTYPE Release() override;

  //------IDocHostUIHandler------
  HRESULT STDMETHODCALLTYPE ShowContextMenu(
      /* [annotation][in] */
      _In_ DWORD id,
      /* [annotation][in] */
      _In_ POINT* ppt,
      /* [annotation][in] */
      _In_ IUnknown* cmd_reserved,
      /* [annotation][in] */
      _In_ IDispatch* dispatch_reserved) override;

  HRESULT STDMETHODCALLTYPE GetHostInfo(
      /* [annotation][out][in] */
      _Inout_ DOCHOSTUIINFO* doc_host_ui_info) override;

  HRESULT STDMETHODCALLTYPE ShowUI(
      /* [annotation][in] */
      _In_ DWORD id,
      /* [annotation][in] */
      _In_ IOleInPlaceActiveObject* active_object,
      /* [annotation][in] */
      _In_ IOleCommandTarget* command_target,
      /* [annotation][in] */
      _In_ IOleInPlaceFrame* ole_in_place_frame,
      /* [annotation][in] */
      _In_ IOleInPlaceUIWindow* ole_in_place_ui_window) override;

  HRESULT STDMETHODCALLTYPE HideUI() override;

  HRESULT STDMETHODCALLTYPE UpdateUI() override;

  HRESULT STDMETHODCALLTYPE EnableModeless(
      /* [in] */ BOOL is_enable) override;

  HRESULT STDMETHODCALLTYPE OnDocWindowActivate(
      /* [in] */ BOOL is_activate) override;

  HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(
      /* [in] */ BOOL is_activate) override;

  HRESULT STDMETHODCALLTYPE ResizeBorder(
      /* [annotation][in] */
      _In_ LPCRECT prc_border,
      /* [annotation][in] */
      _In_ IOleInPlaceUIWindow* ui_window,
      /* [annotation][in] */
      _In_ BOOL rame_window) override;

  HRESULT STDMETHODCALLTYPE TranslateAccelerator(
      /* [in] */ LPMSG message,
      /* [in] */ const GUID* guid_cmd_group,
      /* [in] */ DWORD cmd_id) override;

  HRESULT STDMETHODCALLTYPE GetOptionKeyPath(
      /* [annotation][out] */
      _Out_ LPOLESTR* key,
      /* [in] */ DWORD dw) override;

  HRESULT STDMETHODCALLTYPE GetDropTarget(
      /* [annotation][in] */
      _In_ IDropTarget* drop_target_in,
      /* [annotation][out] */
      _Outptr_ IDropTarget** drop_target_out) override;

  HRESULT STDMETHODCALLTYPE GetExternal(
      /* [annotation][out] */
      _Outptr_result_maybenull_ IDispatch** dispatch_out) override;

  HRESULT STDMETHODCALLTYPE TranslateUrl(
      /* [in] */ DWORD translate,
      /* [annotation][in] */
      _In_ LPWSTR url_in,
      /* [annotation][out] */
      _Outptr_ LPWSTR* url_out) override;

  HRESULT STDMETHODCALLTYPE FilterDataObject(
      /* [annotation][in] */
      _In_ IDataObject* data_object_in,
      /* [annotation][out] */
      _Outptr_result_maybenull_ IDataObject** data_object_out) override;

  // IOleCommandTarget
  /* [input_sync] */ HRESULT STDMETHODCALLTYPE QueryStatus(
      /* [unique][in] */ __RPC__in_opt const GUID* guid_cmd_group,
      /* [in] */ ULONG cCmds,
      /* [out][in][size_is] */
      __RPC__inout_ecount_full(cCmds) OLECMD prg_cmds[],
      /* [unique][out][in] */ __RPC__inout_opt OLECMDTEXT* pCmdText) override;

  HRESULT STDMETHODCALLTYPE Exec(
      /* [unique][in] */ __RPC__in_opt const GUID* guid_cmd_group,
      /* [in] */ DWORD cmd_id,
      /* [in] */ DWORD cmd_exec_opt,
      /* [unique][in] */ __RPC__in_opt VARIANT* va_in,
      /* [unique][out][in] */ __RPC__inout_opt VARIANT* va_out) override;

  void EnableMouseRightButton(bool enable);
  void EnableCutCopy(bool enable);
  void EnablePrint(bool enable);
  void EnableSaveFile(bool enable);

  void SetOrgDocHost(IDocHostUIHandler* doc_host);

 private:
  HMENU BuildMenu(DWORD id, IOleCommandTarget* command_target);
  void CopyToClipboard(const std::wstring& content);

 private:
  EventHandler* event_handler_;
  JavaScriptHandler* javascript_handler_;
  WebBrowser* web_browser2_;
  bool mouse_right_button_enabled_;
  bool cut_copy_enabled_;
  volatile long com_ref_count_;
  bool print_enabled_;
  bool save_file_enabled_;

  CComPtr<IDocHostUIHandler> org_doc_host_;
};

class DownLoadManager : public IBindStatusCallbackEx,
                        public IHttpNegotiate,
                        public IDownloadManager {
 public:
  DownLoadManager();
  virtual ~DownLoadManager();

  void CancelDownload();

  //------IUnknown------
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                           void** ppv_object) override;

  ULONG STDMETHODCALLTYPE AddRef() override;

  ULONG STDMETHODCALLTYPE Release() override;

  HRESULT STDMETHODCALLTYPE Download(
      /* [in] */ IMoniker* pmk,
      /* [in] */ IBindCtx* pbc,
      /* [in] */ DWORD bind_verb,
      /* [in] */ LONG grf_bindf,
      /* [in] */ BINDINFO* bind_info,
      /* [in] */ LPCOLESTR headers,
      /* [in] */ LPCOLESTR redir,
      /* [in] */ UINT ui_cp) override;

  // IBindStatusCallbackEx
  STDMETHODIMP GetBindInfoEx(
      /* [out] */ DWORD* grf_bindf,
      /* [unique][out][in] */ BINDINFO* bind_info,
      /* [out] */ DWORD* grf_bindf2,
      /* [out] */ DWORD* pdw_reserved) override;
  // IBindStatusCallback
  STDMETHODIMP OnStartBinding(DWORD reserved, IBinding* binding) override;
  STDMETHODIMP GetPriority(LONG* priority) override;
  STDMETHODIMP OnLowResource(DWORD reserved) override;
  STDMETHODIMP OnProgress(ULONG progress,
                          ULONG progress_max,
                          ULONG status_code,
                          LPCWSTR status_text) override;
  STDMETHODIMP OnStopBinding(HRESULT result, LPCWSTR error) override;
  STDMETHODIMP GetBindInfo(DWORD* grf_bindf, BINDINFO* bind_info) override;
  STDMETHODIMP OnDataAvailable(DWORD bscf,
                               DWORD size,
                               FORMATETC* format_etc,
                               STGMEDIUM* stgmedium) override;
  STDMETHODIMP OnObjectAvailable(REFIID riid, IUnknown* punk) override;

  void ReadDownloadData();
  void ReleaseObject();

  // IHttpNegotiate methods
  STDMETHODIMP BeginningTransaction(LPCWSTR url,
                                    LPCWSTR headers,
                                    DWORD reserved,
                                    LPWSTR* additional_headers) override;

  STDMETHODIMP OnResponse(DWORD response_code,
                          LPCWSTR response_headers,
                          LPCWSTR request_headers,
                          LPWSTR* additional_request_headers) override;

 private:
  volatile long com_ref_count_;
  IBinding* binding_;
  std::wstring download_url;
  IStream* stream_;
  bool is_download_canceled_;
  bool is_downloading_;
  IBindStatusCallback* previsous_bind_status_callback_;
  IBindCtx* bind_context_;
  bool is_abort_;
};

class HTMLOMWindowServices : public IHTMLOMWindowServices {
 public:
  HTMLOMWindowServices(EventHandler* handler);
  virtual ~HTMLOMWindowServices();

  //------IUnknown------
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                           void** ppv_object) override;

  ULONG STDMETHODCALLTYPE AddRef() override;

  ULONG STDMETHODCALLTYPE Release() override;

  // IHTMLOMWindowServices
  HRESULT STDMETHODCALLTYPE moveTo(
      /* [in] */ LONG x,
      /* [in] */ LONG y) override;

  HRESULT STDMETHODCALLTYPE moveBy(
      /* [in] */ LONG x,
      /* [in] */ LONG y) override;

  HRESULT STDMETHODCALLTYPE resizeTo(
      /* [in] */ LONG x,
      /* [in] */ LONG y) override;

  HRESULT STDMETHODCALLTYPE resizeBy(
      /* [in] */ LONG x,
      /* [in] */ LONG y) override;

 private:
  volatile long com_ref_count_;
  EventHandler* delegate_;
};

class DomainAuthenticate : public IAuthenticate {
 public:
  DomainAuthenticate(EventHandler* handler, IWebBrowser2* web_browser2);
  virtual ~DomainAuthenticate();

  //------IUnknown------
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                           void** ppv_object) override;

  ULONG STDMETHODCALLTYPE AddRef() override;

  ULONG STDMETHODCALLTYPE Release() override;

  // IAuthenticate
  HRESULT STDMETHODCALLTYPE Authenticate(
      /* [out] */ __RPC__deref_out_opt HWND* handle,
      /* [out] */ __RPC__deref_out_opt LPWSTR* username,
      /* [out] */ __RPC__deref_out_opt LPWSTR* password) override;

 private:
  volatile long com_ref_count_;
  EventHandler* delegate_;
  IWebBrowser2* web_browser2_;
};

class NewWindowManager : public INewWindowManager {
 public:
  NewWindowManager(EventHandler* event_handler);
  virtual ~NewWindowManager();

  //------IUnknown------
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                           void** ppv_object) override;

  ULONG STDMETHODCALLTYPE AddRef() override;

  ULONG STDMETHODCALLTYPE Release() override;

  // INewWindowManager
  HRESULT STDMETHODCALLTYPE EvaluateNewWindow(LPCWSTR url,
                                              LPCWSTR name,
                                              LPCWSTR url_context,
                                              LPCWSTR features,
                                              BOOL is_eplace,
                                              DWORD flags,
                                              DWORD user_action_time) override;

 private:
  volatile long com_ref_count_;
};

class ServiceProvider : public IServiceProvider {
 public:
  ServiceProvider(EventHandler* handler);
  virtual ~ServiceProvider();

  virtual void SetWebBrowser(IWebBrowser2* web_browser2);

  //------IUnknown------
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                           void** ppv_object) override;

  ULONG STDMETHODCALLTYPE AddRef() override;

  ULONG STDMETHODCALLTYPE Release() override;

  HRESULT STDMETHODCALLTYPE QueryService(
      /* [in] */ _In_ REFGUID guid_service,
      /* [in] */ _In_ REFIID riid,
      /* [out] */ _Outptr_ void __RPC_FAR* __RPC_FAR* ppv_object) override;

 private:
  volatile long com_ref_count_;
  DownLoadManager* download_manager_;
  EventHandler* delegate_;
  HTMLOMWindowServices* html_window_service_;
  DomainAuthenticate* domain_authenticate_;
  IWebBrowser2* web_browser2_;
  NewWindowManager* new_window_manager_;
};

}  // namespace ie

#endif  // CONTENT_COMMON_IE_DOC_HOST_UI_HANDLER_IE_H_
