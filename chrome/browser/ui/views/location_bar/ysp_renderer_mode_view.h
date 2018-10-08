#ifdef IE_REDCORE
#ifndef CHROME_BROWSER_UI_VIEWS_LOCATION_BAR_RENDERER_MODE_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_LOCATION_BAR_RENDERER_MODE_VIEW_H_

#include "base/macros.h"
#include "chrome/browser/ui/views/page_action/page_action_icon_view.h"
#include "content/common/IE/IEVersion.h"
// file name renamed
#include "ui/views/bubble/bubble_dialog_delegate_view.h"

class Browser;
class CommandUpdater;

class RendererModeView : public PageActionIconView 
{
public:
	RendererModeView(CommandUpdater* command_updater, Browser* browser, PageActionIconView::Delegate* delegate);
	 ~RendererModeView() override;

	void SetToggled(RendererMode mode);
	bool Update() override;
	base::string16 GetTextForTooltipAndAccessibleName() const override;

protected:
	// BubbleIconView:
	void OnExecuting(PageActionIconView::ExecuteSource execute_source) override;
	void ExecuteCommand(ExecuteSource source) override;
	views::BubbleDialogDelegateView* GetBubble() const override;
	// bool SetRasterIcon() override;
	const gfx::VectorIcon& GetVectorIcon() const override;

private:
	Browser* browser_;
	RendererMode rendererMode;

	DISALLOW_COPY_AND_ASSIGN(RendererModeView);
};

#endif //CHROME_BROWSER_UI_VIEWS_LOCATION_BAR_RENDERER_MODE_VIEW_H_
#endif
