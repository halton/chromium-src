// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_IE_EVENT_IE_H_
#define CONTENT_COMMON_IE_EVENT_IE_H_

#include "content/common/IE/atl_include.h"

#include <Exdisp.h>
#include <MsHtmHst.h>
#include <Mshtml.h>
#include <OAIdl.h>
#include <comutil.h>
#include <list>
#include <string>
#include <vector>

namespace ie {

class EventHandler;
class DocHostUIHandler;

enum DocumentMode {
  // 0x20000=IE11, 0x19000=IE10, 0x18000=IE9,  0x15000=IE8, 0x13000=IE7,
  // 0x11000=IE5
  IE11 = 0x20000,
  IE10 = 0x19000,
  IE9 = 0x18000,
  IE8 = 0x15000,
  IE7 = 0x13000,
  IE5 = 0x11000
};

enum NewWindowFlag { TRANSITION = 1, POPUP = 2, FROMDIALOG = 4 };

class DocumentEventIe;

class Event : public DWebBrowserEvents2 {
 public:
  Event(EventHandler* delegate, DocHostUIHandler* doc_host);
  virtual ~Event();

  virtual int GetDocMode();
  virtual bool SetDocMode(DocumentMode mode);

  // ----- IUnknown -----

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                           void** ppv_object) override;

  ULONG STDMETHODCALLTYPE AddRef() override;

  ULONG STDMETHODCALLTYPE Release() override;

  //-------IDispatch--------

  HRESULT STDMETHODCALLTYPE GetTypeInfoCount(
      /* [out] */ __RPC__out UINT* pctinfo) override;

  HRESULT STDMETHODCALLTYPE GetTypeInfo(
      /* [in] */ UINT iTInfo,
      /* [in] */ LCID lcid,
      /* [out] */ __RPC__deref_out_opt ITypeInfo** ppTInfo) override;

  HRESULT STDMETHODCALLTYPE GetIDsOfNames(
      /* [in] */ __RPC__in REFIID riid,
      /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR* rgszNames,
      /* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
      /* [in] */ LCID lcid,
      /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID* rgDispId)
      override;

  /* [local] */ HRESULT STDMETHODCALLTYPE Invoke(
      /* [annotation][in] */
      _In_ DISPID disp_id,
      /* [annotation][in] */
      _In_ REFIID riid,
      /* [annotation][in] */
      _In_ LCID lcid,
      /* [annotation][in] */
      _In_ WORD flags,
      /* [annotation][out][in] */
      _In_ DISPPARAMS* disp_params,
      /* [annotation][out] */
      _Out_opt_ VARIANT* var_result,
      /* [annotation][out] */
      _Out_opt_ EXCEPINFO* excep_Info,
      /* [annotation][out] */
      _Out_opt_ UINT* arg_err) override;

 public:
  void SetWebBrowser(IWebBrowser2* web);
  HRESULT ExcuteJs(const std::wstring& function_name,
                   std::vector<variant_t> params,
                   variant_t* result);
  HRESULT ExcuteJs(const std::wstring& code, variant_t* result);
  void SetChildHWND(HWND window_handle);
  void SetParentHwnd(HWND window_handle);
  void SetIEHwnd(HWND hwnd);
  void SetClickUrl(const std::wstring& url);
  void NotifyRefresh(bool is_start);
  void OnFinishLoad(const std::wstring& url);
  void OnClickHtmlElement(CComPtr<IHTMLElement> click_element);
  void OnEnterKeyPressedHtmlElement(CComPtr<IHTMLElement> click_element);
  void ListenDocmentEvent(IHTMLDocument2* document2);
  void ResetDocHostUIHandler();  // 恢复IE内核创建的IDocHostUIHandler接口
 private:
  void GetFaviconUrls(const std::string& page_url,
                      std::vector<std::string>& favicon_urls);

 private:
  void SetCustomDoc(LPDISPATCH lp_disp);

 private:
  volatile long com_ref_count_;
  IWebBrowser2* web_browser_;
  CComPtr<IHTMLDocument2> html_doc2_;
  CComPtr<IHTMLDocument6> html_doc6_;
  CComPtr<IDispatch> javascript_dispath_;
  CComPtr<IDispatch> html_dispatch_;
  // IE内核创建的IDocHostUIHandler接口
  CComPtr<IDocHostUIHandler> org_doc_host_;
  EventHandler* delegate_;
  // 自己实现的IDocHostUIHandler接口
  DocHostUIHandler* doc_host_ui_handler_ie_;
  // VARIANT_BOOL forward;
  // VARIANT_BOOL back;
  HWND child_handle_;
  HWND parent_handle_;
  HWND ie_handle_;
  bool is_find_ie_;
  CComPtr<IHTMLDocument2> listen_click_doc2_;
  DWORD cookie_;
  DocumentEventIe* document_event_;
  std::wstring click_url_;
  std::list<std::wstring> before_navigate_url_list_;
  bool refresh_flag_;
};

class DocumentEventIe : public HTMLDocumentEvents2 {
 public:
  DocumentEventIe(Event* event);
  virtual ~DocumentEventIe();

  void SetWebBrowser(IWebBrowser2* web_browser2);
  void SetRefresh(bool is_refresh);

  // ----- IUnknown -----

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                           void** ppv_object) override;

  ULONG STDMETHODCALLTYPE AddRef() override;

  ULONG STDMETHODCALLTYPE Release() override;

  //-------IDispatch--------

  HRESULT STDMETHODCALLTYPE GetTypeInfoCount(
      /* [out] */ __RPC__out UINT* pctinfo) override;

  HRESULT STDMETHODCALLTYPE GetTypeInfo(
      /* [in] */ UINT t_info,
      /* [in] */ LCID lcid,
      /* [out] */ __RPC__deref_out_opt ITypeInfo** ppt_info) override;

  HRESULT STDMETHODCALLTYPE GetIDsOfNames(
      /* [in] */ __RPC__in REFIID riid,
      /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR* rgszNames,
      /* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
      /* [in] */ LCID lcid,
      /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID* rgDispId)
      override;

  /* [local] */ HRESULT STDMETHODCALLTYPE Invoke(
      /* [annotation][in] */
      _In_ DISPID disp_id,
      /* [annotation][in] */
      _In_ REFIID riid,
      /* [annotation][in] */
      _In_ LCID lcid,
      /* [annotation][in] */
      _In_ WORD flags,
      /* [annotation][out][in] */
      _In_ DISPPARAMS* disp_params,
      /* [annotation][out] */
      _Out_opt_ VARIANT* var_result,
      /* [annotation][out] */
      _Out_opt_ EXCEPINFO* excep_Info,
      /* [annotation][out] */
      _Out_opt_ UINT* arg_err) override;

 private:
  volatile long com_ref_count_;
  Event* event_ie_;
  IWebBrowser2* web_browser2_;
  bool refresh_flag_;
};

}  // namespace ie

#endif  // CONTENT_COMMON_IE_EVENT_IE_H_
