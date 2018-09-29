#ifndef CONTENT_COMMON_IE_BROWSER_PROCESS_H_
#define CONTENT_COMMON_IE_BROWSER_PROCESS_H_
#pragma once
#include "ATLInclude.h"
#include "IEInterface_h.h"
#include "IEEventHandler.h"
#include <comdef.h>
#include <vector>
#include <string>

namespace IE {

  class WebBrowser;

  class ATL_NO_VTABLE BrowserProcess :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<BrowserProcess, &CLSID_BrowserContainer>,
    public IDispatchImpl<IBrowserContainer, &IID_IBrowserContainer, &LIBID_IEBrowserLib, /*wMajor =*/ 0xFFFF, /*wMinor =*/ 0xFFFF>,
    public IEEventHandler
  {
  public:
    BrowserProcess();
    ~BrowserProcess() override;

    DECLARE_NO_REGISTRY()

    BEGIN_COM_MAP(BrowserProcess)
      COM_INTERFACE_ENTRY(IBrowserContainer)
      COM_INTERFACE_ENTRY(IDispatch)
    END_COM_MAP()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
      return S_OK;
    }

    void FinalRelease()
    {
    }


    //---IDispatch---

    STDMETHOD(CreateBrowser)(/*[in]*/ long hWnd, int browserEmulation, VARIANT_BOOL isNewWindow);
    STDMETHOD(get_WebBrowser)(IDispatch** pVal);
    STDMETHOD(CloseBrowser)(void);
    STDMETHOD(SetRect)(int left, int top, int right, int bottom);
    STDMETHOD(putref_HostApp)(IDispatch* app);
    STDMETHOD(ExcuteJS)(BSTR src, VARIANT *result);
    STDMETHOD(Show)(VARIANT_BOOL bShow);
    STDMETHOD(GetDocumentMode)(int* mode);
    STDMETHOD(SetDocumentMode)(int mode);
    STDMETHOD(SetCookie)(BSTR json, int flag);
    STDMETHOD(SetUserAgent)(BSTR uaStr);
    STDMETHOD(FinishDownload)(VARIANT_BOOL isCancel);
    STDMETHOD(SetFunctionControl)(BSTR jsonStr);
    STDMETHOD(AutoLogin)(BSTR frameXPath, BSTR userNameXPath, BSTR userNameValue, BSTR pwdXPath, BSTR pwdValue, BSTR loginBtnXPath);
    STDMETHOD(SetListenLoginXPath)(BSTR url, BSTR frameXPath, BSTR userNameXPath, BSTR pwdXPath, BSTR loginbtnXPath);
    STDMETHOD(NotifyCommand)(BSTR command);
    STDMETHOD(GetDevToolsOpenStatus)(VARIANT_BOOL *isOpen);
    STDMETHOD(ShowDevTools)(VARIANT_BOOL show);

    //----IEEventHandler---
    bool OnBeforeNavigate(std::wstring url, bool clickUrl) override;
    void OnFinishNavigate(std::wstring url, std::wstring faviconUrls) override;
    void OnTitleChange(std::wstring title) override;
    bool OnLoadUrlNewWnd(std::wstring url, long flag, IDispatch** pDisp) override;
    void OnBrowserClosing() override;
    void SetIEHwnd(HWND hwnd) override;
    bool OnShowContxtMenu(int id, POINT pt) override;
    bool OnDownLoadFile(const std::wstring& url, int status, VARIANT variant) override;
    void GetGoBackOrForwardState(bool* goBack, bool* goForward) override;
    void GoBack() override;
    void GoForward() override;
    void OnRequestChangePos(int left, int top, int width, int height) override;
    void OnGetMainWndPos(RECT* rect) override;
    void OnIEServerCreated(HWND hwnd) override;
    void OnUpdateCookie(const std::wstring& cookie) override;
    void OnClickElement(CComPtr<IHTMLElement> clickElement, bool byEnterKey) override;
    void OnGetLoginInfo(std::wstring url, std::wstring userName, std::wstring userPwd) override;
    void OnRequestAcceleratorFromKeyboard(int keyCode, int modifiers) override;
    void OnRequestAcceleratorFromMouseWheel(int delta, int Pt_x, int Pt_y, int modifiers) override;
    void OnWindowDomainAuthenticate(std::wstring* userName, std::wstring* userPwd) override;

  public:
    static bool RegisterClassObject();
    static bool UnregisterClassObject();
    HRESULT DispatchInvoke(IDispatch * pDisp, const wchar_t * name, int type, std::vector<variant_t>& params, variant_t& ret);
    void NotifyHostBrowserWillClose();
  private:
    static DWORD dwRegister;
    WebBrowser* pIEBrowser;
    IDispatch* pHostApp;
    HWND IEHwnd;
    VARIANT_BOOL shouldBeShow;
  };
  OBJECT_ENTRY_AUTO(__uuidof(BrowserContainer), BrowserProcess)

} //namespace IE
#endif