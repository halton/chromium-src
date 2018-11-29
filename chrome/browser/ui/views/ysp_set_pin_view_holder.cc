// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/ysp_set_pin_view_holder.h"

#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/ysp_account_view.h"
#include "chrome/browser/ui/views/ysp_set_pin_view.h"
#include "chrome/common/pref_names.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/grit/theme_resources.h"
#include "components/prefs/pref_service.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/theme_provider.h"
#include "ui/native_theme/native_theme.h"
#include "ui/views/controls/button/checkbox.h"
#include "ui/views/controls/button/image_button.h"
#include "ui/views/controls/image_view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/webview/webview.h"
#include "ui/views/layout/grid_layout.h"

YSPSetPINViewHolder::YSPSetPINViewHolder(
    OpaqueBrowserFrameView* opaque_browser_frame_view,
    BrowserView* browser_view)
    : ysp_set_pin_view_(
          new YSPSetPINView(opaque_browser_frame_view, browser_view)),
      opaque_browser_frame_view_(opaque_browser_frame_view) {
  const ui::ThemeProvider* tp = browser_view->frame()->GetThemeProvider();
  SetImage(tp->GetImageSkiaNamed(IDR_YSP_SET_PIN_BACKGROUND));
  AddChildView(ysp_set_pin_view_);
}

YSPSetPINViewHolder::~YSPSetPINViewHolder() {}

void YSPSetPINViewHolder::show() {
  SetVisible(true);
}

void YSPSetPINViewHolder::hide() {
  SetVisible(false);
}

void YSPSetPINViewHolder::Layout() {
  SetBorder(views::CreateSolidBorder(1, SK_ColorBLACK));
  gfx::Rect bound = bounds();
  bound.Inset(1, 1);
  SetImageSize(bound.size());
  SetDrawCircle(false);

  const int window_width = width();
  const int window_height = height();
  const int dialog_width = 500;
  const int dialog_height = 372;
  int x = (window_width - dialog_width) / 2;
  int y = (window_height - dialog_height) / 2;
  ysp_set_pin_view_->SetBounds(x, y, dialog_width, dialog_height);
}

bool YSPSetPINViewHolder::HandleContextMenu(
    const content::ContextMenuParams& params) {
  return false;
}
