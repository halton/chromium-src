#ifdef IE_REDCORE

#include "chrome/browser/ui/views/location_bar/ysp_renderer_mode_view.h"

#include "base/metrics/histogram.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/bookmarks/bookmark_stats.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/view_ids.h"
#include "chrome/browser/ui/views/ysp_renderer_mode_bubble_view.h"
#include "chrome/browser/ui/views/page_action/page_action_icon_view.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/grit/theme_resources.h"
#include "components/toolbar/vector_icons.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/material_design/material_design_controller.h"
#include "ui/base/resource/resource_bundle.h"
#include "base/win/win_util.h"

RendererModeView::RendererModeView(CommandUpdater * command_updater, Browser * browser, Delegate * delegate)
	: PageActionIconView(command_updater, IDC_RENDERER_MODE_SWITCH, delegate), browser_(browser),
	rendererMode(RendererMode())
{
}

RendererModeView::~RendererModeView()
{
}

bool RendererModeView::Update() {return true;}
// just for compiling
base::string16 RendererModeView::GetTextForTooltipAndAccessibleName() const { return std::wstring();}

void RendererModeView::SetToggled(RendererMode mode)
{
	if(mode.core==NONE_CORE)
		PageActionIconView::SetActiveInternal(false);
	else
		PageActionIconView::SetActiveInternal(true);
	rendererMode = mode;
	// comment UpdateIcon just for compiling
	// UpdateIcon();
	base::string16 tipStr = L"";
	if(rendererMode.core==BLINK_CORE)
		tipStr= l10n_util::GetStringUTF16(IDS_CHROME_CORE);
	else if (rendererMode.core == IE_CORE)
	{
		if(base::win::GetSystemIEVersion() < 8)
			tipStr= l10n_util::GetStringUTF16(IDS_IE_CORE);
		else
		{
			if (rendererMode.emulation == IE::EMULATION7)
				tipStr = l10n_util::GetStringUTF16(IDS_YSP_EMULATION_7) + L" + ";
			else if (rendererMode.emulation == IE::EMULATION8)
				tipStr = l10n_util::GetStringUTF16(IDS_YSP_EMULATION_8) + L" + ";
			else if (rendererMode.emulation == IE::EMULATION9)
				tipStr = l10n_util::GetStringUTF16(IDS_YSP_EMULATION_9) + L" + ";
			else if (rendererMode.emulation == IE::EMULATION10)
				tipStr = l10n_util::GetStringUTF16(IDS_YSP_EMULATION_10) + L" + ";
			else if (rendererMode.emulation == IE::EMULATION11)
				tipStr = l10n_util::GetStringUTF16(IDS_YSP_EMULATION_11) + L" + ";

			if (rendererMode.ver == IE::DOC6)
				tipStr = tipStr + l10n_util::GetStringUTF16(IDS_YSP_DOCMODE_6);
			else if (rendererMode.ver == IE::DOC7)
				tipStr = tipStr + l10n_util::GetStringUTF16(IDS_YSP_DOCMODE_7);
			else if (rendererMode.ver == IE::DOC8)
				tipStr = tipStr + l10n_util::GetStringUTF16(IDS_YSP_DOCMODE_8);
			else if (rendererMode.ver == IE::DOC9)
				tipStr = tipStr + l10n_util::GetStringUTF16(IDS_YSP_DOCMODE_9);
			else if (rendererMode.ver == IE::DOC10)
				tipStr = tipStr + l10n_util::GetStringUTF16(IDS_YSP_DOCMODE_10);
			else if (rendererMode.ver == IE::DOC11)
				tipStr = tipStr + l10n_util::GetStringUTF16(IDS_YSP_DOCMODE_11);
			else if (rendererMode.ver == IE::DOCSYS)
				tipStr = tipStr + l10n_util::GetStringUTF16(IDS_YSP_DOCMODE_SYS);
		}
	}
	//no implements in super class, comment it by webb.
	// SetTooltipText(tipStr);
}

void RendererModeView::OnExecuting(PageActionIconView::ExecuteSource execute_source)
{
}

void RendererModeView::ExecuteCommand(ExecuteSource source)
{
	if (browser_ && rendererMode.core!=NONE_CORE)
	{
		OnExecuting(source);
		chrome::ShowRendererModeSwitchBubble(browser_,rendererMode);
	}
	else
	{
		PageActionIconView::ExecuteCommand(source);
	}
}

views::BubbleDialogDelegateView * RendererModeView::GetBubble() const
{
	return RendererModeBubbleView::GetBubbleView();
}

// comment just for compile
// bool RendererModeView::SetRasterIcon()
// {
// 	if (active())
// 	{
// 		if (rendererMode.core == BLINK_CORE)
// 			SetImage(ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(IDR_YSP_BLINK_CORE_GREY));
// 		else
// 			SetImage(ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(IDR_YSP_IE_CORE_GREY));
// 	}
// 	return true;
// }

// 

const gfx::VectorIcon& RendererModeView::GetVectorIcon() const {
  return active() ? toolbar::kStarActiveIcon : toolbar::kStarIcon;
}

#endif
