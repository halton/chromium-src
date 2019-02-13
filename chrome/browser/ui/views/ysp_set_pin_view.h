// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_YSP_SET_PIN_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_YSP_SET_PIN_VIEW_H_

#include <stddef.h>

#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ysp_login/ysp_login_manager.h"
#include "content/public/browser/web_contents_delegate.h"
#include "ui/views/bubble/bubble_dialog_delegate_view.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/button/label_button_label.h"
#include "ui/views/controls/button/md_text_button.h"
#include "ui/views/controls/image_view.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/controls/textfield/textfield_controller.h"

#if !defined(OS_MACOSX)
#include "chrome/browser/ui/views/frame/opaque_browser_frame_view.h"
#else
#include "chrome/browser/ui/views/frame/ysp_views_manager_mac.h"
#endif

namespace views {
class GridLayout;
class LabelButton;
class ImageView;
class Label;
class Textfield;
}  // namespace views

class Browser;
class BrowserView;
#if !defined(OS_MACOSX)
class OpaqueBrowserFrameView;
#else
class YspViewsManagerMac;
#endif
class YSPSetPINView : public content::WebContentsDelegate,
                      public views::ImageView,
                      public views::ButtonListener,
                      public views::TextfieldController {
 public:
#if !defined(OS_MACOSX)
  YSPSetPINView(OpaqueBrowserFrameView* opaque_browser_frame_view,
#else
  YSPSetPINView(YspViewsManagerMac* ysp_views_manager_mac,
#endif
                BrowserView* browser_view);
  ~YSPSetPINView() override;

 private:
  // views::View implementation.
  void Layout() override;

  // content::WebContentsDelegate:
  bool HandleContextMenu(const content::ContextMenuParams& params) override;

  // views::ButtonListener implementation.
  void ButtonPressed(views::Button* sender, const ui::Event& event) override;

  // views::TextfieldController implementation.
  void ContentsChanged(views::Textfield* sender,
                       const base::string16& new_contents) override;
  bool HandleKeyEvent(views::Textfield* sender,
                      const ui::KeyEvent& key_event) override;

  // views
  views::ImageView* dialog_header_image_;
  views::Label* title_label_;
  views::Label* message_label_;
  views::Label* set_pin_label_;
  views::Textfield* pin_textfield_;
  views::Label* confirm_set_pin_label_;
  views::Textfield* confirm_pin_textfield_;
  views::LabelButton* confirm_button_;
  views::Label* prompt_label_;

#if !defined(OS_MACOSX)
  OpaqueBrowserFrameView* opaque_browser_frame_view_;
#else
  YspViewsManagerMac* ysp_views_manager_mac_;
#endif
  void Submit();
  bool IsDigitString(const base::string16& text);
};
#endif  // CHROME_BROWSER_UI_VIEWS_YSP_SET_PIN_VIEW_H_
