#ifdef IE_REDCORE

//ysp+ { activex download }

#ifndef CHROME_BROWSER_UI_VIEWS_INFOBARS_YSP_DOWNLOAD_ACTIVEX_INFOBAR_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_INFOBARS_YSP_DOWNLOAD_ACTIVEX_INFOBAR_VIEW_H_

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "chrome/browser/ui/views/infobars/infobar_view.h"
#include "ui/views/controls/link_listener.h"

class DownloadActivexInfobarDelegate;
class ElevationIconSetter;

namespace views {
  class Button;
  class Label;
}

class DownloadActivexInfobarView : public InfoBarView,
                       public views::LinkListener {
 public:
  explicit DownloadActivexInfobarView(std::unique_ptr<DownloadActivexInfobarDelegate> delegate);
  ~DownloadActivexInfobarView() override;

 private:
  // InfoBarView:
  void Layout() override;
  void ViewHierarchyChanged(
      const ViewHierarchyChangedDetails& details) override;
  void ButtonPressed(views::Button* sender, const ui::Event& event) override;
  int ContentMinimumWidth() const override;

  // views::LinkListener:
  void LinkClicked(views::Link* source, int event_flags) override;

  DownloadActivexInfobarDelegate* GetDelegate();

  // Returns the width of all content other than the label and link.  Layout()
  // uses this to determine how much space the label and link can take.
  int NonLabelWidth() const;

  views::Label* label_;
  views::Button* ok_button_;
  views::Button* cancel_button_;
  views::Button* no_prompt_button_;
  views::Link* link_;
  std::unique_ptr<ElevationIconSetter> elevation_icon_setter_;

  DISALLOW_COPY_AND_ASSIGN(DownloadActivexInfobarView);
};

#endif  // CHROME_BROWSER_UI_VIEWS_INFOBARS_YSP_DOWNLOAD_ACTIVEX_INFOBAR_VIEW_H_

#endif
