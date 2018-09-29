#ifdef REDCORE
#ifdef IE_REDCORE
#include "ysp_ie_login_view.h"
#include "base/strings/string16.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/controls/image_view.h"
#include "ui/views/controls/label.h"
#include "ui/views/widget/widget.h"
#include "ui/views/background.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/button/label_button.h"
#include "ui/views/controls/scroll_view.h"
#include "ui/aura/window.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/views/bubble/bubble_border.h"
#include "ui/gfx/color_palette.h"
#include "ui/views/painter.h"
#include "ui/views/style/platform_style.h"
#include "ui/gfx/color_utils.h"
#include "ui/base/material_design/material_design_controller.h"
#include "ui/base/l10n/l10n_util.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/grit/theme_resources.h"
#include "components/constrained_window/constrained_window_views.h"
#include "third_party/skia/include/core/SkColor.h"

#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_window.h"

const int ButtonHeight = 50;
const int PopupDlgWidth = 320;

// The default spacing between the icon and text.
// comment unused code by webb.
// const int kSpacing = 5;

#if !(defined(OS_LINUX) && !defined(OS_CHROMEOS))
// Default text and shadow colors for STYLE_BUTTON.
const SkColor kStyleButtonTextColor = SK_ColorBLACK;
const SkColor kStyleButtonShadowColor = SK_ColorWHITE;
#endif

const gfx::FontList& GetDefaultNormalFontList() {
	static base::LazyInstance<gfx::FontList>::Leaky font_list =
		LAZY_INSTANCE_INITIALIZER;
	return font_list.Get();
}

const gfx::FontList& GetDefaultBoldFontList() {
	static base::LazyInstance<gfx::FontList>::Leaky font_list =
		LAZY_INSTANCE_INITIALIZER;
    // comment just for compiling
	// if ((font_list.Get().GetFontStyle() & gfx::Font::Weight::BOLD) == 0) 
	{
		// font_list.Get() = font_list.Get().
	    // DeriveWithStyle(font_list.Get().GetFontStyle() | gfx::Font::Weight::BOLD);
		// DCHECK_NE(font_list.Get().GetFontStyle() & gfx::Font::Weight::BOLD, 0);
	}
	return font_list.Get();
}

 LoginBtnInfo::LoginBtnInfo(){}
 LoginBtnInfo::~LoginBtnInfo(){}

	// static
	const int LoginInfoButton::kHoverAnimationDurationMs = 170;
	const int LoginInfoButton::kFocusRectInset = 3;
	const char LoginInfoButton::kViewClassName[] = "LoginInfoButton";

	LoginInfoButton::LoginInfoButton(views::ButtonListener* listener, 
		const LoginBtnInfo& btnInfo)
		: views::Button(listener),
		label_(new views::Label()),
		label_desc(new views::Label()),
		cached_normal_font_list_(GetDefaultNormalFontList()),
		cached_bold_font_list_(GetDefaultBoldFontList()),
		button_state_images_(),
		button_state_colors_(),
		explicitly_set_colors_(),
		is_default_(false),
		style_(views::Button::STYLE_TEXTBUTTON),
		border_is_themed_border_(true),
		horizontal_alignment_(gfx::ALIGN_LEFT) {
		SetAnimationDuration(kHoverAnimationDurationMs);
		SetBtnInfo(btnInfo);

		AddChildView(label_);
		label_->SetFontList(cached_bold_font_list_);
		label_->SetAutoColorReadabilityEnabled(false);
		label_->SetHorizontalAlignment(gfx::ALIGN_LEFT);

		label_desc->SetHorizontalAlignment(gfx::ALIGN_LEFT);
		label_desc->SetFontList(cached_normal_font_list_);
		AddChildView(label_desc);

		// Inset the button focus rect from the actual border; roughly match Windows.
		SetFocusPainter(views::Painter::CreateDashedFocusPainterWithInsets(gfx::Insets(
			kFocusRectInset, kFocusRectInset, kFocusRectInset, kFocusRectInset)));
	}

	LoginInfoButton::~LoginInfoButton() {}

	const LoginBtnInfo& LoginInfoButton::GetBtnInfo() const {
		return btn_info_;
	}

	void LoginInfoButton::SetBtnInfo(const LoginBtnInfo& info) {
		btn_info_ = info;
		SetAccessibleName(info.appName);
		label_->SetText(info.appName);
		label_desc->SetText(info.appUrl);
	}

	void LoginInfoButton::SetTextColor(ButtonState for_state, SkColor color) {
		button_state_colors_[for_state] = color;
		// if (for_state == STATE_DISABLED)
		// 	label_->SetDisabledColor(color);
		// else if (for_state == state())
		// 	label_->SetEnabledColor(color);
		explicitly_set_colors_[for_state] = true;
	}

	void LoginInfoButton::SetEnabledTextColors(SkColor color) {
		ButtonState states[] = { STATE_NORMAL, STATE_HOVERED, STATE_PRESSED };
		for (auto state : states)
			SetTextColor(state, color);
	}

	void LoginInfoButton::SetTextShadows(const gfx::ShadowValues& shadows) {
		label_->SetShadows(shadows);
	}

	void LoginInfoButton::SetTextSubpixelRenderingEnabled(bool enabled) {
		label_->SetSubpixelRenderingEnabled(enabled);
	}

	const gfx::FontList& LoginInfoButton::GetFontList() const {
		return label_->font_list();
	}

	void LoginInfoButton::SetFontList(const gfx::FontList& font_list) {
		cached_normal_font_list_ = font_list;
		// cached_bold_font_list_ = font_list.DeriveWithStyle(
		// font_list.GetFontStyle() | gfx::Font::Weight::BOLD);

		// STYLE_BUTTON uses bold text to indicate default buttons.
		label_->SetFontList(
			style_ == STYLE_BUTTON && is_default_ ?
			cached_bold_font_list_ : cached_normal_font_list_);
	}

	void LoginInfoButton::SetElideBehavior(gfx::ElideBehavior elide_behavior) {
		label_->SetElideBehavior(elide_behavior);
	}

	void LoginInfoButton::SetHorizontalAlignment(gfx::HorizontalAlignment alignment) {
		DCHECK_NE(gfx::ALIGN_TO_HEAD, alignment);
		horizontal_alignment_ = alignment;
		InvalidateLayout();
	}

	void LoginInfoButton::SetMinSize(const gfx::Size& min_size) {
		min_size_ = min_size;
		ResetCachedPreferredSize();
	}

	void LoginInfoButton::SetMaxSize(const gfx::Size& max_size) {
		max_size_ = max_size;
		ResetCachedPreferredSize();
	}

	void LoginInfoButton::SetIsDefault(bool is_default) {
		if (is_default == is_default_)
			return;
		is_default_ = is_default;
		ui::Accelerator accel(ui::VKEY_RETURN, ui::EF_NONE);
		is_default_ ? AddAccelerator(accel) : RemoveAccelerator(accel);

		// STYLE_BUTTON uses bold text to indicate default buttons.
		if (style_ == STYLE_BUTTON) {
			label_->SetFontList(
				is_default ? cached_bold_font_list_ : cached_normal_font_list_);
			// Usually this function is called before |this| is attached to a widget,
			// but in the cases where |this| is already shown, we need to re-layout
			// because font boldness affects the label's size.
			if (GetWidget())
				Layout();
		}
	}

	void LoginInfoButton::SetStyle(ButtonStyle style) {
		// All callers currently pass STYLE_BUTTON, and should only call this once, to
		// change from the default style.
		DCHECK_EQ(style, STYLE_BUTTON);
		DCHECK_EQ(style_, STYLE_TEXTBUTTON);
		DCHECK(!GetWidget()) << "Can't change button style after adding to a Widget.";

		style_ = style;

		SetFocusPainter(nullptr);
		SetHorizontalAlignment(gfx::ALIGN_LEFT);
		// comment just for compiling
		// SetFocusable(true);
		SetMinSize(gfx::Size(70, 33));

		// Themed borders will be set once the button is added to a Widget, since that
		// provides the value of GetNativeTheme().
	}

	void LoginInfoButton::SetFocusPainter(std::unique_ptr<views::Painter> focus_painter) {
		focus_painter_ = std::move(focus_painter);
	}

	gfx::Size LoginInfoButton::GetPreferredSize() const {
		if (cached_preferred_size_valid_)
			return cached_preferred_size_;

		// Use a temporary label copy for sizing to avoid calculation side-effects.
		views::Label label(btn_info_.appName);
		label.SetShadows(label_->shadows());

		if (style() == STYLE_BUTTON) {
			// Some text appears wider when rendered normally than when rendered bold.
			// Accommodate the widest, as buttons may show bold and shouldn't resize.
			const int current_width = label.GetPreferredSize().width();
			label.SetFontList(cached_bold_font_list_);
			if (label.GetPreferredSize().width() < current_width)
				label.SetFontList(cached_normal_font_list_);
		}

		gfx::Size size;
		// Make the size at least as large as the minimum size needed by the border.
		size.SetToMax(border() ? border()->GetMinimumSize() : gfx::Size());

		// Increase the minimum size monotonically with the preferred size.
		size.SetToMax(min_size_);
		min_size_ = size;

		// Return the largest known size clamped to the maximum size (if valid).
		if (max_size_.width() > 0)
			size.set_width(std::min(max_size_.width(), size.width()));
		if (max_size_.height() > 0)
			size.set_height(std::min(max_size_.height(), size.height()));

		// Cache this computed size, as recomputing it is an expensive operation.
		cached_preferred_size_valid_ = true;
		cached_preferred_size_ = size;
		return cached_preferred_size_;
	}

	int LoginInfoButton::GetHeightForWidth(int w) const {

		int height = std::max(0, label_->GetHeightForWidth(w));
		if (border())
			height = std::max(height, border()->GetMinimumSize().height());

		height = std::max(height, min_size_.height());
		if (max_size_.height() > 0)
			height = std::min(height, max_size_.height());
		return height;
	}

	void LoginInfoButton::Layout() {
		gfx::Rect child_area(GetChildAreaBounds());
		gfx::Rect label_area(child_area);
		label_area.set_y(5);
		label_area.set_height(child_area.height() / 2);
		label_area.set_x(child_area.x() + 10);
		label_area.set_width(child_area.width() - 20);
		label_->SetBoundsRect(label_area);

		gfx::Rect label_desc_area(child_area);
		label_desc_area.set_height(child_area.height() / 2);
		label_desc_area.set_y(child_area.height() / 2);
		label_desc_area.set_x(child_area.x() + 10);
		label_desc_area.set_width(child_area.width() - 20);
		label_desc->SetBoundsRect(label_desc_area);
		Button::Layout();
	}

	const char* LoginInfoButton::GetClassName() const {
		return kViewClassName;
	}

	void LoginInfoButton::EnableCanvasFlippingForRTLUI(bool flip) {
		Button::EnableCanvasFlippingForRTLUI(flip);
	}

	std::unique_ptr<views::LabelButtonBorder> LoginInfoButton::CreateDefaultBorder() const {
		if (style() != views::Button::STYLE_TEXTBUTTON) {
			return std::unique_ptr<views::LabelButtonBorder>(new views::LabelButtonAssetBorder(style()));
		}

		std::unique_ptr<views::LabelButtonBorder> border(new views::LabelButtonBorder());
		border->set_insets(views::LabelButtonAssetBorder::GetDefaultInsetsForStyle(
			Button::STYLE_TEXTBUTTON));
		return border;
		//return views::PlatformStyle::CreateLabelButtonBorder(style());
	}

	void LoginInfoButton::SetBorder(std::unique_ptr<views::Border> border) {
		border_is_themed_border_ = false;
		View::SetBorder(std::move(border));
		ResetCachedPreferredSize();
	}

	gfx::Rect LoginInfoButton::GetChildAreaBounds() {
		return GetLocalBounds();
	}

	void LoginInfoButton::IE_Paint(gfx::Canvas* canvas) {
		View::OnPaint(canvas);
		views::Painter::PaintFocusPainter(this, canvas, focus_painter_.get());
	}

	void LoginInfoButton::OnFocus() {
		View::OnFocus();
		// Typically the border renders differently when focused.
		SchedulePaint();
	}

	void LoginInfoButton::OnBlur() {
		View::OnBlur();
		// Typically the border renders differently when focused.
		SchedulePaint();
	}

	void LoginInfoButton::OnNativeThemeChanged(const ui::NativeTheme* theme) {
		ResetColorsFromNativeTheme();
		UpdateThemedBorder();
		// Invalidate the layout to pickup the new insets from the border.
		InvalidateLayout();
	}

	void LoginInfoButton::StateChanged(ButtonState old_state) {
		const SkColor color = button_state_colors_[state()];
		if (state() != STATE_DISABLED && label_->enabled_color() != color)
			label_->SetEnabledColor(color);
		label_->SetEnabled(state() != STATE_DISABLED);
	}

	void LoginInfoButton::GetExtraParams(ui::NativeTheme::ExtraParams* params) const {
		params->button.checked = false;
		params->button.indeterminate = false;
		params->button.is_default = is_default_;
		params->button.is_focused = HasFocus() && IsAccessibilityFocusable();
		params->button.has_border = false;
		params->button.classic_state = 0;
		params->button.background_color = label_->background_color();
	}

	void LoginInfoButton::ResetColorsFromNativeTheme() {
		const ui::NativeTheme* theme = GetNativeTheme();
		SkColor colors[STATE_COUNT] = {
			theme->GetSystemColor(ui::NativeTheme::kColorId_ButtonEnabledColor),
			theme->GetSystemColor(ui::NativeTheme::kColorId_ButtonHoverColor),
			theme->GetSystemColor(ui::NativeTheme::kColorId_ButtonHoverColor),
			theme->GetSystemColor(ui::NativeTheme::kColorId_ButtonDisabledColor),
		};

		// Certain styles do not change text color when hovered or pressed.
		bool constant_text_color = false;
		// Use hardcoded colors for inverted color scheme support and STYLE_BUTTON.
		if (color_utils::IsInvertedColorScheme()) {
			constant_text_color = true;
			colors[STATE_NORMAL] = SK_ColorWHITE;
			label_->SetBackgroundColor(SK_ColorBLACK);
			label_->SetBackground(views::CreateSolidBackground(SK_ColorBLACK));
			label_->SetAutoColorReadabilityEnabled(true);
			label_->SetShadows(gfx::ShadowValues());
		}
		else if (style() == STYLE_BUTTON) {
			// TODO(erg): This is disabled on desktop linux because of the binary asset
			// confusion. These details should either be pushed into ui::NativeThemeWin
			// or should be obsoleted by rendering buttons with paint calls instead of
			// with static assets. http://crbug.com/350498
#if !(defined(OS_LINUX) && !defined(OS_CHROMEOS))
			constant_text_color = true;
			colors[STATE_NORMAL] = kStyleButtonTextColor;
			label_->SetAutoColorReadabilityEnabled(false);
			label_->SetShadows(gfx::ShadowValues(
				1, gfx::ShadowValue(gfx::Vector2d(0, 1), 0, kStyleButtonShadowColor)));
#endif
			label_->SetBackground(NULL);
		}
		else {
			label_->SetBackground(NULL);
		}

		if (constant_text_color)
			colors[STATE_HOVERED] = colors[STATE_PRESSED] = colors[STATE_NORMAL];

		for (size_t state = STATE_NORMAL; state < STATE_COUNT; ++state) {
			if (!explicitly_set_colors_[state]) {
				SetTextColor(static_cast<ButtonState>(state), colors[state]);
				explicitly_set_colors_[state] = false;
			}
		}
	}

	void LoginInfoButton::UpdateThemedBorder() {
		// Don't override borders set by others.
		if (!border_is_themed_border_)
			return;

		SetBorder(CreateDefaultBorder());
		border_is_themed_border_ = true;
	}

	void LoginInfoButton::ChildPreferredSizeChanged(View* child) {
		ResetCachedPreferredSize();
		PreferredSizeChanged();
	}

	ui::NativeTheme::Part LoginInfoButton::GetThemePart() const {
		return ui::NativeTheme::kPushButton;
	}

	gfx::Rect LoginInfoButton::GetThemePaintRect() const {
		return GetLocalBounds();
	}

	ui::NativeTheme::State LoginInfoButton::GetThemeState(
		ui::NativeTheme::ExtraParams* params) const {
		GetExtraParams(params);
		switch (state()) {
		case STATE_NORMAL:   return ui::NativeTheme::kNormal;
		case STATE_HOVERED:  return ui::NativeTheme::kHovered;
		case STATE_PRESSED:  return ui::NativeTheme::kPressed;
		case STATE_DISABLED: return ui::NativeTheme::kDisabled;
		case STATE_COUNT:    NOTREACHED() << "Unknown state: " << state();
		}
		return ui::NativeTheme::kNormal;
	}

	const gfx::Animation* LoginInfoButton::GetThemeAnimation() const {
		return &hover_animation();
	}

	ui::NativeTheme::State LoginInfoButton::GetBackgroundThemeState(
		ui::NativeTheme::ExtraParams* params) const {
		GetExtraParams(params);
		return ui::NativeTheme::kNormal;
	}

	ui::NativeTheme::State LoginInfoButton::GetForegroundThemeState(
		ui::NativeTheme::ExtraParams* params) const {
		GetExtraParams(params);
		return ui::NativeTheme::kHovered;
	}

	void LoginInfoButton::ResetCachedPreferredSize() {
		cached_preferred_size_valid_ = false;
		cached_preferred_size_ = gfx::Size();
	}



YspIEloginView::YspIEloginView(YspPopupView*dlg, const std::vector<LoginBtnInfo> infoVec)
	:titleDescpLabel(NULL),
	popUpDlg(dlg),
	btnInfoVec(infoVec)
{
	titleDescpLabel = new views::Label;
	titleDescpLabel->SetText(l10n_util::GetStringUTF16(IDS_YSP_IE_WINDOWS_AD_VIEW_DESC));

	this->AddChildView(titleDescpLabel);

	IELoginBtnView* btnView = new IELoginBtnView(this, infoVec);
	btnView->SetSize(gfx::Size(270, ButtonHeight * infoVec.size()));
	scrollView = new views::ScrollView;

	scrollView->SetContents(btnView);
	scrollView->SetBorder(views::CreateSolidBorder(1, SkColorSetRGB(0xC8, 0xC8, 0xC8)));
	AddChildView(scrollView);
}

YspIEloginView::~YspIEloginView()
{
	
}

void YspIEloginView::ButtonPressed(views::Button * sender, const ui::Event & event)
{
	LoginInfoButton* button = static_cast<LoginInfoButton*>(sender);
	if (button && popUpDlg)
		popUpDlg->SetResultInfo(button->GetBtnInfo());
}

void YspIEloginView::Layout()
{
	gfx::Rect child_area(GetLocalBounds());
	if (child_area.height() == 0 || child_area.width() == 0)
		return;

	gfx::Rect titleDescpArea = child_area;
	titleDescpArea.set_height(60);
	titleDescpLabel->SetBoundsRect(titleDescpArea);

	gfx::Rect scrollArea(child_area);
	scrollArea.set_y(60);
	scrollArea.set_x(10);
	scrollArea.set_width(child_area.width() - 20);
	if(btnInfoVec.size()>3)
		scrollArea.set_height((ButtonHeight) * 3+2);
	else
		scrollArea.set_height((ButtonHeight) * btnInfoVec.size() + 2);
	scrollView->SetBoundsRect(scrollArea);

	views::View* contView = scrollView->contents();
	int scrollBarWidth = 0;
	// comment just for compiling
	// if(btnInfoVec.size()>3)
	// 	scrollBarWidth = scrollView->GetScrollBarWidth();
	contView->SetSize(gfx::Size(scrollArea.width() - scrollBarWidth-2, ButtonHeight * btnInfoVec.size()));
}



YspPopupView::YspPopupView(Browser* browser, std::vector<LoginBtnInfo> btnInfoVec)
	:pBrowser(browser),
	loginView(NULL),
	dlg(NULL)
{
	loginView = new YspIEloginView(this, btnInfoVec);
	dlg=constrained_window::CreateBrowserModalDialogViews(this, pBrowser->window()->GetNativeWindow());
	gfx::Size size;
	size.set_width(PopupDlgWidth);
	if (btnInfoVec.size() <= 3)
		size.set_height(130 + (ButtonHeight)*btnInfoVec.size());
	else
		size.set_height(130 + (ButtonHeight) * 3);
	dlg->SetSize(size);
}

YspPopupView::~YspPopupView()
{
}

int YspPopupView::GetDialogButtons() const
{
	return ui::DIALOG_BUTTON_NONE;
}

views::View * YspPopupView::GetContentsView()
{
	return loginView;
}

views::Widget * YspPopupView::GetWidget()
{
	return loginView->GetWidget();
}

const views::Widget * YspPopupView::GetWidget() const
{
	return  loginView->GetWidget();
}

ui::ModalType YspPopupView::GetModalType() const
{
	return ui::MODAL_TYPE_WINDOW;
}

base::string16 YspPopupView::GetWindowTitle() const
{
	return l10n_util::GetStringUTF16(IDS_YSP_IE_WINDOWS_AD_VIEW_TITLE);
}

bool YspPopupView::ShouldShowWindowTitle() const
{
	return true;
}

bool YspPopupView::Close()
{
	// base::MessageLoop::current()->QuitWhenIdle();
	return true;
}

void YspPopupView::SetResultInfo(const LoginBtnInfo & info)
{
	infoTemp = info;
	dlg->Close();
}

LoginBtnInfo YspPopupView::DoModel()
{
	infoTemp = LoginBtnInfo();
	dlg->Show();
	// base::MessageLoop::current()->Run();
	return infoTemp;
}

IELoginBtnView::IELoginBtnView(YspIEloginView * ieLoginView, const std::vector<LoginBtnInfo> infoVec)
{
	std::vector<LoginBtnInfo>::const_iterator iter = infoVec.begin();
	for (; iter != infoVec.end(); iter++)
	{
		LoginInfoButton* pBtn = new LoginInfoButton(ieLoginView, *iter);
		AddChildView(pBtn);
		buttonVec.push_back(pBtn);
	}
}

IELoginBtnView::~IELoginBtnView()
{
	buttonVec.clear();
}

void IELoginBtnView::Layout()
{
	gfx::Rect child_area(GetLocalBounds());
	if (child_area.height() == 0 || child_area.width() == 0)
		return;

	gfx::Rect buttonArea(child_area);
	buttonArea.set_width(child_area.width());
	buttonArea.set_height(ButtonHeight);

	std::vector<LoginInfoButton*>::const_iterator iter = buttonVec.begin();
	for (; iter != buttonVec.end(); iter++)
	{
		(*iter)->SetBoundsRect(buttonArea);
		buttonArea.set_y(buttonArea.y() + ButtonHeight);
	}
}

#endif // IE_REDCORE
#endif //REDCORE