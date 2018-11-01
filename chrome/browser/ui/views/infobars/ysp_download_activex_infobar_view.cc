// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// ysp+ { activex download }

#include "chrome/browser/ui/views/infobars/ysp_download_activex_infobar_view.h"

#include <utility>

#include "base/logging.h"
#include "chrome/browser/infobars/infobar_service.h"
#include "chrome/browser/ui/views/elevation_icon_setter.h"
#include "chrome/browser/ui/ysp_download_activex_infobar_delegate.h"
#include "chrome/browser/ui/views/infobars/confirm_infobar.h"
#include "chrome/browser/ui/views/chrome_layout_provider.h"
#include "ui/base/material_design/material_design_controller.h"
#include "ui/base/window_open_disposition.h"
#include "ui/views/controls/button/label_button.h"
#include "ui/views/controls/button/md_text_button.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/link.h"


// DownloadActivexInfobarDelegate -------------------------------------------------

// static
  std::unique_ptr<infobars::InfoBar> DownloadActivexInfobarDelegate::CreateInfoBar(
	std::unique_ptr<DownloadActivexInfobarDelegate> delegate) {
	return std::unique_ptr<DownloadActivexInfobarView>(new DownloadActivexInfobarView(std::move(delegate)));
}


// ConfirmInfoBar -------------------------------------------------------------

DownloadActivexInfobarView::DownloadActivexInfobarView(std::unique_ptr<DownloadActivexInfobarDelegate> delegate)
    : InfoBarView(std::move(delegate)),
      label_(NULL),
      ok_button_(NULL),
      cancel_button_(NULL),
	    no_prompt_button_(NULL),
      link_(NULL) {}

DownloadActivexInfobarView::~DownloadActivexInfobarView() {
  // Ensure |elevation_icon_setter_| is destroyed before |ok_button_|.
  elevation_icon_setter_.reset();
}

void DownloadActivexInfobarView::Layout() {
  InfoBarView::Layout();

  ChromeLayoutProvider* layout_provider = ChromeLayoutProvider::Get();
  int x = StartX();
  Labels labels;
  labels.push_back(label_);
  labels.push_back(link_);
  AssignWidths(&labels, std::max(0, EndX() - x - NonLabelWidth()));

  label_->SetPosition(gfx::Point(x, OffsetY(label_)));
  if (!label_->text().empty())
    x = label_->bounds().right() + layout_provider->GetDistanceMetric(views::DISTANCE_RELATED_LABEL_HORIZONTAL);

  if (ok_button_) {
    ok_button_->SetPosition(gfx::Point(x, OffsetY(ok_button_)));
    x = ok_button_->bounds().right() + (layout_provider->GetDistanceMetric(
            views::DISTANCE_RELATED_BUTTON_HORIZONTAL));
  }

  if (cancel_button_) {
    cancel_button_->SetPosition(gfx::Point(x, OffsetY(cancel_button_)));
	x = cancel_button_->bounds().right() + (layout_provider->GetDistanceMetric(
            views::DISTANCE_RELATED_BUTTON_HORIZONTAL));
  }

  if (no_prompt_button_)
	  no_prompt_button_->SetPosition(gfx::Point(x, OffsetY(no_prompt_button_)));

  link_->SetPosition(gfx::Point(EndX() - link_->width(), OffsetY(link_)));
}

void DownloadActivexInfobarView::ViewHierarchyChanged(
    const ViewHierarchyChangedDetails& details) {
  if (details.is_add && details.child == this && (label_ == NULL)) {
	  DownloadActivexInfobarDelegate* delegate = GetDelegate();
    label_ = CreateLabel(delegate->GetMessageText());
    AddChildView(label_);

    if (delegate->GetButtons() & DownloadActivexInfobarDelegate::BUTTON_OK) {
      if (delegate->OKButtonTriggersUACPrompt()) {
        // Use a label button even in MD mode as MD buttons don't support icons.
        views::MdTextButton* ok_button_ = views::MdTextButton::Create(
            this, delegate->GetButtonLabel(DownloadActivexInfobarDelegate::BUTTON_OK));
            // ok_button_->SetCallToAction(true);
        elevation_icon_setter_.reset(new ElevationIconSetter(
            ok_button_,
            base::Bind(&DownloadActivexInfobarView::Layout, base::Unretained(this))));
      }
      AddChildView(ok_button_);
      ok_button_->SizeToPreferredSize();
    }

    if (delegate->GetButtons() & DownloadActivexInfobarDelegate::BUTTON_CANCEL) {
      cancel_button_ = views::MdTextButton::Create(
          this,
          delegate->GetButtonLabel(DownloadActivexInfobarDelegate::BUTTON_CANCEL));
      AddChildView(cancel_button_);
      cancel_button_->SizeToPreferredSize();
      // cancel_button_->SetCallToAction(true);
    }

	if (delegate->GetButtons() & DownloadActivexInfobarDelegate::BUTTON_NO_PROMPT) {
		no_prompt_button_ = views::MdTextButton::Create(
			this,
			delegate->GetButtonLabel(DownloadActivexInfobarDelegate::BUTTON_NO_PROMPT));
		AddChildView(no_prompt_button_);
		no_prompt_button_->SizeToPreferredSize();
	}

    base::string16 link_text(delegate->GetLinkText());
    link_ = CreateLink(link_text, this);
    AddChildView(link_);
  }

  // This must happen after adding all other children so InfoBarView can ensure
  // the close button is the last child.
  InfoBarView::ViewHierarchyChanged(details);
}

void DownloadActivexInfobarView::ButtonPressed(views::Button* sender,
                                   const ui::Event& event) {
  if (!owner())
    return;  // We're closing; don't call anything, it might access the owner.
  DownloadActivexInfobarDelegate* delegate = GetDelegate();
  if (sender == ok_button_) {
    if (delegate->Accept())
      RemoveSelf();
  } else if (sender == cancel_button_) {
    if (delegate->Cancel())
      RemoveSelf();
  } else if (sender == no_prompt_button_) {
	if (delegate->NoPrompt())
	  RemoveSelf();
  } else {
    InfoBarView::ButtonPressed(sender, event);
  }
}

int DownloadActivexInfobarView::ContentMinimumWidth() const {
  return label_->GetMinimumSize().width() + link_->GetMinimumSize().width() +
      NonLabelWidth();
}

void DownloadActivexInfobarView::LinkClicked(views::Link* source, int event_flags) {
  if (!owner())
    return;  // We're closing; don't call anything, it might access the owner.
  DCHECK_EQ(link_, source);
  if (GetDelegate()->LinkClicked(ui::DispositionFromEventFlags(event_flags)))
    RemoveSelf();
}

DownloadActivexInfobarDelegate* DownloadActivexInfobarView::GetDelegate() {
  return static_cast<DownloadActivexInfobarDelegate*>(delegate());
}

int DownloadActivexInfobarView::NonLabelWidth() const {
  ChromeLayoutProvider* layout_provider = ChromeLayoutProvider::Get();
  int width = (label_->text().empty() || (!ok_button_ && !cancel_button_)) ?
      0 : (layout_provider->GetDistanceMetric(views::DISTANCE_RELATED_LABEL_HORIZONTAL));
  if (ok_button_)
    width += ok_button_->width() + (cancel_button_ ? (layout_provider->GetDistanceMetric(
            views::DISTANCE_RELATED_BUTTON_HORIZONTAL)) : 0);
  if (cancel_button_)
	  width += cancel_button_->width() + (no_prompt_button_ ? (layout_provider->GetDistanceMetric(
           views::DISTANCE_RELATED_BUTTON_HORIZONTAL)) : 0);
  width += no_prompt_button_ ? no_prompt_button_->width() : 0;
  return width + ((link_->text().empty() || !width) ? 0 : (layout_provider->GetDistanceMetric(views::DISTANCE_RELATED_LABEL_HORIZONTAL)));
}
