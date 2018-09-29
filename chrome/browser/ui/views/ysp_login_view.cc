#ifdef REDCORE

//ysp+ { login }

#include "chrome/browser/ui/views/ysp_login_view.h"

#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/common/pref_names.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/grit/theme_resources.h"
#include "components/prefs/pref_service.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/native_theme/native_theme.h"
#include "ui/views/controls/button/blue_button.h"
#include "ui/views/controls/button/checkbox.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/image_view.h"
#include "ui/views/controls/webview/webview.h"
#include "ui/views/layout/grid_layout.h"

namespace {

const int kFixedLoginViewWidth = 260;
const int kFixedLogoutViewWidth = 220;
//const int kTitleViewNativeWidgetOffset = 8;
const int kHeaderNavItemWidth = 80;
const int kPaddingMargin = 16;
const int kRowPaddingLarge = 32;
const int kRowPaddingMiddle = 16;
const int kRowPaddingSmall = 8;
const int AvatarImageSize = 110;

// Helpers --------------------------------------------------------------------

views::GridLayout* CreateSingleColumnLayout(views::View* view, int width) {
  views::GridLayout* layout = view->SetLayoutManager(
      std::make_unique<views::GridLayout>(view));

  views::ColumnSet* columns = layout->AddColumnSet(0);
  columns->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL, 0,
                     views::GridLayout::FIXED, width, width);
  return layout;
}

views::GridLayout* CreatePaddingSingleColumnLayout(views::View* view, int width, int padding_width) {
  views::GridLayout* layout = view->SetLayoutManager(
      std::make_unique<views::GridLayout>(view));

  views::ColumnSet* columns = layout->AddColumnSet(0);
  columns->AddPaddingColumn(0, padding_width);
  columns->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL, 0,
      views::GridLayout::FIXED, width, width);

  return layout;
}

}  // namespace

// YSPLoginView ---------------------------------------------------------

// static
YSPLoginView* YSPLoginView::login_bubble_ = nullptr;

// static
void YSPLoginView::ShowBubble(
    views::View* anchor_view,
    Browser* browser,
    BrowserView* browser_view,
    base::string16& user_name,
    gfx::ImageSkia* head_image) {
  if (IsShowing()) {
    return;
  }

  login_bubble_ = new YSPLoginView(
    anchor_view, browser, browser_view, user_name, head_image);
  views::Widget* widget =
      views::BubbleDialogDelegateView::CreateBubble(login_bubble_);
  login_bubble_->SetAlignment(views::BubbleBorder::ALIGN_EDGE_TO_ANCHOR_EDGE);
  login_bubble_->SetArrowPaintType(views::BubbleBorder::PAINT_NONE);
  widget->Show();
}

// static
bool YSPLoginView::IsShowing() {
  return login_bubble_ != NULL;
}

// static
void YSPLoginView::Hide() {
  if (IsShowing())
    login_bubble_->GetWidget()->Close();
}

void YSPLoginView::SetAndRefreshAvatar(gfx::ImageSkia * head_image)
{
	if (IsShowing() && login_bubble_ && login_bubble_->avatar_image_) {
		login_bubble_->head_image_ = head_image;
		if (head_image)
			login_bubble_->avatar_image_->SetImage(*(login_bubble_->head_image_));
	}
}

YSPLoginView::YSPLoginView(views::View* anchor_view,
                           Browser* browser,
                           BrowserView* browser_view,
                           base::string16& user_name,
                           gfx::ImageSkia* head_image)
    : view_state_(VS_LOGOUT),
      head_image_(head_image),
      domain_text_(nullptr),
      //domain_prompt_label_(nullptr),
      account_text_(nullptr),
      //account_prompt_label_(nullptr),
      password_text_(nullptr),
      password_prompt_label_(nullptr),
      login_button_(nullptr),
      server_setting_button_(nullptr),
      auto_login_checkbox_(nullptr),
      avatar_image_(nullptr),
      name_label_(nullptr),
      logout_button_(nullptr),
      setting_back_button_(nullptr),
      setting_clear_button_(nullptr),
      host_text_(nullptr),
      setting_ok_button_(nullptr),
      user_name_(user_name) {
  YSPLoginManager::GetInstance()->AddObserver(this);
}

YSPLoginView::~YSPLoginView() {
  YSPLoginManager::GetInstance()->RemoveObserver(this);
}

void YSPLoginView::Init() {
  ShowView();
  if(!YSPLoginManager::GetInstance()->Restore()) {
    view_state_ = VS_LOGIN;
    ShowView();
  }
}

void YSPLoginView::OnNativeThemeChanged(
    const ui::NativeTheme* native_theme) {
  views::BubbleDialogDelegateView::OnNativeThemeChanged(native_theme);
  SetBackground(views::CreateSolidBackground(
      GetNativeTheme()->GetSystemColor(
          ui::NativeTheme::kColorId_DialogBackground)));
}

void YSPLoginView::ShowView() {
  ResetView();
  RemoveAllChildViews(true);

  views::GridLayout* layout = nullptr;
  views::View* sub_view = nullptr;
  views::View* view_to_focus = nullptr;
  int prefer_width = 0;

  if (view_state_ == VS_LOGIN)
    sub_view = CreateLoginSubview(&view_to_focus, &prefer_width);
  else if (view_state_ == VS_LOGOUT)
    sub_view = CreateLogoutSubview(&view_to_focus, &prefer_width);
  else if (view_state_ == VS_SETTING)
    sub_view = CreateSettingSubview(&view_to_focus, &prefer_width);
  else if (view_state_ == VS_LOADING)
    sub_view = CreateLoadingSubview(&view_to_focus, &prefer_width);

  if (!sub_view || prefer_width < 1)
    return;

  layout = CreateSingleColumnLayout(this, prefer_width);
  layout->StartRow(1, 0);
  layout->AddView(sub_view);

  Layout();
  if (GetBubbleFrameView())
    SizeToContents();
  if (view_to_focus)
    view_to_focus->RequestFocus();
}

void YSPLoginView::ResetView() {
  domain_text_ = nullptr;
  //domain_prompt_label_ = nullptr;
  account_text_ = nullptr;
  //account_prompt_label_ = nullptr;
  password_text_ = nullptr;
  password_prompt_label_ = nullptr;
  login_button_ = nullptr;
  server_setting_button_ = nullptr;
  host_text_ = nullptr;
  server_setting_button_ = nullptr;
  setting_back_button_ = nullptr;
  setting_clear_button_ = nullptr;
  avatar_image_ = nullptr;
  name_label_ = nullptr;
  logout_button_ = nullptr;
  setting_back_button_ = nullptr;
  setting_clear_button_ = nullptr;
  host_text_ = nullptr;
  setting_ok_button_ = nullptr;
}

void YSPLoginView::WindowClosing() {
  DCHECK_EQ(login_bubble_, this);
  login_bubble_ = NULL;
}

bool YSPLoginView::AcceleratorPressed(
    const ui::Accelerator& accelerator) {
  if (accelerator.key_code() != ui::VKEY_DOWN &&
      accelerator.key_code() != ui::VKEY_UP)
    return AcceleratorPressed(accelerator);

  // Move the focus up or down.
  GetFocusManager()->AdvanceFocus(accelerator.key_code() != ui::VKEY_DOWN);
  return true;
}

views::View* YSPLoginView::GetInitiallyFocusedView() {
  return NULL;
}

bool YSPLoginView::HandleContextMenu(
    const content::ContextMenuParams& params) {
  // Suppresses the context menu because some features, such as inspecting
  // elements, are not appropriate in a bubble.
  return true;
}

views::View* YSPLoginView::CreateLoginSubview(views::View** focus_view, int* prefer_width) {
  views::View* view = new views::View();
  views::GridLayout* layout = CreatePaddingSingleColumnLayout(
    view, kFixedLoginViewWidth, kPaddingMargin);
  *prefer_width = kFixedLoginViewWidth + 2 * kPaddingMargin;

  ui::ResourceBundle& bundle = ui::ResourceBundle::GetSharedInstance();
  //const gfx::FontList& large_font_list =
  //  bundle.GetFontList(ui::ResourceBundle::LargeFont);

  // header image
  layout->StartRowWithPadding(1, 0, 0, kRowPaddingLarge);
  views::ImageView* branding_image = new views::ImageView;
  branding_image->SetImage(*bundle.GetImageSkiaNamed(IDR_YSP_LOGIN_HEADER));
  layout->AddView(branding_image);

  layout->AddPaddingRow(0, kRowPaddingMiddle);

  // title
  //layout->StartRowWithPadding(1, 0, 0, kRowPaddingMiddle);
  //views::Label* title_label = new views::Label(l10n_util::GetStringUTF16(IDS_YSP_LOGIN_VIEW_TITLE));
  //title_label->SetFontList(large_font_list);
  //layout->AddView(title_label);

  // domain input
  layout->StartRowWithPadding(1, 0, 0, kRowPaddingMiddle);
  domain_text_ = new views::Textfield;
  domain_text_->set_placeholder_text(l10n_util::GetStringUTF16(IDS_YSP_LOGIN_VIEW_DOMAIN_PLACEHOLDER));
  domain_text_->set_controller(this);
  layout->AddView(domain_text_);
  *focus_view = domain_text_;

  // domain prompt
  // layout->StartRow(1, 0);
  // domain_prompt_label_ = new views::Label(l10n_util::GetStringUTF16(IDS_YSP_LOGIN_VIEW_DOMAIN_PROMPT));
  // domain_prompt_label_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  // domain_prompt_label_->SetEnabledColor(SK_ColorRED);
  // domain_prompt_label_->SetVisible(false);
  // layout->AddView(domain_prompt_label_);

  layout->AddPaddingRow(0, kRowPaddingSmall);

  // account input
  layout->StartRowWithPadding(1, 0, 0, kRowPaddingSmall);
  account_text_ = new views::Textfield;
  account_text_->set_placeholder_text(l10n_util::GetStringUTF16(IDS_YSP_LOGIN_VIEW_ACCOUNT_PLACEHOLDER));
  account_text_->set_controller(this);
  layout->AddView(account_text_);

  // account prompt
  // layout->StartRow(1, 0);
  // account_prompt_label_ = new views::Label(l10n_util::GetStringUTF16(IDS_YSP_LOGIN_VIEW_ACCOUNT_PROMPT));
  // account_prompt_label_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  // account_prompt_label_->SetEnabledColor(SK_ColorRED);
  // account_prompt_label_->SetVisible(false);
  // layout->AddView(account_prompt_label_);

  layout->AddPaddingRow(0, kRowPaddingSmall);

  // password input
  layout->StartRowWithPadding(1, 0, 0, kRowPaddingSmall);
  password_text_ = new views::Textfield;
  password_text_->set_placeholder_text(l10n_util::GetStringUTF16(IDS_YSP_LOGIN_VIEW_PASSWORD_PLACEHOLDER));
  password_text_->set_controller(this);
  password_text_->SetTextInputType(ui::TEXT_INPUT_TYPE_PASSWORD);
  layout->AddView(password_text_);

  // password prompt
  layout->StartRow(1, 0);
  password_prompt_label_ = new views::Label(l10n_util::GetStringUTF16(IDS_YSP_LOGIN_VIEW_PASSWORD_PROMPT));
  password_prompt_label_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  password_prompt_label_->SetEnabledColor(SK_ColorRED);
  password_prompt_label_->SetVisible(false);
  layout->AddView(password_prompt_label_);

  // auto login
  layout->StartRowWithPadding(1, 0, 0, kRowPaddingSmall);
  auto_login_checkbox_ = new views::Checkbox(l10n_util::GetStringUTF16(IDS_YSP_LOGIN_VIEW_AUTO_LOGIN));
  auto_login_checkbox_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  auto_login_checkbox_->set_listener(this);
  layout->AddView(auto_login_checkbox_);
  PrefService* prefs = g_browser_process->local_state();
  //prefs->SetBoolean(prefs::kYSPAutoLogin, true);
  bool auto_login = prefs->GetBoolean(prefs::kYSPAutoLogin);
  auto_login_checkbox_->SetChecked(auto_login);
  YSPLoginManager::GetInstance()->set_auto_login(auto_login);

  // default values
  if(true) {
    base::string16 last_cid = base::UTF8ToUTF16(YSPLoginManager::GetInstance()->GetLastCID());
    if(!last_cid.empty())
      domain_text_->SetText(last_cid);
    base::string16 last_uid = base::UTF8ToUTF16(YSPLoginManager::GetInstance()->GetLastUID());
    if(!last_uid.empty())
      account_text_->SetText(last_uid);
  }

  // login button
  layout->StartRowWithPadding(1, 0, 0, kRowPaddingSmall);
  login_button_ = new views::BlueButton(this, l10n_util::GetStringUTF16(IDS_YSP_LOGIN_VIEW_LOGIN));
  login_button_->SetEnabled(false);
  layout->AddView(login_button_);

  // set server host button
  layout->StartRowWithPadding(1, 0, 0, kRowPaddingSmall);
  server_setting_button_ = new views::LabelButton(this, l10n_util::GetStringUTF16(IDS_YSP_LOGIN_VIEW_SET_SERVER));
  server_setting_button_->SetHorizontalAlignment(gfx::ALIGN_CENTER);
  layout->AddView(server_setting_button_);

  layout->AddPaddingRow(0, kRowPaddingMiddle);

  return view;
}

views::View* YSPLoginView::CreateLogoutSubview(views::View** focus_view, int* prefer_width) {
  views::View* view = new views::View();
  views::GridLayout* layout = CreatePaddingSingleColumnLayout(
    view, kFixedLogoutViewWidth, kPaddingMargin);
  *prefer_width = kFixedLogoutViewWidth + 2 * kPaddingMargin;

  ui::ResourceBundle& bundle = ui::ResourceBundle::GetSharedInstance();
  const gfx::FontList& large_font_list =
    bundle.GetFontList(ui::ResourceBundle::LargeFont);

  // avatar image
  layout->StartRowWithPadding(1, 0, 0, kRowPaddingLarge);
  avatar_image_ = new views::ImageView;
  avatar_image_->SetImage(*bundle.GetImageSkiaNamed(IDR_YSP_LOGIN_AVATAR));
  avatar_image_->SetImageSize(gfx::Size(AvatarImageSize, AvatarImageSize));
  avatar_image_->SetDrawCircle(true);
  layout->AddView(avatar_image_);

  // name
  layout->StartRowWithPadding(1, 0, 0, kRowPaddingMiddle);
  name_label_ = new views::Label(user_name_);
  name_label_->SetFontList(large_font_list);
  layout->AddView(name_label_);

  // logout button
  layout->StartRowWithPadding(1, 0, 0, kRowPaddingLarge);
  logout_button_ = new views::BlueButton(this, l10n_util::GetStringUTF16(IDS_YSP_LOGOUT_VIEW_LOGOUT));
  layout->AddView(logout_button_);

  layout->AddPaddingRow(0, kRowPaddingLarge);

  return view;
}

views::View* YSPLoginView::CreateSettingSubview(views::View** focus_view, int* prefer_width) {
  views::View* view = new views::View();
  views::GridLayout* layout = CreatePaddingSingleColumnLayout(
    view, kFixedLoginViewWidth, kPaddingMargin);
  *prefer_width = kFixedLoginViewWidth + 2 * kPaddingMargin;

  views::ColumnSet* title_columns = layout->AddColumnSet(1);
  title_columns->AddPaddingColumn(0, kPaddingMargin);
  title_columns->AddColumn(views::GridLayout::LEADING,
    views::GridLayout::CENTER, 0, views::GridLayout::FIXED, kHeaderNavItemWidth, 0);
  title_columns->AddColumn(views::GridLayout::FILL,
    views::GridLayout::CENTER, 0, views::GridLayout::FIXED,
    kFixedLoginViewWidth - 2*kHeaderNavItemWidth, 0);

  ui::ResourceBundle& bundle = ui::ResourceBundle::GetSharedInstance();
  const gfx::FontList& large_font_list =
    bundle.GetFontList(ui::ResourceBundle::LargeFont);

  // back button
  layout->StartRowWithPadding(1, 1, 0, kRowPaddingMiddle);
  setting_back_button_ = new views::LabelButton(this, l10n_util::GetStringUTF16(IDS_YSP_LOGIN_SETTING_VIEW_BACK));
  setting_back_button_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  layout->AddView(setting_back_button_);

  // title
  views::Label* title_label = new views::Label(l10n_util::GetStringUTF16(IDS_YSP_LOGIN_SETTING_VIEW_TITLE));
  title_label->SetFontList(large_font_list);
  layout->AddView(title_label);

  // clear host
  layout->StartRowWithPadding(1, 0, 0, kRowPaddingMiddle);
  setting_clear_button_ = new views::LabelButton(this, l10n_util::GetStringUTF16(IDS_YSP_LOGIN_SETTING_VIEW_CLEAR));
  setting_clear_button_->SetHorizontalAlignment(gfx::ALIGN_RIGHT);
  setting_clear_button_->SetEnabledTextColors(SK_ColorBLUE);
  layout->AddView(setting_clear_button_);

  // host input
  layout->StartRowWithPadding(1, 0, 0, kRowPaddingSmall);
  host_text_ = new views::Textfield;
  host_text_->set_placeholder_text(l10n_util::GetStringUTF16(IDS_YSP_LOGIN_SETTING_VIEW_HOST_PLACEHOLDER));
  host_text_->set_controller(this);
  layout->AddView(host_text_);
  *focus_view = host_text_;
  std::string prefServer = GetManageServer();
  host_text_->SetText(base::UTF8ToUTF16(prefServer));

  // ok button
  layout->StartRowWithPadding(1, 0, 0, kRowPaddingMiddle);
  setting_ok_button_ = new views::BlueButton(this, l10n_util::GetStringUTF16(IDS_YSP_LOGIN_SETTING_VIEW_OK));
  setting_ok_button_->SetEnabled(false);
  layout->AddView(setting_ok_button_);

  layout->AddPaddingRow(0, kRowPaddingLarge);

  return view;
}

views::View* YSPLoginView::CreateLoadingSubview(
  views::View** focus_view, int* prefer_width) {
  views::View* view = new views::View();
  views::GridLayout* layout = CreatePaddingSingleColumnLayout(
    view, kFixedLogoutViewWidth, kPaddingMargin);
  *prefer_width = kFixedLogoutViewWidth + 2 * kPaddingMargin;

  ui::ResourceBundle& bundle = ui::ResourceBundle::GetSharedInstance();
  const gfx::FontList& large_font_list =
    bundle.GetFontList(ui::ResourceBundle::LargeFont);

  //layout->AddPaddingRow(0, kRowPaddingLarge);

  // header image
  layout->StartRowWithPadding(1, 0, 0, kRowPaddingLarge);
  views::ImageView* branding_image = new views::ImageView;
  branding_image->SetImage(*bundle.GetImageSkiaNamed(IDR_YSP_LOGIN_HEADER));
  layout->AddView(branding_image);

  // title
  layout->StartRowWithPadding(1, 0, 0, kRowPaddingMiddle);
  views::Label* title_label = new views::Label(l10n_util::GetStringUTF16(IDS_TAB_LOADING_TITLE));
  title_label->SetFontList(large_font_list);
  layout->AddView(title_label);

  layout->AddPaddingRow(0, kRowPaddingLarge);

  return view;
}

// views::ButtonListener implementation.
void YSPLoginView::ButtonPressed(views::Button* sender, const ui::Event& event) {
  if (sender == server_setting_button_) {
    view_state_ = VS_SETTING;
    ShowView();
  }
  else if (sender == login_button_) {
    password_prompt_label_->SetVisible(false);
    const std::string cid = base::UTF16ToUTF8(domain_text_->text());
    const std::string userName = base::UTF16ToUTF8(account_text_->text());
    const std::string password = base::UTF16ToUTF8(password_text_->text());
    YSPLoginManager::GetInstance()->StartLogin(cid, userName, password);
    view_state_ = VS_LOADING;
    ShowView();
  }
  else if (sender == logout_button_) {
    YSPLoginManager::GetInstance()->Logout();
    YSPLoginView::Hide();
  }
  else if (sender == setting_ok_button_) {
    std::string url_str = base::UTF16ToUTF8(host_text_->text());
    GURL url(url_str);
    if(url.is_valid() || url_str.empty()) {
      SetManageServer(url_str);
    }
    else {
      ShowFailureWithMessage(base::UTF8ToUTF16("Invalid server address."));
    }
    view_state_ = VS_LOGIN;
    ShowView();
  }
  else if (sender == setting_back_button_) {
    view_state_ = VS_LOGIN;
    ShowView();
  }
  else if (sender == setting_clear_button_) {
    host_text_->SetText(base::string16());
    UpdateControlStateOfSettingView();
  }
  else if (sender == auto_login_checkbox_) {
    PrefService* prefs = g_browser_process->local_state();
    bool auto_login = auto_login_checkbox_->checked();
    prefs->SetBoolean(prefs::kYSPAutoLogin, auto_login);
    YSPLoginManager::GetInstance()->set_auto_login(auto_login);
  }
}

// views::TextfieldController implementation.
void YSPLoginView::ContentsChanged(views::Textfield* sender, const base::string16& new_contents) {
  UpdateControlState();
}

void YSPLoginView::UpdateControlState() {
  if (view_state_ == VS_LOGIN)
    UpdateControlStateOfLoginView();
  else if (view_state_ == VS_SETTING)
    UpdateControlStateOfSettingView();
}

void YSPLoginView::UpdateControlStateOfLoginView() {
  bool enable = domain_text_->text().length() > 0 &&
    account_text_->text().length() > 0 &&
    password_text_->text().length() > 0;
  login_button_->SetEnabled(enable);
  login_button_->SetIsDefault(enable);
}

void YSPLoginView::UpdateControlStateOfSettingView() {
  std::string prefServer = GetManageServer();
  std::string inputServer = base::UTF16ToUTF8(host_text_->text());
  setting_ok_button_->SetEnabled(prefServer.compare(inputServer) != 0);
}

void YSPLoginView::ShowFailureWithMessage(int message_id) {
  base::string16 msg = l10n_util::GetStringUTF16(message_id);
  ShowFailureWithMessage(msg);
}

void YSPLoginView::ShowFailureWithMessage(base::string16 message) {
  view_state_ = VS_LOGIN;
  ShowView();
  if (password_prompt_label_) {
    password_prompt_label_->SetText(message);
    password_prompt_label_->SetVisible(true);
  }
}

// YSPLoginManagerObserver:
void YSPLoginView::OnLoginRequestFailure(const std::string& error) {
  ShowFailureWithMessage(IDS_YSP_LOGIN_ERROR_REQUEST_FAILED);
}

void YSPLoginView::OnLoginResponseParseFailure(const std::string& error) {
  ShowFailureWithMessage(IDS_YSP_LOGIN_ERROR_RESPONSE_PARSE_FAILED);
}

void YSPLoginView::OnLoginFailure(base::string16 message) {
  ShowFailureWithMessage(message);
}

void YSPLoginView::OnLoginSuccess(const base::string16& name,
  const std::string& head_image_url) {
  user_name_ = name;
  view_state_ = VS_LOGOUT;
  ShowView();
  if(head_image_)
    avatar_image_->SetImage(*head_image_);
  else {
	avatar_image_->SetImage(ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(IDR_YSP_LOGIN_AVATAR));
	avatar_image_->SetImageSize(gfx::Size(AvatarImageSize, AvatarImageSize));
  }
}

void YSPLoginView::OnLogout() {
  head_image_ = nullptr;
  view_state_ = VS_LOGIN;

  ShowView();
}

void YSPLoginView::SetManageServer(std::string server) {
  PrefService* prefs = g_browser_process->local_state();
  prefs->SetString(prefs::kYSPUserSetServerAddress, server);
}

std::string YSPLoginView::GetManageServer() {
  PrefService* prefs = g_browser_process->local_state();
  return prefs->GetString(prefs::kYSPUserSetServerAddress);
}
#endif
