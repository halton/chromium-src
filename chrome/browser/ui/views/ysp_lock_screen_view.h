// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef REDCORE

#ifndef CHROME_BROWSER_UI_VIEWS_YSP_LOCK_SCREEN_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_YSP_LOCK_SCREEN_VIEW_H_

#include <stddef.h>
#include "chrome/browser/ui/browser.h"
#include "content/public/browser/web_contents_delegate.h"
#include "ui/views/bubble/bubble_dialog_delegate_view.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/textfield/textfield_controller.h"
#include "ui/views/controls/textfield/textfield.h"
#include "chrome/browser/ysp_login/ysp_login_manager.h"

namespace views {
  class GridLayout;
  class Textfield;
  class Label;
  //class BlueButton;
  //class Checkbox;
  class ImageView;
  class ImageButton;
}

class Browser;
class BrowserView;

class YSPLockScreenView : public content::WebContentsDelegate,
  public views::ImageView,
  public views::ButtonListener,
  public views::TextfieldController,
  public YSPLoginManagerObserver {
public:
  static void ShowLockedScreen(BrowserView *browser_view);
  // Shows the bubble if one is not already showing.
  YSPLockScreenView(views::ButtonListener *listener, BrowserView *browser_view);
  ~YSPLockScreenView() override;
#if 0
  static void ShowBubble(
    views::View* anchor_view,
    Browser* browser,
    BrowserView* browser_view,
    base::string16& user_name,
    gfx::ImageSkia* head_image);
#endif
  //static bool IsShowing();
  //static void Hide();
  //static void SetAndRefreshAvatar(gfx::ImageSkia* head_image);
  bool IsLocked() { return is_locked_; }
  void Lock(Browser::YSPLockStatus status);
  void Unlock();

private:
  void Submit();
  // views::View implementation.
  void Layout() override;
  //void OnMouseReleased(const ui::MouseEvent& event) override;
  //void OnMouseExited(const ui::MouseEvent& event) override;


  // YSPLoginManagerObserver:
  void OnLoginRequestFailure(const std::string& error) override;
  void OnLoginResponseParseFailure(const std::string& error) override;
  void OnLoginFailure(base::string16 message) override;
  void OnLoginSuccess(const base::string16& name,
    const std::string& head_image_url) override;
  void OnLogout() override;
  void OnTokenStatusChanged(const std::string& type) override;

  // content::WebContentsDelegate:
  bool HandleContextMenu(const content::ContextMenuParams& params) override;

  // views::ButtonListener implementation.
  void ButtonPressed(views::Button* sender, const ui::Event& event) override;

  // views::TextfieldController implementation.
  void ContentsChanged(views::Textfield* sender, const base::string16& new_contents) override;
  bool HandleKeyEvent(views::Textfield* sender, const ui::KeyEvent& key_event) override;

  void LockInternal(Browser::YSPLockStatus status);
  void UnlockInternal();
  void OnLockStatusChanged();
#if 0
  // Shows the bubble with the |view_to_display|.
  void ShowView();
  void ResetView();
#endif

  //ViewState view_state_;
#if 0
  // Creates views
  views::View* CreateLoginSubview(views::View** focus_view, int* prefer_width);
  views::View* CreateLogoutSubview(views::View** focus_view, int* prefer_width);
  views::View* CreateSettingSubview(views::View** focus_view, int* prefer_width);
  views::View* CreateLoadingSubview(views::View** focus_view, int* prefer_width);

  void ShowFailureWithMessage(int message_id);
  void ShowFailureWithMessage(base::string16 message);
#endif

  Browser* browser_;
  BrowserView* browser_view_;

  // views
  views::Label* info_label_;
  views::ImageView* avatar_image_;
  views::Label* name_label_;
  views::Textfield* password_text_;
  views::ImageButton* login_button_;
  views::Label* error_prompt_;

  bool is_locked_;
  base::string16 user_name_;
  PrefChangeRegistrar profile_pref_registrar_;

  DISALLOW_COPY_AND_ASSIGN(YSPLockScreenView);
};

#endif //CHROME_BROWSER_UI_VIEWS_YSP_LOGIN_VIEW_H_

#endif
