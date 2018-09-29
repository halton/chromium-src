#ifndef CONTENT_COMMON_IE_HTTP_MONITOR_H_
#define CONTENT_COMMON_IE_HTTP_MONITOR_H_
#pragma once
#include "ATLInclude.h"
#include "IEInterface_h.h"
#include <urlmon.h>
#include <comdef.h>

namespace IE
{
  class IEEventHandler;

  class ATL_NO_VTABLE IEHttpMonitor :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<IEHttpMonitor, &CLSID_HttpMonitor>,
    public IDispatchImpl<IHttpMonitor, &IID_IHttpMonitor, &LIBID_IEBrowserLib, /*wMajor =*/ 0xFFFF, /*wMinor =*/ 0xFFFF>,
    public IInternetProtocol,
    public IWinInetHttpInfo,
    public IInternetBindInfo
  {
  public:
    IEHttpMonitor();
    ~IEHttpMonitor();

    DECLARE_NO_REGISTRY()

    BEGIN_COM_MAP(IEHttpMonitor)
      COM_INTERFACE_ENTRY(IHttpMonitor)
      COM_INTERFACE_ENTRY(IInternetProtocol)
      COM_INTERFACE_ENTRY(IWinInetHttpInfo)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(IInternetBindInfo)
    END_COM_MAP()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
      return S_OK;
    }

    void FinalRelease()
    {
    }

    static void Init();

    // IInternetProtocolRoot
    STDMETHODIMP Start(
      /* [in] */ LPCWSTR szUrl,
      /* [in] */ IInternetProtocolSink *pOIProtSink,
      /* [in] */ IInternetBindInfo *pOIBindInfo,
      /* [in] */ DWORD grfPI,
      /* [in] */ HANDLE_PTR dwReserved) override;

    STDMETHODIMP Continue(
      /* [in] */ PROTOCOLDATA *pProtocolData) override;

    STDMETHODIMP Abort(
      /* [in] */ HRESULT hrReason,
      /* [in] */ DWORD dwOptions) override;

    STDMETHODIMP Terminate(
      /* [in] */ DWORD dwOptions) override;

    STDMETHODIMP Suspend() override;

    STDMETHODIMP Resume() override;

    // IInternetProtocol
    STDMETHODIMP Read(
      /* [in, out] */ void *pv,
      /* [in] */ ULONG cb,
      /* [out] */ ULONG *pcbRead) override;

    STDMETHODIMP Seek(
      /* [in] */ LARGE_INTEGER dlibMove,
      /* [in] */ DWORD dwOrigin,
      /* [out] */ ULARGE_INTEGER *plibNewPosition) override;

    STDMETHODIMP LockRequest(
      /* [in] */ DWORD dwOptions) override;

    STDMETHODIMP UnlockRequest() override;

    // IWinInetInfo
    STDMETHODIMP QueryOption(
      /* [in] */ DWORD dwOption,
      /* [in, out] */ LPVOID pBuffer,
      /* [in, out] */ DWORD *pcbBuf) override;

    // IWinInetHttpInfo
    STDMETHODIMP QueryInfo(
      /* [in] */ DWORD dwOption,
      /* [in, out] */ LPVOID pBuffer,
      /* [in, out] */ DWORD *pcbBuf,
      /* [in, out] */ DWORD *pdwFlags,
      /* [in, out] */ DWORD *pdwReserved) override;

    STDMETHODIMP GetBindInfo(
      /* [out] */ DWORD *grfBINDF,
      /* [unique][out][in] */ BINDINFO *pbindinfo) override;

    STDMETHODIMP GetBindString(
      /* [in] */ ULONG ulStringType,
      /* [annotation][out][in] */
      __RPC__out  LPOLESTR *ppwzStr,
      /* [in] */ ULONG cEl,
      /* [out][in] */ ULONG *pcElFetched) override;

    private:
      IInternetProtocol* pProtocol;
      IInternetProtocolSink* pSink;
      IInternetBindInfo* pBindInfo;
  };
  OBJECT_ENTRY_AUTO(__uuidof(HttpMonitor), IEHttpMonitor)


  class IEProtocolSink:
    public IServiceProvider,
    public IInternetProtocolSink,
    public IHttpNegotiate
  {
  public:
    IEProtocolSink(IInternetProtocolSink* sink);
    virtual ~IEProtocolSink();

    //------IUnknown------
     HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
      void**ppvObject) override;

     ULONG STDMETHODCALLTYPE AddRef(void) override;

     ULONG STDMETHODCALLTYPE Release(void) override;

     HRESULT STDMETHODCALLTYPE QueryService(
      /* [in] */ _In_ REFGUID guidService,
      /* [in] */ _In_ REFIID riid,
      /* [out] */ _Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject) override;

    //IInternetProtocolSink
    STDMETHODIMP Switch(
      /* [in] */ PROTOCOLDATA *pProtocolData) override;

    STDMETHODIMP ReportProgress(
      /* [in] */ ULONG ulStatusCode,
      /* [in] */ LPCWSTR szStatusText) override;

    STDMETHODIMP ReportData(
      /* [in] */ DWORD grfBSCF,
      /* [in] */ ULONG ulProgress,
      /* [in] */ ULONG ulProgressMax) override;

    STDMETHODIMP ReportResult(
      /* [in] */ HRESULT hrResult,
      /* [in] */ DWORD dwError,
      /* [in] */ LPCWSTR szResult) override;

    //IHttpNegotiate
    STDMETHODIMP BeginningTransaction(
      LPCWSTR szURL,
      LPCWSTR szHeaders,
      DWORD dwReserved,
      LPWSTR *pszAdditionalHeaders) override;

    STDMETHODIMP OnResponse(
      DWORD dwResponseCode,
      LPCWSTR szResponseHeaders,
      LPCWSTR szRequestHeaders,
      LPWSTR *pszAdditionalRequestHeaders) override;
  private:
    volatile long comRefCount;
    IInternetProtocolSink* pSink;
  };

} //namespace IE
#endif