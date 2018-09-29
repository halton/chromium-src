#ifndef CONTENT_COMMON_IE_DEV_TOOLS_HOST_H_
#define CONTENT_COMMON_IE_DEV_TOOLS_HOST_H_
#pragma once
#include "ATLInclude.h"
#include <ExDisp.h>

namespace IE
{

  class IEDevToolsSite : public IUnknown
  {
  public:
    STDMETHOD(GetBrowser)(IUnknown **) = 0;
    STDMETHOD(PinUnPin)(BOOL, int) = 0;
    STDMETHOD(OnCloseDevTools)(void) = 0;
    STDMETHOD(OnLoseFocus)(void) = 0;
    STDMETHOD(JITShowDevTools)(int) = 0;
    STDMETHOD(DevToolsOpened)(int *) = 0;
    STDMETHOD(CloseTools)(void) = 0;
    virtual int v_HandleDelete(long *) = 0;
  };

  class IEDevTools : public IUnknown
  {
  public:
    STDMETHOD(SetDevToolSite)(IEDevToolsSite *site) = 0;
    STDMETHOD(ShowDevTools)(BOOL bShow) = 0;
    STDMETHOD(JITShowDevTools)(int) = 0;
    STDMETHOD(CloseDevTools)(void) = 0;
    STDMETHOD(IsVisible)(BOOL *) = 0;
    STDMETHOD(GetPinState)(BOOL *bPin) = 0;
    STDMETHOD(SetPinState)(BOOL bPin) = 0;
    STDMETHOD(IsMinimized)(BOOL *) = 0;
    STDMETHOD(DevToolsReady)(void) = 0;
  };

  class DevToolsHost : public IEDevToolsSite
  {
  public:
    DevToolsHost(IWebBrowser2* pWeb2);
    virtual ~DevToolsHost();

    bool ShowIEDevTools();
    void CloseIEDevTools();
    bool IsOpened();

    // IUnknown
    STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject) override;
    STDMETHOD_(ULONG, AddRef)(void) override;
    STDMETHOD_(ULONG, Release)(void) override;
    // IEDevToolsSite
    STDMETHOD(GetBrowser)(IUnknown **) override;
    STDMETHOD(PinUnPin)(BOOL, int) override;
    STDMETHOD(OnCloseDevTools)(void) override;
    STDMETHOD(OnLoseFocus)(void) override;
    STDMETHOD(JITShowDevTools)(int) override;
    STDMETHOD(DevToolsOpened)(int *) override;
    STDMETHOD(CloseTools)(void) override;
    int v_HandleDelete(long *) override;

  private:
    static HMODULE WINAPI HookLoadLibraryW(LPCTSTR fName);
  private:
    volatile long comRefCount;
    bool isOpen;
    CComPtr<IWebBrowser2> pWebBrowser2;
    CComPtr<IEDevTools> pDevTools;
  };
} // namespace IE

#endif
