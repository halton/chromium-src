#include "ATLInclude.h"
#include <Exdisp.h>
#include <EXDISPID.H>
#include <WinUser.h>
#include <string>
#include "IEEvent.h"
#include "IEEventHandler.h"
#include "DocHostUIHandler.h"
#include <oleauto.h>
#include <initguid.h>
#include <objbase.h>
#include <Wininet.h>
#include <Mshtml.h>
#include <MsHtmdid.h>
#include <MsHtmHst.h>
#include "url/gurl.h"
#include "base/strings/utf_string_conversions.h"
#include <stack>

// define GWL_HWNDPARENT here just for compiling
#define GWL_HWNDPARENT (-8)
using namespace std;

DEFINE_GUID(CGID_MshtmlPriv, 0xa11452bc, 0xe055, 0x4e56, 0xa1, 0x51, 0x7b, 0x16, 0xdb, 0xb4, 0x54, 0x4e);

enum
{
  MSHTMLPRIV_TRUSTFORFIRSTDOWNLOAD,
  MSHTMLPRIV_BROWSERNAVSTATECHANGED,
  MSHTMLPRIV_SETSESSIONDOCUMENTMODE,
  MSHTMLPRIV_GETSESSIONDOCUMENTMODE
};

namespace IE {
  using namespace std;

  static HWND lastHwnd = 0;

  BOOL CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam)
  {
    if (hwndChild == NULL)
      return FALSE;
    if (lastHwnd == hwndChild)
      return FALSE;
    lastHwnd = hwndChild;
    wchar_t name[256] = { 0 };
    ::GetClassName(hwndChild, name, 255);
    if (std::wstring(name) == L"Internet Explorer_Server" ||
      std::wstring(name)==L"SysListView32")   //IE访问FILE://协议的URL，IE子窗口类名为SysListView32，也需要处理
    {
      IEEvent* pEvent = (IEEvent*)lParam;
      pEvent->SetIEHwnd(hwndChild);
      return FALSE;
    }
    return TRUE;
  }

  IEEvent::IEEvent(IEEventHandler* delegate, DocHostUIHandler* pDocHost)
    :comRefCount(0),
    pWebBrowser(NULL),
    pHtmlDoc(NULL),
    pHtmlDoc6(NULL),
    pJScript(NULL),
    pHtmlWnd(NULL),
    pDelegate(NULL),
    pDocHostUIHandler(pDocHost),
    // forward(VARIANT_FALSE),
    // back(VARIANT_FALSE),
    hChildWnd(NULL),
    hParentWnd(NULL),
    IEHwnd(NULL),
    isFindIE(false),
    pListenClickDoc2(NULL),
    dwCookie(0),
    pDocumentEvent(NULL),
    refreshFlg(false)
  {
    pDelegate = delegate;
    pDocumentEvent = new IEDocumentEvent(this);
  }

  IEEvent::~IEEvent()
  {
    if (dwCookie)
    {
      if (pListenClickDoc2)
      {
        IConnectionPointContainer* pCpc = NULL;
        pListenClickDoc2->QueryInterface(IID_IConnectionPointContainer, (void**)&pCpc);
        if (pCpc)
        {
          IConnectionPoint* pCP = NULL;
          pCpc->FindConnectionPoint(DIID_HTMLDocumentEvents2, &pCP);
          if(pCP && dwCookie!=0)
            pCP->Unadvise(dwCookie);
          pCpc->Release();
        }
      }
    }
    if (pDocumentEvent)
      delete pDocumentEvent;
    if (pWebBrowser)
    {
      pWebBrowser->Release();
    }
  }

  int IEEvent::GetDocMode()
  {
    if (pHtmlDoc6)
    {
      VARIANT mode;
      HRESULT hr= pHtmlDoc6->get_documentMode(&mode);
      if (SUCCEEDED(hr))
      {
        int ret = (int)mode.fltVal;
        return ret;
      }
    }
    return 0;
  }

  bool IEEvent::SetDocMode(IEDocumentMode mode)
  {
    if (pHtmlDoc)
    {
      CComPtr<IOleCommandTarget> pOleTarget = NULL;
      HRESULT hr= pHtmlDoc->QueryInterface(IID_IOleCommandTarget, (void**)&pOleTarget);
      if (FAILED(hr) || pOleTarget == NULL)
      {
        return false;
      }
      NotifyRefresh(true);
      VARIANT param;
      param.vt = VT_I4;
      param.intVal = mode;
      hr = pOleTarget->Exec(&CGID_MshtmlPriv, MSHTMLPRIV_SETSESSIONDOCUMENTMODE, OLECMDEXECOPT_DODEFAULT, &param, NULL);
      if (FAILED(hr))
        return false;
      hr = pOleTarget->Exec(NULL, OLECMDID_REFRESH, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
      if (FAILED(hr))
        return false;
      return true;
    }
    return false;
  }

  HRESULT IEEvent::QueryInterface(REFIID riid, void ** ppvObject)
  {
    if (riid == __uuidof(IUnknown))
    {
      (*ppvObject) = static_cast<IUnknown*>(this);
    }
    else if (riid == __uuidof(IDispatch))
    {
      (*ppvObject) = static_cast<IDispatch*>(this);
    }
    else if (riid == __uuidof(DWebBrowserEvents2))
    {
      (*ppvObject) = static_cast<DWebBrowserEvents2*>(this);
    }
    else
    {
      return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
  }

  ULONG IEEvent::AddRef(void)
  {
    InterlockedIncrement(&comRefCount);
    return comRefCount;
  }

  ULONG IEEvent::Release(void)
  {
    InterlockedDecrement(&comRefCount);
    return comRefCount;
  }

  HRESULT IEEvent::GetTypeInfoCount(UINT * pctinfo)
  {
    *pctinfo = 0;
    return S_OK;
  }

  HRESULT IEEvent::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo)
  {
    *ppTInfo = NULL;
    return E_NOTIMPL;
  }

  HRESULT IEEvent::GetIDsOfNames(REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId)
  {
    return S_OK;
  }

  HRESULT IEEvent::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr)
  {
    switch (dispIdMember)
    {
    case DISPID_NAVIGATECOMPLETE2:
    {
      if (pDispParams->rgvarg[0].vt == (VT_VARIANT | VT_BYREF))
      {
        variant_t variant= *(pDispParams->rgvarg[0].pvarVal);
        if (variant.vt == VT_BSTR)
        {
          bstr_t url = variant.bstrVal;
          bool findUrl = false;
          std::list<std::wstring>::iterator iter = beforeNavUrlList.begin();
          for (; iter != beforeNavUrlList.end(); iter++)
          {
            if (*iter == url.GetBSTR())
            {
              beforeNavUrlList.erase(iter);
              findUrl = true;
              break;
            }
          }
          if (findUrl == false && pDelegate)
          {
            pDelegate->OnBeforeNavigate(url.GetBSTR(), false);
          }
        }
      }
      break;
    }
    case  DISPID_BEFORENAVIGATE2:
    {
      if (pDispParams->rgvarg[5].vt == (VT_VARIANT | VT_BYREF))
      {

        //if (pDispParams->rgvarg[4].vt == (VT_VARIANT | VT_BYREF))
        //{
        //  VARIANT* vv=NULL;
        //  int ff = 0;
        //  vv = (VARIANT*)pDispParams->rgvarg[4].byref;
        //  ff = vv->intVal;
        //  if (ff == 64)
        //  {
        //    int a = 10;
        //    a++;
        //  }
        //}

        VARIANT* vtemp = (VARIANT*)pDispParams->rgvarg[5].byref;
        bstr_t url = vtemp->bstrVal;
        beforeNavUrlList.push_back(url.GetBSTR());
        GURL gurl(url.GetBSTR());
        if (pDelegate)
        {
          bool cancel = false;
          bool navNewUrl = false;
          if (GURL(clickUrl) == gurl)  //用户是否点击URL，通知主进程是否需要更新前进后退列表
            navNewUrl = true;
          cancel = pDelegate->OnBeforeNavigate(url.GetBSTR(), navNewUrl);
          if (cancel)
          {
            ((VARIANT*)pDispParams->rgvarg[0].byref)->vt = VT_BOOL;
            ((VARIANT*)pDispParams->rgvarg[0].byref)->boolVal = VARIANT_TRUE;
          }
        }
        clickUrl = L"";
      }
      break;
    }
    case DISPID_DOWNLOADCOMPLETE:
    {
      if (pHtmlDoc == NULL) //将自己实现的IDocHostUIHandler接口替换IE内核创建的，这样能够获取IE内核创建IDocHostUIHandler接口中的GetExternal接口
      {
        CComPtr<IDispatch> pDisp;
        if (pWebBrowser)
          pWebBrowser->get_Document(&pDisp);
        if (pDisp)
        {
          pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pHtmlDoc);
          if (pHtmlDoc)
          {
            CComPtr<IOleObject> pOleObj;
            pHtmlDoc->QueryInterface(IID_IOleObject, (void**)&pOleObj);
            CComPtr<IOleClientSite> pClientSite;
            pOleObj->GetClientSite(&pClientSite);
            if (pClientSite)
            {
              pClientSite->QueryInterface(IID_IDocHostUIHandler, (void**)&pOrgDocHost);

              pDocHostUIHandler->SetOrgDocHost(pOrgDocHost);
              CComPtr<ICustomDoc> pCustomDoc;
              pHtmlDoc->QueryInterface(IID_ICustomDoc, (void**)&pCustomDoc);
              pCustomDoc->SetUIHandler(pDocHostUIHandler);
            }
          }
        }
      }

      if (refreshFlg == false)
        break;
      if (dwCookie == 0 && pWebBrowser)
      {
        CComPtr<IDispatch> pDisp;
        pWebBrowser->get_Document(&pDisp);
        if (pDisp == NULL)
          break;
        CComPtr<IHTMLDocument2> pDoc2;
        pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc2);
        if (pDoc2 == NULL)
          break;
        ListenDocmentEvent(pDoc2);
      }
    }
    break;
    case  DISPID_DOCUMENTCOMPLETE:
    {
      if (pDispParams->rgvarg[1].vt == VT_DISPATCH)
      {
        IDispatch* pDisp = pDispParams->rgvarg[1].pdispVal;
        if (pDisp == pWebBrowser)
        {
          if (pDispParams->rgvarg[0].vt != (VT_VARIANT | VT_BYREF)) //Delegate handler
            return S_OK;
          VARIANT* vtemp = (VARIANT*)pDispParams->rgvarg[0].byref;
          bstr_t url = vtemp->bstrVal;
          if(url.GetBSTR())
            OnFinishLoad(url.GetBSTR());
        }
      }
    }
    break;
    case DISPID_TITLECHANGE:
    {
      if (pDispParams->rgvarg[0].vt == VT_BSTR)
      {
        //为了解决chrome49中 IE无法直接在Chrome里创建子窗口的问题，先让IE建立桌面的子窗口，待窗口创建完成后再将IE设为Chrome的子窗口。
        HWND hPar = ::GetParent(hChildWnd);
        if (::IsWindow(hChildWnd) && ::IsWindow(hParentWnd) && hPar != hParentWnd)
        {

          DWORD err = 0;
          HWND mainHwnd = GetParent(hParentWnd);
          long rr = 0;
          rr = SetWindowLong(hChildWnd, GWL_HWNDPARENT, (LONG)mainHwnd);
          if (rr == 0)
          {
            err = GetLastError();
          }

          HWND hwnd = ::SetParent(hChildWnd, hParentWnd);
          long style = GetWindowLong(hChildWnd, GWL_STYLE);
          style = style & ~WS_POPUP & ~WS_MAXIMIZEBOX;
          style = style | WS_CHILD;
          SetWindowLong(hChildWnd, GWL_STYLE, style);

          ::SetForegroundWindow(hChildWnd);
          if (hwnd == NULL)
          {
            err = GetLastError();
          }
        }
        if(isFindIE==false)
        {
          HWND hWndFound = hChildWnd;
          IEHwnd = NULL;
          lastHwnd = 0;
          BOOL ret = ::EnumChildWindows(hWndFound, EnumChildProc, (LPARAM)this);
          if (::IsWindow(IEHwnd))
          {
            isFindIE = true;
            ShowWindow(hChildWnd, SW_SHOW);
            if (IEHwnd && pDelegate)
              pDelegate->SetIEHwnd(IEHwnd);
          }
          if (ret == FALSE)
          {
            break;
          }
        }

        bstr_t title = pDispParams->rgvarg[0].bstrVal;
        if (pDelegate)
        {
          pDelegate->OnTitleChange(title.GetBSTR());
        }
      }
      break;
    }
    case DISPID_NEWWINDOW3:
    {
      if (pDispParams->rgvarg[0].vt == (VT_BSTR))
      {
        bstr_t url = pDispParams->rgvarg[0].bstrVal;
        // nwmf == 0x00000006 (NWMF_USERINITED | NWMF_FIRST_USERINITED) 是点击链接打开新窗口
        // nwmf == 0x00010086 是从右键菜单中选择在新窗口打开
        // nwmf == 0x00040004 目前已知是弹出窗口
        long flag = IE::NewWindowFlag::TRANSITION;
        long nwmf=pDispParams->rgvarg[2].intVal;
        if(((nwmf & 0x40000) == 0x40000 &&
          (nwmf & 0x20000) == 0) ||
          (nwmf & 0x10000) == 0x10000)
          flag = IE::NewWindowFlag::POPUP;
        else if ((nwmf & 0x00000006) == 0x00000006 ||
          (nwmf & 0x00010086) == 0x00010086 ||
          (nwmf & 0x00000002) == 0x00000002)
          flag = IE::NewWindowFlag::TRANSITION;
        // comment unused part by webb.
        // VARIANT vt = pDispParams->rgvarg[4];
        IDispatch* pdisp = NULL;
        if (pDelegate)
        {
          bool cancel = pDelegate->OnLoadUrlNewWnd(url.GetBSTR(), flag, &pdisp);
          if (pdisp)
          {
            *(pDispParams->rgvarg[4].ppdispVal) = pdisp;
          }
          if (cancel)
          {
            pDispParams->rgvarg[3].vt = VT_BOOL | VT_BYREF;
            *(pDispParams->rgvarg[3].pboolVal) = VARIANT_TRUE;
          }
        }
      }
      break;
    }
    case DISPID_WINDOWSETLEFT:
    {
      if (pDelegate && pDispParams->rgvarg[0].vt == VT_I4)
      {
        int left = pDispParams->rgvarg[0].intVal;
        RECT rc;
        pDelegate->OnGetMainWndPos(&rc);
        int width = rc.right - rc.left;
        pDelegate->OnRequestChangePos(left, rc.top, width, rc.bottom - rc.top);
      }
      break;
    }
    case DISPID_WINDOWSETTOP:
    {
      if (pDelegate && pDispParams->rgvarg[0].vt == VT_I4)
      {
        int top = pDispParams->rgvarg[0].intVal;
        RECT rc;
        pDelegate->OnGetMainWndPos(&rc);
        int height = rc.bottom - rc.top;
        pDelegate->OnRequestChangePos(rc.left, top, rc.right - rc.left, height);
      }
      break;
    }
    case DISPID_WINDOWSETWIDTH:
    {
      if (pDelegate && pDispParams->rgvarg[0].vt == VT_I4)
      {
        int width = pDispParams->rgvarg[0].intVal;
        RECT rc;

        GetWindowRect(hParentWnd, &rc);
        int x = rc.right - rc.left;

        pDelegate->OnGetMainWndPos(&rc);
        //rc.right = rc.right + x;
        int newWidth = rc.right -rc.left- x+width;
        //pDelegate->OnRequestChangePos(rc.left, rc.top, width, rc.bottom - rc.top);
        pDelegate->OnRequestChangePos(rc.left, rc.top, newWidth, rc.bottom - rc.top);
      }
      break;
    }
    case DISPID_WINDOWSETHEIGHT:
    {
      if (pDelegate && pDispParams->rgvarg[0].vt == VT_I4)
      {
        int height = pDispParams->rgvarg[0].intVal;
        RECT rc;

        GetWindowRect(hParentWnd, &rc);
        int y = rc.bottom - rc.top;

        pDelegate->OnGetMainWndPos(&rc);
        int newHeight = rc.bottom-rc.top-y+height;
        //rc.bottom = rc.bottom - rc.top + y;
        //pDelegate->OnRequestChangePos(rc.left, rc.top, rc.right-rc.left, height);
        pDelegate->OnRequestChangePos(rc.left, rc.top, rc.right - rc.left, newHeight);
      }
      break;
    }
    case DISPID_WINDOWCLOSING:
    {
      //IE js执行window.close()方法时，会触发此事件，但此时不能通知主进程IE关闭了自己，
      //因为IE在通知此事件后可能还会有关闭IE页面提示、后续新开页面事件等，
      //所以将通知主进程IE关闭的消息改在通过拦截WM_DESTROY消息时通知主进程，具体参看WebBrowser::SubClassControlWndProc 方法中的WM_DESTROY消息处理
      break;
    }
    case DISPID_CLIENTTOHOSTWINDOW:
    {
      //int* y = pDispParams->rgvarg[0].pintVal;
      //int* x = pDispParams->rgvarg[1].pintVal;
      //POINT pt;
      //pt.x = *x;
      //pt.y = *y;
      //HWND hwnd = GetParent(hChildWnd);
      //RECT rt;
      //GetWindowRect(hwnd, &rt);
      //GetWindowRect(hChildWnd, &rt);
      //GetWindowRect(hParentWnd, &rt);
      //int a = 10;
      //a++;
      break;
    }
    }
    return S_OK;
  }

  void IEEvent::SetWebBrowser(IWebBrowser2 * web)
  {
    pWebBrowser = web;
    pWebBrowser->AddRef();
    if (pDocumentEvent)
      pDocumentEvent->SetWebBrowser(web);
  }


  HRESULT IEEvent::ExcuteJs(std::wstring funName, std::vector<variant_t> params, variant_t * result)
  {
    if (pJScript == NULL)
      return E_UNEXPECTED;
    bstr_t name = funName.c_str();
    long dispID = 0;
    HRESULT hr = pJScript->GetIDsOfNames(IID_NULL, &name.GetBSTR(), 1, LOCALE_SYSTEM_DEFAULT, &dispID);
    if (FAILED(hr))
      return E_INVALIDARG;
    DISPPARAMS dispParams;
    memset(&dispParams, 0, sizeof(DISPPARAMS));
    dispParams.cArgs = params.size();
    dispParams.rgvarg = new VARIANT[dispParams.cArgs];
    dispParams.cNamedArgs = 0;
    int i = dispParams.cArgs - 1;
    int j = 0;
    for (; i >= 0; i--)
    {
      dispParams.rgvarg[j] = params[i];
      j++;
    }
    VARIANT ret;
    EXCEPINFO execInfo;
    memset(&execInfo, 0, sizeof(EXCEPINFO));
    UINT uArgError = 0;
    hr = pJScript->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispParams, &ret, &execInfo, &uArgError);
    if (dispParams.rgvarg)
      delete[] dispParams.rgvarg;
    return hr;
  }

  HRESULT IEEvent::ExcuteJs(std::wstring code, variant_t * result)
  {
    if (pHtmlWnd == NULL)
      return E_UNEXPECTED;
    LPOLESTR ss = L"eval";
    DISPID id = -1;
    HRESULT hr = pHtmlWnd->GetIDsOfNames(IID_NULL, &ss, 1, LOCALE_SYSTEM_DEFAULT, &id);
    if (FAILED(hr))
      return hr;
    DISPPARAMS dispParams;
    memset(&dispParams, 0, sizeof(DISPPARAMS));
    dispParams.cArgs = 1;
    dispParams.cNamedArgs = 0;
    dispParams.rgvarg = new VARIANT[dispParams.cArgs];
    dispParams.rgvarg[0].vt = VT_BSTR;
    bstr_t src = code.c_str();
    dispParams.rgvarg[0].bstrVal = src;
    EXCEPINFO execInfo;
    memset(&execInfo, 0, sizeof(EXCEPINFO));
    UINT uArgError = 0;
    hr = pHtmlWnd->Invoke(id, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispParams, result, &execInfo, &uArgError);
    delete[] dispParams.rgvarg;
    return hr;
  }
  void IEEvent::SetChildHWND(HWND hWnd)
  {
    hChildWnd = hWnd;
  }

  void IEEvent::SetParentHwnd(HWND hWnd)
  {
    hParentWnd = hWnd;
  }

  void IEEvent::SetIEHwnd(HWND hwnd)
  {
    IEHwnd = hwnd;
  }

  void IEEvent::SetClickUrl(const std::wstring url)
  {
    clickUrl = url;
  }

  void IEEvent::NotifyRefresh(bool isStart)
  {
    if (isStart)
      refreshFlg = true;
    else
      refreshFlg = false;
    if (pDocumentEvent)
      pDocumentEvent->SetRefresh(isStart);
  }

  void IEEvent::OnFinishLoad(const std::wstring & url)
  {
    if (pWebBrowser == NULL)
      return;

    CComPtr<IDispatch> pDisp;
    HRESULT hr = pWebBrowser->get_Document(&pDisp); //Get IHtmlDoc interface
    if (SUCCEEDED(hr) && pDisp)
    {
      hr = pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pHtmlDoc);
      if (pHtmlDoc)
      {
        hr = pHtmlDoc->QueryInterface(IID_IHTMLDocument6, (void**)&pHtmlDoc6);
        hr = pHtmlDoc->get_Script((IDispatch**)&pJScript);
        //vector<variant_t> args;
        //variant_t args1;
        //args1 = 1;
        //args.push_back(args1);
        //variant_t args2;
        //args2 = L"哈哈";
        //args.push_back(args2);
        //variant_t ret;
        //ExcuteJs(L"myHello", args, &ret);

        clickUrl = L"";

        GURL gurl(url);
        if (gurl.SchemeIsHTTPOrHTTPS())
        {
          IHTMLWindow2* pw = NULL;
          pHtmlDoc->get_parentWindow(&pw);
          pw->QueryInterface(IID_IDispatch, (void**)&pHtmlWnd);
          pw->Release();
          ListenDocmentEvent(pHtmlDoc);
        }
      }
      NotifyRefresh(false);

      std::vector<std::string> faviconUrls;
      GetFaviconUrls(base::UTF16ToASCII(url), faviconUrls);

      std::string strFaviconUrls;
      for (auto it = faviconUrls.begin(); it != faviconUrls.end(); it++)
      {
        strFaviconUrls += *it;
        strFaviconUrls += '\t';
      }

      if (pDelegate)
        pDelegate->OnFinishNavigate(url, base::ASCIIToUTF16(strFaviconUrls));
    }
  }

  void IEEvent::OnClickHtmlElement(CComPtr<IHTMLElement> clickElement)
  {
    if (pDelegate)
      pDelegate->OnClickElement(clickElement,false);
  }

  void IEEvent::OnEnterKeyPressedHtmlElement(CComPtr<IHTMLElement> clickElement)
  {
    if (pDelegate)
      pDelegate->OnClickElement(clickElement, true);
  }

  void IEEvent::ListenDocmentEvent(IHTMLDocument2 * pDoc2)
  {
    if (pDocumentEvent==NULL || pDoc2==NULL)
      return;

    if (pListenClickDoc2)
    {
      IConnectionPointContainerPtr pUnCPC = NULL;
      pListenClickDoc2->QueryInterface(IID_IConnectionPointContainer, (void**)&pUnCPC);
      if (pUnCPC)
      {
        IConnectionPoint* pUnCP = NULL;
        pUnCPC->FindConnectionPoint(DIID_HTMLDocumentEvents2, &pUnCP);
        if (pUnCP && dwCookie != 0)
          pUnCP->Unadvise(dwCookie);
        dwCookie = 0;
        pUnCPC->Release();
      }
      pListenClickDoc2 = NULL;
    }
    IConnectionPointContainerPtr pCPC = NULL;
    pDoc2->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
    if (pCPC)
    {
      IConnectionPoint* pCP = NULL;
      pCPC->FindConnectionPoint(DIID_HTMLDocumentEvents2, &pCP);
      pCP->Advise(pDocumentEvent, &dwCookie);
      pCPC->Release();
    }
    pListenClickDoc2 = pDoc2;
  }

  void IEEvent::ResetDocHostUIHandler()
  {
    if (pHtmlDoc && pOrgDocHost && pDocHostUIHandler)
    {
      pDocHostUIHandler->SetOrgDocHost(NULL);
      CComPtr<ICustomDoc> pCustomDoc;
      pHtmlDoc->QueryInterface(IID_ICustomDoc, (void**)&pCustomDoc);
      pCustomDoc->SetUIHandler(pOrgDocHost);
      pOrgDocHost = NULL;
    }
  }

  void IEEvent::GetFaviconUrls(std::string pageUrl, std::vector<std::string>& faviconUrls)
  {
    faviconUrls.clear();
    GURL page_gurl = GURL(pageUrl);
    do
    {
      HRESULT hr = S_OK;
      if (pWebBrowser == NULL) break;

      CComPtr<IDispatch> pDoc = NULL;
      hr = pWebBrowser->get_Document(&pDoc);
      if (!SUCCEEDED(hr) || pDoc == NULL) break;

      CComQIPtr<IHTMLDocument3> pDoc3(pDoc);
      if (pDoc3 == NULL) break;

      CComPtr<IHTMLElementCollection> pLinkElements = NULL;
      hr = pDoc3->getElementsByTagName(L"link", &pLinkElements);
      if (!SUCCEEDED(hr) || pLinkElements == NULL) break;

      long nCount = 0;
      hr = pLinkElements->get_length(&nCount);
      DLOG(INFO) << "GetFaviconUrls: nCount = " << nCount;
      if (!SUCCEEDED(hr) || nCount <= 0) break;

      for (long i = 0; i < nCount; i++)
      {
        CComVariant varName(i);
        varName.ChangeType(VT_UINT);
        CComVariant varIndex;
        CComPtr<IDispatch> pElementDisp;
        hr = pLinkElements->item(varName, varIndex, &pElementDisp);
        if (!SUCCEEDED(hr) || pElementDisp == NULL) continue;

        CComQIPtr<IHTMLLinkElement, &IID_IHTMLLinkElement> pLinkElement(pElementDisp);
        if (pLinkElement == NULL) continue;

        CComBSTR rel = NULL;
        pLinkElement->get_rel(&rel);
        if (rel == NULL) continue;
        rel.ToLower();
        if (rel != "icon" && rel != "shortcut icon") continue;

        CComBSTR href = NULL;
        pLinkElement->get_href(&href);
        if (href == NULL) continue;

        std::string strHref = base::UTF16ToASCII(std::wstring(href));
        if (strHref.size() < 2) continue;
        if (strHref[0] == '/' && strHref[1] == '/')
        {
          strHref = page_gurl.scheme() + ":" + strHref;
        }

        std::string favicon_url;
        GURL temp = GURL(strHref);
        if (temp.is_valid())
        {
          favicon_url = strHref;
        }
        else
        {
          favicon_url = page_gurl.scheme() + "://";
          favicon_url += page_gurl.host();
          if (page_gurl.has_port())
          {
            favicon_url += ":";
            favicon_url += page_gurl.port();
          }
          favicon_url += strHref;
        }
        faviconUrls.push_back(favicon_url);
      }
    } while (0);

    if (faviconUrls.size() == 0)
    {
      std::string favicon_url = page_gurl.scheme() + "://";
      favicon_url += page_gurl.host();
      if (page_gurl.has_port())
      {
        favicon_url += ":";
        favicon_url += page_gurl.port();
      }
      favicon_url += "/favicon.ico";
      faviconUrls.push_back(favicon_url);
    }
  }

  IEDocumentEvent::IEDocumentEvent(IEEvent* pEvent)
    :comRefCount(0),
    pIEEvent(pEvent),
    pWebBrowser2(NULL),
    refreshFlg(false)
  {

  }

  IEDocumentEvent::~IEDocumentEvent()
  {
    if (pWebBrowser2)
    {
      pWebBrowser2->Release();
    }
  }

  void IEDocumentEvent::SetWebBrowser(IWebBrowser2 * pWeb2)
  {
    if (pWebBrowser2)
      pWebBrowser2->Release();
    pWebBrowser2 = pWeb2;
    pWebBrowser2->AddRef();
  }

  void IEDocumentEvent::SetRefresh(bool isRefresh)
  {
    refreshFlg = isRefresh;
  }

  HRESULT IEDocumentEvent::QueryInterface(REFIID riid, void ** ppvObject)
  {
    if (riid == __uuidof(IUnknown))
    {
      (*ppvObject) = static_cast<IUnknown*>(this);
    }
    else if (riid == __uuidof(IDispatch))
    {
      (*ppvObject) = static_cast<IDispatch*>(this);
    }
    else if (riid == __uuidof(HTMLDocumentEvents2))
    {
      (*ppvObject) = static_cast<HTMLDocumentEvents2*>(this);
    }
    else
    {
      return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
  }

  ULONG IEDocumentEvent::AddRef(void)
  {
    InterlockedIncrement(&comRefCount);
    return comRefCount;
  }

  ULONG IEDocumentEvent::Release(void)
  {
    InterlockedDecrement(&comRefCount);
    return comRefCount;
  }

  HRESULT IEDocumentEvent::GetTypeInfoCount(UINT * pctinfo)
  {
    *pctinfo = 0;
    return S_OK;
  }

  HRESULT IEDocumentEvent::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo)
  {
    *ppTInfo = NULL;
    return E_NOTIMPL;
  }

  HRESULT IEDocumentEvent::GetIDsOfNames(REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId)
  {
    return S_OK;
  }

  HRESULT IEDocumentEvent::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr)
  {
    if (dispIdMember == DISPID_HTMLDOCUMENTEVENTS2_ONCLICK)
    {
      IHTMLEventObj* pEvtObj = NULL;
      IDispatch* pDisp = pDispParams->rgvarg[0].pdispVal;
      pDisp->QueryInterface(IID_IHTMLEventObj, (void**)&pEvtObj);
      IHTMLElement* pElement = NULL;
      pEvtObj->get_srcElement(&pElement);
      pIEEvent->OnClickHtmlElement(pElement);
      while (pElement) // 逐层向上检查
      {
        _bstr_t strTagname;
        pElement->get_tagName(&strTagname.GetBSTR());
        if (_bstr_t("a") == strTagname || _bstr_t("A") == strTagname)
        {
          // 已找到 "a" 标签，在这里写相应代码
          _variant_t vHref;
          pElement->getAttribute(L"href", 0, &vHref);
          //pEvtObj->put_returnValue(_variant_t(VARIANT_TRUE, VT_BOOL));
          if (pIEEvent && vHref.vt == VT_BSTR && vHref.bstrVal)
          {
            pIEEvent->SetClickUrl(vHref.bstrVal);
          }
          pElement->Release();
          break;
        }
        IHTMLElement* pElementTemp = pElement;
        pElement->get_parentElement(&pElement);
        pElementTemp->Release();
      }
      pEvtObj->Release();
    }
    else if (dispIdMember == DISPID_HTMLDOCUMENTEVENTS2_ONKEYPRESS)
    {
      IHTMLEventObj* pEvtObj = NULL;
      IDispatch* pDisp = pDispParams->rgvarg[0].pdispVal;
      pDisp->QueryInterface(IID_IHTMLEventObj, (void**)&pEvtObj);
      if (pEvtObj)
      {
        long keyCode = 0;
        pEvtObj->get_keyCode(&keyCode);
        if (keyCode == 13 || keyCode == 32)
        {
          IHTMLElement* pElement = NULL;
          pEvtObj->get_srcElement(&pElement);
          pIEEvent->OnEnterKeyPressedHtmlElement(pElement);
        }
        pEvtObj->Release();
      }
    }
    else if (dispIdMember == DISPID_HTMLDOCUMENTEVENTS2_ONREADYSTATECHANGE) //解决刷新没有DISPID_DOCUMENTCOMPLETE事件问题，在此消息中判断readyState判断是否刷新完成
    {
      if (pWebBrowser2 && refreshFlg && pIEEvent)
      {
        CComPtr<IDispatch> pDisp=NULL;
        pWebBrowser2->get_Document(&pDisp);
        if (pDisp)
        {
          CComPtr<IHTMLDocument2> pDoc2=NULL;
          pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc2);
          bstr_t state;
          pDoc2->get_readyState(state.GetAddress());
          if (std::wstring(state.GetBSTR()) == L"complete")
          {
            bstr_t url = L"";
            pDoc2->get_URL(url.GetAddress());
            if (url.GetBSTR())
              pIEEvent->OnFinishLoad(url.GetBSTR());
          }
        }
      }
    }
    return S_OK;
  }
}//namespace IE