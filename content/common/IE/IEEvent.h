#ifndef CONTENT_COMMON_IE_IE_EVENT_H_
#define CONTENT_COMMON_IE_IE_EVENT_H_
#pragma once
#include "ATLInclude.h"
#include <OAIdl.h>
#include <comutil.h>
#include <Exdisp.h>
#include <Mshtml.h>
#include <MsHtmHst.h>
#include <string>
#include <vector>
#include <list>

namespace IE {
  class IEEventHandler;
  class DocHostUIHandler;

  enum IEDocumentMode
  {
    //0x20000=IE11, 0x19000=IE10, 0x18000=IE9,  0x15000=IE8, 0x13000=IE7, 0x11000=IE5
    IE11 = 0x20000,
    IE10 = 0x19000,
    IE9 = 0x18000,
    IE8 = 0x15000,
    IE7 = 0x13000,
    IE5 = 0x11000
  };

  enum NewWindowFlag
  {
    TRANSITION = 1,
      POPUP =2,
    FROMDIALOG=4
  };

  class IEDocumentEvent;

  class IEEvent :
    public DWebBrowserEvents2
  {
  public:
    IEEvent(IEEventHandler* delegate, DocHostUIHandler* pDocHost);
    virtual ~IEEvent();

    virtual int GetDocMode();
    virtual bool SetDocMode(IEDocumentMode mode);

    // ----- IUnknown -----

     HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
      void**ppvObject) override;

     ULONG STDMETHODCALLTYPE AddRef(void) override;

     ULONG STDMETHODCALLTYPE Release(void) override;


    //-------IDispatch--------

     HRESULT STDMETHODCALLTYPE GetTypeInfoCount(
      /* [out] */ __RPC__out UINT *pctinfo)  override;

     HRESULT STDMETHODCALLTYPE GetTypeInfo(
      /* [in] */ UINT iTInfo,
      /* [in] */ LCID lcid,
      /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo) override;

     HRESULT STDMETHODCALLTYPE GetIDsOfNames(
      /* [in] */ __RPC__in REFIID riid,
      /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
      /* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
      /* [in] */ LCID lcid,
      /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) override;

     /* [local] */ HRESULT STDMETHODCALLTYPE Invoke(
      /* [annotation][in] */
      _In_  DISPID dispIdMember,
      /* [annotation][in] */
      _In_  REFIID riid,
      /* [annotation][in] */
      _In_  LCID lcid,
      /* [annotation][in] */
      _In_  WORD wFlags,
      /* [annotation][out][in] */
      _In_  DISPPARAMS *pDispParams,
      /* [annotation][out] */
      _Out_opt_  VARIANT *pVarResult,
      /* [annotation][out] */
      _Out_opt_  EXCEPINFO *pExcepInfo,
      /* [annotation][out] */
      _Out_opt_  UINT *puArgErr) override;

  public:
    void SetWebBrowser(IWebBrowser2* web);
    HRESULT ExcuteJs(std::wstring funName, std::vector<variant_t> params, variant_t* result);
    HRESULT ExcuteJs(std::wstring code, variant_t* result);
    void SetChildHWND(HWND hWnd);
    void SetParentHwnd(HWND hWnd);
    void SetIEHwnd(HWND hwnd);
    void SetClickUrl(const std::wstring url);
    void NotifyRefresh(bool isStart);
    void OnFinishLoad(const std::wstring& url);
    void OnClickHtmlElement(CComPtr<IHTMLElement> clickElement);
    void OnEnterKeyPressedHtmlElement(CComPtr<IHTMLElement> clickElement);
    void ListenDocmentEvent(IHTMLDocument2* pDoc2);
    void ResetDocHostUIHandler(); //恢复IE内核创建的IDocHostUIHandler接口
  private:
    void GetFaviconUrls(std::string pageUrl, std::vector<std::string>& faviconUrls);
  private:
    volatile long comRefCount;
    IWebBrowser2* pWebBrowser;
    CComPtr<IHTMLDocument2>  pHtmlDoc;
    CComPtr<IHTMLDocument6> pHtmlDoc6;
    CComPtr<IDispatch> pJScript;
    CComPtr<IDispatch> pHtmlWnd;
    CComPtr<IDocHostUIHandler> pOrgDocHost; //IE内核创建的IDocHostUIHandler接口
    IEEventHandler* pDelegate;
    DocHostUIHandler* pDocHostUIHandler;  //自己实现的IDocHostUIHandler接口
    // VARIANT_BOOL forward;
    // VARIANT_BOOL back;
    HWND hChildWnd;
    HWND hParentWnd;
    HWND IEHwnd;
    bool isFindIE;
    CComPtr<IHTMLDocument2> pListenClickDoc2;
    DWORD dwCookie;
    IEDocumentEvent* pDocumentEvent;
    std::wstring clickUrl;
    std::list<std::wstring> beforeNavUrlList;
    bool refreshFlg;
  };


  class IEDocumentEvent :public HTMLDocumentEvents2
  {
  public:
    IEDocumentEvent(IEEvent* pEvent);
    virtual ~IEDocumentEvent();

    void SetWebBrowser(IWebBrowser2* pWeb2);
    void SetRefresh(bool isRefresh);

    // ----- IUnknown -----

     HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
      void**ppvObject) override;

     ULONG STDMETHODCALLTYPE AddRef(void) override;

     ULONG STDMETHODCALLTYPE Release(void) override;


    //-------IDispatch--------

     HRESULT STDMETHODCALLTYPE GetTypeInfoCount(
      /* [out] */ __RPC__out UINT *pctinfo)  override;

     HRESULT STDMETHODCALLTYPE GetTypeInfo(
      /* [in] */ UINT iTInfo,
      /* [in] */ LCID lcid,
      /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo) override;

     HRESULT STDMETHODCALLTYPE GetIDsOfNames(
      /* [in] */ __RPC__in REFIID riid,
      /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
      /* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
      /* [in] */ LCID lcid,
      /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) override;

     /* [local] */ HRESULT STDMETHODCALLTYPE Invoke(
      /* [annotation][in] */
      _In_  DISPID dispIdMember,
      /* [annotation][in] */
      _In_  REFIID riid,
      /* [annotation][in] */
      _In_  LCID lcid,
      /* [annotation][in] */
      _In_  WORD wFlags,
      /* [annotation][out][in] */
      _In_  DISPPARAMS *pDispParams,
      /* [annotation][out] */
      _Out_opt_  VARIANT *pVarResult,
      /* [annotation][out] */
      _Out_opt_  EXCEPINFO *pExcepInfo,
      /* [annotation][out] */
      _Out_opt_  UINT *puArgErr) override;
  private:
    volatile long comRefCount;
    IEEvent* pIEEvent;
    IWebBrowser2* pWebBrowser2;
    bool refreshFlg;
  };
}//namespace IE

#endif
