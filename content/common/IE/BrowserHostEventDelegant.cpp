#include "ATLInclude.h"
#include "BrowserHostEventDelegant.h"
#include <exdisp.h>
#include <Shobjidl.h>
#include <MsHtmHst.h>
#include "IEInterface_h.h"
#include "content/browser/web_contents/web_contents_ie.h"
#include "url/gurl.h"
#include "content/public/common/favicon_url.h"
#include "content/public/common/context_menu_params.h"
#include "base/strings/utf_string_conversions.h"
#include "content/browser/frame_host/navigation_controller_impl.h"
#include "ui/aura/window.h"
#include "content/public/browser/web_contents_delegate.h"
#include "base/json/json_reader.h"
#include "base/values.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/download_manager.h"
#include "content/public/browser/browser_context.h"
#include "base/win/windows_version.h"
#include "ui/events/event.h"
#include "ui/events/keycodes/keyboard_code_conversion_win.h"
#include "base/win/win_util.h"
// just for cmp
#define GWL_USERDATA (-21)

#pragma warning ( disable : 4302 )
#pragma warning ( disable : 4189 )

namespace FOR_INCLUDE
{
#include "IEInterface_i.c"
}

namespace {
  void split(std::wstring s, std::wstring delim, std::vector<std::wstring>& result)
  {
    size_t last = 0;
    size_t index = s.find_first_of(delim, last);
    while (index != std::wstring::npos)
    {
      std::wstring temp = s.substr(last, index - last);
      if (temp != L"")
      {
        result.push_back(temp);
      }

      last = index + 1;
      index = s.find_first_of(delim, last);
    }
    if (s.size() - last > 0)
    {
      result.push_back(s.substr(last, s.size() - last));
    }
  }
}

namespace IE {
  using namespace std;
  using namespace content;

  BrowserHostEventDelegant::BrowserHostEventDelegant()
    :hMainWnd(NULL),
    pWebBrowser2(NULL),
    hHostWnd(NULL),
    IEBrowserHwnd(NULL),
    isLoading(false),
    pageTitle(L""),
    pIEContent(NULL),
    pBrowserContainer(NULL),
    weakFactory(this),
    isDownload(false),
    functionControlJson(L""),
    isDisableScreenCapture(false),
    pGetWindowDisplayAffinityFunction(NULL),
    pSetWindowDisplayAffinityFunction(NULL),
    delegantWeakFactory(this),
    updateDLDataWeakFactory(this)
  {
    pSetWindowDisplayAffinityFunction=
      (SetWindowDisplayAffinityPtr)GetProcAddress(GetModuleHandleA("user32.dll"), "SetWindowDisplayAffinity");
    pGetWindowDisplayAffinityFunction=
      (GetWindowDisplayAffinityPtr)GetProcAddress(GetModuleHandleA("user32.dll"), "GetWindowDisplayAffinity");
  }


  BrowserHostEventDelegant::~BrowserHostEventDelegant()
  {
    if (IsWindow(hHostWnd))
    {
      ::DestroyWindow(hHostWnd);
    }
  }

  STDMETHODIMP BrowserHostEventDelegant::OnBeforeNavigate(BSTR url, VARIANT_BOOL navNewUrl, VARIANT_BOOL * cancel)
  {
    *cancel = VARIANT_FALSE;
    isLoading = true;
    if (pIEContent)
    {
      bool clickUrl = false;
      if (navNewUrl == VARIANT_TRUE)
        clickUrl = true;
      bool ret = false;
      pIEContent->OnBeforeNavigate(url, clickUrl, &ret);
      if(ret)
        *cancel = VARIANT_TRUE;
    }
    return S_OK;

  }

  STDMETHODIMP BrowserHostEventDelegant::OnFinishNavigate(BSTR url, BSTR faviconUrls)
  {
    //variant_t ret;
    //ExcuteJavaScript(L"myHello(4,6);", &ret);

    //解决IE屏蔽截屏bug，参看CreateHostWindow函数中的注释
    base::win::Version ver = base::win::GetVersion();
    if (ver >= base::win::VERSION_WIN7 &&
      pSetWindowDisplayAffinityFunction)
    {
      HWND mainHwnd = GetParent(hHostWnd);
      if (isDisableScreenCapture && IsWindow(mainHwnd))
      {
        BOOL ret = FALSE;
        ret = pSetWindowDisplayAffinityFunction(mainHwnd, WDA_MONITOR);
      }
    }

    isLoading = false;
    if (pIEContent)
    {
      GURL gurl = GURL(wstring(url));

      std::vector<std::wstring> favicon_ursl;
      split(std::wstring(faviconUrls), L"\t", favicon_ursl);
      std::vector<GURL> faviconGUrls;
      for (auto it = favicon_ursl.begin(); it != favicon_ursl.end(); it++)
      {
        faviconGUrls.push_back(GURL(*it));
      }

      pIEContent->OnFinishNavigate(gurl, faviconGUrls);
      pIEContent->NotifyNavigationStateChanged(content::INVALIDATE_TYPE_ALL);
    }
    return S_OK;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnTitleChange(BSTR title)
  {
    pageTitle = std::wstring(title, SysStringLen(title));
    if (pIEContent)
    {
      pIEContent->NotifyNavigationStateChanged(content::INVALIDATE_TYPE_ALL);
    }
    return S_OK;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnLoadUrlNewWnd(BSTR url, int flag, VARIANT_BOOL * cancel, IDispatch **pDisp)
  {
    HRESULT hr = E_FAIL;
    if (pWebBrowser2 == NULL)
      return hr;
    if (pIEContent)
    {
      GURL gurl = GURL(wstring(url));
      bool stop = false;
      pIEContent->OnLoadUrlInNewContent(gurl, flag, &stop, pDisp);
      if (stop)
        *cancel = VARIANT_TRUE;
      hr = S_OK;
    }
    return hr;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnBrowserClosing()
  {
    if (pIEContent)
    {
      content::BrowserThread::PostTask(content::BrowserThread::UI, FROM_HERE,
        base::Bind(&BrowserHostEventDelegant::OnJSWindowClose, delegantWeakFactory.GetWeakPtr()));  //先让函数返回完成COM rpc调用在执行关闭过程，否则BrowserHostEventDelegant的引用计数不对无法析构
    }
    return S_OK;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnBeforeBrowserClose(void)
  {
    if (pWebBrowser2)
    {
      pWebBrowser2->Release();
      pWebBrowser2 = NULL;
    }
    if (pBrowserContainer)
    {
      pBrowserContainer->Release();
      pBrowserContainer = NULL;
    }
    return S_OK;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnShowContextMenu(int dwID, int x, int y)
  {
    return S_OK;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnDownLoadFile(BSTR url, int status, VARIANT* variant)
  {
    std::wstring temp = url;
    if (pIEContent)
    {
      if (status == (int)IE::BEGINDOWNLOAD && variant && variant->vt == VT_BSTR)
      {
        isDownload = true;
        downloadDataCallback.Reset();
        std::wstring responHeader = variant->bstrVal;
        pIEContent->OnBeforeDownloadFile(url, responHeader);
      }
      else if (status == (int)DOWNLOADINGDATA &&
        variant && variant->vt == VT_ARRAY)
      {
        SAFEARRAY* pArray = variant->parray;
        HRESULT hr = E_FAIL;
        long Low = 0;
        long High = 0;
        hr = SafeArrayGetLBound(pArray, 1, &Low);//维数索引从1开始，数组下界
        hr = SafeArrayGetUBound(pArray, 1, &High);//维数索引从1开始，数组上界
        long size = High - Low + 1;
        char* pData = NULL;
        hr = SafeArrayAccessData(pArray, (void**)&pData);
        std::string buff = "";
        buff.append(pData, size);
        SafeArrayUnaccessData(pArray);
        content::BrowserThread::PostTask(content::BrowserThread::UI, FROM_HERE,
                 base::Bind(&BrowserHostEventDelegant::OnJSWindowClose, delegantWeakFactory.GetWeakPtr()));
      }
      else if (status == (int)ENDDOWNLOAD || status==(int)DOWNLOADERROR)
      {
        std::string buff = "";
        content::BrowserThread::PostTask(content::BrowserThread::UI, FROM_HERE,
                 base::Bind(&BrowserHostEventDelegant::OnJSWindowClose, delegantWeakFactory.GetWeakPtr()));
      }
    }
    return S_OK;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnCanGoBackOrForward(VARIANT_BOOL * back, VARIANT_BOOL * forward)
  {
    if (pIEContent)
    {
      *back = VARIANT_FALSE;
      *forward = VARIANT_FALSE;
      if (pIEContent->GetController().CanGoBack())
        *back = VARIANT_TRUE;
      if (pIEContent->GetController().CanGoForward())
        *forward = VARIANT_TRUE;
    }
    return S_OK;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnGoBack()
  {
    if (pIEContent)
      pIEContent->GetController().GoBack();
    return S_OK;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnGoForward()
  {
    if (pIEContent)
      pIEContent->GetController().GoForward();
    return S_OK;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnRequestChangePos(int left, int top, int width, int height)
  {
    if (pIEContent)
    {
      gfx::Rect rect;
      rect.SetRect(left, top, width, height);
      pIEContent->RequestSetBounds(rect);
    }
    return S_OK;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnGetMainWndPos(int * left, int * top, int * width, int * height)
  {
    *left = 0;
    *top = 0;
    *width = 0;
    *height = 0;
    if (pIEContent  && IsWindow(hHostWnd))
    {
      HWND hwnd = GetParent(hHostWnd);
      if (IsWindow(hwnd))
      {
        RECT rc;
        ::GetWindowRect(hwnd, &rc);
        *left = rc.left;
        *top = rc.top;
        *width = rc.right - rc.left;
        *height = rc.bottom - rc.top;
      }
    }
    return S_OK;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnIEServerWndCreated(int hwnd)
  {
    IEBrowserHwnd = (HWND)hwnd;
    return S_OK;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnUpdateCookie(BSTR cookie)
  {
    std::wstring jsonStr = cookie;
    std::string temp = base::UTF16ToUTF8(jsonStr);

    std::unique_ptr<base::Value> root = base::JSONReader().ReadToValue(temp);
    if (root == NULL)
      return E_FAIL;
    if (root->is_dict() && pIEContent)
    {
      base::DictionaryValue* rootDict = NULL;
      root->GetAsDictionary(&rootDict);
      std::wstring url=L"";
      rootDict->GetString("url", &url);

      std::vector<std::wstring> vecCookie;
      base::ListValue* list = NULL;
      rootDict->GetList("cookies", &list);
      base::ListValue::const_iterator iter = list->begin();
      for (; iter != list->end(); iter++)
      {
        std::wstring buff = L"";
        (*iter).GetAsString(&buff);
        vecCookie.push_back(buff);
      }
      pIEContent->OnUpdateCookie(GURL(url), vecCookie);
    }
    return S_OK;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnGetLoginInfo(BSTR url, BSTR userName, BSTR userPwd)
  {
    if (url == NULL || userName == NULL || userPwd == NULL)
      return E_FAIL;
    std::wstring uname = userName;
    std::wstring upwd = userPwd;
    if (pIEContent)
      pIEContent->OnGetLoginInfo(url, userName, userPwd);
    return S_OK;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnRequestAcceleratorFromKeyboard(int keyCode, int modifiers)
  {
    if (pIEContent)
    {
      ui::KeyboardCode code = ui::KeyboardCodeForWindowsKeyCode(keyCode);
      ui::KeyEvent uiKeyEvent(ui::ET_KEY_PRESSED, code, 0);
      NativeWebKeyboardEvent event(uiKeyEvent);
      event.SetModifiers(modifiers);
      pIEContent->HandleKeyboardEvent(event);
    }
    return S_OK;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnRequestAcceleratorFromMouseWheel(int delta, int Pt_x, int Pt_y, int modifiers)
  {
    if (pIEContent)
    {
      blink::WebMouseWheelEvent event;
      event.wheel_ticks_y = delta / 120;
      event.SetModifiers(modifiers);
      // comment just for compiling
      // event.x = Pt_x;
      // event.y = Pt_y;
      // event.canScroll = false;
      // pIEContent->HandleWheelEvent(event);
    }
    return S_OK;
  }

  STDMETHODIMP BrowserHostEventDelegant::OnWinDomainAuthenticate(BSTR * userName, BSTR * userPwd)
  {
    if (userName == NULL || userPwd == NULL)
      return E_INVALIDARG;

    std::wstring name = L"";
    std::wstring pwd = L"";
    if (pIEContent)
    {
      pIEContent->GetDelegate()->DidGetWindowsDomainUserInfo(&name, &pwd);
    }
    *userName = ::SysAllocString(name.c_str());
    *userPwd = ::SysAllocString(pwd.c_str());
    return S_OK;
  }

  void BrowserHostEventDelegant::SetHostHWND(HWND hWnd)
  {
    hMainWnd = hWnd;
  }

  bool BrowserHostEventDelegant::CreateBrowser(int browserEmu, bool isNew)
  {
    //wstring path = L"F:\\Source\\src\\out\\Debug\\redcore.exe --type=trident /prefetch:1";
    //STARTUPINFO si = { sizeof(si) };
    //PROCESS_INFORMATION pi = { 0 };

    //BOOL b = CreateProcess(NULL, (LPWSTR)(path.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    //if (b == FALSE)
    //  return false;
    //if (pi.hThread) CloseHandle(pi.hThread);
    //if (pi.hProcess) CloseHandle(pi.hProcess);

    HWND hWnd= GetParent(hMainWnd);
    if (IsWindow(hHostWnd) == FALSE)
    {
      hHostWnd = CreateHostWindow(hWnd);
      //hHostWnd = CreateHostWindow(NULL);
      MoveWindow(hHostWnd, -300, -300, 300, 300, TRUE);
    }


    IClassFactory* cf = NULL;
    while (cf == NULL)
    {
      CoGetClassObject(CLSID_BrowserContainer, CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (LPVOID*)&cf);
    }

    if (cf)
    {
      HRESULT hh = cf->CreateInstance(NULL, IID_IDispatch, (PVOID*)&pBrowserContainer);

      if (pBrowserContainer == NULL)
        return false;

      vector<variant_t> params;
      variant_t param = (long)hHostWnd;
      params.push_back(param);
      params.push_back(browserEmu);
      variant_t paramNew;
      paramNew.vt = VT_BOOL;
      if (isNew)
        paramNew.boolVal = VARIANT_TRUE;
      else
        paramNew.boolVal = VARIANT_FALSE;
      params.push_back(paramNew);
      variant_t ret;
      hh = BrowserHostEventDelegant::DispatchInvoke(pBrowserContainer, L"CreateBrowser", DISPATCH_METHOD, params, ret);

      pWebBrowser2 = GetIWebBrowser2();
      if(pWebBrowser2)
        pWebBrowser2->AddRef();
      SetHostIDispatch(this);
      cf->Release();

      if (functionControlJson.empty() == false)
        SendFunctionControl(functionControlJson);
    }
    return true;
  }

  void BrowserHostEventDelegant::CloseBrowser()
  {
    if (pBrowserContainer)
    {
      vector<variant_t> params;
      variant_t ret;
      DispatchInvoke(pBrowserContainer, L"CloseBrowser", DISPATCH_METHOD, params, ret);
    }
  }

  bool BrowserHostEventDelegant::LoadUrl(std::wstring url)
  {
    if (pWebBrowser2 == NULL)
      return false;

    HRESULT hr = S_OK;
    vector<variant_t> params;
    variant_t ret;

    params.push_back(L"navigate");
    DispatchInvoke(pBrowserContainer, L"NotifyCommand", DISPATCH_METHOD, params, ret);

    params.clear();
    ret.Clear();
    params.push_back(url.c_str());
    variant_t flags = navNoHistory;
    params.push_back(flags);
    variant_t targetFrameName;
    params.push_back(targetFrameName);
    variant_t postData;
    params.push_back(postData);
    variant_t headers;
    params.push_back(headers);
    hr = DispatchInvoke(pWebBrowser2, L"Navigate2", DISPATCH_METHOD, params, ret);
    if (SUCCEEDED(hr))
    {
      pageTitle = L"";
      if(pIEContent)
        pIEContent->NotifyNavigationStateChanged(content::INVALIDATE_TYPE_ALL);
      return true;
    }
    return false;
  }

  void BrowserHostEventDelegant::Stop()
  {
    if (pWebBrowser2 == NULL)
      return;
    isLoading = false;
    vector<variant_t> params;
    variant_t ret;
    DispatchInvoke(pWebBrowser2, L"Stop", DISPATCH_METHOD, params, ret);
  }

  void BrowserHostEventDelegant::Refresh()
  {
    if (pWebBrowser2 == NULL)
      return;
    vector<variant_t> params;
    params.push_back(L"refresh");
    variant_t ret;
    DispatchInvoke(pBrowserContainer, L"NotifyCommand", DISPATCH_METHOD, params, ret);

    params.clear();
    ret.Clear();
    DispatchInvoke(pWebBrowser2, L"Refresh", DISPATCH_METHOD, params, ret);
  }

  void BrowserHostEventDelegant::Find()
  {
    if (pWebBrowser2 == NULL)
      return;
    vector<variant_t> params;
    params.push_back((int)OLECMDID_FIND);
    params.push_back((int)OLECMDEXECOPT_DODEFAULT);
    params.push_back(variant_t());
    params.push_back(variant_t());
    variant_t ret;
    DispatchInvoke(pWebBrowser2, L"ExecWB", DISPATCH_METHOD, params, ret);
  }

  void BrowserHostEventDelegant::Print()
  {
    if (pWebBrowser2 == NULL)
      return;
    vector<variant_t> params;
    params.push_back((int)OLECMDID_PRINTPREVIEW);
    params.push_back((int)OLECMDEXECOPT_DODEFAULT);
    params.push_back(variant_t());
    params.push_back(variant_t());
    variant_t ret;
    DispatchInvoke(pWebBrowser2, L"ExecWB", DISPATCH_METHOD, params, ret);
  }

  void BrowserHostEventDelegant::Cut()
  {
    if (pWebBrowser2 == NULL)
      return;
    vector<variant_t> params;
    params.push_back((int)OLECMDID_CUT);
    params.push_back((int)OLECMDEXECOPT_DODEFAULT);
    params.push_back(variant_t());
    params.push_back(variant_t());
    variant_t ret;
    DispatchInvoke(pWebBrowser2, L"ExecWB", DISPATCH_METHOD, params, ret);
  }

  void BrowserHostEventDelegant::Copy()
  {
    if (pWebBrowser2 == NULL)
      return;
    vector<variant_t> params;
    params.push_back((int)OLECMDID_COPY);
    params.push_back((int)OLECMDEXECOPT_DODEFAULT);
    params.push_back(variant_t());
    params.push_back(variant_t());
    variant_t ret;
    DispatchInvoke(pWebBrowser2, L"ExecWB", DISPATCH_METHOD, params, ret);
  }

  void BrowserHostEventDelegant::Paste()
  {
    if (pWebBrowser2 == NULL)
      return;
    vector<variant_t> params;
    params.push_back((int)OLECMDID_PASTE);
    params.push_back((int)OLECMDEXECOPT_DODEFAULT);
    params.push_back(variant_t());
    params.push_back(variant_t());
    variant_t ret;
    DispatchInvoke(pWebBrowser2, L"ExecWB", DISPATCH_METHOD, params, ret);
  }

  void BrowserHostEventDelegant::Show(bool bShow)
  {
    if (IsWindow(hHostWnd))
    {
      if (bShow)
      {
        ShowWindow(hHostWnd, SW_SHOW);
        vector<variant_t> params;
        VARIANT arg;
        arg.vt = VT_BOOL;
        arg.boolVal = VARIANT_TRUE;
        params.push_back(arg);
        variant_t ret;
        DispatchInvoke(pBrowserContainer, L"Show", DISPATCH_METHOD, params, ret);
      }
      else
        ShowWindow(hHostWnd, SW_HIDE);
    }
  }

  bool BrowserHostEventDelegant::ExcuteJavaScript(std::wstring code, variant_t * result)
  {
    if (result == NULL || pBrowserContainer == NULL)
      return false;
    vector<variant_t> params;
    params.push_back(code.c_str());
    variant_t arg;
    arg.vt = VT_VARIANT | VT_BYREF;
    arg.pvarVal = result;
    params.push_back(arg);
    variant_t ret;
    HRESULT hr = DispatchInvoke(pBrowserContainer, L"ExcuteJS", DISPATCH_METHOD, params, ret);
    if (SUCCEEDED(hr))
    {
      return true;
    }
    return false;
  }

  void BrowserHostEventDelegant::SetBrowserRect(RECT rt)
  {
    if (::IsWindow(hHostWnd))
    {
      ::MoveWindow(hHostWnd, rt.left, rt.top, rt.right-rt.left, rt.bottom-rt.top, TRUE);
    }
    if (pBrowserContainer)
    {
      RECT rc;
      rc.left = 0;
      rc.top = 0;
      rc.right = rt.right-rt.left;
      rc.bottom = rt.bottom - rt.top;
      vector<variant_t> params;
      params.push_back(rc.left);
      params.push_back(rc.top);
      params.push_back(rc.right);
      params.push_back(rc.bottom);
      variant_t ret;
      DispatchInvoke(pBrowserContainer, L"SetRect", DISPATCH_METHOD, params, ret);
    }
  }

  IDispatch * BrowserHostEventDelegant::GetIWebBrowser2()
  {
    IDispatch* pDisp = NULL;
    vector<variant_t> params;
    variant_t param;
    param.vt = VT_DISPATCH | VT_BYREF;
    param.ppdispVal = &pDisp;
    params.push_back(param);
    variant_t ret;
    DispatchInvoke(pBrowserContainer, L"WebBrowser", DISPATCH_PROPERTYGET, params, ret);
    return pDisp;
  }

  void BrowserHostEventDelegant::SetHostIDispatch(IDispatch* pHost)
  {
    DISPID dispID = 0;
    OLECHAR * szMember = L"HostApp";
    HRESULT hh = pBrowserContainer->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_SYSTEM_DEFAULT, &dispID);

    DISPPARAMS dispparams = { NULL, NULL, 1, 1 };
    variant_t host = (IDispatch*)pHost;
    dispparams.rgvarg = &host;
    DISPID dispidPut = DISPID_PROPERTYPUT;
    dispparams.rgdispidNamedArgs = &dispidPut;
    hh = pBrowserContainer->Invoke(dispID, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUTREF,
      &dispparams, NULL, NULL, NULL);
  }

  HWND BrowserHostEventDelegant::CreateHostWindow(HWND parentWnd)
  {
    HINSTANCE hInstance = ::GetModuleHandle(NULL);
    RegisterWndClass(hInstance);
    HWND hWnd = CreateWindowW(L"YSPHostWnd", L"", /*WS_OVERLAPPED,*/WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
      0, 0, 0, 0, parentWnd, nullptr, hInstance, nullptr);

    //为了解决设置SetWindowDisplayAffinity(WDA_MONITOR)用来屏蔽截屏后，IE在发生DISPID_NEWWINDOW3时无法创建IE子窗口的问题，
    //如果设置了WDA_MONITOR，先关闭屏蔽截屏，在IE窗口创建完成后(OnIEServerWndCreated函数中)再打开屏蔽截屏
    base::win::Version ver = base::win::GetVersion();
    if (ver >= base::win::VERSION_WIN7 &&
      pGetWindowDisplayAffinityFunction &&
      pSetWindowDisplayAffinityFunction)
    {
      BOOL ret = FALSE;
      DWORD dwAffinity = 0;
      ret = pGetWindowDisplayAffinityFunction(parentWnd, &dwAffinity);
      if (ret == TRUE && dwAffinity == WDA_MONITOR)
      {
        isDisableScreenCapture = true;
        ret = pSetWindowDisplayAffinityFunction(parentWnd, WDA_NONE);
      }
    }

    SetWindowLong(hWnd, GWL_USERDATA, (LONG)this);
    ::ShowWindow(hWnd, SW_SHOW);
    return hWnd;
  }

  void BrowserHostEventDelegant::RegisterWndClass(HINSTANCE hInstance)
  {

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = L"YSPHostWnd";
    wcex.lpszClassName = L"YSPHostWnd";
    wcex.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
  }

  void BrowserHostEventDelegant::UpdateDownloadData(const int& status, const std::string& data)
  {
    if (downloadDataCallback.is_null())
      return;
    downloadDataCallback.Run(status, data.c_str(), data.size());
  }

  LRESULT BrowserHostEventDelegant::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
  {
    switch (message)
    {
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps);
      // TODO: 在此处添加使用 hdc 的任何绘图代码...
      RECT rect;
      ::GetClientRect(hWnd, &rect);
      HBRUSH hBr = ::CreateSolidBrush(RGB(255, 255, 255));
      ::FillRect(ps.hdc, &rect, hBr);
      ::DeleteObject(hBr);
      EndPaint(hWnd, &ps);
    }
    break;
    case WM_ERASEBKGND:
      break;
    case WM_SIZE:
    {
      break;
    }
    case WM_DESTROY:
    {
      SetWindowLong(hWnd, GWL_USERDATA, 0);
      break;
    }
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
  }

  void BrowserHostEventDelegant::OnJSWindowClose()
  {
    if (pIEContent && pIEContent->GetRenderViewHost())
      pIEContent->Close(pIEContent->GetRenderViewHost());
  }

  HRESULT BrowserHostEventDelegant::DispatchInvoke(IDispatch * pDisp, const wchar_t * name, int type, std::vector<variant_t>& params, variant_t& ret)
  {
    HRESULT hr = E_INVALIDARG;
    if (pDisp == NULL)
      return hr;

    DISPID dispID = 0;
    OLECHAR * szMember = (WCHAR*)name;
    hr = pDisp->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_SYSTEM_DEFAULT, &dispID);
    if (FAILED(hr))
      return hr;

    DISPPARAMS args;
    args.cArgs = params.size();
    args.cNamedArgs = 0;
    args.rgdispidNamedArgs = NULL;
    args.rgvarg = NULL;
    if (args.cArgs > 0)
    {
      args.rgvarg = new VARIANTARG[args.cArgs];
      int i = params.size() - 1;
      int j = 0;
      for (; i >= 0; i--)
      {
        args.rgvarg[j] = params[i];
        j++;
      }
    }
    hr = pDisp->Invoke(dispID, IID_NULL, LOCALE_USER_DEFAULT, type, &args, &ret, NULL, NULL);
    if (args.cArgs)
    {
      delete[] args.rgvarg;
    }
    return hr;
  }
  std::wstring BrowserHostEventDelegant::GetTitle()
  {
    // TODO: 在此处插入 return 语句
    return pageTitle;
  }

  bool BrowserHostEventDelegant::IsLoading()
  {
    return isLoading;
  }
  void BrowserHostEventDelegant::SetWebContent(content::WebContentsIE * pIE)
  {
    pIEContent = pIE;
  }

  int BrowserHostEventDelegant::GetDocMode()
  {
    if (pBrowserContainer)
    {
      vector<variant_t> params;
      int mode=0;
      variant_t param;
      param.vt = VT_I4 | VT_BYREF;
      param.pintVal = &mode;
      params.push_back(param);
      variant_t ret;
      HRESULT hr= DispatchInvoke(pBrowserContainer, L"GetDocumentMode", DISPATCH_METHOD, params, ret);
      if (SUCCEEDED(hr))
      {
        return mode;
      }
    }
    return 0;
  }
  bool BrowserHostEventDelegant::SetDocMode(IEDocumentMode mode)
  {
    if (pBrowserContainer)
    {
      vector<variant_t> params;
      variant_t param;
      param.vt = VT_I4;
      param.intVal = (int)mode;
      params.push_back(param);
      variant_t ret;
      HRESULT hr = DispatchInvoke(pBrowserContainer, L"SetDocumentMode", DISPATCH_METHOD, params, ret);
      if (SUCCEEDED(hr))
        return true;
    }
    return false;
  }

  bool BrowserHostEventDelegant::GetCookie(std::wstring & cookies)
  {
    if (pBrowserContainer)
    {
      vector<variant_t> params;
      BSTR buff = NULL;
      variant_t argCookie;
      argCookie.vt = VT_BSTR | VT_BYREF;
      argCookie.pbstrVal = &buff;
      params.push_back(argCookie);
      variant_t ret;
      HRESULT hr = DispatchInvoke(pBrowserContainer, L"GetCookie", DISPATCH_METHOD, params, ret);
      if (FAILED(hr))
        return false;
      cookies = buff;
      ::SysFreeString(buff);
    }
    return true;
  }

  bool BrowserHostEventDelegant::SetCookie(const  std::wstring & cookies, const int flag)
  {
    if (pBrowserContainer)
    {
      vector<variant_t> params;
      params.push_back(cookies.c_str());
      params.push_back(flag);
      variant_t ret;
      DispatchInvoke(pBrowserContainer, L"SetCookie", DISPATCH_METHOD, params, ret);
    }
    return true;
  }

  bool BrowserHostEventDelegant::SetUserAgent(const std::wstring & uaStr)
  {
    if (pBrowserContainer)
    {
      vector<variant_t> params;
      params.push_back(uaStr.c_str());
      variant_t ret;
      HRESULT hr=DispatchInvoke(pBrowserContainer, L"SetUserAgent", DISPATCH_METHOD, params, ret);
      if (SUCCEEDED(hr))
        return true;
    }
    return false;
  }

  IDispatch * BrowserHostEventDelegant::GetBrowserIDispatch()
  {
    return pWebBrowser2;
  }

  bool BrowserHostEventDelegant::SetParentHWND(HWND hWnd)
  {
    if (::IsWindow(hHostWnd) && ::IsWindow(hWnd))
    {
      SetHostHWND(hWnd);
      SetParent(hHostWnd, hWnd);
      return true;
    }
    return false;
  }

  bool BrowserHostEventDelegant::SendFunctionControl(const std::wstring & json)
  {
    if (pBrowserContainer)
    {
      vector<variant_t> params;
      params.push_back(json.c_str());
      variant_t ret;
      HRESULT hr = DispatchInvoke(pBrowserContainer, L"SetFunctionControl", DISPATCH_METHOD, params, ret);
      if (SUCCEEDED(hr))
      {
        functionControlJson.clear();
        return true;
      }
    }
    else
      functionControlJson = json; //设置的时候有可能IE子进程还未创建完成，如果设置失败就将json串暂存在IE子进程调用CreateBrowser函数时再设置
    return false;
  }

  void BrowserHostEventDelegant::AutoLogin(const std::wstring& frameXPath, const std::wstring & unameXPath, const std::wstring & unameValue,
    const std::wstring & pwdXPath, const std::wstring & pwdValue, const std::wstring & btnXPath)
  {
    if (pBrowserContainer)
    {
      vector<variant_t> params;
      params.push_back(frameXPath.c_str());
      params.push_back(unameXPath.c_str());
      params.push_back(unameValue.c_str());
      params.push_back(pwdXPath.c_str());
      params.push_back(pwdValue.c_str());
      params.push_back(btnXPath.c_str());
      variant_t ret;
      DispatchInvoke(pBrowserContainer, L"AutoLogin", DISPATCH_METHOD, params, ret);
    }
  }

  void BrowserHostEventDelegant::SetListenLoginXPath(const std::wstring & url, const std::wstring& frameXPath, const std::wstring & unameXPath,
    const std::wstring & pwdXPath, const std::wstring & btnXPath)
  {
    if (pBrowserContainer)
    {
      vector<variant_t> params;
      params.push_back(url.c_str());
      params.push_back(frameXPath.c_str());
      params.push_back(unameXPath.c_str());
      params.push_back(pwdXPath.c_str());
      params.push_back(btnXPath.c_str());
      variant_t ret;
      DispatchInvoke(pBrowserContainer, L"SetListenLoginXPath", DISPATCH_METHOD, params, ret);
    }
  }

  bool BrowserHostEventDelegant::IsDevToolsOpened()
  {
    if (pBrowserContainer == NULL)
      return false;

    VARIANT_BOOL isOpen = VARIANT_FALSE;
    vector<variant_t> params;
    variant_t var;
    var.vt = VT_BYREF | VT_BOOL;
    var.pboolVal = &isOpen;
    params.push_back(var);
    variant_t ret;
    HRESULT hr = E_FAIL;
    hr=DispatchInvoke(pBrowserContainer, L"GetDevToolsOpenStatus", DISPATCH_METHOD, params, ret);
    if (SUCCEEDED(hr))
    {
      if (isOpen == VARIANT_TRUE)
        return true;
    }
    return false;
  }

  void BrowserHostEventDelegant::ShowDevTools(bool show)
  {
    if (pBrowserContainer)
    {
      VARIANT_BOOL isShow = VARIANT_FALSE;
      if (show)
        isShow = VARIANT_TRUE;
      vector<variant_t> params;
      params.push_back(isShow);
      variant_t ret;
      DispatchInvoke(pBrowserContainer, L"ShowDevTools", DISPATCH_METHOD, params, ret);
    }
  }

  void BrowserHostEventDelegant::SetZoomPercent(int percent)
  {
    if (pWebBrowser2 == NULL)
      return;
    CComPtr<IWebBrowser2> pWeb2 = NULL;
    pWebBrowser2->QueryInterface(IID_IWebBrowser2, (void**)&pWeb2);
    if (pWeb2 == NULL)
      return;
    CComVariant  varZoom = percent;
    HRESULT hr;
    int ieVer=base::win::GetSystemIEVersion();
    if(ieVer>=7)
      hr=pWeb2->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DODEFAULT, &varZoom, NULL);
  }

  bool BrowserHostEventDelegant::IsCanZoom()
  {
    int ieVer = base::win::GetSystemIEVersion();
    if (ieVer >= 7)
      return true;
    return false;
  }

  void BrowserHostEventDelegant::RegisterCallback(const DataReceivedCallback & sink_callback)
  {
    DCHECK_CURRENTLY_ON(BrowserThread::UI);

    downloadDataCallback = sink_callback;
    pIEContent->GetBrowserContext()->GetDownloadManager(pIEContent->GetBrowserContext())->RegisterCallbackSucceeded();
  }

  void BrowserHostEventDelegant::FinishDownload(bool isCancel)
  {
    DCHECK_CURRENTLY_ON(BrowserThread::UI);
    isDownload = false;
    if (pBrowserContainer)
    {
      vector<variant_t> params;
      VARIANT_BOOL cancel = VARIANT_FALSE;
      if (isCancel)
        cancel = VARIANT_TRUE;
      params.push_back(cancel);
      variant_t ret;
      DispatchInvoke(pBrowserContainer, L"FinishDownload", DISPATCH_METHOD, params, ret);
    }
  }

  base::WeakPtr<IEDownloader> BrowserHostEventDelegant::GetWeakPtr()
  {
    return weakFactory.GetWeakPtr();
  }

  bool BrowserHostEventDelegant::IsDownloading()
  {
    return isDownload;
  }
} //namespace IE
