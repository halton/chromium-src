#ifdef REDCORE
#ifdef IE_REDCORE
#ifndef CONTENT_BROWSER_WEB_CONTENTS_WEB_CONTENTS_IE_VIEW_AURA_H_
#define CONTENT_BROWSER_WEB_CONTENTS_WEB_CONTENTS_IE_VIEW_AURA_H_
#pragma once

#include <vector>

#include "base/gtest_prod_util.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "content/browser/renderer_host/overscroll_controller_delegate.h"
#include "content/browser/renderer_host/render_view_host_delegate_view.h"
#include "content/browser/web_contents/web_contents_view.h"
#include "content/common/content_export.h"
#include "ui/aura/window_delegate.h"
#include "ui/aura/window_observer.h"
#include "ui/aura/client/drag_drop_delegate.h"
#include "content/browser/web_contents/web_contents_view_aura.h"

namespace aura {
  class Window;
}

namespace ui {
  class DropTargetEvent;
  class TouchSelectionController;
}

namespace content {
  class GestureNavSimple;
  class OverscrollNavigationOverlay;
  class RenderWidgetHostImpl;
  class RenderWidgetHostViewAura;
  class ShadowLayerDelegate;
  class TouchSelectionControllerClientAura;
  class WebContentsViewDelegate;
  class WebContentsImpl;
  class WebDragDestDelegate;

  class WebContentsViewIEAura
    :public WebContentsViewAura
  {
  public:
    WebContentsViewIEAura(WebContentsImpl* web_contents,
      WebContentsViewDelegate* delegate);
    ~WebContentsViewIEAura() override;

     void SizeContents(const gfx::Size& size) override;
     void OnBoundsChanged(const gfx::Rect& old_bounds,
      const gfx::Rect& new_bounds) override;
  };
}// namespace content
#endif
#endif
#endif
