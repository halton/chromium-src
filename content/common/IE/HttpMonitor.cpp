#include "HttpMonitor.h"
#include <Wininet.h>
#include "IEEventHandler.h"
#include <string>
#include <algorithm>

namespace IE
{
  using namespace std;

  IE::IEHttpMonitor::IEHttpMonitor()
  {
    pProtocol = NULL;
    pSink = NULL;
    pBindInfo = NULL;
  }

  IEHttpMonitor::~IEHttpMonitor()
  {
    if (pSink)
    {
      pSink = NULL;
    }
    if (pBindInfo)
    {
      pBindInfo = NULL;
    }
    if (pProtocol)
    {
      pProtocol = NULL;
    }

  }

  void IEHttpMonitor::Init()
  {
    CComPtr<IInternetSession> pSession=NULL;
    CoInternetGetSession(0, &pSession, 0);
    if (pSession == NULL)
      return;

    CComPtr<IClassFactory> cf = NULL;
    HRESULT hr = AtlComModuleGetClassObject(&_AtlComModule, CLSID_HttpMonitor, IID_IUnknown, (void**)&cf);

    hr=pSession->RegisterNameSpace(cf, CLSID_NULL, L"http", 0, 0, 0);
    hr = pSession->RegisterNameSpace(cf, CLSID_NULL, L"https", 0, 0, 0);
  }

  STDMETHODIMP IEHttpMonitor::Start(LPCWSTR szUrl, IInternetProtocolSink * pOIProtSink, IInternetBindInfo * pOIBindInfo, DWORD grfPI, HANDLE_PTR dwReserved)
  {
    return INET_E_USE_DEFAULT_PROTOCOLHANDLER;  //暂时不启用拦截过滤HTTP请求、应答的功能

    IClassFactory* pFactory = NULL;
    HRESULT hr;

    std::wstring url = szUrl;
    std::transform(url.begin(), url.end(), url.begin(), ::tolower);
    size_t pos = url.find(L"https://");
    if (pos == 0)
    {
      hr = CoGetClassObject(CLSID_HttpSProtocol, CLSCTX_ALL, 0, IID_IClassFactory, (void**)&pFactory);
    }
    else
    {
      pos = url.find(L"http://");
      if(pos==0)
        hr = CoGetClassObject(CLSID_HttpProtocol, CLSCTX_ALL, 0, IID_IClassFactory, (void**)&pFactory);
    }
    if (pFactory == NULL)
      return INET_E_USE_DEFAULT_PROTOCOLHANDLER;
    hr=pFactory->CreateInstance(NULL, IID_IInternetProtocolRoot, (void**)&pProtocol);
    pFactory->Release();
    if (pProtocol == NULL)
      return INET_E_USE_DEFAULT_PROTOCOLHANDLER;
    pProtocol->AddRef();
    pBindInfo = pOIBindInfo;
    pSink = new IEProtocolSink(pOIProtSink);
    //创建一个新的IInternetProtocol 执行start方法 将自己创建的IInternetProtocolSink和IInternetBindInfo类实例传入，达到能够截取http请求和应答的功能。
    hr= pProtocol->Start(szUrl, pSink, this, grfPI, dwReserved);
    return hr;
  }

  STDMETHODIMP IEHttpMonitor::Continue(PROTOCOLDATA * pProtocolData)
  {
    HRESULT hr = S_OK;
    if (pProtocol)
      hr = pProtocol->Continue(pProtocolData);
    return hr;
  }

  STDMETHODIMP IEHttpMonitor::Abort(HRESULT hrReason, DWORD dwOptions)
  {
    HRESULT hr = S_OK;
    if (pProtocol)
      hr = pProtocol->Abort(hrReason,dwOptions);
    return hr;
  }

  STDMETHODIMP IEHttpMonitor::Terminate(DWORD dwOptions)
  {
    HRESULT hr = S_OK;
    if (pProtocol)
      hr = pProtocol->Terminate(dwOptions);
    return hr;
  }

  STDMETHODIMP IEHttpMonitor::Suspend()
  {
    HRESULT hr = S_OK;
    if (pProtocol)
      hr = pProtocol->Suspend();
    return hr;
  }

  STDMETHODIMP IEHttpMonitor::Resume()
  {
    HRESULT hr = S_OK;
    if (pProtocol)
      hr = pProtocol->Resume();
    return hr;
  }

  STDMETHODIMP IEHttpMonitor::Read(void * pv, ULONG cb, ULONG * pcbRead)
  {
    HRESULT hr = S_OK;
    if (pProtocol)
      hr=pProtocol->Read(pv, cb, pcbRead);
    return hr;
  }

  STDMETHODIMP IEHttpMonitor::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER * plibNewPosition)
  {
    HRESULT hr = S_OK;
    if (pProtocol)
      hr = pProtocol->Seek(dlibMove, dwOrigin, plibNewPosition);
    return hr;
  }

  STDMETHODIMP IEHttpMonitor::LockRequest(DWORD dwOptions)
  {
    HRESULT hr = S_OK;
    if (pProtocol)
      hr = pProtocol->LockRequest(dwOptions);
    return hr;
  }

  STDMETHODIMP IEHttpMonitor::UnlockRequest()
  {
    HRESULT hr = S_OK;
    if (pProtocol)
      hr = pProtocol->UnlockRequest();
    return hr;
  }

  STDMETHODIMP IEHttpMonitor::QueryOption(DWORD dwOption, LPVOID pBuffer, DWORD * pcbBuf)
  {
    if (pProtocol == NULL)
      return S_OK;
    IWinInetHttpInfo* pWinHttpInfo = NULL;
    HRESULT hr;
    hr = pProtocol->QueryInterface(IID_IWinInetHttpInfo, (void**)&pWinHttpInfo);
    if (pWinHttpInfo == NULL)
      return S_OK;
    hr = pWinHttpInfo->QueryOption(dwOption, pBuffer, pcbBuf);
    pWinHttpInfo->Release();
    return hr;
  }

  STDMETHODIMP IEHttpMonitor::QueryInfo(DWORD dwOption, LPVOID pBuffer, DWORD * pcbBuf, DWORD * pdwFlags, DWORD * pdwReserved)
  {
    if(pProtocol==NULL)
      return S_OK;
    IWinInetHttpInfo* pWinHttpInfo=NULL;
    HRESULT hr;
    hr=pProtocol->QueryInterface(IID_IWinInetHttpInfo, (void**)&pWinHttpInfo);
    if (pWinHttpInfo == NULL)
      return S_OK;
    hr=pWinHttpInfo->QueryInfo(dwOption, pBuffer, pcbBuf, pdwFlags, pdwReserved);
    pWinHttpInfo->Release();
    return hr;
  }

  STDMETHODIMP IEHttpMonitor::GetBindInfo(DWORD * grfBINDF, BINDINFO * pbindinfo)
  {
    HRESULT hr = S_OK;
    if (pBindInfo)
      hr = pBindInfo->GetBindInfo(grfBINDF, pbindinfo);
    return hr;
  }

  STDMETHODIMP IEHttpMonitor::GetBindString(ULONG ulStringType, LPOLESTR * ppwzStr, ULONG cEl, ULONG * pcElFetched)
  {
    HRESULT hr=S_OK;
    if (pBindInfo)
      hr=pBindInfo->GetBindString(ulStringType, ppwzStr, cEl, pcElFetched);
    return hr;
  }

  IEProtocolSink::IEProtocolSink(IInternetProtocolSink* sink)
  {
    comRefCount = 0;
    pSink = sink;
    if(pSink)
      pSink->AddRef();
  }

  IEProtocolSink::~IEProtocolSink()
  {
    if (pSink)
    {
      pSink->Release();
      pSink = NULL;
    }
  }

  HRESULT IEProtocolSink::QueryInterface(REFIID riid, void ** ppvObject)
  {
    if (riid == __uuidof(IUnknown))
    {
      (*ppvObject) = this;
    }
    else if (riid == __uuidof(IServiceProvider))
    {
      (*ppvObject) = static_cast<IServiceProvider*>(this);
    }
    else if (riid == __uuidof(IHttpNegotiate))
    {
      (*ppvObject) = static_cast<IHttpNegotiate*>(this);
    }
    else if (riid == __uuidof(IInternetProtocolSink))
    {
      (*ppvObject) = static_cast<IInternetProtocolSink*>(this);
    }
    else
    {
      return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
  }

  ULONG IEProtocolSink::AddRef(void)
  {
    InterlockedIncrement(&comRefCount);
    return comRefCount;
  }

  ULONG IEProtocolSink::Release(void)
  {
    InterlockedDecrement(&comRefCount);
    if (comRefCount == 0)
      delete this;
    return comRefCount;
  }

  HRESULT IEProtocolSink::QueryService(REFGUID guidService, REFIID riid, void ** ppvObject)
  {
    if (riid == IID_IHttpNegotiate)
    {
      this->AddRef();
      *ppvObject = (IHttpNegotiate*)this;
      return S_OK;
    }
    return E_NOINTERFACE;
  }

  STDMETHODIMP IEProtocolSink::Switch(PROTOCOLDATA * pProtocolData)
  {
    HRESULT hr = S_OK;
    if (pSink)
      hr = pSink->Switch(pProtocolData);
    return hr;
  }

  STDMETHODIMP IEProtocolSink::ReportProgress(ULONG ulStatusCode, LPCWSTR szStatusText)
  {
    HRESULT hr = S_OK;
    if (pSink)
      hr = pSink->ReportProgress(ulStatusCode,szStatusText);
    return hr;
  }

  STDMETHODIMP IEProtocolSink::ReportData(DWORD grfBSCF, ULONG ulProgress, ULONG ulProgressMax)
  {
    HRESULT hr = S_OK;
    if (pSink)
      hr = pSink->ReportData(grfBSCF,ulProgress,ulProgressMax);
    return hr;
  }

  STDMETHODIMP IEProtocolSink::ReportResult(HRESULT hrResult, DWORD dwError, LPCWSTR szResult)
  {
    HRESULT hr = S_OK;
    if (pSink)
      hr = pSink->ReportResult(hrResult,dwError,szResult);
    return hr;
  }

  STDMETHODIMP IEProtocolSink::BeginningTransaction(LPCWSTR szURL, LPCWSTR szHeaders, DWORD dwReserved, LPWSTR * pszAdditionalHeaders)
  {
    if (pSink==NULL)
      return E_NOTIMPL;
    IServiceProvider* pProvider = NULL;
    IHttpNegotiate* pHttpNegotiate = NULL;
    HRESULT hr;

    hr=pSink->QueryInterface(IID_IServiceProvider, (void**)&pProvider);
    if (pProvider == NULL)
      return E_NOTIMPL;
    hr = pProvider->QueryService(IID_IHttpNegotiate, IID_IHttpNegotiate, (void**)&pHttpNegotiate);
    if (pHttpNegotiate == NULL)
      return E_NOINTERFACE;
    hr = pHttpNegotiate->BeginningTransaction(szURL, szHeaders, dwReserved, pszAdditionalHeaders);
    pHttpNegotiate->Release();
    pProvider->Release();
    return hr;
  }

  STDMETHODIMP IEProtocolSink::OnResponse(DWORD dwResponseCode, LPCWSTR szResponseHeaders, LPCWSTR szRequestHeaders, LPWSTR * pszAdditionalRequestHeaders)
  {
    if (pSink == NULL)
      return E_NOTIMPL;
    IServiceProvider* pProvider = NULL;
    IHttpNegotiate* pHttpNegotiate = NULL;
    HRESULT hr;

    hr = pSink->QueryInterface(IID_IServiceProvider, (void**)&pProvider);
    if (pProvider == NULL)
      return E_NOTIMPL;
    hr = pProvider->QueryService(IID_IHttpNegotiate, IID_IHttpNegotiate,(void**)&pHttpNegotiate);
    if (pHttpNegotiate == NULL)
      return E_NOINTERFACE;

    hr = pHttpNegotiate->OnResponse(dwResponseCode, szResponseHeaders, szRequestHeaders, pszAdditionalRequestHeaders);
    pHttpNegotiate->Release();
    pProvider->Release();
    return hr;
  }

} //namespace IE
