#ifndef CONTENT_COMMON_IE_BROWSER_HOST_EVENT_DELEGENT_H_
#define CONTENT_COMMON_IE_BROWSER_HOST_EVENT_DELEGENT_H_

#pragma once
#include "ATLInclude.h"
#include "IEInterface_h.h"
#include <vector>
#include <string>
#include <map>
#include <comdef.h>
#include "IEEvent.h"
#include "components/download/public/common/download_file_ie.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"


namespace content
{
  class WebContentsIE;
}

using IE::IEDownloader;
namespace IE {

  class ATL_NO_VTABLE BrowserHostEventDelegant :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<BrowserHostEventDelegant, &CLSID_BrowserHost>,
    public IDispatchImpl<IBrowserEventHandler, &IID_IBrowserEventHandler, &LIBID_IEBrowserLib, /*wMajor =*/ 0xFFFF, /*wMinor =*/ 0xFFFF>,
    public IEDownloader
  {
  public:
    BrowserHostEventDelegant();
    ~BrowserHostEventDelegant() override;

    DECLARE_NO_REGISTRY()

    BEGIN_COM_MAP(BrowserHostEventDelegant)
      COM_INTERFACE_ENTRY(IBrowserEventHandler)
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

    STDMETHOD(OnBeforeNavigate)(BSTR url, VARIANT_BOOL navNewUrl, VARIANT_BOOL* cancel) override;
    STDMETHOD(OnFinishNavigate)(BSTR url, BSTR faviconUrls) override;
    STDMETHOD(OnTitleChange)(BSTR title) override;
    STDMETHOD(OnLoadUrlNewWnd)(BSTR url, int flag, VARIANT_BOOL* cancel, IDispatch **pDisp) override;
    STDMETHOD(OnBrowserClosing)() override;
    STDMETHOD(OnBeforeBrowserClose)(void) override;
    STDMETHOD(OnShowContextMenu)(int dwID, int x, int y) override;
    STDMETHOD(OnDownLoadFile)(BSTR url, int status, VARIANT* variant) override;
    STDMETHOD(OnCanGoBackOrForward)(VARIANT_BOOL *back, VARIANT_BOOL *forward) override;
    STDMETHOD(OnGoBack)() override;
    STDMETHOD(OnGoForward)() override;
    STDMETHOD(OnRequestChangePos)(int left, int top, int width, int height);
    STDMETHOD(OnGetMainWndPos)(int* left, int* top, int* width, int* height);
    STDMETHOD(OnIEServerWndCreated(int hwnd));
    STDMETHOD(OnUpdateCookie)(BSTR cookie);
    STDMETHOD(OnGetLoginInfo)(BSTR url, BSTR userName, BSTR userPwd);
    STDMETHOD(OnRequestAcceleratorFromKeyboard)(int keyCode, int modifiers);
    STDMETHOD(OnRequestAcceleratorFromMouseWheel)(int delta, int Pt_x, int Pt_y, int modifiers);
    STDMETHOD(OnWinDomainAuthenticate)(BSTR* userName, BSTR* userPwd);

    void SetHostHWND(HWND hWnd);
    bool CreateBrowser(int browserEmu, bool isNew);
    void CloseBrowser();
    bool LoadUrl(std::wstring url);
    void Stop();
    void Refresh();
    void Find();
    void Print();
    void Cut();
    void Copy();
    void Paste();
    void Show(bool bShow);
    bool ExcuteJavaScript(std::wstring code, variant_t* result);
    void SetBrowserRect(RECT rt);
    static HRESULT DispatchInvoke(IDispatch * pDisp, const wchar_t * name, int type, std::vector<variant_t>& params, variant_t& ret);
    std::wstring GetTitle();
    bool IsLoading();
    void SetWebContent(content::WebContentsIE* pIE);
    int GetDocMode();
    bool SetDocMode(IEDocumentMode mode);
    bool GetCookie(std::wstring & cookies);
    bool SetCookie(const  std::wstring & cookies, const int flag);
    bool SetUserAgent(const std::wstring& uaStr);
    IDispatch* GetBrowserIDispatch();
    bool SetParentHWND(HWND hWnd);
    bool SendFunctionControl(const std::wstring& json);
    void AutoLogin(const std::wstring& frameXPath,  const std::wstring& unameXPath, const std::wstring& unameValue,
      const std::wstring& pwdXPath, const std::wstring& pwdValue, const std::wstring& btnXPath);
    void SetListenLoginXPath(const std::wstring& url, const std::wstring& frameXPath, const std::wstring& unameXPath,
      const std::wstring& pwdXPath, const std::wstring& btnXPath);
    bool IsDevToolsOpened();
    void ShowDevTools(bool show);
    void SetZoomPercent(int percent);
    bool IsCanZoom();

    //IEDownloader
     void RegisterCallback(const DataReceivedCallback& sink_callback) override;
     void FinishDownload(bool isCancel) override;
     base::WeakPtr<IEDownloader> GetWeakPtr() override;
     bool IsDownloading() override;
  private:
    IDispatch* GetIWebBrowser2();
    void SetHostIDispatch(IDispatch* pHost);

    HWND CreateHostWindow(HWND parentWnd);
    void RegisterWndClass(HINSTANCE hInstance);
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void OnJSWindowClose();
    void UpdateDownloadData(const int& status, const std::string& data);
  private:
    HWND hMainWnd;
    IDispatch* pWebBrowser2;
    HWND hHostWnd;
    HWND IEBrowserHwnd;
    bool isLoading;
    std::wstring pageTitle;
    content::WebContentsIE* pIEContent;
    IDispatch* pBrowserContainer;
    base::WeakPtrFactory<IE::IEDownloader> weakFactory;
    bool isDownload;
    std::wstring functionControlJson;
    bool isDisableScreenCapture;
    DataReceivedCallback downloadDataCallback;
    typedef BOOL(WINAPI *SetWindowDisplayAffinityPtr)(HWND, DWORD);
    typedef BOOL(WINAPI *GetWindowDisplayAffinityPtr)(HWND, DWORD*);
    GetWindowDisplayAffinityPtr pGetWindowDisplayAffinityFunction;
    SetWindowDisplayAffinityPtr pSetWindowDisplayAffinityFunction;
    base::WeakPtrFactory<BrowserHostEventDelegant> delegantWeakFactory;
    base::WeakPtrFactory<BrowserHostEventDelegant> updateDLDataWeakFactory;
  };

  OBJECT_ENTRY_AUTO(__uuidof(BrowserHost), BrowserHostEventDelegant)

}   // namespace IE

#endif
