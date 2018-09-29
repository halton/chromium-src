#include "DevToolsHost.h"
#include <ShlGuid.h>
#include <string>
#include <set>
#include <algorithm>
#include "third_party/minhook/include/MinHook.h"

namespace IE
{

  static const CLSID CLSID_IEDTExplorerBar = {
    0x1a6fe369, 0xf28c, 0x4ad9,{ 0xa3, 0xe6, 0x2b, 0xcb, 0x50, 0x80, 0x7c, 0xf1 }
  };

  static const IID IID_IEDevTools = {
    0x181e3828, 0xfe6e, 0x4602,{ 0xa3, 0x27, 0x78, 0x6a, 0x76, 0xfd, 0xfb, 0x3a }
  };

  static const IID IID_IEDevToolsOld = {
    0x059055df, 0x6bb0, 0x402a,{ 0xba, 0x48, 0x58, 0xbf, 0xa3, 0x43, 0x71, 0x9c }
  };

  static std::set<std::wstring> DependDlls = { L"jsdbgui.dll", L"jsprofilerui.dll", L"networkinspection.dll" };

  typedef HMODULE(WINAPI *LOADLIBRARYW)(LPCTSTR);
  static LOADLIBRARYW fpLoadLibW = NULL;
  static std::wstring ieFIlePath = L"";

  HMODULE WINAPI DevToolsHost::HookLoadLibraryW(LPCTSTR fName)
  {
    if (fpLoadLibW == NULL)
      return NULL;

    std::wstring path = fName;
    wchar_t fdriver[_MAX_DRIVE + 1] = { 0 };
    wchar_t fdir[_MAX_DIR + 1] = { 0 };
    wchar_t fname[_MAX_FNAME + 1] = { 0 };
    wchar_t fext[_MAX_EXT + 1] = { 0 };
    _wsplitpath_s(fName, fdriver, fdir, fname, fext);

    std::wstring temp = std::wstring(fname) + fext;
    std::transform(temp.begin(), temp.end(), temp.begin(), tolower);
    std::set<std::wstring>::iterator iter = DependDlls.find(temp);
    if (iter != DependDlls.end())
    {
      if (ieFIlePath.empty())
      {
        HMODULE hMod = GetModuleHandle(_T("iedvtool.dll"));
        wchar_t iedvtoolPath[MAX_PATH + 1] = { 0 };
        GetModuleFileName(hMod, iedvtoolPath, MAX_PATH);
        wchar_t iedriver[_MAX_DRIVE + 1] = { 0 };
        wchar_t iedir[_MAX_DIR + 1] = { 0 };
        wchar_t iefname[_MAX_FNAME + 1] = { 0 };
        wchar_t ieext[_MAX_EXT + 1] = { 0 };
        _wsplitpath_s(iedvtoolPath, iedriver, iedir, iefname, ieext);
        ieFIlePath = std::wstring(iedriver) + iedir;
      }
      path = ieFIlePath + temp;
    }

    return fpLoadLibW(path.c_str());
  }

  DevToolsHost::DevToolsHost(IWebBrowser2* pWeb2)
    :comRefCount(0),
    isOpen(false),
    pWebBrowser2(pWeb2)
  {
    MH_CreateHookApi(L"Kernel32", "LoadLibraryW", &HookLoadLibraryW, (LPVOID*)&fpLoadLibW);
    MH_EnableHook(&LoadLibraryW);
    // IE 11
    CoCreateInstance(CLSID_IEDTExplorerBar, NULL, CLSCTX_INPROC_SERVER,
      IID_IEDevTools, (void**)&pDevTools);
    if (NULL == pDevTools)
    {
      // IE 8 ~ IE 10
      CoCreateInstance(CLSID_IEDTExplorerBar, NULL, CLSCTX_INPROC_SERVER,
        IID_IEDevToolsOld, (void**)&pDevTools);
    }
  }


  DevToolsHost::~DevToolsHost()
  {
    if (pDevTools && isOpen)
      pDevTools->ShowDevTools(FALSE);
    isOpen = false;
    MH_DisableHook(&fpLoadLibW);
  }

  bool DevToolsHost::ShowIEDevTools()
  {
    if (pDevTools == NULL)
      return false;

    HRESULT hr = E_FAIL;
    hr = pDevTools->SetDevToolSite(this);
    if (FAILED(hr))
      return false;

    pDevTools->SetPinState(FALSE);
    hr = pDevTools->ShowDevTools(TRUE);
    if (FAILED(hr))
      return false;
    hr = pDevTools->DevToolsReady();
    CComPtr<IOleWindow> pOleWnd = NULL;
    hr = pDevTools->QueryInterface(IID_IOleWindow, (void**)&pOleWnd);
    if (FAILED(hr))
      return false;
    HWND hwnd = 0;
    pOleWnd->GetWindow(&hwnd);
    if (IsWindow(hwnd))
      SetForegroundWindow(hwnd);
    isOpen = true;
    return true;
  }

  void DevToolsHost::CloseIEDevTools()
  {
    if (pDevTools == NULL)
      return;
    pDevTools->ShowDevTools(FALSE);
    isOpen = false;
  }

  bool DevToolsHost::IsOpened()
  {
    return isOpen;
  }

  STDMETHODIMP DevToolsHost::QueryInterface(REFIID riid, void ** ppvObject)
  {
    return E_NOTIMPL;
  }

  STDMETHODIMP_(ULONG) DevToolsHost::AddRef(void)
  {
    InterlockedIncrement(&comRefCount);
    return comRefCount;
  }

  STDMETHODIMP_(ULONG) DevToolsHost::Release(void)
  {
    InterlockedDecrement(&comRefCount);
    return comRefCount;
  }

  STDMETHODIMP DevToolsHost::GetBrowser(IUnknown ** unk)
  {
    *unk = pWebBrowser2;
    return S_OK;
  }

  STDMETHODIMP DevToolsHost::PinUnPin(BOOL, int)
  {
    return S_OK;
  }

  STDMETHODIMP DevToolsHost::OnCloseDevTools(void)
  {
    return E_NOTIMPL;
  }

  STDMETHODIMP DevToolsHost::OnLoseFocus(void)
  {
    return S_OK;
  }

  STDMETHODIMP DevToolsHost::JITShowDevTools(int)
  {
    return E_NOTIMPL;
  }

  STDMETHODIMP DevToolsHost::DevToolsOpened(int * p)
  {
    return E_NOTIMPL;
  }

  STDMETHODIMP DevToolsHost::CloseTools(void)
  {
    return E_NOTIMPL;
  }

  int DevToolsHost::v_HandleDelete(long * p)
  {
    DevToolsHost* pHost = (DevToolsHost*)p;
    if(pHost)
      pHost->isOpen = false;
    return 0;
  }
} //namespace IE
