// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/ysp_set_pin_view.h"

#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/ysp_account_view.h"
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

const SkColor kTextfieldBackgroundColor = SkColorSetRGB(241, 243, 244);

YSPSetPINView::YSPSetPINView(OpaqueBrowserFrameView* opaque_browser_frame_view,
                             BrowserView* browser_view)
    : dialog_header_image_(new views::ImageView()),
      title_label_(nullptr),
      message_label_(nullptr),
      set_pin_label_(nullptr),
      pin_textfield_(nullptr),
      confirm_set_pin_label_(nullptr),
      confirm_pin_textfield_(nullptr),
      confirm_button_(nullptr),
      prompt_label_(nullptr),
      opaque_browser_frame_view_(opaque_browser_frame_view) {
  dialog_header_image_->SetDrawCircle(false);

  SetBackground(views::CreateSolidBackground(SK_ColorTRANSPARENT));
  const ui::ThemeProvider* tp = browser_view->frame()->GetThemeProvider();
  SetImage(tp->GetImageSkiaNamed(IDR_YSP_SET_PIN_DIALOG_BACKGROUND));

  const gfx::FontList& bolder_font =
      ui::ResourceBundle::GetSharedInstance().GetFontList(
          ui::ResourceBundle::BoldFont);

  title_label_ = new views::Label(l10n_util::GetStringUTF16(IDS_SET_PIN_TITLE));
  title_label_->SetFontList(
      bolder_font.DeriveWithSizeDelta(17 - bolder_font.GetFontSize()));
  title_label_->SetEnabledColor(SK_ColorWHITE);
  title_label_->SetAutoColorReadabilityEnabled(false);
  AddChildView(title_label_);

  message_label_ =
      new views::Label(l10n_util::GetStringUTF16(IDS_SET_PIN_DESC));
  message_label_->SetFontList(
      bolder_font.DeriveWithSizeDelta(15 - bolder_font.GetFontSize()));
  message_label_->SetEnabledColor(SK_ColorBLACK);
  message_label_->SetAutoColorReadabilityEnabled(false);
  AddChildView(message_label_);

  set_pin_label_ =
      new views::Label(l10n_util::GetStringUTF16(IDS_SET_SIX_PIN_CODE));
  set_pin_label_->SetEnabledColor(SK_ColorBLACK);
  set_pin_label_->SetAutoColorReadabilityEnabled(false);
  AddChildView(set_pin_label_);

  pin_textfield_ = new views::Textfield();
  pin_textfield_->SetBorder(NULL);
  pin_textfield_->SetFocusBehavior(FocusBehavior::ALWAYS);
  pin_textfield_->SetBackgroundColor(kTextfieldBackgroundColor);
  pin_textfield_->SetTextInputType(ui::TEXT_INPUT_TYPE_PASSWORD);
  pin_textfield_->RequestFocus();
  pin_textfield_->set_controller(this);
  AddChildView(pin_textfield_);

  confirm_set_pin_label_ =
      new views::Label(l10n_util::GetStringUTF16(IDS_CONFIRM_PIN_CODE));
  confirm_set_pin_label_->SetEnabledColor(SK_ColorBLACK);
  confirm_set_pin_label_->SetAutoColorReadabilityEnabled(false);
  AddChildView(confirm_set_pin_label_);

  confirm_pin_textfield_ = new views::Textfield();
  confirm_pin_textfield_->SetBorder(NULL);
  confirm_pin_textfield_->SetFocusBehavior(FocusBehavior::ALWAYS);
  confirm_pin_textfield_->SetBackgroundColor(kTextfieldBackgroundColor);
  confirm_pin_textfield_->SetTextInputType(ui::TEXT_INPUT_TYPE_PASSWORD);
  confirm_pin_textfield_->set_controller(this);
  AddChildView(confirm_pin_textfield_);

  confirm_button_ = views::MdTextButton::CreateSecondaryUiBlueButton(
      this, l10n_util::GetStringUTF16(IDS_CONFIRM));
  AddChildView(confirm_button_);

  prompt_label_ = new views::Label(
      l10n_util::GetStringUTF16(IDS_SET_PIN_PASSWORD_NOT_SAME));
  prompt_label_->SetEnabledColor(SK_ColorRED);
  prompt_label_->SetAutoColorReadabilityEnabled(false);
  prompt_label_->SetVisible(false);
  prompt_label_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  AddChildView(prompt_label_);
}

YSPSetPINView::~YSPSetPINView() {}

bool YSPSetPINView::HandleContextMenu(
    const content::ContextMenuParams& params) {
  return false;
}

void YSPSetPINView::ButtonPressed(views::Button* sender,
                                  const ui::Event& event) {
  if (sender == confirm_button_) {
    Submit();
  }
}

void YSPSetPINView::ContentsChanged(views::Textfield* sender,
                                    const base::string16& new_contents) {
  prompt_label_->SetVisible(false);
}

bool YSPSetPINView::HandleKeyEvent(views::Textfield* sender,
                                   const ui::KeyEvent& key_event) {
  if (key_event.key_code() == ui::VKEY_RETURN) {
    Submit();
    return true;
  }
  return false;
}

void YSPSetPINView::Submit() {
  const base::string16& pin_text = pin_textfield_->text();
  const base::string16& confirm_pin_text = confirm_pin_textfield_->text();
  // PIN code must be 6 digit
  if (!IsDigitString(pin_text) || !IsDigitString(confirm_pin_text) ||
      pin_text.length() != 6 || confirm_pin_text.length() != 6) {
    prompt_label_->SetText(l10n_util::GetStringUTF16(IDS_SET_PIN_ERROR_PROMOT));
    prompt_label_->SetVisible(true);
    return;
  }
  // two must same
  if (pin_text != confirm_pin_text) {
    prompt_label_->SetText(
        l10n_util::GetStringUTF16(IDS_SET_PIN_PASSWORD_NOT_SAME));
    prompt_label_->SetVisible(true);
    return;
  }
  YSPLoginManager::GetInstance()->UpdatePinKey(base::UTF16ToUTF8(pin_text));
  opaque_browser_frame_view_->ChangeScreenStatus(
      OpaqueBrowserFrameView::BROWSER_SCREEN);
}

bool YSPSetPINView::IsDigitString(const base::string16& text) {
  for (size_t i = 0; i < text.length(); i++) {
    if (!(text[i] >= '0' && text[i] <= '9'))
      return false;
  }
  return true;
}

void YSPSetPINView::Layout() {
  // SetBorder(views::CreateSolidBorder(1, SK_ColorBLACK));
  gfx::Rect bound = bounds();
  bound.Inset(1, 1);
  SetImageSize(bound.size());
  SetDrawCircle(false);

  const int window_width = width();

  gfx::Size title_label_size = title_label_->GetPreferredSize();
  int title_label_left = (window_width - title_label_size.width()) / 2;
  title_label_->SetBounds(title_label_left, 50, title_label_size.width(),
                          title_label_size.height());

  int y = 140;

  gfx::Size message_label_size = message_label_->GetPreferredSize();
  int left = 60;
  message_label_->SetBounds(left, y, message_label_size.width(),
                            message_label_size.height());

  gfx::Size set_pin_label_size = set_pin_label_->GetPreferredSize();
  y += 30;
  set_pin_label_->SetBounds(left, y, set_pin_label_size.width(), 40);

  int pin_textfield_left = left + set_pin_label_size.width() + 30;
  pin_textfield_->SetBounds(pin_textfield_left, y + 5, 260, 30);
  pin_textfield_->SetEnabled(true);

  gfx::Size confirm_set_pin_label_size =
      confirm_set_pin_label_->GetPreferredSize();
  y += 40;
  confirm_set_pin_label_->SetBounds(left, y, confirm_set_pin_label_size.width(),
                                    40);

  confirm_pin_textfield_->SetBounds(pin_textfield_left, y + 5, 260, 30);
  confirm_pin_textfield_->SetEnabled(true);

  gfx::Size confirm_pin_textfield_size =
      confirm_pin_textfield_->GetPreferredSize();
  int confirm_button_left = y + 45 + confirm_pin_textfield_size.height();
  gfx::Size confirm_button_size = confirm_button_->GetPreferredSize();
  confirm_button_->SetBounds(390, confirm_button_left,
                             confirm_button_size.width(),
                             confirm_button_size.height());

  gfx::Size prompt_label_size = prompt_label_->CalculatePreferredSize();
  prompt_label_->SetBounds(
      pin_textfield_left, y + 10 + confirm_pin_textfield_size.height(),
      prompt_label_size.width(), prompt_label_size.height());
}
