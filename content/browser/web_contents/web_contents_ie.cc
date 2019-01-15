// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/web_contents/web_contents_ie.h"

#include <WinInet.h>
#include <stddef.h>
#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include "base/command_line.h"
#include "base/json/json_writer.h"
#include "base/lazy_instance.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/macros.h"
#include "base/metrics/histogram.h"
#include "base/metrics/user_metrics.h"
#include "base/process/process.h"
#include "base/single_thread_task_runner.h"
#include "base/strings/string16.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread_task_runner_handle.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "base/win/registry.h"
#include "base/win/win_util.h"
#include "build/build_config.h"
#include "chrome/browser/browser_process.h"
#include "components/download/public/common/download_stats.h"
#include "components/download/public/common/download_url_parameters.h"
#include "components/url_formatter/url_formatter.h"
#include "content/browser/accessibility/browser_accessibility_state_impl.h"
#include "content/browser/bad_message.h"
#include "content/browser/browser_plugin/browser_plugin_embedder.h"
#include "content/browser/browser_plugin/browser_plugin_guest.h"
#include "content/browser/child_process_security_policy_impl.h"
#include "content/browser/dom_storage/dom_storage_context_wrapper.h"
#include "content/browser/dom_storage/session_storage_namespace_impl.h"
#include "content/browser/download/mhtml_generation_manager.h"
#include "content/browser/download/save_package.h"
#include "content/browser/frame_host/cross_process_frame_connector.h"
#include "content/browser/frame_host/interstitial_page_impl.h"
#include "content/browser/frame_host/navigation_entry_impl.h"
#include "content/browser/frame_host/navigation_handle_impl.h"
#include "content/browser/frame_host/navigator_impl.h"
#include "content/browser/frame_host/render_frame_host_impl.h"
#include "content/browser/host_zoom_map_impl.h"
#include "content/browser/loader/resource_dispatcher_host_impl.h"
#include "content/browser/manifest/manifest_manager_host.h"
#include "content/browser/media/audio_stream_monitor.h"
#include "content/browser/media/capture/web_contents_audio_muter.h"
#include "content/browser/media/media_web_contents_observer.h"
#include "content/browser/plugin_content_origin_whitelist.h"
#include "content/browser/renderer_host/render_process_host_impl.h"
#include "content/browser/renderer_host/render_view_host_delegate_view.h"
#include "content/browser/renderer_host/render_view_host_impl.h"
#include "content/browser/renderer_host/render_widget_host_impl.h"
#include "content/browser/renderer_host/render_widget_host_input_event_router.h"
#include "content/browser/renderer_host/render_widget_host_view_aura.h"
#include "content/browser/renderer_host/render_widget_host_view_base.h"
#include "content/browser/renderer_host/render_widget_host_view_child_frame.h"
#include "content/browser/site_instance_impl.h"
#include "content/browser/web_contents/web_contents_view_aura.h"
#include "content/browser/web_contents/web_contents_view_guest.h"
#include "content/browser/webui/generic_handler.h"
#include "content/browser/webui/web_ui_controller_factory_registry.h"
#include "content/browser/webui/web_ui_impl.h"
#include "content/child/image_decoder.h"
#include "content/common/IE/browser_host_event_delegant_ie.h"
#include "content/common/IE/browser_process_ie.h"
#include "content/common/IE/common_ie.h"
#include "content/common/browser_plugin/browser_plugin_constants.h"
#include "content/common/browser_plugin/browser_plugin_messages.h"
#include "content/common/frame_messages.h"
#include "content/common/input_messages.h"
#include "content/common/resource_messages.h"
#include "content/common/view_messages.h"
#include "content/public/browser/ax_event_notification_details.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_plugin_guest_manager.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/browser/devtools_agent_host.h"
#include "content/public/browser/download_manager.h"
#include "content/public/browser/download_request_utils.h"
#include "content/public/browser/guest_mode.h"
#include "content/public/browser/invalidate_type.h"
#include "content/public/browser/javascript_dialog_manager.h"
#include "content/public/browser/load_notification_details.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/navigation_details.h"
#include "content/public/browser/notification_details.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/render_widget_host_iterator.h"
#include "content/public/browser/security_style_explanations.h"
#include "content/public/browser/site_isolation_policy.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/common/bindings_policy.h"
#include "content/public/common/browser_side_navigation_policy.h"
#include "content/public/common/content_constants.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/page_zoom.h"
#include "content/public/common/result_codes.h"
#include "content/public/common/url_constants.h"
#include "content/public/common/url_utils.h"
#include "content/public/common/web_preferences.h"
#include "net/base/data_url.h"
#include "net/base/filename_util.h"
#include "net/cookies/cookie_monster.h"
#include "net/dns/host_resolver_impl.h"
#include "net/http/http_cache.h"
#include "net/http/http_transaction_factory.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "net/url_request/url_fetcher.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_getter.h"
#include "third_party/blink/public/common/mime_util/mime_util.h"
#include "third_party/iaccessible2/ia2_api_all.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/aura/window.h"
#include "ui/base/layout.h"
#include "ui/display/display.h"
#include "ui/display/screen.h"
#include "ui/gfx/codec/png_codec.h"
#include "ui/gfx/favicon_size.h"
#include "ui/gfx/icon_util.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gl/gl_switches.h"
#include "url/gurl.h"
#include "web_contents_ie.h"

#if defined(OS_ANDROID)
#include "content/browser/android/content_video_view.h"
#include "content/browser/media/android/media_session.h"
#include "content/browser/media/android/media_web_contents_observer_android.h"
#endif  // OS_ANDROID

#if defined(OS_ANDROID) && !defined(USE_AURA)
#include "content/browser/android/date_time_chooser_android.h"
#include "content/browser/web_contents/web_contents_android.h"
#endif  // OS_ANDROID && !USE_AURA

#if defined(OS_MACOSX)
#include "base/mac/foundation_util.h"
#endif

#if defined(MOJO_SHELL_CLIENT)
#include "content/browser/web_contents/web_contents_view_mus.h"
#include "content/public/common/mojo_shell_connection.h"
#include "ui/aura/mus/mus_util.h"
#endif

namespace content {

int WebContentsIE::next_image_download_id_ = 80000000;
// unused const
// const wchar_t defaultIE6UA[] = L"Mozilla/4.0 (compatible; MSIE 6.0; Windows
// NT 5.1; SV1)\r\n"; const wchar_t defaultIE7UA[] = L"Mozilla/4.0 (compatible;
// MSIE 7.0; Windows NT 5.1; SV1)\r\n"; const wchar_t defaultIE8UA[] =
// L"Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; Trident/4.0)\r\n";
// const wchar_t defaultIE11UA[] = L"Mozilla/5.0 (Windows NT 6.2; WOW64;
// Trident/7.0; rv:11.0) like Gecko";

WebContentsIE::WebContentsIE(BrowserContext* browser_context)
    : WebContentsImpl(browser_context),
      browser_event_handler_(NULL),
      render_host_hwnd_(NULL),
      trident_created_(false),
      init_as_hide_(false),
      cookie_json_(L""),
      cookie_event_(NULL),
      is_navigate_stopped_(false),
      need_fire_unload_event_(false),
      is_new_window_(false),
      query_dns_json_string_(L""),
      weak_factory_for_io_(this),
      self_(this) {
  cookie_event_ = CreateEvent(NULL, TRUE, FALSE, NULL);
  ResetEvent(cookie_event_);
}

WebContentsIE::~WebContentsIE() {
  if (view_) {
    view_->OnWebContentDestroying();
  }
  CloseHandle(cookie_event_);
}

void WebContentsIE::Init(const WebContents::CreateParams& params) {
  init_as_hide_ = params.initially_hidden;
  __super::Init(params);

  // 关闭IE 检查发行商证书是否吊销，防止IE弹出提示窗口
  base::win::RegKey key;
  int64_t ret = key.Open(
      HKEY_CURRENT_USER,
      L"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
      KEY_ALL_ACCESS);
  if (ret == ERROR_SUCCESS) {
    key.WriteValue(L"CertificateRevocation", (DWORD)0);
    key.Close();
  }

  // chrome delete GetRenderProcessHost() from WebContentsImpl
  RenderProcessHost* pHost = GetMainFrame()->GetProcess();
  if (pHost && render_mode_.core == ie::IE_CORE)
    pHost->SetTridentCore(true);

  CComObject<ie::BrowserHostEventDelegant>::CreateInstance(
      &browser_event_handler_);
  if (browser_event_handler_) {
    browser_event_handler_->AddRef();
    browser_event_handler_->SetWebContent(this);
  }
}

void WebContentsIE::OnRendererHostViewSize(const gfx::Size& size) {
  if (browser_event_handler_ == NULL || GetRenderWidgetHostView() == NULL)
    return;
  GetRenderWidgetHostView()->Hide();
  RECT rc;
  ::GetWindowRect(render_host_hwnd_, &rc);
  browser_event_handler_->SetBrowserRect(rc);
}

void WebContentsIE::OnWindowMove() {
  gfx::Size size(0, 0);
  OnRendererHostViewSize(size);
}

void WebContentsIE::SetIECookie(const GURL& url) {
  net::CookieList list;
  list = LoadCookie(url);
  if (list.empty() == false && browser_event_handler_) {
    std::wstring cookie = L"";
    std::string cookieUrl = url.scheme() + "://" + url.host();
    cookie = SerializationCookieList(GURL(cookieUrl), list);
    browser_event_handler_->SetCookie(cookie, 0);
  }
}

void WebContentsIE::SetIEAutoLoginInfo(const std::wstring& frame_xpath,
                                       const std::wstring& name_xpath,
                                       const std::wstring& name_value,
                                       const std::wstring& pwdXPath,
                                       const std::wstring& pwd_value,
                                       const std::wstring& login_button_xpath) {
  if (browser_event_handler_ == NULL)
    return;
  browser_event_handler_->AutoLogin(frame_xpath, name_xpath, name_value,
                                    pwdXPath, pwd_value, login_button_xpath);
}

void WebContentsIE::SetListenLoginXPath(
    const std::wstring& url,
    const std::wstring& frame_xpath,
    const std::wstring& name_xpath,
    const std::wstring& pwdXPath,
    const std::wstring& login_button_xpath) {
  if (url.empty() == true || name_xpath.empty() || pwdXPath.empty() ||
      login_button_xpath.empty() || browser_event_handler_ == NULL)
    return;

  browser_event_handler_->SetListenLoginXPath(url, frame_xpath, name_xpath,
                                              pwdXPath, login_button_xpath);
}

void WebContentsIE::OnGetLoginInfo(const std::wstring& url,
                                   const std::wstring& name,
                                   const std::wstring& pwd) {
  RenderFrameHostImpl* impl = GetMainFrame();
  if (impl == NULL)
    return;

  // comment just for cmpl by webb
  for (WebContentsObserver& observer : observers_) {
    observer.OnGetLoginContext(impl, base::UTF16ToUTF8(url),
                               base::UTF16ToUTF8(name), base::UTF16ToUTF8(pwd));
  }
}

bool WebContentsIE::IsDevToolsOpened() {
  if (browser_event_handler_ == NULL)
    return false;
  return browser_event_handler_->IsDevToolsOpened();
}

void WebContentsIE::ShowDevTools(bool show) {
  if (browser_event_handler_ == NULL)
    return;
  browser_event_handler_->ShowDevTools(show);
}

void WebContentsIE::SetBrowserZoom(int percent) {
  if (browser_event_handler_)
    browser_event_handler_->SetZoomPercent(percent);
}

void WebContentsIE::Print() {
  if (browser_event_handler_) {
    browser_event_handler_->Print();
  }
}

void WebContentsIE::FindWithoutParams() {
  if (browser_event_handler_) {
    browser_event_handler_->Find();
  }
}

void WebContentsIE::Cut() {
  if (browser_event_handler_) {
    browser_event_handler_->Cut();
  }
}

void WebContentsIE::Copy() {
  if (browser_event_handler_) {
    browser_event_handler_->Copy();
  }
}

void WebContentsIE::Paste() {
  if (browser_event_handler_) {
    browser_event_handler_->Paste();
  }
}

void WebContentsIE::SetDelegate(WebContentsDelegate* delegate) {
  __super::SetDelegate(delegate);
  RendererResponsive(NULL);
}

void WebContentsIE::WasShown() {
  controller_.SetActive(true);
  for (RenderWidgetHostView* view : GetRenderWidgetHostViewsInTree()) {
    if (view) {
      view->Hide();
    }
  }
  OnRendererHostViewSize(gfx::Size());
  if (browser_event_handler_)
    browser_event_handler_->Show(true);
  SetVisibility(Visibility::VISIBLE);
}

void WebContentsIE::WasHidden() {
  for (RenderWidgetHostView* view : GetRenderWidgetHostViewsInTree()) {
    if (view) {
      view->Hide();
    }
  }
  if (browser_event_handler_)
    browser_event_handler_->Show(false);
  SetVisibility(Visibility::HIDDEN);
}

const base::string16& WebContentsIE::GetTitle() const {
  if (browser_event_handler_) {
    title_ = browser_event_handler_->GetTitle();
  }
  return title_;
}

bool WebContentsIE::IsLoading() const {
  if (browser_event_handler_) {
    return browser_event_handler_->IsLoading();
  }
  return false;
}

void Find(int request_id,
          const base::string16& search_text,
          const blink::WebFindOptions& options) {}

int WebContentsIE::DownloadImage(const GURL& url,
                                 bool is_favicon,
                                 uint32_t max_bitmap_size,
                                 bool bypass_cache,
                                 ImageDownloadCallback callback) {
  const int download_id = ++next_image_download_id_;
  // can not apply operator =
  // need more investigation by webb
  image_download_callback_ = std::move(callback);
  image_fetcher_ =
      net::URLFetcher::Create(GURL(url), net::URLFetcher::GET, this);
  image_fetcher_->SetRequestContext(
      content::BrowserContext::GetDefaultStoragePartition(GetBrowserContext())
          ->GetURLRequestContext());
  image_fetcher_->Start();
  return download_id;
}

void WebContentsIE::Focus() {
  __super::Focus();
  for (RenderWidgetHostView* view : GetRenderWidgetHostViewsInTree()) {
    if (view) {
      view->Hide();
    }
  }
}

RenderFrameHostImpl* WebContentsIE::GetFocusedFrame() {
  return GetMainFrame();
}

void WebContentsIE::SetParentNativeViewAccessible(
    gfx::NativeViewAccessible accessible_parent) {
  if (accessible_parent == NULL)
    return;
  IDispatch* pAccDisp = NULL;
  accessible_parent->get_accParent(reinterpret_cast<IDispatch**>(&pAccDisp));
  if (pAccDisp == NULL)
    return;
  IAccessible2* pAccParent = NULL;
  pAccDisp->QueryInterface(IID_IAccessible2,
                           reinterpret_cast<void**>(&pAccParent));
  if (pAccParent == NULL)
    return;
  HWND hWnd = 0;
  pAccParent->get_windowHandle(&hWnd);
  pAccDisp->Release();
  pAccParent->Release();
  if (hWnd && browser_event_handler_) {
    browser_event_handler_->SetParentHWND(hWnd);
  }
}

void WebContentsIE::Stop() {
  if (browser_event_handler_ == NULL)
    return;
  browser_event_handler_->Stop();
  for (WebContentsObserver& observer : observers_) {
    observer.NavigationStopped();
  }
  DidStopLoading();
  is_navigate_stopped_ = true;
}

bool WebContentsIE::NeedToFireBeforeUnload() {
  if (!need_fire_unload_event_ && browser_event_handler_) {
    browser_event_handler_->CloseBrowser();
    browser_event_handler_->Release();
    browser_event_handler_ = NULL;
  }
  return need_fire_unload_event_;
}

void WebContentsIE::DispatchBeforeUnload() {
  if (need_fire_unload_event_ && browser_event_handler_) {
    browser_event_handler_->CloseBrowser();
    browser_event_handler_->Release();
    browser_event_handler_ = NULL;
  }
}

int WebContentsIE::GetMinimumZoomPercent() const {
  if (browser_event_handler_ == NULL)
    return 100;
  if (browser_event_handler_->IsCanZoom())
    return __super::GetMinimumZoomPercent();
  return 100;
}

int WebContentsIE::GetMaximumZoomPercent() const {
  if (browser_event_handler_ == NULL)
    return 100;
  if (browser_event_handler_->IsCanZoom())
    return __super::GetMaximumZoomPercent();
  return 100;
}

void WebContentsIE::CreateRenderWidgetHostViewForRenderManager(
    RenderViewHost* render_view_host) {
  RenderWidgetHostViewBase* rwh_view =
      view_->CreateViewForWidget(render_view_host->GetWidget(), false);

  // Now that the RenderView has been created, we need to tell it its size.
  if (rwh_view) {
    gfx::Size size = GetSizeForNewRenderView(true);
    rwh_view->SetSize(size);
    if (IsTridentCreated() && browser_event_handler_) {
      browser_event_handler_->CloseBrowser();
      trident_created_ = false;
    }
    if (IsTridentCreated() == false) {
      gfx::AcceleratedWidget hWnd =
          rwh_view->AccessibilityGetAcceleratedWidget();
      render_host_hwnd_ = hWnd;
      // hWnd = GetParent(hWnd); //chjy test
      if (CreateTridentWebView(hWnd, size)) {
        trident_created_ = true;
        if (init_as_hide_)
          WasHidden();
        else
          WasShown();
      }
    }
  }
}
void WebContentsIE::RendererUnresponsive(
    RenderWidgetHostImpl* render_widget_host,
    base::RepeatingClosure hang_monitor_restarter) {
  return;
}

void WebContentsIE::RendererResponsive(
    RenderWidgetHostImpl* render_widget_host) {
  if (delegate_)
    delegate_->RendererResponsive(this, render_widget_host);
}

void WebContentsIE::Activate() {
  __super::Activate();
  for (RenderWidgetHostView* view : GetRenderWidgetHostViewsInTree()) {
    if (view) {
      view->Hide();
    }
  }
  if (browser_event_handler_)
    browser_event_handler_->Show(true);
}

bool WebContentsIE::LoadUrl(const NavigationController::LoadURLParams& params) {
  NavigationController::LoadURLParams* bind_param =
      new NavigationController::LoadURLParams(params.url);
  bind_param->transition_type = params.transition_type;
  content::BrowserThread::PostTask(
      content::BrowserThread::UI, FROM_HERE,
      base::Bind(&WebContentsIE::NavigateUrl, self_.GetWeakPtr(), bind_param));
  return true;
}

void WebContentsIE::OnFinishNavigate(const GURL& url,
                                     const std::vector<GURL>& favicon_urls) {
  if (browser_event_handler_) {
    int mode = browser_event_handler_->GetDocMode();
    if (mode > 0 && mode != static_cast<int>(render_mode_.version) &&
        render_mode_.version != ie::DOCNONE &&
        render_mode_.version != ie::DOCSYS) {
      ie::DocumentMode doc_mode = ie::IE8;
      switch (render_mode_.version) {
        case ie::DOC6:
          doc_mode = ie::IE5;
          break;
        case ie::DOC7:
          doc_mode = ie::IE7;
          break;
        case ie::DOC8:
          doc_mode = ie::IE8;
          break;
        case ie::DOC9:
          doc_mode = ie::IE9;
          break;
        case ie::DOC10:
          doc_mode = ie::IE10;
          break;
        case ie::DOC11:
          doc_mode = ie::IE11;
          break;
        case ie::DOCSYS:
        case ie::DOCNONE:
          break;
      }
      browser_event_handler_->SetDocMode(doc_mode);
      return;
    }
  }

  int index = GetController().GetCurrentEntryIndex();
  NavigationEntryImpl* entry = GetController().GetEntryAtIndex(index);
  if (entry) {
    if (entry->GetURL() != url && url.SchemeIsFile())
      CommitToEntry(url, true);
  }
  LoadFinishedAndUpdateEntry(url);

  std::vector<content::FaviconURL> vec;
  for (auto it = favicon_urls.begin(); it != favicon_urls.end(); it++) {
    content::FaviconURL favUrl;
    favUrl.icon_type = content::FaviconURL::IconType::kFavicon;
    GURL tempUrl = GURL(*it);
    favUrl.icon_url = tempUrl;
    vec.push_back(favUrl);
  }

  for (WebContentsObserver& observer : observers_) {
    observer.DidUpdateFaviconURL(vec);
  }
}

void WebContentsIE::OnLoadUrlInNewContent(const GURL& url,
                                          int flag,
                                          bool* cancel,
                                          IDispatch** dispatch) {
  if (cancel == NULL || browser_event_handler_ == NULL)
    return;

  ie::RenderMode mode;
  WebContentsDelegate* delegate = GetDelegate();
  if (delegate && !delegate->UrlCompared(url, mode))
    mode = render_mode_;

  scoped_refptr<SiteInstance> site_instance =
      SiteInstance::CreateForURL(GetBrowserContext(), url);
  if (mode.core == ie::IE_CORE)
    site_instance->GetProcess()->SetTridentCore(true);
  else
    site_instance->GetProcess()->SetTridentCore(false);
  site_instance->GetProcess()->Init();

  CreateParams create_params(GetBrowserContext(), site_instance.get());
  create_params.renderer_mode = mode;
  create_params.initial_size = GetContainerBounds().size();
  WebContentsImpl* new_contents = NULL;
  std::unique_ptr<WebContents> wbc = WebContents::Create(create_params);
  new_contents = (WebContentsImpl*)(wbc.get());
  if (new_contents->GetRendererMode().core == ie::IE_CORE) {
    WebContentsIE* web_contents_ie = (WebContentsIE*)(new_contents);
    if (web_contents_ie)
      web_contents_ie->SetCreateByIENewWindow(true);
  }

  if (delegate) {
    WindowOpenDisposition disposition =
        WindowOpenDisposition::NEW_FOREGROUND_TAB;
    if ((flag & ie::POPUP) == ie::POPUP)
      disposition = WindowOpenDisposition::NEW_POPUP;

    gfx::Rect initial_rect;
    initial_rect.set_size(create_params.initial_size);
    delegate->AddNewContents(this, std::move(wbc), disposition, initial_rect,
                             false, NULL);
  }

  // new_contents->CreateRenderWidgetHostViewForRenderManager(new_contents->GetRenderViewHost());

  ui::PageTransition transition = ui::PAGE_TRANSITION_LINK;
  if (mode.core == ie::IE_CORE && (flag & ie::FROMDIALOG) == 0)
    transition = ui::PAGE_TRANSITION_IE_NEWWINDOW;
  OpenURLParams open_params(url, Referrer(), WindowOpenDisposition::CURRENT_TAB,
                            transition, true /* is_renderer_initiated */);
  new_contents->OpenURL(open_params);

  *cancel = true;
  if (mode.core == ie::IE_CORE) {
    WebContentsIE* ie_contents = NULL;
    ie_contents = static_cast<WebContentsIE*>(new_contents);
    if (ie_contents == NULL)
      return;
    *cancel = false;
    if ((flag & ie::FROMDIALOG) == ie::FROMDIALOG)
      *cancel = true;
    IDispatch* idisp = ie_contents->GetWebBrowserIDispatch();
    *dispatch = idisp;
  }
}

void WebContentsIE::OnBeforeNavigate(BSTR url, bool click_url, bool* cancel) {
  *cancel = false;
  if (IsDownloading()) {
    OpenURLParams open_params(
        GURL(url), Referrer(), WindowOpenDisposition::NEW_FOREGROUND_TAB,
        ui::PAGE_TRANSITION_LINK, false /* is_renderer_initiated */);
    delegate_->OpenURLFromTab(this, open_params);
    *cancel = true;
    return;
  }
  GURL navUrl(url);
  SetIECookie(navUrl);

  RenderFrameHostImpl* rfh = GetMainFrame();
  is_navigate_stopped_ = false;
  if (rfh) {
    if (click_url)
      DidStartLoading(rfh->frame_tree_node(), true);
    std::vector<GURL> vc_gurl;
    rfh->frame_tree_node()->navigator()->DidStartProvisionalLoad(
        rfh, navUrl, vc_gurl,
        base::TimeTicks::Now());  // will call
                                  // WebContentsObserver::DidStartNavigation may
                                  // be switch renderer
    //{
    //  DidStartLoading(rfh->frame_tree_node(), true);
    //  rfh->frame_tree_node()->navigator()->
    //    DidStartProvisionalLoad(rfh, navUrl, base::TimeTicks::Now());  //will
    //    call WebContentsObserver::DidStartNavigation may be switch renderer
    //}
  }
  if (is_navigate_stopped_) {
    *cancel = true;
    return;
  }
  int index = GetController().GetCurrentEntryIndex();
  NavigationEntryImpl* entry = GetController().GetEntryAtIndex(index);
  if (entry == NULL)
    return;
  GURL curUrl = entry->GetURL();
  if (click_url && curUrl != navUrl) {
    if (is_navigate_stopped_ == false)
      CommitToEntry(navUrl, true);
    else
      *cancel = true;
  }
}

IDispatch* WebContentsIE::GetWebBrowserIDispatch() {
  IDispatch* idisp = NULL;
  if (browser_event_handler_)
    idisp = browser_event_handler_->GetBrowserIDispatch();
  return idisp;
}

void WebContentsIE::OnBeforeDownloadFile(BSTR url,
                                         const std::wstring& response_header) {
  GURL gurl(url);
  DownloadManager* manager =
      BrowserContext::GetDownloadManager(GetBrowserContext());
  if (manager == NULL)
    return;
  std::unique_ptr<download::DownloadUrlParameters> downloadParam(
      content::DownloadRequestUtils::CreateDownloadForWebContentsMainFrame(
          this, gurl, NO_TRAFFIC_ANNOTATION_YET));
  downloadParam->SetIEDownloader(browser_event_handler_);
  downloadParam->SetIEDownloadResponseheader(response_header);
  manager->DownloadUrl(std::move(downloadParam));
  DidStopLoading();
}

void WebContentsIE::OnUpdateCookie(const GURL& url,
                                   const std::vector<std::wstring>& cookies) {
  scoped_refptr<net::URLRequestContextGetter> url_request_context_getter =
      content::BrowserContext::GetDefaultStoragePartition(GetBrowserContext())
          ->GetURLRequestContext();
  if (url_request_context_getter == NULL || url.SchemeIsHTTPOrHTTPS() == false)
    return;
  content::BrowserThread::PostTask(
      content::BrowserThread::IO, FROM_HERE,
      base::BindOnce(&WebContentsIE::OnBeginSaveCookie, base::Unretained(this),
                     url, cookies, std::move(url_request_context_getter)));
}

bool WebContentsIE::IsDownloading() {
  if (browser_event_handler_)
    return browser_event_handler_->IsDownloading();
  return false;
}

void WebContentsIE::SendFunctionControl(const std::wstring& jsonStr) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  if (browser_event_handler_ == NULL)
    return;
  browser_event_handler_->SendFunctionControl(jsonStr);
}

void WebContentsIE::SetCreateByIENewWindow(bool is_new) {
  is_new_window_ = is_new;
}

void WebContentsIE::OnQueryPrivateDns(const std::wstring& host,
                                      std::wstring* ip_list_json_string) {
  query_dns_json_string_.clear();
  content::BrowserThread::PostTask(
      content::BrowserThread::IO, FROM_HERE,
      base::Bind(&WebContentsIE::QueryDnsOnIOThread,
                 weak_factory_for_io_.GetWeakPtr(), host));
  // base::MessageLoopForUI::current()->Run();
  *ip_list_json_string = query_dns_json_string_;
}

void WebContentsIE::QueryDnsOnIOThread(const std::wstring& host) {
  std::string hostStr = "";
  std::wstring ip_list_json_string = L"";
  hostStr = base::UTF16ToASCII(host);
  base::ListValue* json = NULL;
  json = net::HostResolverImpl::PrivateDnsCompared(hostStr);
  if (json) {
    std::string buff = "";
    base::JSONWriter::Write(*json, &buff);
    ip_list_json_string = base::UTF8ToUTF16(buff);
  }
  content::BrowserThread::PostTask(
      content::BrowserThread::UI, FROM_HERE,
      base::Bind(&WebContentsIE::QueryDnsFinished, self_.GetWeakPtr(),
                 ip_list_json_string));
}

void WebContentsIE::QueryDnsFinished(const std::wstring& ip_list_json_string) {
  if (ip_list_json_string.empty() == false) {
    query_dns_json_string_ = ip_list_json_string;
  }
  // base::MessageLoopForUI::current()->QuitWhenIdle();
}

void WebContentsIE::OnURLFetchComplete(const net::URLFetcher* source) {
  if (image_download_callback_.is_null())
    return;

  int http_code = 400;
  do {
    if (!source->GetStatus().is_success())
      break;
    http_code = source->GetResponseCode();
    if (http_code != 200)
      break;

    std::string buf = "";
    source->GetResponseAsString(&buf);
    if (buf.size() < 8)
      break;

    SkBitmap bitmap;

    // PNG header : 89 50 4E 47 0D 0A 1A 0A
    unsigned char* image_data = (unsigned char*)buf.data();
    if (image_data[0] == 0x89 && image_data[1] == 0x50 &&
        image_data[2] == 0x4E && image_data[3] == 0x47 &&
        image_data[4] == 0x0D && image_data[5] == 0x0A &&
        image_data[6] == 0x1A && image_data[7] == 0x0A) {
      if (!gfx::PNGCodec::Decode(image_data, buf.size(), &bitmap))
        break;
    } else {
      base::FilePath path = this->GetBrowserContext()->GetPath();
      std::wstring fileName = path.value() + L"\\ie_temp.ico";

      FILE* file = _wfopen(fileName.c_str(), L"wb");
      if (file == NULL)
        break;
      fwrite(buf.data(), 1, buf.size(), file);
      fclose(file);

      HICON hicon = (HICON)::LoadImage(NULL, fileName.c_str(), IMAGE_ICON, 0, 0,
                                       LR_LOADFROMFILE);
      if (hicon == NULL)
        break;

      SkBitmap tempBmp = IconUtil::CreateSkBitmapFromHICON(hicon);
      ::DestroyIcon(hicon);
      if (tempBmp.isNull())
        break;
      // can not find any commits of SkBitmap.h by webb
      bitmap = tempBmp;
      // tempBmp->copyTo(&bitmap);
      // delete tempBmp;
    }

    std::vector<SkBitmap> images;
    images.push_back(bitmap);
    std::vector<gfx::Size> original_image_sizes;
    gfx::Size size(bitmap.width(), bitmap.height());
    original_image_sizes.push_back(size);
    std::move(image_download_callback_)
        .Run(next_image_download_id_, source->GetResponseCode(),
             source->GetURL(), images, original_image_sizes);

    return;
  } while (0);

  std::vector<SkBitmap> images;
  std::vector<gfx::Size> original_image_sizes;
  std::move(image_download_callback_)
      .Run(next_image_download_id_, http_code, source->GetURL(), images,
           original_image_sizes);
}

bool WebContentsIE::CreateTridentWebView(
    const gfx::AcceleratedWidget& hwnd_parent,
    const gfx::Size& size) {
  if (browser_event_handler_ == NULL || IsWindow(hwnd_parent) == FALSE)
    return false;
  browser_event_handler_->SetHostHWND(hwnd_parent);
  SetBrowserEmulation(render_mode_.emulation);
  if (browser_event_handler_->CreateBrowser(render_mode_.emulation,
                                            is_new_window_)) {
    // std::wstring ua = L"";
    // switch (ieVer_)
    //{
    // case ie::DOC6:
    //  ua = defaultIE6UA;
    //  break;
    // case ie::DOC7:
    //  ua = defaultIE7UA;
    //  break;
    // case ie::DOC8:
    //  ua = defaultIE8UA;
    //  break;
    //}
    // if (!ua.empty())
    //  browser_event_handler_->SetUserAgent(ua);
    RendererResponsive(GetRenderViewHost()->GetWidget());
    return true;
  }
  return false;
}

bool WebContentsIE::IsTridentCreated() {
  return trident_created_;
}

void WebContentsIE::CommitToEntry(GURL gurl, bool is_set_history) {
  RenderFrameHostImpl* rfh = GetMainFrame();
  if (rfh == NULL)
    return;
  FrameHostMsg_DidCommitProvisionalLoad_Params params;
  params.http_status_code = 200;
  params.original_request_url = gurl;
  params.origin = url::Origin::Create(gurl);
  params.url = gurl;
  params.page_state = PageState::CreateFromURL(gurl);
  int index = GetController().GetCurrentEntryIndex();
  if (index >= 0) {
    NavigationEntryImpl* entry = GetController().GetEntryAtIndex(index);
    int id = entry->GetUniqueID();
    params.nav_entry_id = id;
  }
  LoadCommittedDetails details;
  details.did_replace_entry = false;
  if (is_set_history) {
    details.type = NAVIGATION_TYPE_NEW_PAGE;
    params.did_create_new_entry = true;
    params.intended_as_new_entry = true;
  } else {
    details.type = NAVIGATION_TYPE_EXISTING_PAGE;
    params.did_create_new_entry = false;
    params.intended_as_new_entry = false;
  }

  // comment just for cmpl
  std::unique_ptr<NavigationHandleImpl> navHandle =
      NavigationHandleImpl::Create(
          gurl, std::vector<GURL>(), rfh->frame_tree_node(),
          true,   // is_renderer_initiated
          false,  // is_same_document
          base::TimeTicks::Now(), 0,
          false,                  // started_from_context_menu
          CSPDisposition::CHECK,  // should_check_main_world_csp
          false,                  // is_form_submission
          nullptr,                // navigation_ui_data
          "GET", net::HttpRequestHeaders(),
          nullptr,  // resource_request_body
          Referrer(),
          false,  // has_user_gesture
          ui::PAGE_TRANSITION_LINK,
          false,  // is_external_protocol
          REQUEST_CONTEXT_TYPE_LOCATION);
  GetController().RendererDidNavigate(rfh, params, &details, true,
                                      navHandle.get());

  for (WebContentsObserver& observer : observers_) {
    observer.NavigationEntryCommitted(details);
  }
}

void WebContentsIE::LoadFinishedAndUpdateEntry(GURL gurl) {
  RenderFrameHostImpl* rfh = GetMainFrame();
  if (rfh == NULL)
    return;
  std::wstring url;
  int index = GetController().GetCurrentEntryIndex();
  NavigationEntryImpl* entry = GetController().GetEntryAtIndex(index);
  if (entry) {
    entry->SetVirtualURL(gurl);
    entry->SetURL(gurl);
    // WebContentsImpl delete it.
    // DidCommitProvisionalLoad(rfh, gurl, entry->GetTransitionType());
  }
  NotifyNavigationStateChanged(INVALIDATE_TYPE_URL);
  for (WebContentsObserver& observer : observers_) {
    observer.DocumentLoadedInFrame(rfh);
  }

  for (WebContentsObserver& observer : observers_) {
    observer.DidFinishLoad(rfh, gurl);
  }

  DidStopLoading();
}

void WebContentsIE::SetBrowserEmulation(ie::Emulation emulation) {
  DWORD value = 7000;
  switch (emulation) {
    case ie::EMULATION7:
      value = 7000;
      break;
    case ie::EMULATION8:
      value = 8888;
      break;
    case ie::EMULATION9:
      value = 9999;
      break;
    case ie::EMULATION10:
      value = 10001;
      break;
    case ie::EMULATION11:
      value = 11001;
      break;
    default:
      break;
  }
  int64_t err = 0;
  HKEY key = 0;
  err = RegOpenKey(HKEY_CURRENT_USER,
                   L"Software\\Microsoft\\Internet "
                   L"Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION",
                   &key);
  if (err == ERROR_SUCCESS) {
    err = RegSetValueEx(key, L"RedCore.exe", 0, REG_DWORD, (const BYTE*)&value,
                        sizeof(value));
    RegCloseKey(key);
  }
}

net::CookieList WebContentsIE::LoadCookie(const GURL& url) {
  net::CookieList list;
  scoped_refptr<net::URLRequestContextGetter> url_request_context_getter =
      content::BrowserContext::GetDefaultStoragePartition(GetBrowserContext())
          ->GetURLRequestContext();
  if (url_request_context_getter == NULL || url.SchemeIsHTTPOrHTTPS() == false)
    return list;
  if (cookie_event_ == NULL)
    return list;
  ResetEvent(cookie_event_);
  content::BrowserThread::PostTask(
      content::BrowserThread::IO, FROM_HERE,
      base::BindOnce(&WebContentsIE::OnBeginLoadCookie, base::Unretained(this),
                     url, std::move(url_request_context_getter)));
  if (WaitForSingleObject(cookie_event_, INFINITE) == WAIT_OBJECT_0) {
    list = cookie_list_temp_;
  }
  return list;
}

void WebContentsIE::OnBeginLoadCookie(
    const GURL& url,
    scoped_refptr<net::URLRequestContextGetter> url_request_context_getter) {
  net::CookieStore* pStore =
      url_request_context_getter->GetURLRequestContext()->cookie_store();
  pStore->GetAllCookiesAsync(
      base::Bind(&WebContentsIE::OnCookieLoaded, base::Unretained(this)));
}

void WebContentsIE::OnCookieLoaded(const net::CookieList& cookie_list) {
  if (cookie_event_ == NULL)
    return;
  cookie_list_temp_ = cookie_list;
  SetEvent(cookie_event_);
}

/* cookie序列化为json后的 json格式如下:
  {
    "allCookies":
    [
      {
        "url": "http: //www.123.com",
        "cookie": "a=bc"
      },
      {
        "url": "http: //www.456.com",
        "cookie": "d=ef; HTTPONLY"
      }
    ]
  }
*/
std::wstring WebContentsIE::SerializationCookieList(
    const GURL& url,
    const net::CookieList& list) {
  base::DictionaryValue rootValue;
  // rootValue.SetString("url", );
  // rootValue.SetString("url", url.spec());
  base::ListValue* cookiesList = new base::ListValue;
  net::CookieList::const_iterator iter = list.begin();
  for (; iter != list.end(); iter++) {
    base::DictionaryValue* cookieDict = new base::DictionaryValue;
    GURL source = GetGurlFromCookie(*iter);
    if (source.is_empty()) {
      std::string domain = iter->Domain();
      if (domain.length() > 1 && domain[0] == '.')
        domain = domain.substr(1);

      if (iter->IsSecure())
        source = GURL(std::string(url::kHttpsScheme) +
                      url::kStandardSchemeSeparator + domain + "/");
      else
        source = GURL(std::string(url::kHttpScheme) +
                      url::kStandardSchemeSeparator + domain + "/");
    }
    cookieDict->SetString("url", source.spec());

    std::string buff = iter->Name() + "=" + iter->Value();
    if (iter->Path().empty() == false)
      buff = buff + "; PATH=" + iter->Path();
    if (iter->Domain().empty() == false)
      buff = buff + "; DOMAIN=" + iter->Domain();
    if (iter->IsHttpOnly())
      buff = buff + "; HTTPONLY";
    if (iter->IsSecure())
      buff = buff + "; Secure=true";

    // IsFirstPartyOnly -> IsSameSite, comment for cmpl by webb
    // 4654941d380d76638610198ac497015b8d979bb9
    // if (iter->IsSameSite() == false)
    //   buff = buff + "; INTERNET_COOKIE_THIRD_PARTY";

    cookieDict->SetString("cookie", buff);

    cookiesList->Append(std::unique_ptr<base::DictionaryValue>(cookieDict));
  }
  rootValue.Set("allCookies", std::unique_ptr<base::ListValue>(cookiesList));
  std::string buff = "";
  base::JSONWriter::Write(rootValue, &buff);
  std::wstring cookie = base::UTF8ToUTF16(buff);
  return cookie;
}

void WebContentsIE::OnBeginSaveCookie(
    const GURL& url,
    std::vector<std::wstring> cookies,
    scoped_refptr<net::URLRequestContextGetter> url_request_context_getter) {
  net::CookieStore* cookieStore =
      url_request_context_getter->GetURLRequestContext()->cookie_store();
  if (cookieStore == NULL)
    return;
  std::vector<std::wstring>::iterator iter = cookies.begin();
  for (; iter != cookies.end(); iter++) {
    net::CookieOptions option;
    option.set_include_httponly();
    // option.set_include_first_party_only_cookies();
    std::string buff = base::UTF16ToASCII(*iter);
    cookieStore->SetCookieWithOptionsAsync(
        url, buff, option,
        base::Bind(&WebContentsIE::OnCookieSaveSuccess,
                   base::Unretained(this)));
  }
}

void WebContentsIE::OnCookieSaveSuccess(bool success) {
  // if (success == false)
  //{
  //  int a = 10;
  //  a++;
  //}
}

GURL WebContentsIE::GetGurlFromCookie(const net::CanonicalCookie& cookie) {
  // just for cmpl by
  // GURL url = cookie.Source();
  GURL url;
  if (url.SchemeIsFile())
    return url;

  url::Replacements<char> replacements;
  replacements.ClearPort();

  return url.GetOrigin().ReplaceComponents(replacements);
}

void WebContentsIE::NavigateUrl(
    const NavigationController::LoadURLParams* params) {
  if (browser_event_handler_ == NULL)
    return;

  //  old navigation code is removed: pending_render_view_host()
  // if (GetRenderManager() && GetRenderManager()->pending_render_view_host())
  // {
  //   RenderFrameHost* pFrameHost =
  //   GetRenderManager()->pending_render_view_host()->GetMainFrame();
  //   RenderFrameHostImpl* pFrameHostImpl =
  //   static_cast<RenderFrameHostImpl*>(pFrameHost); if (pFrameHost)
  //     GetRenderManager()->DidNavigateFrame(pFrameHostImpl, false);
  // }

  RenderFrameHostImpl* rfh = GetMainFrame();
  if (rfh) {
    // NavigationHandleImpl::Create会触发WebContentsImpl::DidStartNavigation函数，从而进入黑白名单过滤功能
    is_navigate_stopped_ = false;
    std::unique_ptr<NavigationHandleImpl> navHandle =
        NavigationHandleImpl::Create(
            params->url, std::vector<GURL>(), rfh->frame_tree_node(),
            true,   // is_renderer_initiated
            false,  // is_same_document
            base::TimeTicks::Now(), 0,
            false,                  // started_from_context_menu
            CSPDisposition::CHECK,  // should_check_main_world_csp
            false,                  // is_form_submission
            nullptr,                // navigation_ui_data
            "GET", net::HttpRequestHeaders(),
            nullptr,  // resource_request_body
            Referrer(),
            false,  // has_user_gesture
            ui::PAGE_TRANSITION_LINK,
            false,  // is_external_protocol
            REQUEST_CONTEXT_TYPE_LOCATION);
    // NavigationHandleImpl::Create(params.url, rfh->frame_tree_node(),
    // base::TimeTicks::Now());
    DidFinishNavigation(navHandle.get());
    navHandle.release();
    if (is_navigate_stopped_) {
      // 让地址栏的URL恢复到之前的URL
      FrameHostMsg_DidFailProvisionalLoadWithError_Params errorParams;
      errorParams.url = params->url;
      errorParams.error_code = -3;
      errorParams.error_description = L"未知错误";
      rfh->frame_tree_node()->navigator()->DidFailProvisionalLoadWithError(
          rfh, errorParams);
      return;
    }
  }

  base::FilePath path;
  bool isFilePath = false;
  if (params->url.SchemeIsFile())
    isFilePath = net::FileURLToFilePath(params->url, &path);

  std::wstring url = L"";
  if (isFilePath)
    url = path.AsUTF16Unsafe();
  else
    url = base::ASCIIToUTF16(params->url.spec());

  GURL curUrl;
  NavigationEntryImpl* entry = GetController().GetLastCommittedEntry();
  if (entry)
    curUrl = entry->GetURL();

  if (!PageTransitionTypeIncludingQualifiersIs(
          params->transition_type,
          ui::PageTransition::PAGE_TRANSITION_RELOAD) &&
      !PageTransitionTypeIncludingQualifiersIs(
          params->transition_type,
          ui::PageTransition::PAGE_TRANSITION_FORWARD_BACK) &&
      curUrl != params->url) {
    GURL cookieUrl = GURL(params->url.scheme() + ":\\" + params->url.host());
    SetIECookie(cookieUrl);

    content::BrowserThread::PostTask(
        content::BrowserThread::UI, FROM_HERE,
        base::Bind(&WebContentsIE::CommitToEntry, self_.GetWeakPtr(),
                   params->url, true));
  } else {
    content::BrowserThread::PostTask(
        content::BrowserThread::UI, FROM_HERE,
        base::Bind(&WebContentsIE::CommitToEntry, self_.GetWeakPtr(),
                   params->url, false));
  }

  if (PageTransitionTypeIncludingQualifiersIs(
          params->transition_type,
          ui::PageTransition::PAGE_TRANSITION_RELOAD)) {
    browser_event_handler_->Refresh();
  } else {
    browser_event_handler_->LoadUrl(url);
  }

  // //chjy
  // RenderFrameHostImpl* rfh = GetMainFrame();
  // is_navigate_stopped_ = false;
  // if (rfh)
  //  {
  //   DidStartLoading(rfh->frame_tree_node(), true);
  //   rfh->frame_tree_node()->navigator()->
  //     DidStartProvisionalLoad(rfh, GURL(url)
  //     , std::vector<GURL>(), base::TimeTicks::Now());  //will call
  //     WebContentsObserver::DidStartNavigation may be switch renderer
  //  }
  // //chjy
}

}  // namespace content
