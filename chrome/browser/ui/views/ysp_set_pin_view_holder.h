// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_YSP_SET_PIN_VIEW_HOLDER_H_
#define CHROME_BROWSER_UI_VIEWS_YSP_SET_PIN_VIEW_HOLDER_H_

#include <stddef.h>

#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/frame/opaque_browser_frame_view.h"
#include "chrome/browser/ui/views/ysp_set_pin_view.h"
#include "chrome/browser/ysp_login/ysp_login_manager.h"
#include "content/public/browser/web_contents_delegate.h"
#include "ui/views/bubble/bubble_dialog_delegate_view.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/image_view.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/controls/textfield/textfield_controller.h"

namespace views {
class ImageView;
}  // namespace views

class Browser;
class BrowserView;
class YSPSetPINView;
class OpaqueBrowserFrameView;

class YSPSetPINViewHolder : public content::WebContentsDelegate,
                            public views::ImageView {
 public:
  YSPSetPINViewHolder(OpaqueBrowserFrameView* opaque_browser_frame_view,
                      BrowserView* browser_view);
  ~YSPSetPINViewHolder() override;

  void show();
  void hide();

 private:
  // views::View implementation.
  void Layout() override;

  // content::WebContentsDelegate:
  bool HandleContextMenu(const content::ContextMenuParams& params) override;

  YSPSetPINView* ysp_set_pin_view_;
};

#endif  // CHROME_BROWSER_UI_VIEWS_YSP_SET_PIN_VIEW_HOLDER_H_
