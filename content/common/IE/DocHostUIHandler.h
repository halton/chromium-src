#ifndef CONTENT_COMMON_IE_DOC_HOST_UI_HANDLER_H_
#define CONTENT_COMMON_IE_DOC_HOST_UI_HANDLER_H_
#pragma once
#include <MsHtmHst.h>
#include <DocObj.h>
#include <string>

#include <Servprov.h>
#include <DownloadMgr.h>
#include <MsHtmHst.h>
#include <ExDisp.h>
#include <ShObjIdl.h>
#include "ATLInclude.h"

namespace IE {

  class JavaScriptHandler;
  class IEEventHandler;
  class WebBrowser;

  class DocHostUIHandler :
    public IDocHostUIHandler,
    public IOleCommandTarget
  {
  public:
    DocHostUIHandler(IEEventHandler* handler, WebBrowser* browser);
    virtual ~DocHostUIHandler();

    //------IUnknown------
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
      void**ppvObject) override;

    ULONG STDMETHODCALLTYPE AddRef(void) override;

     ULONG STDMETHODCALLTYPE Release(void) override;

    //------IDocHostUIHandler------
     HRESULT STDMETHODCALLTYPE ShowContextMenu(
      /* [annotation][in] */
      _In_  DWORD dwID,
      /* [annotation][in] */
      _In_  POINT *ppt,
      /* [annotation][in] */
      _In_  IUnknown *pcmdtReserved,
      /* [annotation][in] */
      _In_  IDispatch *pdispReserved) override;

     HRESULT STDMETHODCALLTYPE GetHostInfo(
      /* [annotation][out][in] */
      _Inout_  DOCHOSTUIINFO *pInfo) override;

     HRESULT STDMETHODCALLTYPE ShowUI(
      /* [annotation][in] */
      _In_  DWORD dwID,
      /* [annotation][in] */
      _In_  IOleInPlaceActiveObject *pActiveObject,
      /* [annotation][in] */
      _In_  IOleCommandTarget *pCommandTarget,
      /* [annotation][in] */
      _In_  IOleInPlaceFrame *pFrame,
      /* [annotation][in] */
      _In_  IOleInPlaceUIWindow *pDoc) override;

     HRESULT STDMETHODCALLTYPE HideUI(void) override;

     HRESULT STDMETHODCALLTYPE UpdateUI(void) override;

     HRESULT STDMETHODCALLTYPE EnableModeless(
      /* [in] */ BOOL fEnable) override;

     HRESULT STDMETHODCALLTYPE OnDocWindowActivate(
      /* [in] */ BOOL fActivate) override;

     HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(
      /* [in] */ BOOL fActivate) override;

     HRESULT STDMETHODCALLTYPE ResizeBorder(
      /* [annotation][in] */
      _In_  LPCRECT prcBorder,
      /* [annotation][in] */
      _In_  IOleInPlaceUIWindow *pUIWindow,
      /* [annotation][in] */
      _In_  BOOL fRameWindow) override;

     HRESULT STDMETHODCALLTYPE TranslateAccelerator(
      /* [in] */ LPMSG lpMsg,
      /* [in] */ const GUID *pguidCmdGroup,
      /* [in] */ DWORD nCmdID) override;

     HRESULT STDMETHODCALLTYPE GetOptionKeyPath(
      /* [annotation][out] */
      _Out_  LPOLESTR *pchKey,
      /* [in] */ DWORD dw) override;

     HRESULT STDMETHODCALLTYPE GetDropTarget(
      /* [annotation][in] */
      _In_  IDropTarget *pDropTarget,
      /* [annotation][out] */
      _Outptr_  IDropTarget **ppDropTarget) override;

     HRESULT STDMETHODCALLTYPE GetExternal(
      /* [annotation][out] */
      _Outptr_result_maybenull_  IDispatch **ppDispatch) override;

     HRESULT STDMETHODCALLTYPE TranslateUrl(
      /* [in] */ DWORD dwTranslate,
      /* [annotation][in] */
      _In_  LPWSTR pchURLIn,
      /* [annotation][out] */
      _Outptr_  LPWSTR *ppchURLOut) override;

     HRESULT STDMETHODCALLTYPE FilterDataObject(
      /* [annotation][in] */
      _In_  IDataObject *pDO,
      /* [annotation][out] */
      _Outptr_result_maybenull_  IDataObject **ppDORet) override;

    // IOleCommandTarget
     /* [input_sync] */ HRESULT STDMETHODCALLTYPE QueryStatus(
      /* [unique][in] */ __RPC__in_opt const GUID *pguidCmdGroup,
      /* [in] */ ULONG cCmds,
      /* [out][in][size_is] */ __RPC__inout_ecount_full(cCmds) OLECMD prgCmds[],
      /* [unique][out][in] */ __RPC__inout_opt OLECMDTEXT *pCmdText) override;

     HRESULT STDMETHODCALLTYPE Exec(
      /* [unique][in] */ __RPC__in_opt const GUID *pguidCmdGroup,
      /* [in] */ DWORD nCmdID,
      /* [in] */ DWORD nCmdexecopt,
      /* [unique][in] */ __RPC__in_opt VARIANT *pvaIn,
      /* [unique][out][in] */ __RPC__inout_opt VARIANT *pvaOut) override;

    void EnableMouseRightButton(bool enable);
    void EnableCutCopy(bool enable);
    void EnablePrint(bool enable);
    void EnableSaveFile(bool enable);

    void SetOrgDocHost(IDocHostUIHandler* pDocHost);

  private:
    HMENU BuildMenu(DWORD dwID, IOleCommandTarget* pCmdTarget);
    void CopyToClipboard(const std::wstring& txt);
  private:
    IEEventHandler* pEventHandler;
    JavaScriptHandler* pJSHandler;
    WebBrowser* pWebBrowser;
    bool mouseRightButtonEnabled;
    bool cutCopyEnabled;
    volatile long comRefCount;
    bool printEnabled;
    bool saveFileEnabled;

    CComPtr<IDocHostUIHandler> pOrgDocHost;
  };

  class DownLoadMgr : public IBindStatusCallbackEx,
                      public IHttpNegotiate,
                      public IDownloadManager

  {
  public:
    DownLoadMgr();
    virtual ~DownLoadMgr();

    void CancelDownload();

    //------IUnknown------
     HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
      void**ppvObject) override;

     ULONG STDMETHODCALLTYPE AddRef(void) override;

     ULONG STDMETHODCALLTYPE Release(void) override;

     HRESULT STDMETHODCALLTYPE Download(
      /* [in] */ IMoniker *pmk,
      /* [in] */ IBindCtx *pbc,
      /* [in] */ DWORD dwBindVerb,
      /* [in] */ LONG grfBINDF,
      /* [in] */ BINDINFO *pBindInfo,
      /* [in] */ LPCOLESTR pszHeaders,
      /* [in] */ LPCOLESTR pszRedir,
      /* [in] */ UINT uiCP) override;

    //IBindStatusCallbackEx
    STDMETHODIMP GetBindInfoEx(
      /* [out] */ DWORD *grfBINDF,
      /* [unique][out][in] */ BINDINFO *pbindinfo,
      /* [out] */ DWORD *grfBINDF2,
      /* [out] */ DWORD *pdwReserved) override;
    //IBindStatusCallback
    STDMETHODIMP OnStartBinding(DWORD dwReserved, IBinding * pib) override;
    STDMETHODIMP GetPriority(LONG * pnPriority) override;
    STDMETHODIMP OnLowResource(DWORD reserved) override;
    STDMETHODIMP OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText) override;
    STDMETHODIMP OnStopBinding(HRESULT hresult, LPCWSTR szError) override;
    STDMETHODIMP GetBindInfo(DWORD *grfBINDF, BINDINFO * pbindinfo) override;
    STDMETHODIMP OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC* pformatetc, STGMEDIUM* pstgmed) override;
    STDMETHODIMP OnObjectAvailable(REFIID riid, IUnknown* punk) override;

    void ReadDownloadData();
    void ReleaseObject();

    // IHttpNegotiate methods
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
    IBinding* pBinding;
    std::wstring downloadUrl;
    IStream* pStream;
    bool IsDownloadCanceled;
    bool isDownloading;
    unsigned long fileSize;
    unsigned long dataReceiveSize;
    IBindStatusCallback *pPrevBSCB;
    IBindCtx* pBindCtx;
    bool isAbort;
  };

  class HTMLOMWindowServices :public IHTMLOMWindowServices
  {
  public:
    HTMLOMWindowServices(IEEventHandler* handler);
    virtual ~HTMLOMWindowServices();

    //------IUnknown------
     HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
      void**ppvObject) override;

     ULONG STDMETHODCALLTYPE AddRef(void) override;

     ULONG STDMETHODCALLTYPE Release(void) override;

    //IHTMLOMWindowServices
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
    volatile long comRefCount;
    IEEventHandler* pDelegate;
  };

  class DomainAuthenticate : public IAuthenticate
  {
  public:
    DomainAuthenticate(IEEventHandler* handler, IWebBrowser2* pWeb2);
    virtual ~DomainAuthenticate();

    //------IUnknown------
     HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
      void**ppvObject) override;

     ULONG STDMETHODCALLTYPE AddRef(void) override;

     ULONG STDMETHODCALLTYPE Release(void) override;

    //IAuthenticate
    HRESULT STDMETHODCALLTYPE Authenticate(
      /* [out] */ __RPC__deref_out_opt HWND *phwnd,
      /* [out] */ __RPC__deref_out_opt LPWSTR *pszUsername,
      /* [out] */ __RPC__deref_out_opt LPWSTR *pszPassword) override;

  private:
    volatile long comRefCount;
    IEEventHandler* pDelegate;
    IWebBrowser2* pWebBrowser;
  };

  class NewWindowManager :public INewWindowManager
  {
  public:
    NewWindowManager(IEEventHandler* pHandler);
    virtual ~NewWindowManager();

    //------IUnknown------
     HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
      void**ppvObject) override;

     ULONG STDMETHODCALLTYPE AddRef(void) override;

     ULONG STDMETHODCALLTYPE Release(void) override;

    //INewWindowManager
     HRESULT STDMETHODCALLTYPE EvaluateNewWindow(
      LPCWSTR pszUrl,
      LPCWSTR pszName,
      LPCWSTR pszUrlContext,
      LPCWSTR pszFeatures,
      BOOL fReplace,
      DWORD dwFlags,
      DWORD dwUserActionTime) override;
  private:
    volatile long comRefCount;
    IEEventHandler* pDelegate;
  };

  class ServiceProvider :public IServiceProvider
  {
  public:
    ServiceProvider(IEEventHandler* handler);
    virtual ~ServiceProvider();

    virtual void SetWebBrowser(IWebBrowser2* pWeb2);

    //------IUnknown------
     HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
      void**ppvObject) override;

     ULONG STDMETHODCALLTYPE AddRef(void) override;

     ULONG STDMETHODCALLTYPE Release(void) override;

     HRESULT STDMETHODCALLTYPE QueryService(
      /* [in] */ _In_ REFGUID guidService,
      /* [in] */ _In_ REFIID riid,
      /* [out] */ _Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject) override;

  private:
    volatile long comRefCount;
    DownLoadMgr* pDownLoadMgr;
    IEEventHandler* pDelegate;
    HTMLOMWindowServices* pHtmlOM;
    DomainAuthenticate* pAuthenticate;
    IWebBrowser2* pWebBrowser;
    NewWindowManager* pNewWndMgr;
  };


} //namespace IE

#endif
