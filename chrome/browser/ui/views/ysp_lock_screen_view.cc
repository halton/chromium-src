// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/ysp_lock_screen_view.h"

#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/ysp_account_view.h"
#include "chrome/common/pref_names.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/grit/theme_resources.h"
#include "components/prefs/pref_service.h"
#include "crypto/sha2.cc"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/theme_provider.h"
#include "ui/gfx/color_utils.h"
#include "ui/native_theme/native_theme.h"
#include "ui/views/controls/button/checkbox.h"
#include "ui/views/controls/button/image_button.h"
#include "ui/views/controls/image_view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/webview/webview.h"
#include "ui/views/layout/grid_layout.h"

#if defined(WATERMARK) && defined(IE_REDCORE)
#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/containers/queue.h"
#include "base/location.h"
#include "ui/aura/window.h"
#include "ui/aura/window_tree_host.h"
#endif

void YSPLockScreenView::ShowLockedScreen(BrowserView* browser_view) {
  //static YSPLockScreenView * locked_view = new YSPLockScreenView(browser_view);
}

YSPLockScreenView::YSPLockScreenView(
    OpaqueBrowserFrameView* opaque_browser_frame_view,
    BrowserView* browser_view)
    : browser_view_(browser_view),
      opaque_browser_frame_view_(opaque_browser_frame_view) {
  browser_ = browser_view->browser();

  const ui::ThemeProvider* tp = browser_view_->frame()->GetThemeProvider();
  SetImage(tp->GetImageSkiaNamed(IDR_YSP_LOCK_SCREEN_BACKGROUND));

  const gfx::FontList& small_font =
      ui::ResourceBundle::GetSharedInstance().GetFontList(
          ui::ResourceBundle::SmallFont);

  const gfx::FontList& bold_font =
      ui::ResourceBundle::GetSharedInstance().GetFontList(
          ui::ResourceBundle::BoldFont);

  info_label_ =
      new views::Label(l10n_util::GetStringUTF16(IDS_UNLOCK_SCREEN_TITLE));
  info_label_->SetFontList(
      bold_font.DeriveWithSizeDelta(15 - bold_font.GetFontSize()));
  info_label_->SetAutoColorReadabilityEnabled(false);
  info_label_->SetEnabledColor(SK_ColorWHITE);
  AddChildView(info_label_);

  avatar_image_ = new views::ImageView();
  avatar_image_->SetDrawCircle(true);
  avatar_image_->SetImageSize(gfx::Size(62, 62));
  avatar_image_->SetImage(tp->GetImageSkiaNamed(IDR_YSP_LOGIN_AVATAR));
  AddChildView(avatar_image_);

  name_label_ = new views::Label();
  name_label_->SetFontList(
      small_font.DeriveWithSizeDelta(15 - small_font.GetFontSize()));
  name_label_->SetAutoColorReadabilityEnabled(false);
  name_label_->SetEnabledColor(SK_ColorWHITE);
  AddChildView(name_label_);

  for (int i = 0; i < 6; i++) {
    views::Textfield* password_textfield = new views::Textfield();
    password_textfield->set_controller(this);
    password_textfield->SetHorizontalAlignment(gfx::ALIGN_CENTER);
    password_textfield->SetFontList(
        small_font.DeriveWithSizeDelta(17 - small_font.GetFontSize()));
    password_textfield->SetTextInputType(ui::TEXT_INPUT_TYPE_PASSWORD);
    AddChildView(password_textfield);
    password_text_.push_back(password_textfield);
  }

  error_prompt_ = new views::Label(l10n_util::GetStringUTF16(IDS_YSP_LOCK_BROWSER_REENTER_PASSWORD));
  error_prompt_->SetFontList(
      small_font.DeriveWithSizeDelta(15 - small_font.GetFontSize()));
  error_prompt_->SetAutoColorReadabilityEnabled(false);
  error_prompt_->SetEnabledColor(SK_ColorWHITE);
  error_prompt_->SetVisible(false);
  AddChildView(error_prompt_);

  error_confirm_button_ =
      views::MdTextButton::Create(this, l10n_util::GetStringUTF16(IDS_CONFIRM));
  error_confirm_button_->SetVisible(false);
  error_confirm_button_->SetEnabledTextColors(SK_ColorWHITE);
  const SkColor c_color = SkColorSetRGB(151, 151, 151);
  error_confirm_button_->SetBgColorOverride(c_color);
  error_confirm_button_->SetFocusBehavior(FocusBehavior::NEVER);
  error_confirm_button_->SetBackground(
      views::CreateSolidBackground(SK_ColorWHITE));
  AddChildView(error_confirm_button_);

  forget_pin_ink_label_ =
      new views::Link(l10n_util::GetStringUTF16(IDS_FORGET_PIN_CODE));
  forget_pin_ink_label_->SetVisible(true);
  forget_pin_ink_label_->SetAutoColorReadabilityEnabled(false);
  forget_pin_ink_label_->SetEnabledColor(SK_ColorWHITE);
  forget_pin_ink_label_->set_listener(this);
  forget_pin_ink_label_->SetFocusBehavior(FocusBehavior::NEVER);
  AddChildView(forget_pin_ink_label_);

  forget_password_message_bg_image_ = new views::ImageView();
  forget_password_message_bg_image_->SetDrawCircle(false);
  forget_password_message_bg_image_->SetImage(
      tp->GetImageSkiaNamed(IDR_YSP_FORGET_PIN_BG));
  forget_password_message_bg_image_->SetImageSize(gfx::Size(500, 232));
  AddChildView(forget_password_message_bg_image_);

  forget_password_message_title_label_ =
      new views::Label(l10n_util::GetStringUTF16(IDS_FORGET_PIN_CODE));
  forget_password_message_title_label_->SetFontList(
      bold_font.DeriveWithSizeDelta(16 - bold_font.GetFontSize()));
  forget_password_message_title_label_->SetAutoColorReadabilityEnabled(false);
  forget_password_message_title_label_->SetEnabledColor(SK_ColorBLACK);
  AddChildView(forget_password_message_title_label_);

  forget_password_message_label_ =
      new views::Label(l10n_util::GetStringUTF16(IDS_FORGET_PIN_CODE_MESSAGE));
  forget_password_message_label_->SetFontList(
      small_font.DeriveWithSizeDelta(15 - small_font.GetFontSize()));
  forget_password_message_label_->SetAutoColorReadabilityEnabled(false);
  forget_password_message_label_->SetEnabledColor(SK_ColorBLACK);
  forget_password_message_label_->SetMultiLine(true);
  forget_password_message_label_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  AddChildView(forget_password_message_label_);

  forget_password_message_confirm_button_ =
      views::MdTextButton::CreateSecondaryUiBlueButton(
          this, l10n_util::GetStringUTF16(IDS_CONFIRM));
  forget_password_message_confirm_button_->SetFocusBehavior(
      FocusBehavior::NEVER);
  AddChildView(forget_password_message_confirm_button_);

  ShowForgetPasswordDialog(false);

  YSPLoginManager::GetInstance()->AddObserver(this);
  profile_pref_registrar_.Init(g_browser_process->local_state());
  profile_pref_registrar_.Add(
      prefs::kYSPLockScreen, base::Bind(&YSPLockScreenView::OnLockStatusChanged,
                                        base::Unretained(this)));
}

YSPLockScreenView::~YSPLockScreenView() {
  YSPLoginManager::GetInstance()->RemoveObserver(this);
}

void YSPLockScreenView::Submit() {
  std::string pin = std::string();
  for (size_t i = 0; i < password_text_.size(); i++) {
    pin += base::UTF16ToUTF8(password_text_.at(i)->text());
    password_text_.at(i)->SetText(base::EmptyString16());
  }
  password_text_.at(0)->RequestFocus();
  if (pin.length() == 6) {
    std::string pin_key = YSPLoginManager::GetInstance()->GetUserPinKey();
    std::string text_sha_key = crypto::SHA256HashString(pin);
    text_sha_key = base::HexEncode(text_sha_key.data(), text_sha_key.length());
    if (text_sha_key == pin_key) {
      Unlock();
      return;
    }
  }
  ShowError(true);
}

void YSPLockScreenView::DownloadImage(const std::string& url) {
  content::WebContents* webContents = browser_view_->GetActiveWebContents();
  if (!webContents)  return;

  webContents->DownloadImage(GURL(url), true, 0, true,
                             base::Bind(&YSPLockScreenView::DidDownloadFavicon,
                                        base::Unretained(this)));
}

void YSPLockScreenView::DidDownloadFavicon(
    int id,
    int http_status_code,
    const GURL& image_url,
    const std::vector<SkBitmap>& bitmaps,
    const std::vector<gfx::Size>& original_bitmap_sizes) {
  DLOG(INFO) << "YSPAccountView::DidDownloadFavicon";
  if (bitmaps.size() > 0) {
    avatar_image_->SetImage(
        new gfx::ImageSkia(gfx::ImageSkiaRep(bitmaps[0], 1)));
  }
}

void YSPLockScreenView::ShowForgetPasswordDialog(bool show) {
  forget_pin_ink_label_->SetEnabled(!show);
  for (size_t i = 0; i < password_text_.size(); i++) {
    password_text_.at(i)->SetVisible(!show);
    password_text_.at(i)->SetText(base::EmptyString16());
  }
  password_text_.at(0)->RequestFocus();
  if (show) {
    forget_password_message_bg_image_->SetVisible(true);
    forget_password_message_title_label_->SetVisible(true);
    forget_password_message_label_->SetVisible(true);
    forget_password_message_confirm_button_->SetVisible(true);
    error_confirm_button_->SetVisible(false);
    error_prompt_->SetVisible(false);
    name_label_->SetVisible(false);
    avatar_image_->SetVisible(false);
    info_label_->SetVisible(false);
  } else {
    forget_password_message_bg_image_->SetVisible(false);
    forget_password_message_title_label_->SetVisible(false);
    forget_password_message_label_->SetVisible(false);
    forget_password_message_confirm_button_->SetVisible(false);
    error_confirm_button_->SetVisible(false);
    error_prompt_->SetVisible(false);
    name_label_->SetVisible(true);
    avatar_image_->SetVisible(true);
    info_label_->SetVisible(true);
  }
}

void YSPLockScreenView::ShowError(bool show) {
  for (size_t i = 0; i < password_text_.size(); i++) {
    password_text_.at(i)->SetVisible(!show);
  }
  error_prompt_->SetVisible(show);
  error_confirm_button_->SetVisible(show);
  if (!show) {
    password_text_.at(0)->RequestFocus();
  }
}

void YSPLockScreenView::OnLockStatusChanged() {
  Browser::YSPLockStatus lock = static_cast<Browser::YSPLockStatus>(
      g_browser_process->local_state()->GetInteger(prefs::kYSPLockScreen));
  if (lock) {
    LockInternal();
  } else {
    UnlockInternal();
  }
}

void YSPLockScreenView::Lock() {
  PrefService* pref = g_browser_process->local_state();
  if (Browser::SCREEN_LOCKED != pref->GetInteger(prefs::kYSPLockScreen))
    pref->SetInteger(prefs::kYSPLockScreen, Browser::SCREEN_LOCKED);
}

void YSPLockScreenView::Unlock() {
  PrefService* pref = g_browser_process->local_state();
  if (pref->GetInteger(prefs::kYSPLockScreen) != Browser::UNLOCKED)
    pref->SetInteger(prefs::kYSPLockScreen, Browser::UNLOCKED);
}

void YSPLockScreenView::LockInternal() {
  opaque_browser_frame_view_->ChangeScreenStatus(
      OpaqueBrowserFrameView::LOCK_SCREEN);
  if ((password_text_.at(0)->text()).empty()) {
    views::FocusManager* focus_manager = browser_view_->GetFocusManager();
    if (focus_manager) {
      focus_manager->ClearFocus();
    }
    password_text_.at(0)->RequestFocus();
  }
  DLOG(INFO) << "request focus";
}

void YSPLockScreenView::UnlockInternal() {
  opaque_browser_frame_view_->ChangeScreenStatus(
      OpaqueBrowserFrameView::BROWSER_SCREEN);
}

int YSPLockScreenView::GetLeftTop(int window_width, int view_width) {
  return (window_width > view_width) ? (window_width - view_width) / 2 : 0;
}

void YSPLockScreenView::Layout() {
  // TODO (LIUWEI) hard code for now
  // SetBorder(views::CreateSolidBorder(1, SK_ColorBLACK));
  gfx::Rect bound = bounds();
  bound.Inset(1, 1);
  SetImageSize(bound.size());
  SetDrawCircle(false);

  const int window_width = width();
  const int window_height = height();
  int top = (double)window_height * 0.374;
  gfx::Size info_size = info_label_->GetPreferredSize();
  int x = GetLeftTop(window_width, info_size.width());

  //gfx::Rect bounds(width() / 2, height() / 2, 80, 80);
  info_label_->SetBounds(x, top, info_size.width(), info_size.height());
  top += info_size.height() + 40;

  x = GetLeftTop(window_width, 62);
  avatar_image_->SetBounds(x, top, 62, 62);
  top += 62 + 20;

  gfx::Size name_size = name_label_->GetPreferredSize();
  x = GetLeftTop(window_width, name_size.width());
  name_label_->SetBounds(x, top, name_size.width(), 15);
  top += 15 + 30;

  int total_length = 36 * 6 + 5 * 10;
  x = GetLeftTop(window_width, total_length);
  for (size_t i = 0; i < password_text_.size(); i++) {
    int start_x = x + 46 * i;
    password_text_.at(i)->SetBounds(start_x, top, 36, 36);
  }

  gfx::Size error_size = error_prompt_->GetPreferredSize();
  x = GetLeftTop(window_width, error_size.width());
  error_prompt_->SetBounds(x, top, error_size.width(), error_size.height());

  top += 30 + error_size.height();
  x = GetLeftTop(window_width, 68);
  error_confirm_button_->SetBounds(x, top, 68, 32);

  gfx::Size forget_button_size = forget_pin_ink_label_->GetPreferredSize();
  int f_top = height() - 30 - forget_button_size.height();
  x = GetLeftTop(window_width, forget_button_size.width());
  forget_pin_ink_label_->SetBounds(x, f_top, forget_button_size.width(),
                                   forget_button_size.height());

  gfx::Size forget_bg_image_size =
      forget_password_message_bg_image_->GetPreferredSize();
  int bg_image_x = GetLeftTop(window_width, forget_bg_image_size.width());
  int bg_image_y = GetLeftTop(window_height, forget_bg_image_size.height());
  forget_password_message_bg_image_->SetBounds(bg_image_x, bg_image_y,
                                               forget_bg_image_size.width(),
                                               forget_bg_image_size.height());

  gfx::Size forget_password_title_size =
      forget_password_message_title_label_->GetPreferredSize();
  bg_image_x += 60;
  bg_image_y += 40;
  forget_password_message_title_label_->SetBounds(
      bg_image_x, bg_image_y, forget_password_title_size.width(),
      forget_password_title_size.height());

  gfx::Size forget_password_message_size =
      forget_password_message_label_->GetPreferredSize();
  bg_image_y += (23 + forget_password_title_size.height());
  forget_password_message_label_->SetBounds(
      bg_image_x, bg_image_y, 370, forget_password_message_size.height());

  gfx::Size forget_password_button_size =
      forget_password_message_confirm_button_->GetPreferredSize();
  bg_image_y += (20 + forget_password_message_size.height());
  bg_image_x += 337;
  forget_password_message_confirm_button_->SetBounds(
      bg_image_x, bg_image_y, forget_password_button_size.width(),
      forget_password_button_size.height());

  if (YSPLoginManager::GetInstance()->GetUserPinKey().empty())  return;

  Browser::YSPLockStatus lock = static_cast<Browser::YSPLockStatus>(
      g_browser_process->local_state()->GetInteger(prefs::kYSPLockScreen));
  if (lock == Browser::UNLOCKED) {
    UnlockInternal();
  } else {
    LockInternal();
  }
}

bool YSPLockScreenView::HandleContextMenu(
    const content::ContextMenuParams& params) {
  return false;
}

void YSPLockScreenView::ButtonPressed(views::Button* sender,
                                      const ui::Event& event) {
  if (sender == error_confirm_button_) {
    ShowError(false);
  } else if (sender == forget_password_message_confirm_button_) {
    ShowForgetPasswordDialog(false);
  }
}

void YSPLockScreenView::LinkClicked(views::Link* source, int event_flags) {
  if (source == forget_pin_ink_label_) {
    ShowForgetPasswordDialog(true);
  }
}

void YSPLockScreenView::ContentsChanged(views::Textfield* sender,
                                        const base::string16& new_contents) {
  size_t current = 0;
  for (size_t i = 0; i < password_text_.size(); i++) {
    if (password_text_.at(i) == sender) {
      current = i;
      break;
    }
  }
  if (!new_contents.empty()) {
    const base::string16& single =
        (base::string16)new_contents.substr(new_contents.length() - 1, 1);
    if (single[0] >= '0' && single[0] <= '9') {
      sender->SetText(single);
    } else {
      sender->SetText(base::EmptyString16());
      return;
    }
    if (current + 1 < password_text_.size()) {
      password_text_.at(current + 1)->RequestFocus();
    } else {
      password_text_.at(current)->RequestFocus();
    }
    if (current == 5) {
      Submit();
    }
  }
}

bool YSPLockScreenView::HandleKeyEvent(views::Textfield* sender,
                                       const ui::KeyEvent& key_event) {
  if (key_event.key_code() != ui::VKEY_BACK ||
      key_event.type() != ui::ET_KEY_PRESSED)
    return false;
  for (size_t i = password_text_.size() - 1; i >= 0 && i<= 5; i--) {
    if (!password_text_.at(i)->text().empty()) {
      password_text_.at(i)->RequestFocus();
      password_text_.at(i)->SetText(base::EmptyString16());
      break;
    }
  }
  return false;
}

bool YSPLockScreenView::HandleMouseEvent(views::Textfield* sender,
                                         const ui::MouseEvent& mouse_event) {
  return false;
}

void YSPLockScreenView::OnLoginRequestFailure(const std::string& error) {}

void YSPLockScreenView::OnLoginResponseParseFailure(const std::string& error) {}

void YSPLockScreenView::OnLoginFailure(const base::string16& message) {}

void YSPLockScreenView::OnLoginSuccess(const base::string16& name,
                                       const std::string& head_image_url) {
  name_label_->SetText(name);
  if (!head_image_url.empty()) {
    DownloadImage(head_image_url);
  } else {
    const ui::ThemeProvider* tp = browser_view_->frame()->GetThemeProvider();
    avatar_image_->SetImage(tp->GetImageSkiaNamed(IDR_YSP_LOGIN_AVATAR));
  }
}

void YSPLockScreenView::OnLogout() {}

void YSPLockScreenView::OnTokenStatusChanged(const std::string& type) {}
