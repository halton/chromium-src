// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef YSP_VIEWS_MANAGER_MAC_H
#define YSP_VIEWS_MANAGER_MAC_H

#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/view_ids.h"
#include "chrome/browser/ui/views/ysp_lock_screen_view.h"
#include "chrome/browser/ui/views/ysp_account_view.h"
#include "chrome/browser/ui/views/ysp_set_pin_view_holder.h"
#include "ui/views/controls/button/image_button.h"

class BrowserView;
class YSPLockScreenView;
class YSPSetPINViewHolder;
class YSPAccountView;

namespace views {
class ImageButton;
}

class YspViewsManagerMac : public views::ButtonListener {
public:
  enum YSPScreenStatus { SET_PIN_SCREEN, LOCK_SCREEN, BROWSER_SCREEN };
  YspViewsManagerMac(BrowserNonClientFrameView* no_client_frame_view,
                     BrowserView* browser_view);
  ~YspViewsManagerMac() override;
  void ChangeScreenStatus(YSPScreenStatus ysp_screen_status);
  void InitRedcoreViews();
  void FirstCreateCheck();  
  int HitTestImpl(const gfx::Point& point) const;
  void LayoutRedcoreViews(int x, int y, int width, int height);
  bool DoesIntersectRectImpl() const;
  // views::ButtonListener:
  void ButtonPressed(views::Button* sender, const ui::Event& event) override;
  bool OnMousePressedImpl(const ui::MouseEvent& event) const;
private:
  void InitViews();
  views::ImageButton* InitWindowCaptionButton(int normal_image_id,
                                              int hot_image_id,
                                              int pushed_image_id,
                                              int mask_image_id,
                                              int accessibility_string_id,
                                              ViewID view_id);
  BrowserView* browser_view_;  
  BrowserNonClientFrameView* non_client_frame_view_;
  YSPAccountView* account_view_;
  views::ImageButton* lock_button_;
  YSPLockScreenView* locked_view_;
  YSPSetPINViewHolder* ysp_set_pin_view_holder_;  

  DISALLOW_COPY_AND_ASSIGN(YspViewsManagerMac);
};

#endif  // YSP_VIEWS_MANAGER_MAC_H

