// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_WEB_CONTENTS_WEB_CONTENTS_IE_H_
#define CONTENT_BROWSER_WEB_CONTENTS_WEB_CONTENTS_IE_H_

#include <stdint.h>
#include <map>
#include <set>
#include <string>

#include "base/compiler_specific.h"
#include "base/gtest_prod_util.h"
#include "base/macros.h"
#include "base/observer_list.h"
#include "base/process/process.h"
#include "base/synchronization/condition_variable.h"
#include "base/values.h"
#include "base/win/atl.h"
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
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/common/IE/common_ie.h"
#include "content/common/IE/render_mode_ie.h"
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
#include "net/cookies/cookie_store.h"
#include "net/http/http_response_headers.h"
#include "net/url_request/url_fetcher_delegate.h"
#include "net/url_request/url_request_context_getter.h"
#include "ui/base/page_transition_types.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/size.h"


namespace ie {
class BrowserHostEventDelegant;
struct RenderMode;
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

class CONTENT_EXPORT WebContentsIE : public WebContentsImpl,
                                     public net::URLFetcherDelegate {
 public:
  explicit WebContentsIE(BrowserContext* browser_context);
  ~WebContentsIE() override;

  void Init(const WebContents::CreateParams& params) override;
  void SetParentNativeViewAccessible(
      gfx::NativeViewAccessible accessible_parent);

  virtual void OnRendererHostViewSize(const gfx::Size& size);
  virtual void SetIECookie(const GURL& url);
  virtual void SetIEAutoLoginInfo(const std::wstring& frame_xpath,
                                  const std::wstring& name_xpath,
                                  const std::wstring& name_value,
                                  const std::wstring& pwd_xpath,
                                  const std::wstring& pwd_value,
                                  const std::wstring& login_button_xpath);
  virtual void SetListenLoginXPath(const std::wstring& url,
                                   const std::wstring& frame_xpath,
                                   const std::wstring& name_xpath,
                                   const std::wstring& pwd_xpath,
                                   const std::wstring& login_button_xpath);
  virtual void OnGetLoginInfo(const std::wstring& url,
                              const std::wstring& name,
                              const std::wstring& pwd);
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

  // URLFetcherDelegate--------------------------------------------------------
  void OnURLFetchComplete(const net::URLFetcher* source) override;

  bool LoadUrl(const NavigationController::LoadURLParams& params);
  void OnFinishNavigate(const GURL& url, const std::vector<GURL>& favicon_urls);
  void OnLoadUrlInNewContent(const GURL& url,
                             int flag,
                             bool* cancel,
                             IDispatch** dispatch);
  void OnBeforeNavigate(BSTR url, bool click_url, bool* cancel);
  IDispatch* GetWebBrowserIDispatch();
  void OnBeforeDownloadFile(BSTR url, const std::wstring& response_header);
  void OnUpdateCookie(const GURL& url,
                      const std::vector<std::wstring>& cookies);
  bool IsDownloading();

  void SendFunctionControl(const std::wstring& jsonStr);
  void SetCreateByIENewWindow(bool is_new);

  void OnQueryPrivateDns(const std::wstring& host,
                         std::wstring* ip_list_json_string);
  void QueryDnsOnIOThread(const std::wstring& host);
  void QueryDnsFinished(const std::wstring& ip_list_json_string);

  void OnWindowMove() override;

 private:
  bool CreateTridentWebView(const gfx::AcceleratedWidget& hwnd_parent,
                            const gfx::Size& size);
  bool IsTridentCreated();
  void CommitToEntry(GURL gurl, bool is_set_history);
  void LoadFinishedAndUpdateEntry(GURL gurl);
  void SetBrowserEmulation(ie::Emulation emulation);
  net::CookieList LoadCookie(const GURL& url);
  void OnBeginLoadCookie(
      const GURL& url,
      scoped_refptr<net::URLRequestContextGetter> url_request_context_getter);
  void OnCookieLoaded(const net::CookieList& cookie_list);
  std::wstring SerializationCookieList(const GURL& url,
                                       const net::CookieList& list);
  void OnBeginSaveCookie(
      const GURL& url,
      std::vector<std::wstring> cookies,
      scoped_refptr<net::URLRequestContextGetter> url_request_context_getter);
  void OnCookieSaveSuccess(bool success);
  GURL GetGurlFromCookie(const net::CanonicalCookie& cookie);
  void NavigateUrl(const NavigationController::LoadURLParams* params);

 private:
  friend class WebContentsObserver;
  friend class WebContents;  // To implement factory methods.

 private:
  friend class WebContents;
  friend class WebContentsImpl;
  friend class WebContentsViewAura;
  friend class WebContentsViewIEAura;

  mutable base::string16 title_;

  CComObject<ie::BrowserHostEventDelegant>* browser_event_handler_;
  gfx::AcceleratedWidget render_host_hwnd_;
  bool trident_created_;
  std::unique_ptr<net::URLFetcher> image_fetcher_;
  ImageDownloadCallback image_download_callback_;
  static int next_image_download_id_;
  bool init_as_hide_;
  std::wstring cookie_json_;
  HANDLE cookie_event_;
  net::CookieList cookie_list_temp_;
  bool is_navigate_stopped_;
  bool need_fire_unload_event_;
  // 是否是由IE的DISPID_NEWWINDOW3事件创建的WebContent
  bool is_new_window_;
  std::wstring query_dns_json_string_;
  base::WeakPtrFactory<WebContentsIE> weak_factory_for_io_;
  base::WeakPtrFactory<WebContentsIE> self_;

  DISALLOW_COPY_AND_ASSIGN(WebContentsIE);
};

}  // namespace content

#endif  // CONTENT_BROWSER_WEB_CONTENTS_WEB_CONTENTS_IE_H_
