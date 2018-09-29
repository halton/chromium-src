#ifdef REDCORE
#ifdef IE_REDCORE
#include "content/browser/web_contents/web_contents_view_aura.h"

#include <stddef.h>
#include <stdint.h>

#include "base/auto_reset.h"
#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/macros.h"
#include "base/strings/utf_string_conversions.h"
#include "build/build_config.h"
#include "content/browser/browser_plugin/browser_plugin_guest.h"
#include "content/browser/download/drag_download_util.h"
#include "content/browser/frame_host/interstitial_page_impl.h"
#include "content/browser/frame_host/navigation_entry_impl.h"
#include "content/browser/renderer_host/dip_util.h"
#include "content/browser/renderer_host/input/touch_selection_controller_client_aura.h"
#include "content/browser/renderer_host/overscroll_controller.h"
#include "content/browser/renderer_host/render_view_host_factory.h"
#include "content/browser/renderer_host/render_view_host_impl.h"
#include "content/browser/renderer_host/render_widget_host_impl.h"
#include "content/browser/renderer_host/render_widget_host_view_aura.h"
#include "content/browser/web_contents/aura/gesture_nav_simple.h"
#include "content/browser/web_contents/aura/overscroll_navigation_overlay.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/notification_source.h"
#include "content/public/browser/notification_types.h"
#include "content/public/browser/overscroll_configuration.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/web_contents_view_delegate.h"
#include "content/public/browser/web_drag_dest_delegate.h"
#include "content/public/common/content_client.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/drop_data.h"
#include "net/base/filename_util.h"
#include "third_party/blink/public/platform/web_input_event.h"
#include "ui/aura/client/aura_constants.h"
#include "ui/aura/client/window_parenting_client.h"
#include "ui/aura/env.h"
#include "ui/aura/window.h"
#include "ui/aura/client/drag_drop_client.h"
#include "ui/aura/client/drag_drop_delegate.h"
#include "ui/aura/window_observer.h"
#include "ui/aura/window_tree_host.h"
#include "ui/aura/window_tree_host_observer.h"
#include "ui/base/clipboard/clipboard.h"
#include "ui/base/clipboard/custom_data_helper.h"
#include "ui/base/dragdrop/drag_drop_types.h"
#include "ui/base/dragdrop/drop_target_event.h"
#include "ui/base/dragdrop/os_exchange_data.h"
#include "ui/base/hit_test.h"
#include "ui/compositor/layer.h"
#include "ui/events/blink/web_input_event.h"
#include "ui/events/event.h"
#include "ui/events/event_utils.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/image/image_png_rep.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/display/screen.h"
#include "ui/touch_selection/touch_selection_controller.h"
#include "content/browser/web_contents/web_contents_ie_view_aura.h"
#include "content/browser/web_contents/web_contents_ie.h"

namespace content {

  WebContentsViewIEAura::WebContentsViewIEAura(
    WebContentsImpl * web_contents, WebContentsViewDelegate * delegate)
    :WebContentsViewAura(web_contents, delegate)
  {
  }

  WebContentsViewIEAura::~WebContentsViewIEAura()
  {
  }

  void WebContentsViewIEAura::SizeContents(const gfx::Size & size)
  {
    __super::SizeContents(size);
    WebContentsIE* pWebContentIE = static_cast<WebContentsIE*>(getWebContents());
    if (pWebContentIE)
      pWebContentIE->OnRendererHostViewSize(size);
  }

void WebContentsViewIEAura::OnBoundsChanged(const gfx::Rect& old_bounds, const gfx::Rect& new_bounds)
{
  __super::OnBoundsChanged(old_bounds, new_bounds);
  WebContentsIE* pWebContentIE = static_cast<WebContentsIE*>(getWebContents());
  if (pWebContentIE)
    pWebContentIE->OnRendererHostViewSize(new_bounds.size());
}

} //namespace content
#endif
#endif
