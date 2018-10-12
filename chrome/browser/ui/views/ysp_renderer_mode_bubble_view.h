// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_YSP_RENDERER_MODE_BUBBLE_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_YSP_RENDERER_MODE_BUBBLE_VIEW_H_

#include "base/compiler_specific.h"
#include "base/gtest_prod_util.h"
#include "base/macros.h"
#include "base/strings/string16.h"
#include "chrome/browser/ui/bookmarks/recently_used_folders_combo_model.h"
#include "chrome/browser/ui/sync/bubble_sync_promo_delegate.h"
#include "chrome/browser/ui/views/location_bar/ysp_renderer_mode_view.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/button/label_button.h"
#include "url/gurl.h"

class RendererModeBubbleView : public views::BubbleDialogDelegateView,
                               public views::ButtonListener {
 public:
  // If |anchor_view| is null, |anchor_rect| is used to anchor the bubble and
  // |parent_window| is used to ensure the bubble closes if the parent closes.
  static void ShowBubble(views::View* anchor_view,
                         const gfx::Rect& anchor_rect,
                         gfx::NativeView parent_window,
                         Browser* browser,
                         const GURL& url,
                         RendererMode renderer_mode);

  static void Hide();

  static RendererModeBubbleView* GetBubbleView() {
    return pRendererModeBubbleView;
  }

  ~RendererModeBubbleView() override;

  // views::WidgetDelegate:
  void WindowClosing() override;

 protected:
  // views::BubbleDelegateView method.
  void Init() override;

 private:
  const char* GetClassName() const override;
  views::View* GetInitiallyFocusedView() override;

  // Overridden from views::ButtonListener:
  void ButtonPressed(views::Button* sender, const ui::Event& event) override;

  void SetButtonSelected(views::LabelButton* pSelectButton);

  RendererModeBubbleView(views::View* anchor_view,
                         Browser* browser,
                         const GURL& url,
                         RendererMode mode);

 private:
  static RendererModeBubbleView* pRendererModeBubbleView;
  views::LabelButton* pBlinkButton;
  views::LabelButton* pSysIEButton;
  Browser* pBrowser;
  RendererMode rendererMode;
};

#endif  // CHROME_BROWSER_UI_VIEWS_YSP_RENDERER_MODE_BUBBLE_VIEW_H_
