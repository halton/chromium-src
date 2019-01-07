// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/ysp_renderer_mode_bubble_view.h"

#include <utility>

#include "base/macros.h"
#include "base/strings/string16.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/win/win_util.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/platform_util.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/browser_dialogs.h"
#include "chrome/browser/ui/sync/sync_promo_ui.h"
#include "chrome/browser/ui/views/sync/bubble_sync_promo_view.h"
#include "chrome/grit/chromium_strings.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/grit/theme_resources.h"
#include "components/strings/grit/components_strings.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "ui/accessibility/ax_enums.mojom.h"
#include "ui/accessibility/ax_node_data.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/events/keycodes/keyboard_codes.h"
#include "ui/gfx/canvas.h"
#include "ui/views/background.h"
#include "ui/views/bubble/bubble_frame_view.h"
#include "ui/views/controls/button/label_button.h"
#include "ui/views/controls/combobox/combobox.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/link.h"
#include "ui/views/controls/menu/menu_config.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/widget/widget.h"

class RendererModeButtonBackGround : public views::Background {
 public:
  explicit RendererModeButtonBackGround() {}

  ~RendererModeButtonBackGround() override {}

  // Overridden from views::Background.
  void Paint(gfx::Canvas* canvas, views::View* view) const override {
    views::Button* button = views::Button::AsButton(view);
    views::Button::ButtonState state =
        button ? button->state() : views::Button::STATE_NORMAL;
    if (state == views::Button::STATE_HOVERED ||
        state == views::Button::STATE_PRESSED) {
      canvas->DrawColor(SkColorSetARGB(255, 230, 230, 230));
    }
  }
};

RendererModeBubbleView* RendererModeBubbleView::renderer_mode_bubble_view_ =
    NULL;

RendererModeBubbleView::RendererModeBubbleView(views::View* anchor_view,
                                               Browser* browser,
                                               const GURL& url,
                                               ie::RenderMode mode)
    : BubbleDialogDelegateView(anchor_view, views::BubbleBorder::TOP_RIGHT),
      blink_button_(NULL),
      ie_button_(NULL),
      browser_(browser),
      render_mode_(mode) {
  gfx::Insets inset;
  inset.Set(0, 0, 0, 0);
  set_margins(inset);
  // Compensate for built-in vertical padding in the anchor view's image.
  set_anchor_view_insets(gfx::Insets(2, 0, 2, 0));
}

void RendererModeBubbleView::ShowBubble(views::View* anchor_view,
                                        const gfx::Rect& anchor_rect,
                                        gfx::NativeView parent_window,
                                        Browser* browser,
                                        const GURL& url,
                                        ie::RenderMode mode) {
  if (renderer_mode_bubble_view_)
    return;

  renderer_mode_bubble_view_ =
      new RendererModeBubbleView(anchor_view, browser, url, mode);
  if (!anchor_view) {
    renderer_mode_bubble_view_->SetAnchorRect(anchor_rect);
    renderer_mode_bubble_view_->set_parent_window(parent_window);
  }
  views::BubbleDialogDelegateView::CreateBubble(
	  renderer_mode_bubble_view_)->Show();
}

void RendererModeBubbleView::Hide() {
  if (renderer_mode_bubble_view_)
    renderer_mode_bubble_view_->GetWidget()->Hide();
}

RendererModeBubbleView::~RendererModeBubbleView() {}

void RendererModeBubbleView::WindowClosing() {
  renderer_mode_bubble_view_ = NULL;
}

void RendererModeBubbleView::Init() {
  blink_button_ =
      new views::LabelButton(this, l10n_util::GetStringUTF16(IDS_CHROME_CORE));
  gfx::ImageSkia* blink_image =
      ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(
          IDR_YSP_BLINK_CORE);
  blink_button_->SetImageLabelSpacing(10);
  blink_button_->SetImage(views::Button::STATE_NORMAL, *blink_image);
  blink_button_->SetBackground(
      std::make_unique<RendererModeButtonBackGround>());
  blink_button_->SetBorder(views::CreateEmptyBorder(4, 15, 4, 15));

  ie_button_ =
      new views::LabelButton(this, l10n_util::GetStringUTF16(IDS_IE_CORE));
  gfx::ImageSkia* ie_image =
      ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(
          IDR_YSP_IE_CORE);
  ie_button_->SetImageLabelSpacing(10);
  ie_button_->SetImage(views::Button::STATE_NORMAL, *ie_image);
  ie_button_->SetBackground(std::make_unique<RendererModeButtonBackGround>());
  ie_button_->SetBorder(views::CreateEmptyBorder(4, 15, 4, 15));

  views::GridLayout* layout =
      SetLayoutManager(std::make_unique<views::GridLayout>(this));
  views::ColumnSet* column_set = layout->AddColumnSet(0);
  column_set->AddPaddingColumn(0, 0);
  column_set->AddColumn(views::GridLayout::FILL, views::GridLayout::FILL, 0,
                        views::GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0, 0);
  layout->AddView(blink_button_);

  layout->StartRow(1, 0);
  layout->AddView(ie_button_);

  switch (render_mode_.core) {
    case ie::BLINK_CORE:
      SetButtonSelected(blink_button_);
      break;
    case ie::IE_CORE:
      SetButtonSelected(ie_button_);
      break;
    default:
      break;
  }
}

const char* RendererModeBubbleView::GetClassName() const {
  return "RendererModeBubbleView";
}

views::View* RendererModeBubbleView::GetInitiallyFocusedView() {
  return nullptr;
}

void RendererModeBubbleView::ButtonPressed(views::Button* sender,
                                           const ui::Event& event) {
  if (browser_ == NULL)
    return;

  GURL url;
  ie::RenderMode mode;
  if (sender == blink_button_ && render_mode_.core != ie::BLINK_CORE) {
    mode.core = ie::BLINK_CORE;
    chrome::SwitchRendererMode(browser_, url, mode, false);
  } else if (sender == ie_button_ && render_mode_.core != ie::IE_CORE) {
    mode.core = ie::IE_CORE;
    mode.version = ie::DOCSYS;
    mode.emulation = (ie::Emulation)base::win::GetSystemIEVersion();
    chrome::SwitchRendererMode(browser_, url, mode, false);
  }
  GetWidget()->Close();
}

void RendererModeBubbleView::SetButtonSelected(
    views::LabelButton* select_button) {
  if (select_button == NULL)
    return;

  gfx::ImageSkia* img = NULL;
  if (select_button == blink_button_)
    img = ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(
        IDR_YSP_BLINK_CORE_WHITE);
  else
    img = ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(
        IDR_YSP_IE_CORE_WHITE);
  select_button->SetImage(views::Button::STATE_NORMAL, *img);
  select_button->SetBackground(
      views::CreateSolidBackground(SkColorSetARGB(255, 55, 159, 255)));
}

int RendererModeBubbleView::GetDialogButtons() const {
  return ui::DIALOG_BUTTON_NONE;
}
