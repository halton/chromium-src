#include "ATLInclude.h"
#include "DocHostUIHandler.h"
#include "JavaScriptHandler.h"
#include "WebBrowser.h"
#include "IEEventHandler.h"
#include <ExDisp.h>
#include <mshtml.h>
#include "url/gurl.h"
#include "base/strings/utf_string_conversions.h"
#include <mshtmcid.h>
#include <vector>
#include <Strsafe.h>
#include <WinInet.h>
#include <ShlGuid.h>
#include "content/common/IE/WebBrowser.h"
#include "IEVersion.h"

#include "content/public/browser/browser_thread.h"
#include "base/bind.h"
#include "base/message_loop/message_loop.h"


namespace IE {

  using namespace std;

  DocHostUIHandler::DocHostUIHandler(IEEventHandler* handler, WebBrowser* browser)
    :pEventHandler(NULL),
     pJSHandler(NULL),
     pWebBrowser(browser),
    mouseRightButtonEnabled(true),
    cutCopyEnabled(true),
    printEnabled(true)
  {
    comRefCount = 0;
    pEventHandler = handler;
  }


  DocHostUIHandler::~DocHostUIHandler()
  {
    if (pJSHandler)
    {
      delete pJSHandler;
    }
  }

  HRESULT DocHostUIHandler::QueryInterface(REFIID riid, void ** ppvObject)
  {
    if (riid == __uuidof(IUnknown))
    {
      (*ppvObject) = this;
    }
    else if (riid == __uuidof(IDocHostUIHandler))
    {
      (*ppvObject) = static_cast<IDocHostUIHandler*>(this);
    }
    else if (riid == __uuidof(IOleCommandTarget))
    {
      (*ppvObject) = static_cast<IOleCommandTarget*>(this);
    }
    else
    {
      return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
  }

  ULONG DocHostUIHandler::AddRef(void)
  {
    InterlockedIncrement(&comRefCount);
    return comRefCount;
  }

  ULONG DocHostUIHandler::Release(void)
  {
    InterlockedDecrement(&comRefCount);
    return comRefCount;
  }

  HRESULT DocHostUIHandler::ShowContextMenu(DWORD dwID, POINT * ppt, IUnknown * pcmdtReserved, IDispatch * pdispReserved)
  {
    if (pdispReserved == NULL)
      return E_NOTIMPL;

    if (mouseRightButtonEnabled == false)
      return S_OK;

    HRESULT hr;
    IOleCommandTarget* pCmdTarget =NULL;
    if (pcmdtReserved == NULL)
      return E_NOTIMPL;
    pcmdtReserved->QueryInterface(IID_IOleCommandTarget, (void**)&pCmdTarget);
    if (pCmdTarget == NULL)
      return E_NOTIMPL;

    HMENU hMenu = BuildMenu(dwID, pCmdTarget);
    pCmdTarget->Release();
    if (hMenu == NULL)
      return E_NOTIMPL;

    int iSelection = 0;
    IOleWindow  *oleWnd = NULL;
    hr = pcmdtReserved->QueryInterface(IID_IOleWindow, (void**)&oleWnd);
    HWND hwnd = NULL;
    hr = oleWnd->GetWindow(&hwnd);
    oleWnd->Release();

    iSelection = TrackPopupMenu(hMenu,TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                                  ppt->x, ppt->y, 0, hwnd, NULL);
    DestroyMenu(hMenu);

    switch (iSelection)
    {
    case IDM_COPYSHORTCUT:
    {
      IHTMLElement *pElem;
      HRESULT hr = pdispReserved->QueryInterface(IID_IHTMLElement,(void**)&pElem);
      if (pElem == NULL)
        return E_NOTIMPL;
      IHTMLAnchorElement* pAnchor = NULL;
      hr = pElem->QueryInterface(IID_IHTMLAnchorElement, (void**)&pAnchor);
      if (pAnchor == NULL)
        return E_NOTIMPL;
      BSTR bstr;
      pAnchor->get_href(&bstr);
      std::wstring url = bstr;
      SysFreeString(bstr);
      pAnchor->Release();
      pElem->Release();
      if (iSelection == IDM_COPYSHORTCUT)
        CopyToClipboard(url);
    }
    break;
    case IDM_GOBACKWARD:
      pEventHandler->GoBack();
      break;
    case IDM_GOFORWARD:
      pEventHandler->GoForward();
      break;
    case IDM_REFRESH:
      if (pWebBrowser)
        pWebBrowser->NotifyCommand(L"refresh");
      SendMessage(hwnd, WM_COMMAND, iSelection, NULL);
      break;
    case 0:
      break;
    default:
      SendMessage(hwnd, WM_COMMAND, iSelection, NULL);
      break;
    }
    return S_OK;
  }

  HRESULT DocHostUIHandler::GetHostInfo(DOCHOSTUIINFO * pInfo)
  {
    pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_THEME |
      DOCHOSTUIFLAG_NO3DOUTERBORDER/* | DOCHOSTUIFLAG_ENABLE_REDIRECT_NOTIFICATION*/;
    return S_OK;
  }

  HRESULT DocHostUIHandler::ShowUI(DWORD dwID, IOleInPlaceActiveObject * pActiveObject, IOleCommandTarget * pCommandTarget, IOleInPlaceFrame * pFrame, IOleInPlaceUIWindow * pDoc)
  {
    return S_OK;
  }

  HRESULT DocHostUIHandler::HideUI(void)
  {
    return S_OK;
  }

  HRESULT DocHostUIHandler::UpdateUI(void)
  {
    return S_OK;
  }

  HRESULT DocHostUIHandler::EnableModeless(BOOL fEnable)
  {
    return S_OK;
  }

  HRESULT DocHostUIHandler::OnDocWindowActivate(BOOL fActivate)
  {
    return S_OK;
  }

  HRESULT DocHostUIHandler::OnFrameWindowActivate(BOOL fActivate)
  {
    return S_OK;
  }

  HRESULT DocHostUIHandler::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow * pUIWindow, BOOL fRameWindow)
  {
    return S_OK;
  }

  HRESULT DocHostUIHandler::TranslateAccelerator(LPMSG lpMsg, const GUID * pguidCmdGroup, DWORD nCmdID)
  {
    return S_FALSE;
  }

  HRESULT DocHostUIHandler::GetOptionKeyPath(LPOLESTR * pchKey, DWORD dw)
  {
    HRESULT hr= E_NOTIMPL;
    //WCHAR* szKey = L"Software\\AllMobilize\\Redcore\\Trident";

    //size_t cbLength;
    //hr = StringCbLengthW(szKey, 1280, &cbLength);

    //if (pchKey)
    //{
    //  *pchKey = (LPOLESTR)CoTaskMemAlloc(cbLength + sizeof(WCHAR));
    //  if (*pchKey)
    //    hr = StringCbCopyW(*pchKey, cbLength + sizeof(WCHAR), szKey);
    //}

    return hr;
  }

  HRESULT DocHostUIHandler::GetDropTarget(IDropTarget * pDropTarget, IDropTarget ** ppDropTarget)
  {
    return E_NOTIMPL;
  }

  HRESULT DocHostUIHandler::GetExternal(IDispatch ** ppDispatch)
  {
    //if (pJSHandler == NULL)
    //  pJSHandler = new JavaScriptHandler;
    //*ppDispatch = pJSHandler;

    // if (pOrgDocHost)
    // {
    //   IDispatch* pDisp = NULL;
    //   pOrgDocHost->GetExternal(&pDisp);
    //   *ppDispatch = pDisp;
    // }
    return S_OK;
  }

  HRESULT DocHostUIHandler::TranslateUrl(DWORD dwTranslate, LPWSTR pchURLIn, LPWSTR * ppchURLOut)
  {
    return S_FALSE;
  }

  HRESULT DocHostUIHandler::FilterDataObject(IDataObject * pDO, IDataObject ** ppDORet)
  {
    return S_FALSE;
  }
  HRESULT DocHostUIHandler::QueryStatus(const GUID * pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT * pCmdText)
  {
    return E_NOTIMPL;
  }
  HRESULT DocHostUIHandler::Exec(const GUID * pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT * pvaIn, VARIANT * pvaOut)
  {
    if (nCmdID == OLECMDID_SHOWSCRIPTERROR)
    {
      //获取js脚本错误信息并log输出
      IHTMLDocument2* pDoc = NULL;
      IHTMLWindow2* pWindow = NULL;
      IHTMLEventObj* pEventObj = NULL;
      BSTR rgwszNames[5] =
      {
        SysAllocString(L"errorLine"),
        SysAllocString(L"errorCharacter"),
        SysAllocString(L"errorCode"),
        SysAllocString(L"errorMessage"),
        SysAllocString(L"errorUrl")
      };
      DISPID rgDispIDs[5];
      VARIANT rgvaEventInfo[5];

      HRESULT hr = E_FAIL;
      hr = pvaIn->punkVal->QueryInterface(IID_IHTMLDocument2, (void **)&pDoc);
      hr = pDoc->get_parentWindow(&pWindow);
      pDoc->Release();
      hr = pWindow->get_event(&pEventObj);

      if (pEventObj == NULL)
        return hr;
      std::wstring errMsg = L"";
      int i = 0;
      for (i = 0; i < 5; i++)
      {
        hr = pEventObj->GetIDsOfNames(IID_NULL, &rgwszNames[i], 1,
          LOCALE_SYSTEM_DEFAULT, &rgDispIDs[i]);

        DISPPARAMS params;
        params.cArgs = 0;
        params.cNamedArgs = 0;
        hr = pEventObj->Invoke(rgDispIDs[i], IID_NULL,
          LOCALE_SYSTEM_DEFAULT,
          DISPATCH_PROPERTYGET, &params, &rgvaEventInfo[i],
          NULL, NULL);

        std::wstring buffStr = L"";
        if (rgvaEventInfo[i].vt == VT_I4)
        {
          wchar_t temp[512] = { 0 };
          _itow_s(rgvaEventInfo[i].intVal,temp, 10);
          buffStr = temp;
        }
        else if (rgvaEventInfo[i].vt == VT_BSTR && rgvaEventInfo[i].bstrVal!=NULL)
        {
          buffStr = rgvaEventInfo[i].bstrVal;
        }
        errMsg = errMsg + rgwszNames[i] + L": " + buffStr + L" \r\n";
        VariantClear(&rgvaEventInfo[i]);
        SysFreeString(rgwszNames[i]);
      }
      LOG(WARNING) << "IE Javascript error : " << errMsg;
      pWindow->Release();
      pEventObj->Release();

      // 拦截js错误弹出框, 继续运行脚本.
      (*pvaOut).vt = VT_BOOL;
      (*pvaOut).boolVal = VARIANT_TRUE;
      return S_OK;
    }
    return E_NOTIMPL;
  }

  void DocHostUIHandler::EnableMouseRightButton(bool enable)
  {
    mouseRightButtonEnabled = enable;
  }

  void DocHostUIHandler::EnableCutCopy(bool enable)
  {
    cutCopyEnabled = enable;
  }

  void DocHostUIHandler::EnablePrint(bool enable)
  {
    printEnabled = enable;
  }

  void DocHostUIHandler::EnableSaveFile(bool enable)
  {
    saveFileEnabled = enable;
  }

  void DocHostUIHandler::SetOrgDocHost(IDocHostUIHandler * pDocHost)
  {
    pOrgDocHost = pDocHost;
  }

  HMENU DocHostUIHandler::BuildMenu(DWORD dwID, IOleCommandTarget * pCmdTarget)
  {
    HMENU hMenu = CreatePopupMenu();
    vector<pair<OLECMD, wstring>> menuItemArry;
    switch (dwID)
    {
    case CONTEXT_MENU_DEFAULT:
    {
      if (pEventHandler)
      {
        bool goBack = false;
        bool goForward = false;
        pEventHandler->GetGoBackOrForwardState(&goBack, &goForward);
        if (goBack)
          AppendMenu(hMenu, MF_STRING, IDM_GOBACKWARD, L"返回");
        else
          AppendMenu(hMenu, MF_STRING | MF_DISABLED | MF_GRAYED, IDM_GOBACKWARD, L"返回");
        if (goForward)
          AppendMenu(hMenu, MF_STRING, IDM_GOFORWARD, L"前进");
        else
          AppendMenu(hMenu, MF_STRING | MF_DISABLED | MF_GRAYED, IDM_GOFORWARD, L"前进");
      }
      OLECMD cmd;
      cmd.cmdID = IDM_REFRESH;
      cmd.cmdf = 0;
      menuItemArry.push_back(make_pair(cmd, L"重新加载"));

      if (saveFileEnabled)
      {
        cmd.cmdID = IDM_SAVEAS;
        menuItemArry.push_back(make_pair(cmd, L"另存为..."));
      }

      if (printEnabled)
      {
        cmd.cmdID = IDM_PRINTPREVIEW;
        menuItemArry.push_back(make_pair(cmd, L"打印..."));
      }
    }
    break;
    case CONTEXT_MENU_IMAGE:
    {
      OLECMD cmd;
      cmd.cmdf = 0;

      if (saveFileEnabled)
      {
        cmd.cmdID = IDM_SAVEPICTURE;
        menuItemArry.push_back(make_pair(cmd, L"图片另存为"));
      }

      if (cutCopyEnabled)
      {
        cmd.cmdID = IDM_COPY;
        menuItemArry.push_back(make_pair(cmd, L"复制图片"));
      }

      cmd.cmdID = IDM_FOLLOWLINKN;
      menuItemArry.push_back(make_pair(cmd, L"在新标签页中打开链接"));
    }
    break;
    case CONTEXT_MENU_TEXTSELECT:
    {
      OLECMD cmd;
      cmd.cmdf = 0;
      if (cutCopyEnabled)
      {
        cmd.cmdID = IDM_CUT;
        menuItemArry.push_back(make_pair(cmd, L"剪切"));

        cmd.cmdID = IDM_COPY;
        menuItemArry.push_back(make_pair(cmd, L"复制"));
      }

      cmd.cmdID = IDM_PASTE;
      menuItemArry.push_back(make_pair(cmd, L"粘贴"));

      cmd.cmdID = IDM_SELECTALL;
      menuItemArry.push_back(make_pair(cmd, L"全选"));
    }
    break;
    case CONTEXT_MENU_ANCHOR:
    {
      OLECMD cmd;
      cmd.cmdID = IDM_FOLLOWLINKN;
      cmd.cmdf = 0;
      menuItemArry.push_back(make_pair(cmd, L"在新标签页中打开链接"));

      if (cutCopyEnabled)
      {
        cmd.cmdID = IDM_COPYSHORTCUT;
        menuItemArry.push_back(make_pair(cmd, L"复制链接地址"));
      }

      if (saveFileEnabled)
      {
        cmd.cmdID = IDM_SAVETARGET;
        menuItemArry.push_back(make_pair(cmd, L"链接另存为"));
      }
    }
    break;
    case CONTEXT_MENU_CONTROL:
    {
      OLECMD cmd;
      cmd.cmdID = IDM_UNDO;
      cmd.cmdf = 0;
      menuItemArry.push_back(make_pair(cmd, L"撤销"));

      cmd.cmdID = MF_SEPARATOR;
      menuItemArry.push_back(make_pair(cmd, L""));

      if (cutCopyEnabled)
      {
        cmd.cmdID = IDM_CUT;
        menuItemArry.push_back(make_pair(cmd, L"剪切"));

        cmd.cmdID = IDM_COPY;
        menuItemArry.push_back(make_pair(cmd, L"复制"));
      }

      cmd.cmdID = IDM_PASTE;
      menuItemArry.push_back(make_pair(cmd, L"粘贴"));

      cmd.cmdID = IDM_DELETE;
      menuItemArry.push_back(make_pair(cmd, L"删除"));

      cmd.cmdID = IDM_SELECTALL;
      menuItemArry.push_back(make_pair(cmd, L"全选"));
    }
    break;
    }

    size_t i = 0;
    for (i = 0; i < menuItemArry.size(); i++)
    {
      if (menuItemArry[i].first.cmdID == MF_SEPARATOR)
      {
        AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
        continue;
      }
      HRESULT hr = pCmdTarget->QueryStatus(&CGID_MSHTML, 1, &(menuItemArry[i].first), NULL);
      if (SUCCEEDED(hr))
      {
        if ((menuItemArry[i].first.cmdf & OLECMDF_ENABLED) == OLECMDF_ENABLED)
          AppendMenu(hMenu, MF_STRING, menuItemArry[i].first.cmdID, menuItemArry[i].second.c_str());
        else if ((menuItemArry[i].first.cmdf & OLECMDF_SUPPORTED) == OLECMDF_SUPPORTED)
          AppendMenu(hMenu, MF_STRING | MF_DISABLED | MF_GRAYED, menuItemArry[i].first.cmdID, menuItemArry[i].second.c_str());
      }
    }
    return hMenu;
  }

  void DocHostUIHandler::CopyToClipboard(const std::wstring & txt)
  {
    if (OpenClipboard(NULL))
    {
      EmptyClipboard();

      HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(wchar_t)*(txt.size() + 1));
      wchar_t* pData = (wchar_t*)GlobalLock(hData);
      memcpy(pData, txt.c_str(), sizeof(wchar_t)*(txt.size()));
      pData[txt.size()] = 0;
      GlobalUnlock(hData);

      SetClipboardData(CF_UNICODETEXT, hData);
      CloseClipboard();
    }
  }

  ServiceProvider::ServiceProvider(IEEventHandler* handler)
  {
    comRefCount = 0;
    pDownLoadMgr = NULL;
    pHtmlOM = NULL;
    pAuthenticate = NULL;
    pDelegate = handler;
    pWebBrowser = NULL;
    pNewWndMgr = NULL;
  }

  ServiceProvider::~ServiceProvider()
  {
    if (pDownLoadMgr)
      delete pDownLoadMgr;
    if (pHtmlOM)
      delete pHtmlOM;
    if (pAuthenticate)
      delete pAuthenticate;
    if (pNewWndMgr)
      delete pNewWndMgr;
    if (pWebBrowser)
      pWebBrowser->Release();
  }

  void ServiceProvider::SetWebBrowser(IWebBrowser2 * pWeb2)
  {
    if (pWebBrowser)
    {
      pWebBrowser->Release();
      pWebBrowser = NULL;
    }
    pWebBrowser = pWeb2;
    pWebBrowser->AddRef();
  }

  ULONG ServiceProvider::AddRef()
  {
    InterlockedIncrement(&comRefCount);
    return comRefCount;
  }

  ULONG ServiceProvider::Release()
  {
    InterlockedDecrement(&comRefCount);
    return comRefCount;
  }

  HRESULT ServiceProvider::QueryInterface(REFIID riid, void**ppvObject)
  {
    if (riid == __uuidof(IUnknown))
    {
      (*ppvObject) = this;
    }
    else if (riid == __uuidof(IServiceProvider))
    {
      (*ppvObject) = static_cast<IServiceProvider*>(this);
    }
    else
    {
      return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
  }

  HRESULT ServiceProvider::QueryService(
    /* [in] */ _In_ REFGUID guidService,
    /* [in] */ _In_ REFIID riid,
    /* [out] */ _Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)
  {
    HRESULT hr = E_NOINTERFACE;

    if (guidService == SID_SDownloadManager && riid == IID_IDownloadManager)
    {
      if (pDownLoadMgr == NULL)
        pDownLoadMgr = new DownLoadMgr;
      hr=pDownLoadMgr->QueryInterface(IID_IDownloadManager, ppvObject);
    }
    if (guidService == SID_SHTMLOMWindowServices && riid == IID_IHTMLOMWindowServices)
    {
      if (pHtmlOM == NULL)
        pHtmlOM = new HTMLOMWindowServices(pDelegate);
      hr = pHtmlOM->QueryInterface(IID_IHTMLOMWindowServices, ppvObject);
    }
    if (riid == IID_IAuthenticate)
    {
      if (pAuthenticate == NULL)
        pAuthenticate = new DomainAuthenticate(pDelegate, pWebBrowser);
      hr = pAuthenticate->QueryInterface(IID_IAuthenticate, ppvObject);
    }
    if (guidService == SID_SNewWindowManager && riid == IID_INewWindowManager)
    {
      if (pNewWndMgr == NULL)
        pNewWndMgr = new NewWindowManager(pDelegate);
      hr = pNewWndMgr->QueryInterface(IID_INewWindowManager, ppvObject);
    }
    return hr;
  }


  const LPOLESTR oBindCtxBSCBObjectParam = L"_BSCB_Holder_";

  DownLoadMgr::DownLoadMgr()
  {
    comRefCount = 0;
    pBinding = NULL;
    downloadUrl = L"";
    IsDownloadCanceled = false;
    fileSize = 0;
    dataReceiveSize = 0;
    pStream = NULL;
    isDownloading = false;
    pPrevBSCB = NULL;
    pBindCtx = NULL;
    isAbort = false;
  }

  DownLoadMgr::~DownLoadMgr()
  {
  }

  void DownLoadMgr::CancelDownload()
  {
    IsDownloadCanceled = true;
    isDownloading = false;
  }

  HRESULT DownLoadMgr::QueryInterface(REFIID riid, void ** ppvObject)
  {
    if (riid == __uuidof(IUnknown))
    {
      (*ppvObject) = this;
    }
    else if (riid == __uuidof(IDownloadManager))
    {
      (*ppvObject) = static_cast<IDownloadManager*>(this);
    }
    else if (riid == __uuidof(IBindStatusCallback))
    {
      (*ppvObject) = static_cast<IBindStatusCallback*>(this);
    }
    else if (riid == __uuidof(IBindStatusCallbackEx))
    {
      (*ppvObject) = static_cast<IBindStatusCallbackEx*>(this);
    }
    else
    {
      return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
  }

  ULONG DownLoadMgr::AddRef(void)
  {
    InterlockedIncrement(&comRefCount);
    return comRefCount;
  }

  ULONG DownLoadMgr::Release(void)
  {
    InterlockedDecrement(&comRefCount);
    return comRefCount;
  }

  HRESULT DownLoadMgr::Download(IMoniker * pmk, IBindCtx * pbc, DWORD dwBindVerb, LONG grfBINDF, BINDINFO * pBindInfo, LPCOLESTR pszHeaders, LPCOLESTR pszRedir, UINT uiCP)
  {
    //return E_FAIL;  //IE下载使用IE自己的下载工具

    //接管IE下载UI展现和数据保存成文件功能，使用Chrome界面做UI展现，下载的数据由Chrome主进程保存成文件
    if (isDownloading)
      return S_OK;
    LPOLESTR name=NULL;
    name= (LPOLESTR)CoTaskMemAlloc(sizeof(wchar_t)*10240);
    pmk->GetDisplayName(pbc,NULL, &name);
    downloadUrl = name;
    CoTaskMemFree(name);

    HRESULT hr;
    pBindCtx = pbc;
    pBindCtx->AddRef();
    pPrevBSCB = NULL;
    hr = RegisterBindStatusCallback(pbc,
      reinterpret_cast<IBindStatusCallback*>(this), &pPrevBSCB, 0L);

    if ((FAILED(hr)) && (pPrevBSCB))
    {
      //RevokeObjectParam for current BSCB, so we can register our BSCB
      hr = pbc->RevokeObjectParam(oBindCtxBSCBObjectParam);
      if (SUCCEEDED(hr))
      {
        //Attempt register again, should succeed now
        hr = RegisterBindStatusCallback(pbc,
          reinterpret_cast<IBindStatusCallback*>(this), 0, 0L);
      }
    }

    if (pPrevBSCB)
      pPrevBSCB->AddRef();
    isDownloading = true;
    IStream *pstm=NULL;
    hr = pmk->BindToStorage(pbc, 0, IID_IStream, (void**)&pstm);
    if (pstm)
      pstm->Release();
    return S_OK;
  }

  STDMETHODIMP DownLoadMgr::GetBindInfoEx(DWORD * grfBINDF, BINDINFO * pbindinfo, DWORD * grfBINDF2, DWORD * pdwReserved)
  {
    *grfBINDF = BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE | BINDF_PULLDATA | BINDF_GETNEWESTVERSION | BINDF_NOWRITECACHE;
    *grfBINDF2 = BINDF2_READ_DATA_GREATER_THAN_4GB;

    DWORD cbSize = pbindinfo->cbSize = sizeof(BINDINFO);
    memset(pbindinfo, 0, cbSize);
    pbindinfo->cbSize = cbSize;
    pbindinfo->dwBindVerb =  BINDVERB_GET;
    pbindinfo->szExtraInfo = NULL;
    memset(&pbindinfo->stgmedData, 0, TYMED_NULL);
    pbindinfo->grfBindInfoF = 0;
    pbindinfo->szCustomVerb = NULL;
    //Reserved. Must be set to 0
    pbindinfo->dwOptions = 0;
    pbindinfo->dwOptionsFlags = 0;
    pbindinfo->dwReserved = 0;
    return S_OK;
  }

  STDMETHODIMP DownLoadMgr::OnStartBinding(DWORD dwReserved, IBinding * pib)
  {
    pBinding = pib;
    if (pBinding == NULL)
    {
      return E_FAIL;
    }
    pBinding->AddRef();

    CComPtr<IWinInetHttpInfo> spWinInetHttpInfo;
    HRESULT hrTemp = pBinding->QueryInterface(IID_IWinInetHttpInfo,
      reinterpret_cast<void**>(&spWinInetHttpInfo));
    if (spWinInetHttpInfo == NULL)
      return E_FAIL;
    DWORD size = 0;
    DWORD flags = 0;
    hrTemp = spWinInetHttpInfo->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF /*| HTTP_QUERY_FLAG_REQUEST_HEADERS*/,
      0, &size, &flags, 0);
    if (SUCCEEDED(hrTemp))
    {
      char* pbuf = new char[size + 1];
      memset(pbuf, 0, sizeof(char)*(size + 1));
      hrTemp = spWinInetHttpInfo->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF /*| HTTP_QUERY_FLAG_REQUEST_HEADERS*/,
        pbuf, &size, &flags, 0);
      std::wstring responseHeader = L"";
      responseHeader = base::ASCIIToUTF16(pbuf);
      IsDownloadCanceled = false;
      isAbort = false;
      if (WebBrowser::GetWebBrowser())
      {
        variant_t var;
        var = responseHeader.c_str();
        WebBrowser::GetWebBrowser()->DownLoadFile(downloadUrl, (int)IE::BEGINDOWNLOAD, var);
      }
      delete[] pbuf;
    }
    if (pPrevBSCB)
    {
      pPrevBSCB->OnStopBinding(HTTP_STATUS_OK, NULL);
    }
    return S_OK;
  }

  STDMETHODIMP DownLoadMgr::GetPriority(LONG * pnPriority)
  {
    return E_NOTIMPL;
  }

  STDMETHODIMP DownLoadMgr::OnLowResource(DWORD reserved)
  {
    return E_NOTIMPL;
  }

  STDMETHODIMP DownLoadMgr::OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
  {
    if (ulStatusCode == BINDSTATUS_BEGINDOWNLOADDATA)
    {
      std::wstring ss = L"";
      ss = szStatusText;

      CComPtr<IWinInetHttpInfo> spWinInetHttpInfo;
      if (pBinding == NULL)
      {
        return E_FAIL;
      }
      HRESULT hrTemp = pBinding->QueryInterface(IID_IWinInetHttpInfo,
        reinterpret_cast<void**>(&spWinInetHttpInfo));
      DWORD size = 0;
      DWORD flags = 0;
      hrTemp = spWinInetHttpInfo->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF,
        0, &size, &flags, 0);
      if (SUCCEEDED(hrTemp))
      {
        char* pbuf = new char[size + 1];
        memset(pbuf, 0, sizeof(char)*(size + 1));
        hrTemp = spWinInetHttpInfo->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF,
          pbuf, &size, &flags, 0);
        std::string header = "";
        header = pbuf;
        delete[] pbuf;
      }
    }
    else if (ulStatusCode == BINDSTATUS_REDIRECTING)
    {
      std::wstring ss = L"";
      ss = szStatusText;

    }
    else if (ulStatusCode == BINDSTATUS_ENDDOWNLOADDATA)
    {

    }
    if (pPrevBSCB)
    {
      //Need to do this otherwise a filedownload dlg wil be displayed
      //as we are downloading the file.
      //Interestingly, according to MSDN,
      //BINDSTATUS_CONTENTDISPOSITIONATTACH is declared obsolete????
      if (ulStatusCode == BINDSTATUS_CONTENTDISPOSITIONATTACH)
        return S_OK;
      pPrevBSCB->OnProgress(ulProgress, ulProgressMax, ulStatusCode, szStatusText);
    }
    return S_OK;
  }

  STDMETHODIMP DownLoadMgr::OnStopBinding(HRESULT hresult, LPCWSTR szError)
  {
    if (hresult != S_OK && hresult!=E_ABORT)
    {
      if (WebBrowser::GetWebBrowser())
      {
        variant_t var;
        WebBrowser::GetWebBrowser()->DownLoadFile(downloadUrl, (int)IE::DOWNLOADERROR, var);
        IsDownloadCanceled = true;
      }
    }
    if (hresult == E_ABORT)
      isAbort = true;
    return S_OK;
  }

  STDMETHODIMP DownLoadMgr::GetBindInfo(DWORD * grfBINDF, BINDINFO * pbindinfo)
  {
    *grfBINDF = BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE | BINDF_PULLDATA | BINDF_GETNEWESTVERSION | BINDF_NOWRITECACHE;

    DWORD cbSize = pbindinfo->cbSize = sizeof(BINDINFO);
    memset(pbindinfo, 0, cbSize);
    pbindinfo->cbSize = cbSize;
    pbindinfo->dwBindVerb = BINDVERB_GET;
    pbindinfo->szExtraInfo = NULL;
    memset(&pbindinfo->stgmedData, 0, TYMED_NULL);
    pbindinfo->grfBindInfoF = 0;
    pbindinfo->szCustomVerb = NULL;
    //Reserved. Must be set to 0
    pbindinfo->dwOptions = 0;
    pbindinfo->dwOptionsFlags = 0;
    pbindinfo->dwReserved = 0;
    return S_OK;
  }

  STDMETHODIMP DownLoadMgr::OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC* pformatetc, STGMEDIUM* pstgmed)
  {
    if (IsDownloadCanceled == true)
    {
      if (pBinding)
      {
        pBinding->Abort();  //必须在OnDataAvailable方法中调用abort 取消下载，在其他地方调用会失败，造成无法再继续下载的问题，如果没有机会调用abort，就只能让IE继续网络传输，我们能做的就是不再往文件中写入。
      }
      return E_ABORT;
    }
    if (pstgmed == NULL || pstgmed->pstm == NULL)
      return E_INVALIDARG;
    if (BSCF_FIRSTDATANOTIFICATION & grfBSCF)
    {
      pStream = pstgmed->pstm;
      if (pStream)
        pStream->AddRef();
      //由于有些IE版本在没有CONTENT-LENGTH的数据时，IE只会调用一次OnDataAvailable函数，之后再也不会调用，只能一直使用IStream::Read，直到下载完成
      //为了不过多占用UI线程资源，采用定时器读取下载数据
      // base::TimeDelta delay = base::TimeDelta::FromMilliseconds(50);
      // comment jus for compiling
      // base::MessageLoopForUI::current()->PostDelayedTask(FROM_HERE,
      //   base::Bind(&DownLoadMgr::ReadDownloadData, base::Unretained(this)), delay);
    }
    return S_OK;
  }

  STDMETHODIMP DownLoadMgr::OnObjectAvailable(REFIID riid, IUnknown * punk)
  {
    return E_NOTIMPL;
  }

  void DownLoadMgr::ReadDownloadData()
  {
    if (pStream == NULL)
      return;

    DWORD dwActuallyRead = 0;

    HRESULT hr = S_OK;
    const int buffSize = 16 * 1024;
    do
    {
      std::string dataReceived = "";
      char* buff = new char[buffSize];
      memset(buff, 0, sizeof(char)*(buffSize));
      hr = pStream->Read(buff, buffSize, &dwActuallyRead);
      if (dwActuallyRead > 0 &&
        WebBrowser::GetWebBrowser() &&
        IsDownloadCanceled==false)
      {
        dataReceived.append(buff, dwActuallyRead);
        SAFEARRAYBOUND Bound[1];
        Bound[0].lLbound = 0;
        Bound[0].cElements = dataReceived.size();
        SAFEARRAY* pArray = SafeArrayCreate(VT_UI1, 1, Bound);
        unsigned char* pData = NULL;
        HRESULT hr = SafeArrayAccessData(pArray, (void**)&pData);
        memcpy(pData, dataReceived.c_str(), dataReceived.size());
        hr = SafeArrayUnaccessData(pArray);
        variant_t var;
        var.vt = VT_ARRAY;
        var.parray = pArray;
        WebBrowser::GetWebBrowser()->DownLoadFile(downloadUrl, (int)IE::DOWNLOADINGDATA, var);
        SafeArrayDestroy(pArray);
      }
      delete[] buff;
    } while (hr == S_OK);

    if (IsDownloadCanceled == true && isAbort==true)
    {
      ReleaseObject();
      return;
    }

    //由于有些IE版本在http response 头中CONTENT-LENGTH没有的情况下不返回BSCF_LASTDATANOTIFICATION状态，
    //所以无法通过BSCF_LASTDATANOTIFICATION判断下载完成，
    //改为使用IStream::Read返回值为S_FALSE且没有读取到任何数据，判断为下载结束
    if (hr == S_FALSE && dwActuallyRead == 0)
    {
      if (WebBrowser::GetWebBrowser())
      {
        variant_t var;
        WebBrowser::GetWebBrowser()->DownLoadFile(downloadUrl, (int)IE::ENDDOWNLOAD, var);
      }
      isDownloading = false;
      ReleaseObject();
    }
    else
    {
      // base::TimeDelta delay = base::TimeDelta::FromMilliseconds(50);
      // base::MessageLoopForUI::current()->PostDelayedTask(FROM_HERE,
      //   base::Bind(&DownLoadMgr::ReadDownloadData, base::Unretained(this)), delay);
    }
  }

  void DownLoadMgr::ReleaseObject()
  {
    if (pBindCtx && pPrevBSCB)
    {
      pBindCtx->RegisterObjectParam(oBindCtxBSCBObjectParam,
        reinterpret_cast<IUnknown*>(pPrevBSCB));
    }
    if (pBindCtx)
    {
      pBindCtx->Release();
      pBindCtx = NULL;
    }
    if (pStream)
    {
      pStream->Release();
      pStream = NULL;
    }
    if (pPrevBSCB)
    {
      pPrevBSCB->Release();
      pPrevBSCB = NULL;
    }
    if (pBinding)
    {
      pBinding->Release();
      pBinding = NULL;
    }
    isDownloading = false;
  }

  STDMETHODIMP DownLoadMgr::BeginningTransaction(LPCWSTR szURL, LPCWSTR szHeaders, DWORD dwReserved, LPWSTR * pszAdditionalHeaders)
  {
    return E_NOTIMPL;
  }

  STDMETHODIMP DownLoadMgr::OnResponse(DWORD dwResponseCode, LPCWSTR szResponseHeaders, LPCWSTR szRequestHeaders, LPWSTR * pszAdditionalRequestHeaders)
  {
    return E_NOTIMPL;
  }


  //HTMLOMWindowServices
  HTMLOMWindowServices::HTMLOMWindowServices(IEEventHandler * handler)
    :comRefCount(0),
    pDelegate(NULL)
  {
    pDelegate = handler;
  }

  HTMLOMWindowServices::~HTMLOMWindowServices()
  {
  }

  HRESULT HTMLOMWindowServices::QueryInterface(REFIID riid, void ** ppvObject)
  {
    if (riid == __uuidof(IUnknown))
    {
      (*ppvObject) = this;
    }
    else if (riid == __uuidof(IHTMLOMWindowServices))
    {
      (*ppvObject) = static_cast<IHTMLOMWindowServices*>(this);
    }
    else
    {
      return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
  }

  ULONG HTMLOMWindowServices::AddRef(void)
  {
    InterlockedIncrement(&comRefCount);
    return comRefCount;
  }

  ULONG HTMLOMWindowServices::Release(void)
  {
    InterlockedDecrement(&comRefCount);
    return comRefCount;
  }

  HRESULT HTMLOMWindowServices::moveTo(LONG x, LONG y)
  {
    if (pDelegate)
    {
      RECT rc;
      pDelegate->OnGetMainWndPos(&rc);
      int width = rc.right - rc.left;
      int height = rc.bottom - rc.top;
      pDelegate->OnRequestChangePos(x, y, width, height);
    }
    return S_OK;
  }
  HRESULT HTMLOMWindowServices::moveBy(LONG x, LONG y)
  {
    if (pDelegate)
    {
      RECT rc;
      pDelegate->OnGetMainWndPos(&rc);
      int width = rc.right - rc.left;
      int height = rc.bottom - rc.top;
      pDelegate->OnRequestChangePos(rc.left+x, rc.top+y, width, height);
    }
    return S_OK;
  }
  HRESULT HTMLOMWindowServices::resizeTo(LONG x, LONG y)
  {
    if (pDelegate)
    {
      RECT rc;
      pDelegate->OnGetMainWndPos(&rc);
      pDelegate->OnRequestChangePos(rc.left, rc.top, x, y);
    }
    return S_OK;
  }
  HRESULT HTMLOMWindowServices::resizeBy(LONG x, LONG y)
  {
    if (pDelegate)
    {
      RECT rc;
      pDelegate->OnGetMainWndPos(&rc);
      int width = rc.right - rc.left;
      int height = rc.bottom - rc.top;
      pDelegate->OnRequestChangePos(rc.left, rc.top, width+x, height+y);
    }
    return S_OK;
  }

  //DomainAuthenticate
  DomainAuthenticate::DomainAuthenticate(IEEventHandler* handler, IWebBrowser2* pWeb2)
    :comRefCount(0),
    pDelegate(handler),
    pWebBrowser(pWeb2)
  {
    if (pWebBrowser)
      pWebBrowser->AddRef();
  }

  DomainAuthenticate::~DomainAuthenticate()
  {
    if (pWebBrowser)
      pWebBrowser->Release();
  }

  HRESULT DomainAuthenticate::QueryInterface(REFIID riid, void ** ppvObject)
  {
    if (riid == __uuidof(IUnknown))
    {
      (*ppvObject) = this;
    }
    else if (riid == __uuidof(IAuthenticate))
    {
      (*ppvObject) = static_cast<IAuthenticate*>(this);
    }
    else
    {
      return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
  }

  ULONG DomainAuthenticate::AddRef(void)
  {
    InterlockedIncrement(&comRefCount);
    return comRefCount;
  }

  ULONG DomainAuthenticate::Release(void)
  {
    InterlockedDecrement(&comRefCount);
    return comRefCount;
  }

  HRESULT DomainAuthenticate::Authenticate(HWND * phwnd, LPWSTR * pszUsername, LPWSTR * pszPassword)
  {
    if (phwnd == NULL || pszUsername == NULL || pszPassword == NULL)
      return E_INVALIDARG;

    if (pWebBrowser == NULL)
      return E_ACCESSDENIED;

    *phwnd = NULL;

    std::wstring name = L"";
    std::wstring pwd = L"";
    if (pDelegate)
      pDelegate->OnWindowDomainAuthenticate(&name, &pwd);

    if (name.empty() == false && pwd.empty() == false)
    {
      int lenName = (name.size() + 1) * sizeof(wchar_t);
      int lenPwd = (pwd.size() + 1) * sizeof(wchar_t);
      *pszUsername = (LPWSTR)CoTaskMemAlloc(lenName);
      memset(*pszUsername, 0, lenName);
      memcpy_s(*pszUsername, lenName, name.c_str(), name.size() * sizeof(wchar_t));

      *pszPassword = (LPWSTR)CoTaskMemAlloc(lenPwd);
      memset(*pszPassword, 0, lenPwd);
      memcpy_s(*pszPassword, lenPwd, pwd.c_str(), pwd.size() * sizeof(wchar_t));
    }
    else
    {
      IOleObject* pOleObj = NULL;
      pWebBrowser->QueryInterface(IID_IOleObject, (void**)&pOleObj);
      if (pOleObj == NULL)
        return S_FALSE;
      IOleClientSite* pOleClientSite = NULL;
      pOleObj->GetClientSite(&pOleClientSite);
      if (pOleClientSite == NULL)
        return S_FALSE;
      IOleInPlaceSite* pOleInPlaceSite = NULL;
      pOleClientSite->QueryInterface(IID_IOleInPlaceSite, (void**)&pOleInPlaceSite);
      if (pOleInPlaceSite == NULL)
        return S_FALSE;
      pOleInPlaceSite->GetWindow(phwnd);
      pOleInPlaceSite->Release();
      pOleClientSite->Release();
      pOleObj->Release();
    }

    return S_OK;
  }

  //NewWindowManager

  IWebBrowser2* GetIWebBrowserFromHwnd(HWND hwnd)
  {
    return NULL;
  }


  NewWindowManager::NewWindowManager(IEEventHandler * pHandler)
    :pDelegate(pHandler)
  {
    comRefCount = 0;
  }

  NewWindowManager::~NewWindowManager()
  {
  }

  HRESULT NewWindowManager::QueryInterface(REFIID riid, void ** ppvObject)
  {
    if (riid == __uuidof(IUnknown))
    {
      (*ppvObject) = this;
    }
    else if (riid == __uuidof(INewWindowManager))
    {
      (*ppvObject) = static_cast<INewWindowManager*>(this);
    }
    else
    {
      return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
  }

  ULONG NewWindowManager::AddRef(void)
  {
    InterlockedIncrement(&comRefCount);
    return comRefCount;
  }

  ULONG NewWindowManager::Release(void)
  {
    InterlockedDecrement(&comRefCount);
    return comRefCount;
  }

  HRESULT NewWindowManager::EvaluateNewWindow(LPCWSTR pszUrl, LPCWSTR pszName,
    LPCWSTR pszUrlContext, LPCWSTR pszFeatures, BOOL fReplace,
    DWORD dwFlags, DWORD dwUserActionTime)
  {
    if ((dwFlags & 0x00000040) == 0x00000040) //是在showModelessDialog或者showModeDialog打开的弹出窗口中，又新开的页面
    {
      long flag = IE::NewWindowFlag::TRANSITION;
      if (((dwFlags & 0x40000) == 0x40000 &&
        (dwFlags & 0x20000) == 0) ||
        (dwFlags & 0x10000) == 0x10000)
        flag = IE::NewWindowFlag::POPUP;
      else if ((dwFlags & 0x00000006) == 0x00000006 ||
        (dwFlags & 0x00010086) == 0x00010086 ||
        (dwFlags & 0x00000002) == 0x00000002)
        flag = IE::NewWindowFlag::TRANSITION;
      flag = flag | IE::NewWindowFlag::FROMDIALOG;
      // if (pDelegate)
      // {
      //   CComPtr<IDispatch> pDisp;
      //   bool cancel=pDelegate->OnLoadUrlNewWnd(pszUrl, flag, &pDisp);
      //   if (cancel)
      //     return S_FALSE;
      // }
    }
    return E_FAIL;
  }
} //namespace IE