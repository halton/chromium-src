#ifdef REDCORE
#ifdef IE_REDCORE
#ifndef CONTENT_BROWSER_WEB_CONTENTS_WEB_CONTENTS_IE_H_
#define CONTENT_BROWSER_WEB_CONTENTS_WEB_CONTENTS_IE_H_
#pragma once

#include "content/common/IE/ATLInclude.h"
#include <stdint.h>

#include <map>
#include <set>
#include <string>

#include "base/compiler_specific.h"
#include "base/gtest_prod_util.h"
#include "base/macros.h"
#include "base/observer_list.h"
#include "base/process/process.h"
#include "base/values.h"
#include "build/build_config.h"
#include "content/browser/frame_host/frame_tree.h"
#include "content/browser/frame_host/navigation_controller_delegate.h"
#include "content/browser/frame_host/navigation_controller_impl.h"
#include "content/browser/frame_host/navigator_delegate.h"
#include "content/browser/frame_host/render_frame_host_delegate.h"
#include "content/browser/frame_host/render_frame_host_manager.h"
#include "content/browser/media/audio_stream_monitor.h"
#include "content/browser/renderer_host/render_view_host_delegate.h"
#include "content/browser/renderer_host/render_view_host_impl.h"
#include "content/browser/renderer_host/render_widget_host_delegate.h"
#include "content/common/content_export.h"
#include "content/public/browser/color_chooser.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/common/page_importance_signals.h"
#include "content/public/common/renderer_preferences.h"
#include "content/public/common/resource_type.h"
#include "content/public/common/three_d_api_types.h"
#include "net/base/load_states.h"
#include "net/http/http_response_headers.h"
#include "ui/base/page_transition_types.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/size.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "net/url_request/url_fetcher_delegate.h"
#include "content/common/IE/IEVersion.h"
#include "net/cookies/cookie_store.h"
#include "base/synchronization/condition_variable.h"
#include "net/url_request/url_request_context_getter.h"

struct RendererMode;

namespace IE {
  class BrowserHostEventDelegant;
}

namespace net {
  class URLFetcher;
}
namespace content {
class BrowserPluginEmbedder;
class BrowserPluginGuest;
class DateTimeChooserAndroid;
class DownloadItem;
class GeolocationServiceContext;
class InterstitialPageImpl;
class JavaScriptDialogManager;
class ManifestManagerHost;
class MediaWebContentsObserver;
class PluginContentOriginWhitelist;
class PowerSaveBlocker;
class RenderViewHost;
class RenderViewHostDelegateView;
class RenderWidgetHostImpl;
class RenderWidgetHostInputEventRouter;
class SavePackage;
class ScreenOrientationDispatcherHost;
class SiteInstance;
class TestWebContents;
class WakeLockServiceContext;
class WebContentsAudioMuter;
class WebContentsDelegate;
class WebContentsImpl;
class WebContentsView;
class WebContentsViewDelegate;
struct AXEventNotificationDetails;
struct ColorSuggestion;
struct FaviconURL;
struct LoadNotificationDetails;
struct ResourceRedirectDetails;
struct ResourceRequestDetails;
class WebContentsView;
class WebContentsViewDelegate;
struct LoadURLParams;

  class CONTENT_EXPORT WebContentsIE
                        :public WebContentsImpl,
                        public net::URLFetcherDelegate
  {
  public:
    WebContentsIE(BrowserContext* browser_context);
    ~WebContentsIE() override;
    void Init(const WebContents::CreateParams& params) override;
    void SetParentNativeViewAccessible(gfx::NativeViewAccessible accessible_parent);

    virtual void OnRendererHostViewSize(const gfx::Size& size);
    virtual void SetIECookie(const GURL& url);
    virtual void SetIEAutoLoginInfo(const std::wstring& frameXPath, const std::wstring& nameXPath, const std::wstring& nameValue,
      const std::wstring& pwdXPath, const std::wstring& pwdValue, const std::wstring& loginBtnXpath);
    virtual void SetListenLoginXPath(const std::wstring& url, const std::wstring& frameXPath, const std::wstring& nameXPath,
      const std::wstring& pwdXPath, const std::wstring& loginBtnXpath);
    virtual void OnGetLoginInfo(const std::wstring& url, const std::wstring& name, const std::wstring& pwd);
    bool IsDevToolsOpened();
    void ShowDevTools(bool show);
    void SetBrowserZoom(int percent);

    virtual void Print();
    virtual void FindWithoutParams();

    void Cut() override;
    void Copy() override;
    void Paste() override;

    // WebContents ------------------------------------------------------
    void SetDelegate(WebContentsDelegate* delegate) override;
    void WasShown() override;
    void WasHidden() override;
    const base::string16& GetTitle() const override;
    bool IsLoading() const override;
    int DownloadImage(const GURL& url,
      bool is_favicon,
      uint32_t max_bitmap_size,
      bool bypass_cache,
      ImageDownloadCallback callback) override;
    void Focus() override;
    RenderFrameHostImpl* GetFocusedFrame() override;
    void Stop() override;
    bool NeedToFireBeforeUnload() override;
    void DispatchBeforeUnload() override;
    int GetMinimumZoomPercent() const override;
    int GetMaximumZoomPercent() const override;

    // RenderFrameHostManager::Delegate ------------------------------------------

    void CreateRenderWidgetHostViewForRenderManager(
      RenderViewHost* render_view_host) override;

    void RendererUnresponsive(
        RenderWidgetHostImpl* render_widget_host,
        base::RepeatingClosure hang_monitor_restarter) override;
    void RendererResponsive(RenderWidgetHostImpl* render_widget_host) override;
    void Activate() override;

    //URLFetcherDelegate--------------------------------------------------------
    void OnURLFetchComplete(const net::URLFetcher* source) override;

    bool LoadUrl(const NavigationController::LoadURLParams& params);
    void OnFinishNavigate(GURL& url, std::vector<GURL>& faviconUrls);
    void OnLoadUrlInNewContent(GURL& url, int flag, bool* cancel, IDispatch** pDisp);
    void OnBeforeNavigate(BSTR url, bool clickUrl, bool * cancel);
    IDispatch* GetWebBrowserIDispatch();
    void OnBeforeDownloadFile(BSTR url, const std::wstring& responseHeader);
    void OnUpdateCookie(const GURL& url, const std::vector<std::wstring>& cookies);
    bool IsDownloading();

    void SendFunctionControl(const std::wstring& jsonStr);
    void SetCreateByIENewWindow(bool isNew);

  private:
    bool CreateTridentWebView(const gfx::AcceleratedWidget& hwnd_parent, const gfx::Size& size);
    bool IsTridentCreated();
    void CommitToEntry(GURL gurl, bool isSetHistory);
    void LoadFinishedAndUpdateEntry(GURL gurl);
    void SetBrowserEmulation(IE::IEEmulation emu);
    net::CookieList LoadCookie(const GURL& url);
    void OnBeginLoadCookie(const GURL& url, scoped_refptr<net::URLRequestContextGetter> pGetter);
    void OnCookieLoaded(const net::CookieList& cookie_list);
    std::wstring SerializationCookieList(const GURL& url, const net::CookieList& list);
    void OnBeginSaveCookie(const GURL& url, 
                          std::vector<std::wstring> cookies,
                          scoped_refptr<net::URLRequestContextGetter> pGetter);
    void OnCookieSaveSuccess(bool success);
    GURL GetGurlFromCookie(const net::CanonicalCookie& cookie);
    void NavigateUrl(const NavigationController::LoadURLParams* params);
  private:
     friend class WebContentsObserver;
     friend class WebContents;  // To implement factory methods.
  private:
    mutable base::string16 title;
    CComObject<IE::BrowserHostEventDelegant>* pBroEventHandler;
    gfx::AcceleratedWidget renderHostHWnd;
    bool tridentCreated;
    std::unique_ptr<net::URLFetcher> imageFetcher;
    ImageDownloadCallback image_download_callback_;
    static int next_image_download_id_;
    bool initAsHide;
    std::wstring cookie_json;
    HANDLE cookieEvent;
    net::CookieList cookieListTemp;
    bool isNavigateStoped;
    bool needFireUnloadEvent;
    bool isNewWindow; //是否是由IE的DISPID_NEWWINDOW3事件创建的WebContent
    base::WeakPtrFactory<WebContentsIE> weakFactory;
    friend class WebContents;  // To implement factory methods.
    friend class WebContentsImpl;
    //friend class RenderFrameHostImpl;
    friend class WebContentsViewAura;
    friend class WebContentsViewIEAura;

    DISALLOW_COPY_AND_ASSIGN(WebContentsIE);
  };
} //namespace content

#endif
#endif
#endif
