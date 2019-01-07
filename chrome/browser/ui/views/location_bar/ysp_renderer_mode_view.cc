// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/location_bar/ysp_renderer_mode_view.h"

#include "base/metrics/histogram.h"
#include "base/strings/utf_string_conversions.h"
#include "base/win/win_util.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/bookmarks/bookmark_stats.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/view_ids.h"
#include "chrome/browser/ui/views/page_action/page_action_icon_view.h"
#include "chrome/browser/ui/views/ysp_renderer_mode_bubble_view.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/grit/theme_resources.h"
#include "components/toolbar/vector_icons.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/material_design/material_design_controller.h"
#include "ui/base/resource/resource_bundle.h"

YspRendererModeView::YspRendererModeView(CommandUpdater* command_updater,
                                         Browser* browser,
                                         Delegate* delegate)
    : PageActionIconView(command_updater, IDC_RENDERER_MODE_SWITCH, delegate),
      browser_(browser),
      render_mode_(ie::RenderMode()) {}

YspRendererModeView::~YspRendererModeView() {}

bool YspRendererModeView::Update() {
  return true;
}
// just for compiling
base::string16 YspRendererModeView::GetTextForTooltipAndAccessibleName() const {
  return tooltip_text_;
}

void YspRendererModeView::SetToggled(ie::RenderMode mode) {
  if (mode.core == ie::NONE_CORE)
    PageActionIconView::SetActiveInternal(false);
  else
    PageActionIconView::SetActiveInternal(true);
  render_mode_ = mode;
  UpdateImage();
  base::string16 tip_str = L"";
  if (render_mode_.core == ie::BLINK_CORE)
    tip_str = l10n_util::GetStringUTF16(IDS_CHROME_CORE);
  else if (render_mode_.core == ie::IE_CORE) {
    if (base::win::GetSystemIEVersion() < 8)
      tip_str = l10n_util::GetStringUTF16(IDS_IE_CORE);
    else {
      if (render_mode_.emulation == ie::EMULATION7)
        tip_str = l10n_util::GetStringUTF16(IDS_YSP_EMULATION_7) + L" + ";
      else if (render_mode_.emulation == ie::EMULATION8)
        tip_str = l10n_util::GetStringUTF16(IDS_YSP_EMULATION_8) + L" + ";
      else if (render_mode_.emulation == ie::EMULATION9)
        tip_str = l10n_util::GetStringUTF16(IDS_YSP_EMULATION_9) + L" + ";
      else if (render_mode_.emulation == ie::EMULATION10)
        tip_str = l10n_util::GetStringUTF16(IDS_YSP_EMULATION_10) + L" + ";
      else if (render_mode_.emulation == ie::EMULATION11)
        tip_str = l10n_util::GetStringUTF16(IDS_YSP_EMULATION_11) + L" + ";

      if (render_mode_.version == ie::DOC6)
        tip_str = tip_str + l10n_util::GetStringUTF16(IDS_YSP_DOCMODE_6);
      else if (render_mode_.version == ie::DOC7)
        tip_str = tip_str + l10n_util::GetStringUTF16(IDS_YSP_DOCMODE_7);
      else if (render_mode_.version == ie::DOC8)
        tip_str = tip_str + l10n_util::GetStringUTF16(IDS_YSP_DOCMODE_8);
      else if (render_mode_.version == ie::DOC9)
        tip_str = tip_str + l10n_util::GetStringUTF16(IDS_YSP_DOCMODE_9);
      else if (render_mode_.version == ie::DOC10)
        tip_str = tip_str + l10n_util::GetStringUTF16(IDS_YSP_DOCMODE_10);
      else if (render_mode_.version == ie::DOC11)
        tip_str = tip_str + l10n_util::GetStringUTF16(IDS_YSP_DOCMODE_11);
      else if (render_mode_.version == ie::DOCSYS)
        tip_str = tip_str + l10n_util::GetStringUTF16(IDS_YSP_DOCMODE_SYS);
    }
  }
  tooltip_text_ = tip_str;
}

void YspRendererModeView::OnExecuting(
    PageActionIconView::ExecuteSource execute_source) {}

void YspRendererModeView::ExecuteCommand(ExecuteSource source) {
  if (browser_ && render_mode_.core != ie::NONE_CORE) {
    OnExecuting(source);
    chrome::ShowRendererModeSwitchBubble(browser_, render_mode_);
  } else {
    PageActionIconView::ExecuteCommand(source);
  }
}

views::BubbleDialogDelegateView* YspRendererModeView::GetBubble() const {
  return RendererModeBubbleView::GetBubbleView();
}

void YspRendererModeView::OnNativeThemeChanged(
    const ui::NativeTheme* native_theme) {
  UpdateImage();
}

void YspRendererModeView::UpdateImage() {
  if (!active())
    return;

  if (render_mode_.core == ie::BLINK_CORE) {
    SetImage(*ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(
        IDR_YSP_BLINK_CORE_GREY));
  } else {
    SetImage(*ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(
        IDR_YSP_IE_CORE_GREY));
  }
}

const gfx::VectorIcon& YspRendererModeView::GetVectorIcon() const {
  return active() ? toolbar::kStarActiveIcon : toolbar::kStarIcon;
}
