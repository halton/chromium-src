// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_IE_HTTP_MONITOR_IE_H_
#define CONTENT_COMMON_IE_HTTP_MONITOR_IE_H_

// atl.h need be in front of the COM headers
#include "base/win/atl.h"

#include <comdef.h>
#include <urlmon.h>

#include "content/common/IE/IEInterface.h"

namespace ie {
class EventHandler;

class ATL_NO_VTABLE HttpMonitorIe
    : public CComObjectRootEx<CComSingleThreadModel>,
      public CComCoClass<HttpMonitorIe, &CLSID_HttpMonitor>,
      public IDispatchImpl<IHttpMonitor,
                           &IID_IHttpMonitor,
                           &LIBID_IEBrowserLib,
                           0xFFFF,
                           0xFFFF>,
      public IInternetProtocol,
      public IWinInetHttpInfo,
      public IInternetBindInfo {
 public:
  HttpMonitorIe();
  ~HttpMonitorIe();

  DECLARE_NO_REGISTRY()

  BEGIN_COM_MAP(HttpMonitorIe)
  COM_INTERFACE_ENTRY(IHttpMonitor)
  COM_INTERFACE_ENTRY(IInternetProtocol)
  COM_INTERFACE_ENTRY(IWinInetHttpInfo)
  COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY(IInternetBindInfo)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct() { return S_OK; }

  void FinalRelease() {}

  static void Init();

  // IInternetProtocolRoot
  STDMETHODIMP Start(
      /* [in] */ LPCWSTR url,
      /* [in] */ IInternetProtocolSink* protocol_sink,
      /* [in] */ IInternetBindInfo* bind_info,
      /* [in] */ DWORD flags,
      /* [in] */ HANDLE_PTR reserved) override;

  STDMETHODIMP Continue(
      /* [in] */ PROTOCOLDATA* protocol_data) override;

  STDMETHODIMP Abort(
      /* [in] */ HRESULT hrReason,
      /* [in] */ DWORD dwOptions) override;

  STDMETHODIMP Terminate(
      /* [in] */ DWORD dwOptions) override;

  STDMETHODIMP Suspend() override;

  STDMETHODIMP Resume() override;

  // IInternetProtocol
  STDMETHODIMP Read(
      /* [in, out] */ void* pv,
      /* [in] */ ULONG cb,
      /* [out] */ ULONG* pcbRead) override;

  STDMETHODIMP Seek(
      /* [in] */ LARGE_INTEGER dlibMove,
      /* [in] */ DWORD dwOrigin,
      /* [out] */ ULARGE_INTEGER* plibNewPosition) override;

  STDMETHODIMP LockRequest(
      /* [in] */ DWORD dwOptions) override;

  STDMETHODIMP UnlockRequest() override;

  // IWinInetInfo
  STDMETHODIMP QueryOption(
      /* [in] */ DWORD dwOption,
      /* [in, out] */ LPVOID pBuffer,
      /* [in, out] */ DWORD* pcbBuf) override;

  // IWinInetHttpInfo
  STDMETHODIMP QueryInfo(
      /* [in] */ DWORD dwOption,
      /* [in, out] */ LPVOID pBuffer,
      /* [in, out] */ DWORD* pcbBuf,
      /* [in, out] */ DWORD* pdwFlags,
      /* [in, out] */ DWORD* preserved) override;

  STDMETHODIMP GetBindInfo(
      /* [out] */ DWORD* grf_bindf,
      /* [unique][out][in] */ BINDINFO* bind_info) override;

  STDMETHODIMP GetBindString(
      /* [in] */ ULONG ulStringType,
      /* [annotation][out][in] */
      __RPC__out LPOLESTR* ppwzStr,
      /* [in] */ ULONG cEl,
      /* [out][in] */ ULONG* pcElFetched) override;

 private:
  IInternetProtocol* protocol_;
  IInternetProtocolSink* protocal_sink_;
  IInternetBindInfo* bind_info_;
};

OBJECT_ENTRY_AUTO(__uuidof(HttpMonitor), HttpMonitorIe)

class ProtocolSinkIe : public IServiceProvider,
                       public IInternetProtocolSink,
                       public IHttpNegotiate {
 public:
  ProtocolSinkIe(IInternetProtocolSink* sink);
  virtual ~ProtocolSinkIe();

  //------IUnknown------
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                           void** ppv_object) override;

  ULONG STDMETHODCALLTYPE AddRef() override;

  ULONG STDMETHODCALLTYPE Release() override;

  HRESULT STDMETHODCALLTYPE QueryService(
      /* [in] */ _In_ REFGUID guid_service,
      /* [in] */ _In_ REFIID riid,
      /* [out] */ _Outptr_ void __RPC_FAR* __RPC_FAR* ppv_object) override;

  // IInternetProtocolSink
  STDMETHODIMP Switch(
      /* [in] */ PROTOCOLDATA* protocol_data) override;

  STDMETHODIMP ReportProgress(
      /* [in] */ ULONG status_code,
      /* [in] */ LPCWSTR status_text) override;

  STDMETHODIMP ReportData(
      /* [in] */ DWORD bscf,
      /* [in] */ ULONG progress,
      /* [in] */ ULONG progress_max) override;

  STDMETHODIMP ReportResult(
      /* [in] */ HRESULT hrResult,
      /* [in] */ DWORD dwError,
      /* [in] */ LPCWSTR szResult) override;

  // IHttpNegotiate
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
  IInternetProtocolSink* protocal_sink_;
};

}  // namespace ie

#endif  // CONTENT_COMMON_IE_HTTP_MONITOR_IE_H_
