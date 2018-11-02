// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_IE_DEVTOOLS_HOST_H_
#define CONTENT_COMMON_IE_DEVTOOLS_HOST_H_

#include "content/common/IE/atl_include.h"

#include <ExDisp.h>

namespace ie {

class DevToolsSite : public IUnknown {
 public:
  STDMETHOD(GetBrowser)(IUnknown**) = 0;
  STDMETHOD(PinUnPin)(BOOL, int) = 0;
  STDMETHOD(OnCloseDevTools)() = 0;
  STDMETHOD(OnLoseFocus)() = 0;
  STDMETHOD(JITShowDevTools)(int) = 0;
  STDMETHOD(DevToolsOpened)(int*) = 0;
  STDMETHOD(CloseTools)() = 0;
  virtual int HandleDelete(long*) = 0;
};

class DevTools : public IUnknown {
 public:
  STDMETHOD(SetDevToolSite)(DevToolsSite* site) = 0;
  STDMETHOD(ShowDevTools)(BOOL bShow) = 0;
  STDMETHOD(JITShowDevTools)(int) = 0;
  STDMETHOD(CloseDevTools)() = 0;
  STDMETHOD(IsVisible)(BOOL*) = 0;
  STDMETHOD(GetPinState)(BOOL* bPin) = 0;
  STDMETHOD(SetPinState)(BOOL bPin) = 0;
  STDMETHOD(IsMinimized)(BOOL*) = 0;
  STDMETHOD(DevToolsReady)() = 0;
};

class DevToolsHost : public DevToolsSite {
 public:
  DevToolsHost(IWebBrowser2* web_browser2);
  virtual ~DevToolsHost();

  bool ShowDevTools();
  void CloseDevTools();
  bool IsOpened();

  // IUnknown
  STDMETHOD(QueryInterface)(REFIID riid, void** ppv_object) override;
  STDMETHOD_(ULONG, AddRef)() override;
  STDMETHOD_(ULONG, Release)() override;
  // DevToolsSite
  STDMETHOD(GetBrowser)(IUnknown**) override;
  STDMETHOD(PinUnPin)(BOOL, int) override;
  STDMETHOD(OnCloseDevTools)() override;
  STDMETHOD(OnLoseFocus)() override;
  STDMETHOD(JITShowDevTools)(int) override;
  STDMETHOD(DevToolsOpened)(int*) override;
  STDMETHOD(CloseTools)() override;
  int HandleDelete(long*) override;

 private:
  static HMODULE WINAPI HookLoadLibraryW(LPCTSTR function_name);

 private:
  volatile long com_ref_count_;
  bool is_open_;
  CComPtr<IWebBrowser2> web_browser2_;
  CComPtr<DevTools> devtools_;
};

}  // namespace ie

#endif  // CONTENT_COMMON_IE_DEVTOOLS_HOST_H_
