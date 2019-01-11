// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/IE/http_monitor_ie.h"

#include <Wininet.h>
#include <algorithm>
#include <string>

#include "base/strings/sys_string_conversions.h"
#include "content/common/IE/event_handler_ie.h"
#include "net/dns/host_resolver_impl.h"

namespace ie {

ie::HttpMonitorIe::HttpMonitorIe()
    : protocol_(NULL), protocal_sink_(NULL), bind_info_(NULL) {}

HttpMonitorIe::~HttpMonitorIe() {
  if (protocal_sink_) {
    protocal_sink_ = NULL;
  }

  if (bind_info_) {
    bind_info_ = NULL;
  }

  if (protocol_) {
    protocol_ = NULL;
  }
}

void HttpMonitorIe::Init() {
  CComPtr<IInternetSession> session = NULL;
  CoInternetGetSession(0, &session, 0);
  if (session == NULL)
    return;

  CComPtr<IClassFactory> cf = NULL;
  HRESULT handle_result = AtlComModuleGetClassObject(
      &_AtlComModule, CLSID_HttpMonitor, IID_IUnknown, (void**)&cf);

  handle_result = session->RegisterNameSpace(cf, CLSID_NULL, L"http", 0, 0, 0);
  handle_result = session->RegisterNameSpace(cf, CLSID_NULL, L"https", 0, 0, 0);
}

STDMETHODIMP HttpMonitorIe::Start(LPCWSTR url,
                                  IInternetProtocolSink* protocol_sink,
                                  IInternetBindInfo* bind_info,
                                  DWORD flags,
                                  HANDLE_PTR reserved) {
  IClassFactory* factory = NULL;
  HRESULT handle_result;

  std::string port = "";
  std::string domain = "";
  if (!net::HostResolverImpl::AbsoluteLinkUrlComparedAndRevert(
      GURL(url), domain, port))
    return INET_E_USE_DEFAULT_PROTOCOLHANDLER;

  std::wstring url_tmp = url;
  std::transform(url_tmp.begin(), url_tmp.end(), url_tmp.begin(), ::tolower);
  if (url_tmp.find(L"https://") == 0) {
    handle_result = CoGetClassObject(CLSID_HttpSProtocol, CLSCTX_ALL, 0,
                                     IID_IClassFactory, (void**)&factory);
  } else {
    if (url_tmp.find(L"http://") == 0)
      handle_result = CoGetClassObject(CLSID_HttpSProtocol, CLSCTX_ALL, 0,
                                       IID_IClassFactory, (void**)&factory);
  }

  if (factory == NULL)
    return INET_E_USE_DEFAULT_PROTOCOLHANDLER;
  handle_result = factory->CreateInstance(NULL, IID_IInternetProtocolRoot,
                                          (void**)&protocol_);
  factory->Release();
  if (protocol_ == NULL)
    return INET_E_USE_DEFAULT_PROTOCOLHANDLER;

  // 绝对链接处理
  std::string dest_url =
      "https://" + domain + ":" + port + GURL(url).PathForRequest();
  DLOG(INFO) << "dest_url: " << dest_url;
  url_tmp = base::SysNativeMBToWide(dest_url);

  protocol_->AddRef();
  bind_info_ = bind_info;
  protocal_sink_ = new ProtocolSinkIe(protocol_sink);
  // 创建一个新的IInternetProtocol 执行start方法
  // 将自己创建的IInternetProtocolSink和IInternetBindInfo类实例传入，
  // 达到能够截取http请求和应答的功能。
  handle_result =
      protocol_->Start(url_tmp.data(), protocal_sink_, this, flags, reserved);
  return handle_result;
}

STDMETHODIMP HttpMonitorIe::Continue(PROTOCOLDATA* protocol_data) {
  HRESULT handle_result = S_OK;
  if (protocol_)
    handle_result = protocol_->Continue(protocol_data);

  return handle_result;
}

STDMETHODIMP HttpMonitorIe::Abort(HRESULT handle_resultReason,
                                  DWORD dwOptions) {
  HRESULT handle_result = S_OK;
  if (protocol_)
    handle_result = protocol_->Abort(handle_resultReason, dwOptions);

  return handle_result;
}

STDMETHODIMP HttpMonitorIe::Terminate(DWORD dwOptions) {
  HRESULT handle_result = S_OK;
  if (protocol_)
    handle_result = protocol_->Terminate(dwOptions);

  return handle_result;
}

STDMETHODIMP HttpMonitorIe::Suspend() {
  HRESULT handle_result = S_OK;
  if (protocol_)
    handle_result = protocol_->Suspend();

  return handle_result;
}

STDMETHODIMP HttpMonitorIe::Resume() {
  HRESULT handle_result = S_OK;
  if (protocol_)
    handle_result = protocol_->Resume();

  return handle_result;
}

STDMETHODIMP HttpMonitorIe::Read(void* pv, ULONG cb, ULONG* pcbRead) {
  HRESULT handle_result = S_OK;
  if (protocol_)
    handle_result = protocol_->Read(pv, cb, pcbRead);

  return handle_result;
}

STDMETHODIMP HttpMonitorIe::Seek(LARGE_INTEGER dlibMove,
                                 DWORD dwOrigin,
                                 ULARGE_INTEGER* plibNewPosition) {
  HRESULT handle_result = S_OK;
  if (protocol_)
    handle_result = protocol_->Seek(dlibMove, dwOrigin, plibNewPosition);

  return handle_result;
}

STDMETHODIMP HttpMonitorIe::LockRequest(DWORD dwOptions) {
  HRESULT handle_result = S_OK;
  if (protocol_)
    handle_result = protocol_->LockRequest(dwOptions);

  return handle_result;
}

STDMETHODIMP HttpMonitorIe::UnlockRequest() {
  HRESULT handle_result = S_OK;
  if (protocol_)
    handle_result = protocol_->UnlockRequest();

  return handle_result;
}

STDMETHODIMP HttpMonitorIe::QueryOption(DWORD dwOption,
                                        LPVOID pBuffer,
                                        DWORD* pcbBuf) {
  if (protocol_ == NULL)
    return S_OK;

  IWinInetHttpInfo* http_info = NULL;
  HRESULT handle_result;
  handle_result =
      protocol_->QueryInterface(IID_IWinInetHttpInfo, (void**)&http_info);
  if (http_info == NULL)
    return S_OK;

  handle_result = http_info->QueryOption(dwOption, pBuffer, pcbBuf);
  http_info->Release();
  return handle_result;
}

STDMETHODIMP HttpMonitorIe::QueryInfo(DWORD dwOption,
                                      LPVOID pBuffer,
                                      DWORD* pcbBuf,
                                      DWORD* pdwFlags,
                                      DWORD* preserved) {
  if (protocol_ == NULL)
    return S_OK;

  IWinInetHttpInfo* http_info = NULL;
  HRESULT handle_result;
  handle_result =
      protocol_->QueryInterface(IID_IWinInetHttpInfo, (void**)&http_info);
  if (http_info == NULL)
    return S_OK;

  handle_result =
      http_info->QueryInfo(dwOption, pBuffer, pcbBuf, pdwFlags, preserved);
  http_info->Release();
  return handle_result;
}

STDMETHODIMP HttpMonitorIe::GetBindInfo(DWORD* grf_bindf, BINDINFO* bind_info) {
  HRESULT handle_result = S_OK;
  if (bind_info_)
    handle_result = bind_info_->GetBindInfo(grf_bindf, bind_info);

  return handle_result;
}

STDMETHODIMP HttpMonitorIe::GetBindString(ULONG ulStringType,
                                          LPOLESTR* ppwzStr,
                                          ULONG cEl,
                                          ULONG* pcElFetched) {
  HRESULT handle_result = S_OK;
  if (bind_info_)
    handle_result =
        bind_info_->GetBindString(ulStringType, ppwzStr, cEl, pcElFetched);

  return handle_result;
}

ProtocolSinkIe::ProtocolSinkIe(IInternetProtocolSink* sink) {
  com_ref_count_ = 0;
  protocal_sink_ = sink;
  if (protocal_sink_)
    protocal_sink_->AddRef();
}

ProtocolSinkIe::~ProtocolSinkIe() {
  if (protocal_sink_) {
    protocal_sink_->Release();
    protocal_sink_ = NULL;
  }
}

HRESULT ProtocolSinkIe::QueryInterface(REFIID riid, void** ppv_object) {
  if (riid == __uuidof(IUnknown)) {
    (*ppv_object) = this;
  } else if (riid == __uuidof(IServiceProvider)) {
    (*ppv_object) = static_cast<IServiceProvider*>(this);
  } else if (riid == __uuidof(IHttpNegotiate)) {
    (*ppv_object) = static_cast<IHttpNegotiate*>(this);
  } else if (riid == __uuidof(IInternetProtocolSink)) {
    (*ppv_object) = static_cast<IInternetProtocolSink*>(this);
  } else {
    return E_NOINTERFACE;
  }

  AddRef();
  return S_OK;
}

ULONG ProtocolSinkIe::AddRef() {
  InterlockedIncrement(&com_ref_count_);
  return com_ref_count_;
}

ULONG ProtocolSinkIe::Release() {
  InterlockedDecrement(&com_ref_count_);
  if (com_ref_count_ == 0)
    delete this;

  return com_ref_count_;
}

HRESULT ProtocolSinkIe::QueryService(REFGUID guid_service,
                                     REFIID riid,
                                     void** ppv_object) {
  if (riid == IID_IHttpNegotiate) {
    this->AddRef();
    *ppv_object = (IHttpNegotiate*)this;
    return S_OK;
  }

  return E_NOINTERFACE;
}

STDMETHODIMP ProtocolSinkIe::Switch(PROTOCOLDATA* protocol_data) {
  HRESULT handle_result = S_OK;
  if (protocal_sink_)
    handle_result = protocal_sink_->Switch(protocol_data);

  return handle_result;
}

STDMETHODIMP ProtocolSinkIe::ReportProgress(ULONG status_code,
                                            LPCWSTR status_text) {
  HRESULT handle_result = S_OK;
  if (protocal_sink_)
    handle_result = protocal_sink_->ReportProgress(status_code, status_text);

  return handle_result;
}

STDMETHODIMP ProtocolSinkIe::ReportData(DWORD bscf,
                                        ULONG progress,
                                        ULONG progress_max) {
  HRESULT handle_result = S_OK;
  if (protocal_sink_)
    handle_result = protocal_sink_->ReportData(bscf, progress, progress_max);

  return handle_result;
}

STDMETHODIMP ProtocolSinkIe::ReportResult(HRESULT handle_resultResult,
                                          DWORD dwError,
                                          LPCWSTR szResult) {
  HRESULT handle_result = S_OK;
  if (protocal_sink_)
    handle_result =
        protocal_sink_->ReportResult(handle_resultResult, dwError, szResult);
  return handle_result;
}

STDMETHODIMP ProtocolSinkIe::BeginningTransaction(LPCWSTR url,
                                                  LPCWSTR headers,
                                                  DWORD reserved,
                                                  LPWSTR* additional_headers) {
  if (protocal_sink_ == NULL)
    return E_NOTIMPL;

  IServiceProvider* provider = NULL;
  IHttpNegotiate* http_negotiate = NULL;
  HRESULT handle_result;

  handle_result =
      protocal_sink_->QueryInterface(IID_IServiceProvider, (void**)&provider);
  if (provider == NULL)
    return E_NOTIMPL;

  handle_result = provider->QueryService(IID_IHttpNegotiate, IID_IHttpNegotiate,
                                         (void**)&http_negotiate);
  if (http_negotiate == NULL)
    return E_NOINTERFACE;

  handle_result = http_negotiate->BeginningTransaction(url, headers, reserved,
                                                       additional_headers);
  http_negotiate->Release();
  provider->Release();
  return handle_result;
}

STDMETHODIMP ProtocolSinkIe::OnResponse(DWORD response_code,
                                        LPCWSTR response_headers,
                                        LPCWSTR request_headers,
                                        LPWSTR* additional_request_headers) {
  if (protocal_sink_ == NULL)
    return E_NOTIMPL;

  IServiceProvider* provider = NULL;
  IHttpNegotiate* http_negotiate = NULL;
  HRESULT handle_result;

  handle_result =
      protocal_sink_->QueryInterface(IID_IServiceProvider, (void**)&provider);
  if (provider == NULL)
    return E_NOTIMPL;

  handle_result = provider->QueryService(IID_IHttpNegotiate, IID_IHttpNegotiate,
                                         (void**)&http_negotiate);
  if (http_negotiate == NULL)
    return E_NOINTERFACE;

  handle_result =
      http_negotiate->OnResponse(response_code, response_headers,
                                 request_headers, additional_request_headers);
  http_negotiate->Release();
  provider->Release();
  return handle_result;
}

}  // namespace ie
