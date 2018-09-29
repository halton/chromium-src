#ifndef CONTENT_COMMON_IE_WEB_BROWSER_H_
#define CONTENT_COMMON_IE_WEB_BROWSER_H_
#pragma once
#include <comdef.h>
#include <Exdisp.h>
#include <Mshtml.h>
#include <string>
#include <vector>
#include <map>
#include <tchar.h>
#include <Windows.h>
#include "DocHostUIHandler.h"
#include "IEEvent.h"
#include "IEVersion.h"

namespace IE {

  class IEEventHandler;
  class DevToolsHost;

  class WebBrowser :
    public IOleClientSite,
    public IOleInPlaceSite,
    public IStorage
  {

  public:

    WebBrowser(HWND hWndParent, IEEventHandler* delegate, int browserEmu, bool isNew);
    virtual ~WebBrowser();

    static WebBrowser* GetWebBrowser();


    virtual void SetRect(const RECT& _rc);

    // ----- Control methods -----

    void GoBack();

    void GoForward();

    void Refresh();

    void Navigate(std::wstring szUrl);

    IDispatch* GetIeDispatch();

    void Show();
    void Hide();

    HRESULT ExcuteJavaScript(std::wstring& funName, std::vector<variant_t>& params, variant_t* result);
    HRESULT ExcuteJavaScript(std::wstring& code, variant_t* result);
    int GetDocumentMode();
    bool SetDocumentMode(IEDocumentMode mode);
    bool SetBrowserCookie(const std::wstring& json, const int& flag);
    std::vector<std::wstring> SpliteCookieKeyVal(const std::wstring& cookie);
    HRESULT SetUserAgent(const std::wstring& ua);   //param ua like "User-Agent: MyUserAgent\r\n"
    bool EnableSubClass(HWND hWnd);
    void DisableSubClass(HWND hWnd);
    bool DownLoadFile(const std::wstring& url, const int& status, const VARIANT& var);
    void UpdateCookie(const std::wstring json);
    void FinishDownload(bool isCancel);
    void SetFunctionControl(const std::wstring& json);
    bool AutoLoginToSite(const std::wstring& frameXPath, const std::wstring& unameXPath, const std::wstring& unameValue,
      const std::wstring& pwdXPath, const std::wstring& pwdValue, const std::wstring& loginBtnXPath);
    void NotifyCommand(const std::wstring& cmd);
    void TryGetLoginInfo(CComPtr<IHTMLElement> clickElement, bool enterKeyPressed);
    void SetListenLoginXPath(const std::wstring& url, const std::wstring& frameXPath, const std::wstring & nameXpath,
      const std::wstring & pwdXpath, const std::wstring & loginBtnXpath);
    void ConfirmLoginSuccess();
    bool IsDevToolsOpened();
    void ShowIEDevTools(bool show);

    // ----- IUnknown -----

     HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
      void**ppvObject) override;

     ULONG STDMETHODCALLTYPE AddRef(void) override;

     ULONG STDMETHODCALLTYPE Release(void) override;

    // ---------- IOleWindow ----------

     HRESULT STDMETHODCALLTYPE GetWindow(
      __RPC__deref_out_opt HWND *phwnd) override;

     HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(
      BOOL fEnterMode) override;

    // ---------- IOleInPlaceSite ----------

     HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void) override;

     HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void) override;

     HRESULT STDMETHODCALLTYPE OnUIActivate(void) override;

     HRESULT STDMETHODCALLTYPE GetWindowContext(
      __RPC__deref_out_opt IOleInPlaceFrame **ppFrame,
      __RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc,
      __RPC__out LPRECT lprcPosRect,
      __RPC__out LPRECT lprcClipRect,
      __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo) override;

     HRESULT STDMETHODCALLTYPE Scroll(
      SIZE scrollExtant) override;

     HRESULT STDMETHODCALLTYPE OnUIDeactivate(
      BOOL fUndoable) override;

    virtual HWND GetControlWindow();

     HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void) override;

     HRESULT STDMETHODCALLTYPE DiscardUndoState(void) override;

     HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void) override;

     HRESULT STDMETHODCALLTYPE OnPosRectChange(
      __RPC__in LPCRECT lprcPosRect) override;

    // ---------- IOleClientSite ----------

     HRESULT STDMETHODCALLTYPE SaveObject(void) override;

     HRESULT STDMETHODCALLTYPE GetMoniker(
      DWORD dwAssign,
      DWORD dwWhichMoniker,
      __RPC__deref_out_opt IMoniker **ppmk) override;

     HRESULT STDMETHODCALLTYPE GetContainer(
      __RPC__deref_out_opt IOleContainer **ppContainer) override;

     HRESULT STDMETHODCALLTYPE ShowObject(void) override;
     HRESULT STDMETHODCALLTYPE OnShowWindow(
      BOOL fShow) override;

     HRESULT STDMETHODCALLTYPE RequestNewObjectLayout(void) override;

    // ----- IStorage -----

     HRESULT STDMETHODCALLTYPE CreateStream(
      __RPC__in_string const OLECHAR *pwcsName,
      DWORD grfMode,
      DWORD reserved1,
      DWORD reserved2,
      __RPC__deref_out_opt IStream **ppstm) override;

     HRESULT STDMETHODCALLTYPE OpenStream(
      const OLECHAR *pwcsName,
      void *reserved1,
      DWORD grfMode,
      DWORD reserved2,
      IStream **ppstm) override;

     HRESULT STDMETHODCALLTYPE CreateStorage(
      __RPC__in_string const OLECHAR *pwcsName,
      DWORD grfMode,
      DWORD reserved1,
      DWORD reserved2,
      __RPC__deref_out_opt IStorage **ppstg) override;

     HRESULT STDMETHODCALLTYPE OpenStorage(
      __RPC__in_opt_string const OLECHAR *pwcsName,
      __RPC__in_opt IStorage *pstgPriority,
      DWORD grfMode,
      __RPC__deref_opt_in_opt SNB snbExclude,
      DWORD reserved,
      __RPC__deref_out_opt IStorage **ppstg) override;

     HRESULT STDMETHODCALLTYPE CopyTo(
      DWORD ciidExclude,
      const IID *rgiidExclude,
      __RPC__in_opt  SNB snbExclude,
      IStorage *pstgDest) override;

     HRESULT STDMETHODCALLTYPE MoveElementTo(
      __RPC__in_string const OLECHAR *pwcsName,
      __RPC__in_opt IStorage *pstgDest,
      __RPC__in_string const OLECHAR *pwcsNewName,
      DWORD grfFlags) override;

     HRESULT STDMETHODCALLTYPE Commit(
      DWORD grfCommitFlags) override;

     HRESULT STDMETHODCALLTYPE Revert(void) override;

     HRESULT STDMETHODCALLTYPE EnumElements(
      DWORD reserved1,
      void *reserved2,
      DWORD reserved3,
      IEnumSTATSTG **ppenum) override;

     HRESULT STDMETHODCALLTYPE DestroyElement(
      __RPC__in_string const OLECHAR *pwcsName) override;

     HRESULT STDMETHODCALLTYPE RenameElement(
      __RPC__in_string const OLECHAR *pwcsOldName,
      __RPC__in_string const OLECHAR *pwcsNewName) override;

     HRESULT STDMETHODCALLTYPE SetElementTimes(
      __RPC__in_opt_string const OLECHAR *pwcsName,
      __RPC__in_opt const FILETIME *pctime,
      __RPC__in_opt const FILETIME *patime,
      __RPC__in_opt const FILETIME *pmtime) override;

     HRESULT STDMETHODCALLTYPE SetClass(
      __RPC__in REFCLSID clsid) override;
     HRESULT STDMETHODCALLTYPE SetStateBits(
      DWORD grfStateBits,
      DWORD grfMask) override;

     HRESULT STDMETHODCALLTYPE Stat(
      __RPC__out STATSTG *pstatstg,
      DWORD grfStatFlag) override;

  protected:
    bool CreateBrowser(bool isNew);

    RECT PixelToHiMetric(const RECT& _rc);

    bool OnBrowserWndActive();

    void MaybeCloseOnFinishDownload();

    static LRESULT CALLBACK SubClassWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    static LRESULT CALLBACK SubClassControlWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    static bool PreTranslateMsg(LPMSG msg);

    static LRESULT CALLBACK MessageProc(int code, WPARAM wParam, LPARAM lParam);

    static HWND GetMainFrameHwnd(HWND hWnd);

    void AddUAString(const std::wstring& uaStr);

    CComPtr<IHTMLDOMNode> GetRootDomNode();

    CComPtr<IHTMLDOMNode> GetSubFrameDocDomNode(CComPtr<IHTMLDOMNode> pParentDocDom, const std::wstring& frameXPath);

    CComPtr<IHTMLDOMNode> GetDomNodeFromXPath(CComPtr<IHTMLDOMNode> pDocDomNode, const std::wstring& xpath);

    void ClearListenLoginXpath();

  protected:
    
    volatile long comRefCount;  
    IWebBrowser2* pWebBrowser2;
    IEEvent* pEvHandler;
    DocHostUIHandler* pDocHostHandler;
    ServiceProvider* pServerProv;
    DevToolsHost* pDevToolsHost;
    IConnectionPoint * pCP;
    DWORD dwCookie;
    HWND hWndParent;
    HWND hWndControl;
    IOleInPlaceObject* oleInPlaceObject;
    IOleObject* oleObject;
    RECT rObject;
    IEEventHandler* pDelegate;
    HWND IEBrowserHwnd;   
    HMODULE hMod;
    HMODULE hModWininet;
    HMODULE hModUrlMon;
    typedef HRESULT(__stdcall *UrlSetSessionOptionFun)(DWORD, LPVOID, DWORD, DWORD);
    UrlSetSessionOptionFun fpUrlSetSessionOption;
    typedef HRESULT(__stdcall *UrlMkGetSessionOptionFun)(DWORD, LPVOID, DWORD, DWORD*, DWORD);
    UrlMkGetSessionOptionFun fpUrlGetSessionOption;
    bool isDownloading;
    IE::IEEmulation browserEmulation;
    std::wstring oldUaString;
    bool printEnabled;
    bool mouseRightButtonEnabled;
    bool cutCopyEnabled;
    bool saveFileEnabled;
    std::map<std::wstring, DWORD> autoLoginTimes;  


    static WNDPROC oldWndProc;
    static WNDPROC oldControlWndProc;
    static HHOOK hNextHook;
    static WebBrowser* pThis;
    static IOleInPlaceActiveObject* pIOIPAO;

    
    std::wstring uaAddString;
    
    
    
    
    std::wstring getLoginInfo_frameXpath;
    std::wstring getLoginInfo_nameXpath;
    std::wstring getLoginInfo_pwdXpath;
    std::wstring getLoginInfo_btnXpath;
    std::wstring getLoginInfo_name;
    std::wstring getLoginInfo_pwd;
    std::wstring getLoginInfo_url;
  };
} //namespace IE

 #endif