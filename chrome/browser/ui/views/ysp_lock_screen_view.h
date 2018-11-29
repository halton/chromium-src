// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_YSP_LOCK_SCREEN_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_YSP_LOCK_SCREEN_VIEW_H_

#include <stddef.h>

#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/views/frame/opaque_browser_frame_view.h"
#include "chrome/browser/ysp_login/ysp_login_manager.h"
#include "content/public/browser/web_contents_delegate.h"
#include "ui/views/bubble/bubble_dialog_delegate_view.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/button/md_text_button.h"
#include "ui/views/controls/image_view.h"
#include "ui/views/controls/link.h"
#include "ui/views/controls/link_listener.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/controls/textfield/textfield_controller.h"

namespace views {
class GridLayout;
class ImageButton;
class ImageView;
class Label;
class Textfield;
class MdTextButton;
class Link;
class LinkListener;
}  // namespace views

class Browser;
class BrowserView;
class OpaqueBrowserFrameView;

class YSPLockScreenView : public content::WebContentsDelegate,
                          public views::ImageView,
                          public views::ButtonListener,
                          public views::TextfieldController,
                          public views::LinkListener {
 public:
  static void ShowLockedScreen(BrowserView* browser_view);
  YSPLockScreenView(OpaqueBrowserFrameView* opaque_browser_frame_view,
                    BrowserView* browser_view);
  ~YSPLockScreenView() override;

  void Lock();
  void Unlock();

 private:
  void Submit();
  int GetLeftTop(int window_width, int view_width);
  void ShowError(bool show);
  void ShowForgetPasswordDialog(bool show);
  void SetAvatar();
  void DownloadImage(const std::string& url);
  void DidDownloadFavicon(int id,
                          int http_status_code,
                          const GURL& image_url,
                          const std::vector<SkBitmap>& bitmaps,
                          const std::vector<gfx::Size>& original_bitmap_sizes);

  // views::View implementation.
  void Layout() override;

  // content::WebContentsDelegate:
  bool HandleContextMenu(const content::ContextMenuParams& params) override;

  // views::ButtonListener implementation.
  void ButtonPressed(views::Button* sender, const ui::Event& event) override;

  // views::LinkListener implementation.
  void LinkClicked(views::Link* source, int event_flags) override;

  // views::TextfieldController implementation.
  void ContentsChanged(views::Textfield* sender,
                       const base::string16& new_contents) override;
  bool HandleKeyEvent(views::Textfield* sender,
                      const ui::KeyEvent& key_event) override;

  Browser* browser_;
  BrowserView* browser_view_;

  // views
  views::Label* info_label_;
  views::ImageView* avatar_image_;
  views::Label* name_label_;
  views::Textfield* password_text_;
  views::ImageButton* login_button_;
  views::Label* error_prompt_;
  views::MdTextButton* error_confirm_button_;
  views::Link* forget_pin_ink_label_;

  views::ImageView* forget_password_message_bg_image_;
  views::Label* forget_password_message_title_label_;
  views::Label* forget_password_message_label_;
  views::LabelButton* forget_password_message_confirm_button_;

  base::string16 user_name_;
  PrefChangeRegistrar profile_pref_registrar_;
  OpaqueBrowserFrameView* opaque_browser_frame_view_;

  DISALLOW_COPY_AND_ASSIGN(YSPLockScreenView);
};

#endif  // CHROME_BROWSER_UI_VIEWS_YSP_LOGIN_VIEW_H_
