// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/frame/ysp_views_manager_mac.h"

#include "chrome/browser/browser_process.h"
#include "chrome/browser/ui/views/tabs/tab_strip.h"
#include "chrome/browser/themes/theme_properties.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/grit/theme_resources.h"
#include "chrome/common/pref_names.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/views/resources/grit/views_resources.h"
#include "ui/base/theme_provider.h"
#include "ui/base/hit_test.h"

#include "chrome/browser/ysp_login/ysp_login_manager.h"

namespace {
  const int blank_strip_width_ = 7;
  const SkColor kLockScreenBackgroundColor = SkColorSetRGB(160, 160, 160);
}

YspViewsManagerMac::YspViewsManagerMac(
    BrowserNonClientFrameView* non_client_frame_view,
    BrowserView* browser_view)
    : browser_view_(browser_view),
      non_client_frame_view_(non_client_frame_view),
      account_view_(nullptr),
      lock_button_(nullptr),
      locked_view_(nullptr),
      ysp_set_pin_view_holder_(nullptr) {}

YspViewsManagerMac::~YspViewsManagerMac() {}

void YspViewsManagerMac::ChangeScreenStatus(
    YspViewsManagerMac::YSPScreenStatus ysp_screen_status) {
  switch (ysp_screen_status) {
    case SET_PIN_SCREEN:
      locked_view_->SetVisible(false);
      ysp_set_pin_view_holder_->SetVisible(true);
      browser_view_->SetVisible(false);
      account_view_->SetVisible(false);
      lock_button_->SetVisible(false);
      break;
    case LOCK_SCREEN:
      locked_view_->SetVisible(true);
      ysp_set_pin_view_holder_->SetVisible(false);
      browser_view_->SetVisible(false);
      account_view_->SetVisible(false);
      lock_button_->SetVisible(false);
      break;
    case BROWSER_SCREEN:
      locked_view_->SetVisible(false);
      ysp_set_pin_view_holder_->SetVisible(false);
      browser_view_->SetVisible(true);
      account_view_->SetVisible(true);
      lock_button_->SetVisible(true);
      break;
  }
  if (browser_view_ && browser_view_->frame() &&
      browser_view_->frame()->GetFrameView()) {
    browser_view_->frame()->UpdateWindowTitle();
  }
}

void YspViewsManagerMac::ButtonPressed(views::Button* sender, 
                                       const ui::Event& event) {
  if (sender == lock_button_) {
    locked_view_->Lock();
  }
}

void YspViewsManagerMac::InitRedcoreViews() {
// account view init 
  account_view_ = new YSPAccountView(browser_view_);
  non_client_frame_view_->AddChildView(account_view_);
  account_view_->Init();

// lock button init 
  lock_button_ = InitWindowCaptionButton(IDR_YSP_LOCK_SCREEN,
                                         IDR_YSP_LOCK_SCREEN_H,
                                         IDR_YSP_LOCK_SCREEN_P,
                                         IDR_YSP_LOCK_SCREEN_MASK,
                                         IDS_ACCNAME_YSP_LOCK_SCREEN,
                                         VIEW_ID_LOCK_SCREEN_BUTTON);
//pin view init
  ysp_set_pin_view_holder_ = new YSPSetPINViewHolder(this, browser_view_);
  if (ysp_set_pin_view_holder_) {
    ysp_set_pin_view_holder_->set_id(VIEW_ID_YSP_SET_PIN_VIEW_HOLDER);
    non_client_frame_view_->AddChildViewAt(ysp_set_pin_view_holder_, 0);
  }
//lock screen init
  locked_view_ = new YSPLockScreenView(this, browser_view_);
  if (locked_view_) {
    locked_view_->SetBackground(views::CreateSolidBackground(kLockScreenBackgroundColor));
    locked_view_->set_id(VIEW_ID_LOCK_SCREEN_VIEW);
    non_client_frame_view_->AddChildViewAt(locked_view_, 0);
  }    
}

void YspViewsManagerMac::FirstCreateCheck() {
  std::string pin_key = YSPLoginManager::GetInstance()->GetUserPinKey();
  if (pin_key.empty()) {
    ChangeScreenStatus(SET_PIN_SCREEN);
  } else {
    bool first_create = browser_view_->browser()->FirstCreate();
    if (first_create) {
      locked_view_->Lock();
    } else {
      PrefService* pref = g_browser_process->local_state();
      if (pref->GetInteger(prefs::kYSPLockScreen) == Browser::UNLOCKED) {
        locked_view_->Unlock();
      } else {
        locked_view_->Lock();
      }
    }
  }
}

void YspViewsManagerMac::LayoutRedcoreViews(int x, int y, int width, int height) {
  const int view_size = kAvatarImageSize;
  const int tab_width = width;

  if(account_view_) {
    account_view_->SetBounds(tab_width - 2*view_size - 2*blank_strip_width_,
                            blank_strip_width_,
                            view_size,
                            view_size);
    account_view_->SetVisible(true);
  }

  if(lock_button_) {
    lock_button_->SetBounds(tab_width - view_size - blank_strip_width_,
                            blank_strip_width_,
                            view_size,
                            view_size);
  }

  if (ysp_set_pin_view_holder_->visible() || locked_view_->visible()) {
    lock_button_->SetVisible(false);
  } else {
    lock_button_->SetVisible(true);
  }
  
  ysp_set_pin_view_holder_->SetBounds(x, y, width,height);                                      
  locked_view_->SetBounds(x, y, width, height);
}

int YspViewsManagerMac::HitTestImpl(const gfx::Point& point) const {
  if (account_view_ && account_view_->visible() &&
      account_view_->GetMirroredBounds().Contains(point))
    return HTCLIENT;
  if (lock_button_ && lock_button_->visible() &&
      lock_button_->GetMirroredBounds().Contains(point))
    return HTCLIENT;

  return HTNOWHERE;
}

bool YspViewsManagerMac::DoesIntersectRectImpl() const {
  if ((locked_view_ && locked_view_->visible()) ||
      (ysp_set_pin_view_holder_ && ysp_set_pin_view_holder_->visible())) {
    return true;
  }

  return false;
}

bool YspViewsManagerMac::OnMousePressedImpl(const ui::MouseEvent& event) const {
  if ((locked_view_ && locked_view_->visible() && event.IsRightMouseButton()) ||
      (ysp_set_pin_view_holder_ && ysp_set_pin_view_holder_->visible() &&
       event.IsRightMouseButton())) {
    return true;
  }

  return false;
}

views::ImageButton* YspViewsManagerMac::InitWindowCaptionButton(
    int normal_image_id,
    int hot_image_id,
    int pushed_image_id,
    int mask_image_id,
    int accessibility_string_id,
    ViewID view_id) {
  views::ImageButton* button = new views::ImageButton(this);
  const ui::ThemeProvider* tp = non_client_frame_view_->frame()->GetThemeProvider();
  button->SetImage(views::Button::STATE_NORMAL,
                   tp->GetImageSkiaNamed(normal_image_id));
  button->SetImage(views::Button::STATE_HOVERED,
                   tp->GetImageSkiaNamed(hot_image_id));
  button->SetImage(views::Button::STATE_PRESSED,
                   tp->GetImageSkiaNamed(pushed_image_id));

  if (browser_view_->IsBrowserTypeNormal()) {
    button->SetBackgroundImage(
        tp->GetColor(ThemeProperties::COLOR_BUTTON_BACKGROUND),
        tp->GetImageSkiaNamed(IDR_THEME_WINDOW_CONTROL_BACKGROUND),
        tp->GetImageSkiaNamed(mask_image_id));
  } 
  button->SetTooltipText(l10n_util::GetStringUTF16(accessibility_string_id));
  button->SetAccessibleName(
      l10n_util::GetStringUTF16(accessibility_string_id));
  button->set_id(view_id);
  button->SetImageAlignment(views::ImageButton::ALIGN_CENTER, views::ImageButton::ALIGN_MIDDLE);  
  non_client_frame_view_->AddChildView(button);
  return button;
}

