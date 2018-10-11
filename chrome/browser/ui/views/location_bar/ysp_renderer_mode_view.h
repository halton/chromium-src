// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_LOCATION_BAR_YSP_RENDERER_MODE_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_LOCATION_BAR_YSP_RENDERER_MODE_VIEW_H_

#include "base/macros.h"
#include "chrome/browser/ui/views/page_action/page_action_icon_view.h"
#include "content/common/IE/IEVersion.h"
#include "ui/views/bubble/bubble_dialog_delegate_view.h"

class Browser;
class CommandUpdater;

class YspRendererModeView : public PageActionIconView {
 public:
  YspRendererModeView(CommandUpdater* command_updater,
                      Browser* browser,
                      PageActionIconView::Delegate* delegate);
  ~YspRendererModeView() override;

  void SetToggled(RendererMode mode);
  bool Update() override;
  base::string16 GetTextForTooltipAndAccessibleName() const override;

 protected:
  void OnExecuting(PageActionIconView::ExecuteSource execute_source) override;
  void ExecuteCommand(ExecuteSource source) override;
  views::BubbleDialogDelegateView* GetBubble() const override;
  const gfx::VectorIcon& GetVectorIcon() const override;

 private:
  Browser* browser_;
  RendererMode renderer_mode_;

  DISALLOW_COPY_AND_ASSIGN(YspRendererModeView);
};

#endif //CHROME_BROWSER_UI_VIEWS_LOCATION_BAR_RENDERER_MODE_VIEW_H_
