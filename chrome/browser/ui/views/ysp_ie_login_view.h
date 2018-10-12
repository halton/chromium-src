// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_YSP_IE_LOGIN_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_YSP_IE_LOGIN_VIEW_H_

#include <vector>

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "ui/gfx/font_list.h"
#include "ui/gfx/shadow_value.h"
#include "ui/gfx/text_constants.h"
#include "ui/views/bubble/bubble_frame_view.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/button/label_button_border.h"
#include "ui/views/native_theme_delegate.h"
#include "ui/views/view.h"
#include "ui/views/window/dialog_delegate.h"

namespace views {
class Label;
class Textfield;
class GridLayout;
class ImageView;
class ImageButton;
class Painter;
class ScrollView;
}  // namespace views

class Browser;
class YspPopupView;
class YspIEloginView;

struct LoginBtnInfo {
  base::string16 appName;
  base::string16 appUrl;
  base::string16 loginName;
  base::string16 loginPwd;

 public:
  LoginBtnInfo();
  ~LoginBtnInfo();
  LoginBtnInfo(const LoginBtnInfo& others);
};

LoginBtnInfo::LoginBtnInfo(const LoginBtnInfo& others) = default;

class LoginInfoButton : public views::Button,
                        public views::NativeThemeDelegate {
 public:
  // The length of the hover fade animation.
  static const int kHoverAnimationDurationMs;

  // Amount to inset each edge of the button when drawing the focus rectangle.
  static const int kFocusRectInset;

  static const char kViewClassName[];

  LoginInfoButton(views::ButtonListener* listener,
                  const LoginBtnInfo& button_info);
  ~LoginInfoButton() override;

  // Gets or sets the text shown on the button.
  const LoginBtnInfo& GetBtnInfo() const;
  void SetBtnInfo(const LoginBtnInfo& button_info);

  // Sets the text color shown for the specified button |for_state| to |color|.
  void SetTextColor(ButtonState for_state, SkColor color);

  // Sets the text colors shown for the non-disabled states to |color|.
  void SetEnabledTextColors(SkColor color);

  // Sets drop shadows underneath the text.
  void SetTextShadows(const gfx::ShadowValues& shadows);

  // Sets whether subpixel rendering is used on the label.
  void SetTextSubpixelRenderingEnabled(bool enabled);

  // Gets or sets the font list used by this button.
  const gfx::FontList& GetFontList() const;
  void SetFontList(const gfx::FontList& font_list);

  // Sets the elide behavior of this button.
  void SetElideBehavior(gfx::ElideBehavior elide_behavior);

  // Sets the horizontal alignment used for the button; reversed in RTL. The
  // optional image will lead the text, unless the button is right-aligned.
  void SetHorizontalAlignment(gfx::HorizontalAlignment alignment);

  // Call SetMinSize(gfx::Size()) to clear the monotonically increasing size.
  void SetMinSize(const gfx::Size& min_size);
  void SetMaxSize(const gfx::Size& max_size);

  // Gets or sets the option to handle the return key; false by default.
  bool is_default() const { return is_default_; }
  void SetIsDefault(bool is_default);

  // Gets or sets the button's overall style; the default is |STYLE_TEXTBUTTON|.
  ButtonStyle style() const { return style_; }
  void SetStyle(ButtonStyle style);

  void SetFocusPainter(std::unique_ptr<views::Painter> focus_painter);
  views::Painter* focus_painter() { return focus_painter_.get(); }

  // Creates the default border for this button. This can be overridden by
  // subclasses.
  virtual std::unique_ptr<views::LabelButtonBorder> CreateDefaultBorder() const;

  // View:
  void SetBorder(std::unique_ptr<views::Border> border) override;
  gfx::Size GetPreferredSize() const;
  int GetHeightForWidth(int width) const override;
  void Layout() override;
  const char* GetClassName() const override;
  void EnableCanvasFlippingForRTLUI(bool flip) override;

 protected:
  views::Label* label() const { return label_; }

  // Returns the available area for the label and image. Subclasses can change
  // these bounds if they need room to do manual painting.
  virtual gfx::Rect GetChildAreaBounds();

  // View:custom painting should use PaintButtonContents.
  void Paint(gfx::Canvas* canvas);
  void OnFocus() override;
  void OnBlur() override;
  void OnNativeThemeChanged(const ui::NativeTheme* theme) override;

  // CustomButton:
  void StateChanged(ButtonState old_state) override;

  // Fills |params| with information about the button.
  virtual void GetExtraParams(ui::NativeTheme::ExtraParams* params) const;

  // Resets colors from the NativeTheme, explicitly set colors are unchanged.
  virtual void ResetColorsFromNativeTheme();

  // Updates the border as per the NativeTheme, unless a different border was
  // set with SetBorder.
  void UpdateThemedBorder();

  // NativeThemeDelegate:
  gfx::Rect GetThemePaintRect() const override;

 private:
  FRIEND_TEST_ALL_PREFIXES(LabelButtonTest, Init);
  FRIEND_TEST_ALL_PREFIXES(LabelButtonTest, Label);
  FRIEND_TEST_ALL_PREFIXES(LabelButtonTest, Image);
  FRIEND_TEST_ALL_PREFIXES(LabelButtonTest, LabelAndImage);
  FRIEND_TEST_ALL_PREFIXES(LabelButtonTest, FontList);
  FRIEND_TEST_ALL_PREFIXES(LabelButtonTest, ButtonStyleIsDefaultSize);

  // View:
  void ChildPreferredSizeChanged(View* child) override;

  // NativeThemeDelegate:
  ui::NativeTheme::Part GetThemePart() const override;
  ui::NativeTheme::State GetThemeState(
      ui::NativeTheme::ExtraParams* params) const override;
  const gfx::Animation* GetThemeAnimation() const override;
  ui::NativeTheme::State GetBackgroundThemeState(
      ui::NativeTheme::ExtraParams* params) const override;
  ui::NativeTheme::State GetForegroundThemeState(
      ui::NativeTheme::ExtraParams* params) const override;

  // Resets |cached_preferred_size_| and marks |cached_preferred_size_valid_|
  // as false.
  void ResetCachedPreferredSize();

  // The image and label shown in the button.
  views::Label* label_;
  views::Label* label_desc;

  // The cached font lists in the normal and bold style.
  gfx::FontList cached_normal_font_list_;
  gfx::FontList cached_bold_font_list_;

  // The images and colors for each button state.
  gfx::ImageSkia button_state_images_[STATE_COUNT];
  SkColor button_state_colors_[STATE_COUNT];

  // Used to track whether SetTextColor() has been invoked.
  bool explicitly_set_colors_[STATE_COUNT];

  // |min_size_| increases monotonically with the preferred size.
  mutable gfx::Size min_size_;
  // |max_size_| may be set to clamp the preferred size.
  gfx::Size max_size_;

  // Cache the last computed preferred size.
  mutable gfx::Size cached_preferred_size_;
  mutable bool cached_preferred_size_valid_;

  // Flag indicating default handling of the return key via an accelerator.
  // Whether or not the button appears or behaves as the default button in its
  // current context;
  bool is_default_;

  // The button's overall style.
  ButtonStyle style_;

  // True if current border was set by UpdateThemedBorder. Defaults to true.
  bool border_is_themed_border_;

  // Alignment of the button. This can be different from the alignment of the
  // text; for example, the label may be set to ALIGN_TO_HEAD (alignment matches
  // text direction) while |this| is laid out as ALIGN_LEFT (alignment matches
  // UI direction).
  gfx::HorizontalAlignment horizontal_alignment_;

  std::unique_ptr<views::Painter> focus_painter_;

  LoginBtnInfo btn_info_;

  DISALLOW_COPY_AND_ASSIGN(LoginInfoButton);
};

class IELoginBtnView : public views::View {
 public:
  IELoginBtnView(YspIEloginView* ie_login_view,
                 const std::vector<LoginBtnInfo> info_vector);
  ~IELoginBtnView() override;

  void Layout() override;

 private:
  std::vector<LoginInfoButton*> buttonVec;
};

class YspIEloginView : public views::View, public views::ButtonListener {
 public:
  YspIEloginView(YspPopupView* ysp_popup_view,
                 const std::vector<LoginBtnInfo> info_vector);
  ~YspIEloginView() override;

  void ButtonPressed(views::Button* sender, const ui::Event& event) override;
  void Layout() override;

 private:
  views::Label* title_label_;
  YspPopupView* popup_dialog_;
  std::vector<LoginBtnInfo> button_info_vector_;
  views::ScrollView* scroll_view_;
};

class YspPopupView : public views::DialogDelegate {
 public:
  YspPopupView(Browser* browser, std::vector<LoginBtnInfo> button_info_vector);
  ~YspPopupView() override;

  int GetDialogButtons() const override;
  views::View* GetContentsView() override;
  views::Widget* GetWidget() override;
  const views::Widget* GetWidget() const override;
  ui::ModalType GetModalType() const override;
  base::string16 GetWindowTitle() const override;
  bool ShouldShowWindowTitle() const override;
  bool Close() override;

  virtual void SetResultInfo(const LoginBtnInfo& info);
  virtual LoginBtnInfo DoModel();

 private:
  Browser* browser_;
  YspIEloginView* login_view_;
  views::Widget* ysp_popup_view_;
  LoginBtnInfo info_temp_;
};

#endif  // CHROME_BROWSER_UI_VIEWS_YSP_IE_LOGIN_VIEW_H_
