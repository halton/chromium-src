#include "BrowserProcess.h"
#include "WebBrowser.h"
#include <Shobjidl.h>
#include "url/gurl.h"
#include <commdlg.h>
#include <ShlObj.h>
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/browser_thread.h"

namespace IE {
  using namespace std;

  DWORD BrowserProcess::dwRegister = 0;

  BrowserProcess::BrowserProcess()
  {
    pIEBrowser = NULL;
    pHostApp = NULL;
    shouldBeShow = VARIANT_FALSE;
    IEHwnd = NULL;
  }

  BrowserProcess::~BrowserProcess()
  {
  }

  STDMETHODIMP BrowserProcess::CreateBrowser(long hWnd, int browserEmulation, VARIANT_BOOL isNewWindow)
  {
    bool isNew = false;
    if (isNewWindow == VARIANT_TRUE)
      isNew = true;
    pIEBrowser = new WebBrowser((HWND)hWnd, this, browserEmulation, isNew);
    return S_OK;
  }

  STDMETHODIMP BrowserProcess::get_WebBrowser(IDispatch ** pVal)
  {
    if (pVal == NULL || pIEBrowser == NULL)
      return E_INVALIDARG;
    *pVal = pIEBrowser->GetIeDispatch();
    return S_OK;
  }

  STDMETHODIMP BrowserProcess::CloseBrowser(void)
  {
    if (pIEBrowser)
    {
      pIEBrowser->DisableSubClass(IEHwnd);
      delete pIEBrowser;
      pIEBrowser = NULL;
    }
    NotifyHostBrowserWillClose();
    if (pHostApp)
    {
      pHostApp->Release();
      pHostApp = NULL;
    }
    return S_OK;
  }

  STDMETHODIMP BrowserProcess::SetRect(int left, int top, int right, int bottom)
  {
    if (pIEBrowser)
    {
      RECT rc = { left,top,right,bottom };
      pIEBrowser->SetRect(rc);
      return S_OK;
    }
    return E_NOTIMPL;
  }

  STDMETHODIMP BrowserProcess::putref_HostApp(IDispatch * app)
  {
    pHostApp = app;
    pHostApp->AddRef(); //因为是putref 所以调用AddRef 加引用，保证以后可用
    return S_OK;
  }

  STDMETHODIMP BrowserProcess::ExcuteJS(BSTR src, VARIANT * result)
  {
    if (result == NULL || pIEBrowser == NULL)
      return E_FAIL;
    wstring code = src;
    variant_t ret;
    HRESULT hr = pIEBrowser->ExcuteJavaScript(code, &ret);
    *result = ret;
    return hr;
  }

  STDMETHODIMP BrowserProcess::Show(VARIANT_BOOL bShow)
  {
    if (pIEBrowser == NULL)
      return E_FAIL;
    shouldBeShow = bShow;
    if (bShow == VARIANT_TRUE)
    {
      if (IsWindow(IEHwnd))
      {
        pIEBrowser->Show();
      }
    }
    else
      pIEBrowser->Hide();
    return S_OK;
  }

  STDMETHODIMP BrowserProcess::GetDocumentMode(int * mode)
  {
    if (pIEBrowser)
    {
      *mode=pIEBrowser->GetDocumentMode();
      return S_OK;
    }
    return E_FAIL;
  }

  STDMETHODIMP BrowserProcess::SetDocumentMode(int mode)
  {
    if (pIEBrowser)
    {
      IEDocumentMode temp = static_cast<IEDocumentMode>(mode);
      bool b=pIEBrowser->SetDocumentMode(temp);
      if (b)
        return S_OK;
    }
    return E_FAIL;
  }

  STDMETHODIMP BrowserProcess::SetCookie(BSTR json, int flag)
  {
    if (pIEBrowser)
    {
      bool ret = false;
      ret = pIEBrowser->SetBrowserCookie(json, flag);
      if (ret)
        return S_OK;
    }
    return E_FAIL;
  }

  STDMETHODIMP BrowserProcess::SetUserAgent(BSTR uaStr)
  {
    if (pIEBrowser)
    {
      HRESULT hr = pIEBrowser->SetUserAgent(uaStr);
      return hr;
    }
    return E_FAIL;
  }

  STDMETHODIMP BrowserProcess::FinishDownload(VARIANT_BOOL isCancel)
  {
    bool cancel = false;
    if (isCancel == VARIANT_TRUE)
      cancel = true;
    if (pIEBrowser)
      pIEBrowser->FinishDownload(cancel);
    return S_OK;
  }

  STDMETHODIMP BrowserProcess::SetFunctionControl(BSTR jsonStr)
  {
    if (pIEBrowser == NULL)
      return E_FAIL;
    pIEBrowser->SetFunctionControl(jsonStr);
    return S_OK;
  }

  STDMETHODIMP BrowserProcess::AutoLogin(BSTR frameXPath, BSTR userNameXPath, BSTR userNameValue, BSTR pwdXPath, BSTR pwdValue, BSTR loginBtnXPath)
  {
    if (pIEBrowser == NULL)
      return E_FAIL;
    pIEBrowser->AutoLoginToSite(frameXPath, userNameXPath, userNameValue, pwdXPath, pwdValue, loginBtnXPath);
    return S_OK;
  }

  STDMETHODIMP BrowserProcess::SetListenLoginXPath(BSTR url, BSTR frameXPath, BSTR userNameXPath, BSTR pwdXPath, BSTR loginbtnXPath)
  {
    if(pIEBrowser == NULL)
      return E_FAIL;

    pIEBrowser->SetListenLoginXPath(url, frameXPath, userNameXPath, pwdXPath, loginbtnXPath);
    return S_OK;
  }

  STDMETHODIMP BrowserProcess::NotifyCommand(BSTR command)
  {
    if (pIEBrowser == NULL)
      return E_FAIL;
    pIEBrowser->NotifyCommand(command);
    return S_OK;
  }

  STDMETHODIMP BrowserProcess::GetDevToolsOpenStatus(VARIANT_BOOL * isOpen)
  {
    if (pIEBrowser == NULL || isOpen == NULL)
      return E_FAIL;
    bool open = pIEBrowser->IsDevToolsOpened();
    VARIANT_BOOL retOpen = VARIANT_FALSE;
    if (open)
      retOpen = VARIANT_TRUE;
    *isOpen = retOpen;
    return S_OK;
  }

  STDMETHODIMP BrowserProcess::ShowDevTools(VARIANT_BOOL show)
  {
    if (pIEBrowser == NULL)
      return E_FAIL;
    bool isShow = false;
    if (show == VARIANT_TRUE)
      isShow = true;
    pIEBrowser->ShowIEDevTools(isShow);
    return S_OK;
  }

  bool BrowserProcess::OnBeforeNavigate(std::wstring url, bool clickUrl)
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      params.push_back(url.c_str());
      if (clickUrl)
        params.push_back(VARIANT_TRUE);
      else
        params.push_back(VARIANT_FALSE);
      VARIANT_BOOL cancel = VARIANT_FALSE;
      variant_t v;
      v.pboolVal = &cancel;
      v.vt = VT_BOOL | VT_BYREF;
      params.push_back(v);
      variant_t ret;
      HRESULT hr = DispatchInvoke(pHostApp, L"OnBeforeNavigate", DISPATCH_METHOD, params, ret);
      if (SUCCEEDED(hr) && cancel != VARIANT_FALSE)
        return true;
    }
    return false;
  }

  void BrowserProcess::OnFinishNavigate(std::wstring url, std::wstring faviconUrls)
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      params.push_back(url.c_str());
      params.push_back(faviconUrls.c_str());
      variant_t ret;
      DispatchInvoke(pHostApp, L"OnFinishNavigate", DISPATCH_METHOD, params, ret);
    }
    if (pIEBrowser == NULL)
      return;
    pIEBrowser->ConfirmLoginSuccess();
    HWND hwnd = 0;
    hwnd = pIEBrowser->GetControlWindow();
    if (IsWindow(hwnd))
    {
      RECT rect;
      GetClientRect(hwnd, &rect);
      ::MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left-1, rect.bottom - rect.top-1, FALSE);
      ::MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
    }
  }

  void BrowserProcess::OnTitleChange(std::wstring title)
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      params.push_back(title.c_str());
      variant_t ret;
      DispatchInvoke(pHostApp, L"OnTitleChange", DISPATCH_METHOD, params, ret);
    }
  }

  bool BrowserProcess::OnLoadUrlNewWnd(std::wstring url, long flag, IDispatch** pDisp)
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      variant_t argUrl;
      argUrl.vt = VT_BSTR;
      argUrl.bstrVal = bstr_t(url.c_str());
      params.push_back(argUrl);
      variant_t argFlag;
      argFlag.vt = VT_I4;
      argFlag.intVal = flag;
      params.push_back(argFlag);
      VARIANT_BOOL cancel = VARIANT_FALSE;
      variant_t v;
      v.pboolVal = &cancel;
      v.vt = VT_BOOL | VT_BYREF;
      params.push_back(v);
      IDispatch* idisp = NULL;
      variant_t argDisp;
      argDisp.vt = VT_BYREF | VT_DISPATCH;
      argDisp.ppdispVal = &idisp;
      params.push_back(argDisp);
      variant_t ret;
      HRESULT hr = DispatchInvoke(pHostApp, L"OnLoadUrlNewWnd", DISPATCH_METHOD, params, ret);
      if (SUCCEEDED(hr))
      {
        if (idisp)
        {
          idisp->AddRef();
          *pDisp = idisp;
        }
      }
      if (cancel == VARIANT_TRUE)
        return true;
    }
    return false;
  }

  void BrowserProcess::OnBrowserClosing()
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      variant_t ret;
      HRESULT hr = E_FAIL;
      hr = DispatchInvoke(pHostApp, L"OnBrowserClosing", DISPATCH_METHOD, params, ret);
    }
  }

  void BrowserProcess::SetIEHwnd(HWND hwnd)
  {
    IEHwnd = hwnd;
    wchar_t wndClassName[256] = { 0 };
    GetClassName(IEHwnd, wndClassName, 255);  //如果打开的是FILE://协议的url，IE不会创建Internet Explorer_Server子窗口，所以不能subclass
    if (pIEBrowser && std::wstring(wndClassName)==L"Internet Explorer_Server")
    {
      pIEBrowser->EnableSubClass(IEHwnd);
    }
    Show(shouldBeShow);
    OnIEServerCreated(hwnd);
  }

  bool BrowserProcess::OnShowContxtMenu(int id, POINT pt)
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      params.push_back(id);
      params.push_back(pt.x);
      params.push_back(pt.y);
      variant_t ret;
      DispatchInvoke(pHostApp, L"OnShowContextMenu", DISPATCH_METHOD, params, ret);
      return true;
    }
    return false;
  }

  bool BrowserProcess::OnDownLoadFile(const std::wstring& url, int status, VARIANT variant)
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      params.push_back(url.c_str());
      params.push_back(status);
      variant_t var;
      var.vt = VT_BYREF | VT_VARIANT;
      var.pvarVal = &variant;
      params.push_back(var);
      variant_t ret;
      HRESULT hr = DispatchInvoke(pHostApp, L"OnDownLoadFile", DISPATCH_METHOD, params, ret);
      if (SUCCEEDED(hr))
        return true;
    }
    return false;
  }

  void BrowserProcess::GetGoBackOrForwardState(bool * goBack, bool * goForward)
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      variant_t temp;
      temp.vt = VT_BOOL | VT_BYREF;
      VARIANT_BOOL back = VARIANT_FALSE;
      temp.pboolVal = &back;
      params.push_back(temp);

      VARIANT_BOOL forward = VARIANT_FALSE;
      temp.pboolVal = &forward;
      params.push_back(temp);
      variant_t ret;
      HRESULT hr = E_FAIL;
      hr = DispatchInvoke(pHostApp, L"OnCanGoBackOrForward", DISPATCH_METHOD, params, ret);
      if (SUCCEEDED(hr))
      {
        if (back == VARIANT_TRUE)
          *goBack = true;
        if (forward == VARIANT_TRUE)
          *goForward = true;
      }
    }
  }

  void BrowserProcess::GoBack()
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      variant_t ret;
      HRESULT hr = E_FAIL;
      hr = DispatchInvoke(pHostApp, L"OnGoBack", DISPATCH_METHOD, params, ret);
    }
  }

  void BrowserProcess::GoForward()
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      variant_t ret;
      HRESULT hr = E_FAIL;
      hr = DispatchInvoke(pHostApp, L"OnGoForward", DISPATCH_METHOD, params, ret);
    }
  }

  void BrowserProcess::OnRequestChangePos(int left, int top, int width, int height)
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      params.push_back(left);
      params.push_back(top);
      params.push_back(width);
      params.push_back(height);
      variant_t ret;
      HRESULT hr = E_FAIL;
      hr = DispatchInvoke(pHostApp, L"OnRequestChangePos", DISPATCH_METHOD, params, ret);
    }
  }

  void BrowserProcess::OnGetMainWndPos(RECT * rect)
  {
    int left = 0;
    int top = 0;
    int width = 0;
    int height = 0;
    ::SetRect(rect, 0, 0, 0, 0);
    if (pHostApp)
    {
      vector<variant_t> params;
      variant_t temp;
      temp.vt = VT_I4 | VT_BYREF;
      temp.pintVal = &left;
      params.push_back(temp);

      temp.pintVal = &top;
      params.push_back(temp);

      temp.pintVal = &width;
      params.push_back(temp);

      temp.pintVal = &height;
      params.push_back(temp);
      variant_t ret;
      HRESULT hr = E_FAIL;
      hr = DispatchInvoke(pHostApp, L"OnGetMainWndPos", DISPATCH_METHOD, params, ret);
      if (SUCCEEDED(hr))
        ::SetRect(rect, left, top, left + width, top + height);
    }
  }

  void BrowserProcess::OnIEServerCreated(HWND hwnd)
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      params.push_back((int)hwnd);
      variant_t ret;
      HRESULT hr = E_FAIL;
      hr = DispatchInvoke(pHostApp, L"OnIEServerWndCreated", DISPATCH_METHOD, params, ret);
    }
  }

  void BrowserProcess::OnUpdateCookie(const std::wstring & cookie)
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      params.push_back(cookie.c_str());
      variant_t ret;
      HRESULT hr = E_FAIL;
      hr = DispatchInvoke(pHostApp, L"OnUpdateCookie", DISPATCH_METHOD, params, ret);
    }
  }

  void BrowserProcess::OnClickElement(CComPtr<IHTMLElement> clickElement, bool byEnterKey)
  {
    if (pIEBrowser)
      pIEBrowser->TryGetLoginInfo(clickElement, byEnterKey);
  }

  void BrowserProcess::OnGetLoginInfo(std::wstring url, std::wstring userName, std::wstring userPwd)
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      params.push_back(url.c_str());
      params.push_back(userName.c_str());
      params.push_back(userPwd.c_str());
      variant_t ret;
      HRESULT hr = E_FAIL;
      hr = DispatchInvoke(pHostApp, L"OnGetLoginInfo", DISPATCH_METHOD, params, ret);
    }
  }

  void BrowserProcess::OnRequestAcceleratorFromKeyboard(int keyCode, int modifiers)
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      params.push_back(keyCode);
      params.push_back(modifiers);
      variant_t ret;
      HRESULT hr = E_FAIL;
      hr = DispatchInvoke(pHostApp, L"OnRequestAcceleratorFromKeyboard", DISPATCH_METHOD, params, ret);
    }
  }

  void BrowserProcess::OnRequestAcceleratorFromMouseWheel(int delta, int Pt_x, int Pt_y, int modifiers)
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      params.push_back(delta);
      params.push_back(Pt_x);
      params.push_back(Pt_y);
      params.push_back(modifiers);
      variant_t ret;
      HRESULT hr = E_FAIL;
      hr = DispatchInvoke(pHostApp, L"OnRequestAcceleratorFromMouseWheel", DISPATCH_METHOD, params, ret);
    }
  }

  void BrowserProcess::OnWindowDomainAuthenticate(std::wstring * userName, std::wstring * userPwd)
  {
    if (pHostApp && userName && userPwd)
    {
      *userName = L"";
      *userPwd = L"";
      vector<variant_t> params;
      bstr_t uNameStr = L"";
      variant_t arg1;
      arg1.vt = VT_BYREF | VT_BSTR;
      arg1.pbstrVal = uNameStr.GetAddress();
      bstr_t uPwdStr = L"";
      variant_t arg2;
      arg2.vt = VT_BYREF | VT_BSTR;
      arg2.pbstrVal = uPwdStr.GetAddress();
      params.push_back(arg1);
      params.push_back(arg2);
      variant_t ret;
      HRESULT hr = E_FAIL;
      hr = DispatchInvoke(pHostApp, L"OnWinDomainAuthenticate", DISPATCH_METHOD, params, ret);
      if (SUCCEEDED(hr) && uNameStr.length()>0 && uPwdStr.length()>0)
      {
        *userName = uNameStr;
        *userPwd = uPwdStr;
      }
    }
  }

	void BrowserProcess::OnQueryPrivateDNS(const std::wstring & host, std::wstring * ipListJsonStr)
	{
		if (pHostApp && host.empty() == false && ipListJsonStr)
		{
			*ipListJsonStr = L"";
			vector<variant_t> params;
			bstr_t hostArg = host.c_str();
			params.push_back(hostArg);
			bstr_t ipListArg = L"";
			variant_t arg2;
			arg2.vt = VT_BYREF | VT_BSTR;
			arg2.pbstrVal = ipListArg.GetAddress();
			params.push_back(arg2);
			variant_t ret;
			HRESULT hr = E_FAIL;
			hr = DispatchInvoke(pHostApp, L"OnQueryPrivateDNS", DISPATCH_METHOD, params, ret);
			if (SUCCEEDED(hr) && ipListArg.GetBSTR())
			{
				*ipListJsonStr = ipListArg.GetBSTR();
			}
		}
	}
  bool BrowserProcess::RegisterClassObject()
  {
    CComPtr<IUnknown> cf;
    HRESULT hr = AtlComModuleGetClassObject(&_AtlComModule, CLSID_BrowserContainer, IID_IUnknown, (void**)&cf);
    if (FAILED(hr))
    {
      return false;
    }
    hr = CoRegisterClassObject(CLSID_BrowserContainer, cf, CLSCTX_LOCAL_SERVER,
      REGCLS_SINGLEUSE, &dwRegister);
    if (SUCCEEDED(hr))
      return true;
    return false;
  }

  bool BrowserProcess::UnregisterClassObject()
  {
    if (dwRegister != 0)
    {
      HRESULT hr= CoRevokeClassObject(dwRegister);
      if (SUCCEEDED(hr))
        return true;
    }
    return false;
  }

  HRESULT BrowserProcess::DispatchInvoke(IDispatch * pDisp, const wchar_t * name, int type, std::vector<variant_t>& params, variant_t& ret)
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

  void BrowserProcess::NotifyHostBrowserWillClose()
  {
    if (pHostApp)
    {
      vector<variant_t> params;
      variant_t ret;
      DispatchInvoke(pHostApp, L"OnBeforeBrowserClose", DISPATCH_METHOD, params, ret);
    }
  }
}// namespace IE
