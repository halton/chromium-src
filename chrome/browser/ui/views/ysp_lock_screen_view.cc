
#ifdef REDCORE

#include "base/strings/utf_string_conversions.h"

#include "chrome/browser/browser_process.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/ysp_account_view.h"    //ysp+ { custom ui }
#include "chrome/browser/ui/views/ysp_lock_screen_view.h"
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

#if defined(WATERMARK) && defined(IE_REDCORE)
#include "ui/aura/window.h"
#include "ui/aura/window_tree_host.h"
#endif

void YSPLockScreenView::ShowLockedScreen(BrowserView *browser_view)
{
  //static YSPLockScreenView * locked_view = new YSPLockScreenView(browser_view);
}

YSPLockScreenView::YSPLockScreenView(views::ButtonListener *listener, BrowserView *browser_view)
      : browser_view_(browser_view),
        is_locked_(false)
{
  if (!browser_view)
    return;

  browser_ = browser_view->browser();
  const gfx::FontList& small_font =
      ui::ResourceBundle::GetSharedInstance().GetFontList(
          ui::ResourceBundle::SmallFont);

  info_label_ = new views::Label(l10n_util::GetStringUTF16(IDS_YSP_LOCK_BROWSER_IN_LOCK_MODE));
  info_label_->SetFontList(small_font.DeriveWithSizeDelta(14 - small_font.GetFontSize()));
  info_label_->SetAutoColorReadabilityEnabled(false);
  info_label_->SetEnabledColor(SK_ColorWHITE);
  AddChildView(info_label_);

  avatar_image_ = new views::ImageView();
  avatar_image_->SetDrawCircle(true);

  //avatar_image_->set_background(views::Background::CreateSolidBackground(255, 0, 255));
  AddChildView(avatar_image_);

  name_label_ = new views::Label(base::string16());
  name_label_->SetFontList(small_font.DeriveWithSizeDelta(14 - small_font.GetFontSize()));
  name_label_->SetAutoColorReadabilityEnabled(false);
  name_label_->SetEnabledColor(SK_ColorWHITE);
  AddChildView(name_label_);

  password_text_ = new views::Textfield();
  password_text_->set_controller(this);
  password_text_->SetFocusBehavior(FocusBehavior::ALWAYS);
  password_text_->SetBorder(NULL);
  password_text_->SetTextInputType(ui::TEXT_INPUT_TYPE_PASSWORD);
  password_text_->set_placeholder_text(l10n_util::GetStringUTF16(IDS_YSP_LOCK_BROWSER_ENTER_PASSWORD_TO_UNLOCK));
  password_text_->set_controller(this);
  AddChildView(password_text_);

  login_button_ = new views::ImageButton(this);
  const ui::ThemeProvider* tp = browser_view->frame()->GetThemeProvider();
  login_button_->SetImage(views::Button::ButtonState::STATE_NORMAL, tp->GetImageSkiaNamed(IDR_YSP_LOCK_SCREEN_ENTER));
  login_button_->SetImage(views::Button::ButtonState::STATE_HOVERED, tp->GetImageSkiaNamed(IDR_YSP_LOCK_SCREEN_ENTER_H));
  login_button_->SetImage(views::Button::ButtonState::STATE_PRESSED, tp->GetImageSkiaNamed(IDR_YSP_LOCK_SCREEN_ENTER_P));
  login_button_->SetImageAlignment(views::ImageButton::ALIGN_CENTER, views::ImageButton::ALIGN_MIDDLE);
  login_button_->SetBackground(views::CreateSolidBackground(SK_ColorWHITE));
  AddChildView(login_button_);

  error_prompt_ = new views::Label(l10n_util::GetStringUTF16(IDS_YSP_LOCK_BROWSER_REENTER_PASSWORD));
  error_prompt_->SetFontList(small_font.DeriveWithSizeDelta(14 - small_font.GetFontSize()));
  error_prompt_->SetEnabledColor(SK_ColorWHITE);
  error_prompt_->SetVisible(false);
  AddChildView(error_prompt_);

  YSPLoginManager::GetInstance()->AddObserver(this);
  profile_pref_registrar_.Init(g_browser_process->local_state());
  profile_pref_registrar_.Add(
    prefs::kYSPLockScreen,
    base::Bind(&YSPLockScreenView::OnLockStatusChanged, base::Unretained(this)));

}

YSPLockScreenView::~YSPLockScreenView()
{
	YSPLoginManager::GetInstance()->RemoveObserver(this);
}

static int GetLeftTop(int window_width, int view_width)
{
  return (window_width > view_width) ? (window_width - view_width) / 2 : 0;
}

void YSPLockScreenView::Lock(Browser::YSPLockStatus status)
{
  PrefService *pref = g_browser_process->local_state();
  if (status != pref->GetInteger(prefs::kYSPLockScreen))
    pref->SetInteger(prefs::kYSPLockScreen, status);
}

void YSPLockScreenView::LockInternal(Browser::YSPLockStatus status)
{
  if (!is_locked_) {
    is_locked_ = true;
    if (status == Browser::SCREEN_LOCKED)
      info_label_->SetText(l10n_util::GetStringUTF16(IDS_YSP_LOCK_BROWSER_IN_LOCK_MODE));
    else if (status == Browser::TOKEN_EXPIRED_LOCKED)
      info_label_->SetText(l10n_util::GetStringUTF16(IDS_YSP_FLOAT_LOGIN_WINDOW_VIEW_TITLE));
    else
      NOTREACHED();
    const ui::ThemeProvider* tp = browser_view_->frame()->GetThemeProvider();
    SetImage(tp->GetImageSkiaNamed(IDR_YSP_LOCK_SCREEN_BACKGROUND));
    avatar_image_->SetImage(browser_view_->account_view()->GetHeadImage());
    avatar_image_->SetImageSize(gfx::Size(80, 80));
    error_prompt_->SetVisible(false);
    browser_view_->SetVisible(false);
    //browser_->SetLockStatus(status);
    SetVisible(true);
    password_text_->RequestFocus();
    login_button_->SetEnabled(false);
#if defined(WATERMARK) && defined(IE_REDCORE)
    aura::WindowTreeHost* pHost = NULL;
    pHost = browser_view_->GetNativeWindow()->GetToplevelWindow()->GetHost();
    if (pHost) {
      pHost->OnLockScreenChanged(true);
    }
#endif
    parent()->Layout();
  }
}

void YSPLockScreenView::Unlock()
{
  PrefService *pref = g_browser_process->local_state();
  if (pref->GetInteger(prefs::kYSPLockScreen) != Browser::UNLOCKED)
    pref->SetInteger(prefs::kYSPLockScreen, Browser::UNLOCKED);
}

void YSPLockScreenView::UnlockInternal()
{
  if (is_locked_) {
    is_locked_ = false;
    browser_view_->SetVisible(true);
    //browser_->SetLockStatus(Browser::UNLOCKED);
    SetVisible(false);
    password_text_->SetText(base::string16());
    browser_view_->frame()->GetFrameView()->Layout();
#if defined(WATERMARK) && defined(IE_REDCORE)
    aura::WindowTreeHost* pHost = NULL;
    pHost = browser_view_->GetNativeWindow()->GetToplevelWindow()->GetHost();
    if (pHost)
      pHost->OnLockScreenChanged(false);
#endif
  }
}


void YSPLockScreenView::Submit()
{
  std::string text = base::UTF16ToUTF8(password_text_->text());
  if (g_browser_process->local_state()->GetInteger(prefs::kYSPLockScreen) == Browser::TOKEN_EXPIRED_LOCKED) {
    YSPLoginManager::GetInstance()->StartLogin(YSPLoginManager::GetInstance()->GetLastCID(),
                                               YSPLoginManager::GetInstance()->GetLastUID(),
                                               text);
    return;
  }

  if (YSPLoginManager::GetInstance()->isValidPassword(text)) {
    Unlock();
  }
  else {
    error_prompt_->SetVisible(true);
    InvalidateLayout();
  }
}

void YSPLockScreenView::Layout()
{
  // TODO (LIUWEI) hard code for now
  SetBorder(views::CreateSolidBorder(1, SK_ColorBLACK));
  gfx::Rect bound = bounds();
  bound.Inset(1, 1);
  SetImageSize(bound.size());

  const int window_width = width();
  int top = (double)height() * (0.85/1.85);
  gfx::Size info_size = info_label_->GetPreferredSize();
  int x = GetLeftTop(window_width, info_size.width());

  //gfx::Rect bounds(width() / 2, height() / 2, 80, 80);
  info_label_->SetBounds(x, top, info_size.width(), 14);
  top += 14 + 40;

  x = GetLeftTop(window_width, 80);
  avatar_image_->SetBounds(x, top, 80, 80);
  top += 80 + 20;


  name_label_->SetText(browser_view_->GetUserNameString());
  gfx::Size name_size = name_label_->GetPreferredSize();
  x = GetLeftTop(window_width, name_size.width());
  name_label_->SetBounds(x, top, name_size.width(), 14);
  top += 14 + 20;

  x = GetLeftTop(window_width, 320);
  password_text_->SetBounds(x, top, 280, 40);
  login_button_->SetBounds(x + 280, top, 40, 40);
  top += 40 + 10;

  gfx::Size error_size = error_prompt_->GetPreferredSize();
  error_prompt_->SetBounds(x, top, error_size.width(), error_size.height());

  // ysp: frame view is destroyed in case of Windows user switch but browser view will not,
  // check/restore the lock state in this case

  Browser::YSPLockStatus lock = static_cast<Browser::YSPLockStatus>(
    g_browser_process->local_state()->GetInteger(prefs::kYSPLockScreen));
  if (lock != Browser::UNLOCKED)
    LockInternal(lock);
}

void YSPLockScreenView::OnLoginRequestFailure(const std::string& error)
{
}

void YSPLockScreenView::OnLoginResponseParseFailure(const std::string& error)
{
}

void YSPLockScreenView::OnLoginFailure(base::string16 message)
{
}

void YSPLockScreenView::OnLoginSuccess(const base::string16 & name, const std::string & head_image_url)
{
}

void YSPLockScreenView::OnLogout()
{
}

void YSPLockScreenView::OnTokenStatusChanged(const std::string& type)
{
  if (type == "TokenExpired") {
    Lock(Browser::TOKEN_EXPIRED_LOCKED);
  }
  else if (type == "TokenAvailable") {
    if (is_locked_)
      Unlock();
  }
  else if (type == "failure") {
    error_prompt_->SetVisible(true);
    InvalidateLayout();
  }
}


bool YSPLockScreenView::HandleContextMenu(const content::ContextMenuParams & params)
{
  return false;
}

void YSPLockScreenView::ButtonPressed(views::Button * sender, const ui::Event & event)
{
  if (sender == login_button_) {
    // TODO: (LIUWEI) Implement it
    Submit();
  }
}

void YSPLockScreenView::ContentsChanged(views::Textfield * sender, const base::string16 & new_contents)
{
  bool enable = password_text_->text().length() > 0;
  login_button_->SetEnabled(enable);
}

bool YSPLockScreenView::HandleKeyEvent(views::Textfield * sender, const ui::KeyEvent & key_event)
{
  // TODO: (LIUWEI)
  if (key_event.key_code() == ui::VKEY_RETURN) {
    if (password_text_->text().length() > 0)
      Submit();
    return true;
  }

  return false;
}

void YSPLockScreenView::OnLockStatusChanged()
{
  Browser::YSPLockStatus lock = static_cast<Browser::YSPLockStatus>(
                                  g_browser_process->local_state()->GetInteger(prefs::kYSPLockScreen));
  if (lock)
    LockInternal(lock);
  else
    UnlockInternal();
}

#endif
