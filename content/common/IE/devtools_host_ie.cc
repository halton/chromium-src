// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/IE/devtools_host_ie.h"

#include <ShlGuid.h>
#include <algorithm>
#include <set>
#include <string>

#include "third_party/minhook/include/MinHook.h"

namespace ie {

static const CLSID CLSID_IEDTExplorerBar = {
    0x1a6fe369,
    0xf28c,
    0x4ad9,
    {0xa3, 0xe6, 0x2b, 0xcb, 0x50, 0x80, 0x7c, 0xf1}};

static const IID IID_DevTools = {
    0x181e3828,
    0xfe6e,
    0x4602,
    {0xa3, 0x27, 0x78, 0x6a, 0x76, 0xfd, 0xfb, 0x3a}};

static const IID IID_DevToolsOld = {
    0x059055df,
    0x6bb0,
    0x402a,
    {0xba, 0x48, 0x58, 0xbf, 0xa3, 0x43, 0x71, 0x9c}};

static std::set<std::wstring> s_depends_dlls = {
    L"jsdbgui.dll", L"jsprofilerui.dll", L"networkinspection.dll"};

typedef HMODULE(WINAPI* LOADLIBRARYW)(LPCTSTR);
static LOADLIBRARYW s_load_library_function = NULL;
static std::wstring s_ie_file_path = L"";

HMODULE WINAPI DevToolsHost::HookLoadLibraryW(LPCTSTR function_name) {
  if (s_load_library_function == NULL)
    return NULL;

  std::wstring path = function_name;
  wchar_t fdriver[_MAX_DRIVE + 1] = {0};
  wchar_t fdir[_MAX_DIR + 1] = {0};
  wchar_t fname[_MAX_FNAME + 1] = {0};
  wchar_t fext[_MAX_EXT + 1] = {0};
  _wsplitpath_s(function_name, fdriver, fdir, fname, fext);

  std::wstring temp = std::wstring(fname) + fext;
  std::transform(temp.begin(), temp.end(), temp.begin(), tolower);
  std::set<std::wstring>::iterator iter = s_depends_dlls.find(temp);
  if (iter != s_depends_dlls.end()) {
    if (s_ie_file_path.empty()) {
      HMODULE module_handle = GetModuleHandle(_T("iedvtool.dll"));
      wchar_t iedvtoolPath[MAX_PATH + 1] = {0};
      GetModuleFileName(module_handle, iedvtoolPath, MAX_PATH);
      wchar_t iedriver[_MAX_DRIVE + 1] = {0};
      wchar_t iedir[_MAX_DIR + 1] = {0};
      wchar_t iefname[_MAX_FNAME + 1] = {0};
      wchar_t ieext[_MAX_EXT + 1] = {0};
      _wsplitpath_s(iedvtoolPath, iedriver, iedir, iefname, ieext);
      s_ie_file_path = std::wstring(iedriver) + iedir;
    }
    path = s_ie_file_path + temp;
  }

  return s_load_library_function(path.c_str());
}

DevToolsHost::DevToolsHost(IWebBrowser2* web_browser2)
    : com_ref_count_(0), is_open_(false), web_browser2_(web_browser2) {
  MH_CreateHookApi(L"Kernel32", "LoadLibraryW", &HookLoadLibraryW,
                   (LPVOID*)&s_load_library_function);
  MH_EnableHook(&LoadLibraryW);
  // IE 11
  CoCreateInstance(CLSID_IEDTExplorerBar, NULL, CLSCTX_INPROC_SERVER,
                   IID_DevTools, (void**)&devtools_);
  if (NULL == devtools_) {
    // IE 8 ~ IE 10
    CoCreateInstance(CLSID_IEDTExplorerBar, NULL, CLSCTX_INPROC_SERVER,
                     IID_DevToolsOld, (void**)&devtools_);
  }
}

DevToolsHost::~DevToolsHost() {
  if (devtools_ && is_open_)
    devtools_->ShowDevTools(FALSE);

  is_open_ = false;
  MH_DisableHook(&s_load_library_function);
}

bool DevToolsHost::ShowDevTools() {
  if (devtools_ == NULL)
    return false;

  HRESULT hr = E_FAIL;
  hr = devtools_->SetDevToolSite(this);
  if (FAILED(hr))
    return false;

  devtools_->SetPinState(FALSE);
  hr = devtools_->ShowDevTools(TRUE);
  if (FAILED(hr))
    return false;
  hr = devtools_->DevToolsReady();
  CComPtr<IOleWindow> pOleWnd = NULL;
  hr = devtools_->QueryInterface(IID_IOleWindow, (void**)&pOleWnd);
  if (FAILED(hr))
    return false;
  HWND hwnd = 0;
  pOleWnd->GetWindow(&hwnd);
  if (IsWindow(hwnd))
    SetForegroundWindow(hwnd);
  is_open_ = true;
  return true;
}

void DevToolsHost::CloseDevTools() {
  if (devtools_ == NULL)
    return;

  devtools_->ShowDevTools(FALSE);
  is_open_ = false;
}

bool DevToolsHost::IsOpened() {
  return is_open_;
}

STDMETHODIMP DevToolsHost::QueryInterface(REFIID riid, void** ppv_object) {
  return E_NOTIMPL;
}

STDMETHODIMP_(ULONG) DevToolsHost::AddRef() {
  InterlockedIncrement(&com_ref_count_);
  return com_ref_count_;
}

STDMETHODIMP_(ULONG) DevToolsHost::Release() {
  InterlockedDecrement(&com_ref_count_);
  return com_ref_count_;
}

STDMETHODIMP DevToolsHost::GetBrowser(IUnknown** unk) {
  *unk = web_browser2_;
  return S_OK;
}

STDMETHODIMP DevToolsHost::PinUnPin(BOOL, int) {
  return S_OK;
}

STDMETHODIMP DevToolsHost::OnCloseDevTools() {
  return E_NOTIMPL;
}

STDMETHODIMP DevToolsHost::OnLoseFocus() {
  return S_OK;
}

STDMETHODIMP DevToolsHost::JITShowDevTools(int) {
  return E_NOTIMPL;
}

STDMETHODIMP DevToolsHost::DevToolsOpened(int* p) {
  return E_NOTIMPL;
}

STDMETHODIMP DevToolsHost::CloseTools() {
  return E_NOTIMPL;
}

int DevToolsHost::HandleDelete(long* p) {
  DevToolsHost* pHost = (DevToolsHost*)p;
  if (pHost)
    pHost->is_open_ = false;

  return 0;
}

}  // namespace ie
