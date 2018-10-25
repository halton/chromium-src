// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef REDCORE

// ysp+ { login }

#ifndef CHROME_BROWSER_UI_VIEWS_YSP_LOGIN_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_YSP_LOGIN_VIEW_H_

#include <stddef.h>

#include "chrome/browser/ysp_login/ysp_login_manager.h"
#include "content/public/browser/web_contents_delegate.h"
#include "ui/views/bubble/bubble_dialog_delegate_view.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/textfield/textfield_controller.h"
#include "ui/views/controls/textfield/textfield.h"

namespace views {
class GridLayout;
class Textfield;
class Label;
class BlueButton;
class LabelButton;
class Checkbox;
class ImageView;
}

class Browser;
class BrowserView;

// This bubble view is displayed when the user clicks on the avatar button.
class YSPLoginView : public content::WebContentsDelegate,
                     public views::BubbleDialogDelegateView,
                     public views::ButtonListener,
                     public views::TextfieldController,
                     public YSPLoginManagerObserver {
 public:
  // Shows the bubble if one is not already showing.
  static void ShowBubble(
      views::View* anchor_view,
      Browser* browser,
      BrowserView* browser_view,
      base::string16& user_name,
      gfx::ImageSkia* head_image);
  static bool IsShowing();
  static void Hide();
  static void SetAndRefreshAvatar(gfx::ImageSkia* head_image);

 private:
   enum ViewState {
     VS_LOADING = 0,
     VS_LOGIN,
     VS_LOGOUT,
     VS_SETTING
   };

   YSPLoginView(views::View* anchor_view,
                Browser* browser,
                BrowserView* browser_view,
                base::string16& user_name,
                gfx::ImageSkia* head_image);
  ~YSPLoginView() override;

  // views::BubbleDialogDelegateView:
  void Init() override;
  void OnNativeThemeChanged(const ui::NativeTheme* native_theme) override;
  void WindowClosing() override;
  bool AcceleratorPressed(const ui::Accelerator& accelerator) override;
  views::View* GetInitiallyFocusedView() override;

  // YSPLoginManagerObserver:
  void OnLoginRequestFailure(const std::string& error) override;
  void OnLoginResponseParseFailure(const std::string& error) override;
  void OnLoginFailure(base::string16 message) override;
  void OnLoginSuccess(const base::string16& name,
                      const std::string& head_image_url) override;
  void OnLogout() override;

  static YSPLoginView* login_bubble_;

  // content::WebContentsDelegate:
  bool HandleContextMenu(const content::ContextMenuParams& params) override;

  // views::ButtonListener implementation.
  void ButtonPressed(views::Button* sender, const ui::Event& event) override;

  // views::TextfieldController implementation.
  void ContentsChanged(views::Textfield* sender,
    const base::string16& new_contents) override;

  // Shows the bubble with the |view_to_display|.
  void ShowView();
  void ResetView();

  ViewState view_state_;

  // Creates views
  views::View* CreateLoginSubview(views::View** focus_view, int* prefer_width);
  views::View* CreateLogoutSubview(views::View** focus_view, int* prefer_width);
  views::View* CreateSettingSubview(views::View** focus_view, int* prefer_width);
  views::View* CreateLoadingSubview(views::View** focus_view, int* prefer_width);

  void UpdateControlState();
  void UpdateControlStateOfLoginView();
  void UpdateControlStateOfSettingView();
  void SetManageServer(std::string server);
  std::string GetManageServer();

  void ShowFailureWithMessage(int message_id);
  void ShowFailureWithMessage(base::string16 message);

  gfx::ImageSkia* head_image_;

  // login view
  views::Textfield* domain_text_;
  //views::Label* domain_prompt_label_;
  views::Textfield* account_text_;
  //views::Label* account_prompt_label_;
  views::Textfield* password_text_;
  views::Label* password_prompt_label_;
  views::BlueButton* login_button_;
  views::LabelButton* server_setting_button_;
  views::Checkbox* auto_login_checkbox_;
  // logout view
  views::ImageView* avatar_image_;
  views::Label* name_label_;
  views::BlueButton* logout_button_;
  // setting view
  views::LabelButton* setting_back_button_;
  views::LabelButton* setting_clear_button_;
  views::Textfield* host_text_;
  views::BlueButton* setting_ok_button_;

  base::string16 user_name_;

  DISALLOW_COPY_AND_ASSIGN(YSPLoginView);
};

#endif  // CHROME_BROWSER_UI_VIEWS_YSP_LOGIN_VIEW_H_

#endif
