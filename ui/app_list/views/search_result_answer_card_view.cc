// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/app_list/views/search_result_answer_card_view.h"

#include <vector>

#include "ash/app_list/model/search/search_result_observer.h"
#include "ui/accessibility/ax_node.h"
#include "ui/accessibility/ax_node_data.h"
#include "ui/app_list/app_list_constants.h"
#include "ui/app_list/app_list_view_delegate.h"
#include "ui/views/background.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/fill_layout.h"

namespace app_list {

// Container of the search answer view.
class SearchResultAnswerCardView::SearchAnswerContainerView
    : public views::Button,
      public views::ButtonListener,
      public SearchResultObserver {
 public:
  explicit SearchAnswerContainerView(AppListViewDelegate* view_delegate)
      : Button(this), view_delegate_(view_delegate) {
    SetFocusBehavior(FocusBehavior::ALWAYS);
    // Center the card horizontally in the container. Padding is set on the
    // server.
    auto answer_container_layout =
        std::make_unique<views::BoxLayout>(views::BoxLayout::kHorizontal);
    answer_container_layout->set_main_axis_alignment(
        views::BoxLayout::MAIN_AXIS_ALIGNMENT_START);
    SetLayoutManager(std::move(answer_container_layout));
  }

  ~SearchAnswerContainerView() override {
    if (search_result_)
      search_result_->RemoveObserver(this);
  }

  bool selected() const { return selected_; }

  void SetSelected(bool selected) {
    if (selected == selected_)
      return;
    selected_ = selected;
    UpdateBackgroundColor();
    if (selected)
      ScrollRectToVisible(GetLocalBounds());
  }

  bool SetSearchResult(SearchResult* search_result) {
    views::View* const old_result_view = child_count() ? child_at(0) : nullptr;
    views::View* const new_result_view =
        search_result ? search_result->view() : nullptr;

    if (old_result_view != new_result_view) {
      if (old_result_view != nullptr)
        RemoveChildView(old_result_view);
      if (new_result_view != nullptr)
        AddChildView(new_result_view);
    }

    base::string16 old_title, new_title;
    if (search_result_) {
      search_result_->RemoveObserver(this);
      old_title = search_result_->title();
    }
    search_result_ = search_result;
    if (search_result_) {
      search_result_->AddObserver(this);
      new_title = search_result_->title();
      SetAccessibleName(new_title);
    }

    return old_title != new_title;
  }

  // views::Button overrides:
  const char* GetClassName() const override {
    return "SearchAnswerContainerView";
  }

  void OnBlur() override {
    SetSelected(false);
    Button::OnBlur();
  }

  void OnFocus() override {
    SetSelected(true);
    NotifyAccessibilityEvent(ui::AX_EVENT_SELECTION, true);
    Button::OnFocus();
  }

  bool OnKeyPressed(const ui::KeyEvent& event) override {
    if (event.key_code() == ui::VKEY_SPACE) {
      // Shouldn't eat Space; we want Space to go to the search box.
      return false;
    }

    return Button::OnKeyPressed(event);
  }

  void GetAccessibleNodeData(ui::AXNodeData* node_data) override {
    // Default button role is atomic for ChromeVox, so assign a generic
    // container role to allow accessibility focus to get into this view.
    node_data->role = ui::AX_ROLE_GENERIC_CONTAINER;
    node_data->SetName(accessible_name());
  }

  // views::ButtonListener overrides:
  void ButtonPressed(views::Button* sender, const ui::Event& event) override {
    DCHECK(sender == this);
    if (search_result_)
      view_delegate_->OpenSearchResult(search_result_, event.flags());
  }

  // SearchResultObserver overrides:
  void OnResultDestroying() override { search_result_ = nullptr; }

 private:
  void UpdateBackgroundColor() {
    if (selected_) {
      SetBackground(views::CreateSolidBackground(kAnswerCardSelectedColor));
    } else {
      SetBackground(nullptr);
    }

    SchedulePaint();
  }

  AppListViewDelegate* const view_delegate_;  // Not owned.
  bool selected_ = false;
  SearchResult* search_result_ = nullptr;  // Not owned.

  DISALLOW_COPY_AND_ASSIGN(SearchAnswerContainerView);
};

SearchResultAnswerCardView::SearchResultAnswerCardView(
    AppListViewDelegate* view_delegate)
    : search_answer_container_view_(
          new SearchAnswerContainerView(view_delegate)) {
  AddChildView(search_answer_container_view_);
  SetLayoutManager(std::make_unique<views::FillLayout>());
}

SearchResultAnswerCardView::~SearchResultAnswerCardView() {}

const char* SearchResultAnswerCardView::GetClassName() const {
  return "SearchResultAnswerCardView";
}

void SearchResultAnswerCardView::OnContainerSelected(
    bool from_bottom,
    bool directional_movement) {
  if (num_results() == 0)
    return;

  SetSelectedIndex(0);
}

int SearchResultAnswerCardView::GetYSize() {
  return num_results();
}

int SearchResultAnswerCardView::DoUpdate() {
  std::vector<SearchResult*> display_results =
      SearchModel::FilterSearchResultsByDisplayType(
          results(), SearchResult::DISPLAY_CARD, 1);

  const bool have_result = !display_results.empty();

  const bool title_changed = search_answer_container_view_->SetSearchResult(
      have_result ? display_results[0] : nullptr);
  parent()->SetVisible(have_result);

  set_container_score(have_result ? display_results.front()->relevance() : 0);
  if (title_changed && search_answer_container_view_->selected())
    search_answer_container_view_->NotifyAccessibilityEvent(
        ui::AX_EVENT_SELECTION, true);
  return have_result ? 1 : 0;
}

void SearchResultAnswerCardView::UpdateSelectedIndex(int old_selected,
                                                     int new_selected) {
  if (new_selected == old_selected)
    return;

  const bool is_selected = new_selected == 0;
  search_answer_container_view_->SetSelected(is_selected);
  if (is_selected)
    search_answer_container_view_->NotifyAccessibilityEvent(
        ui::AX_EVENT_SELECTION, true);
}

bool SearchResultAnswerCardView::OnKeyPressed(const ui::KeyEvent& event) {
  if (selected_index() == 0 &&
      search_answer_container_view_->OnKeyPressed(event)) {
    return true;
  }

  return SearchResultContainerView::OnKeyPressed(event);
}

views::View* SearchResultAnswerCardView::GetSelectedView() const {
  return search_answer_container_view_->selected()
             ? search_answer_container_view_
             : nullptr;
}

views::View* SearchResultAnswerCardView::GetFirstResultView() {
  return num_results() <= 0 ? nullptr : search_answer_container_view_;
}

void SearchResultAnswerCardView::SetFirstResultSelected(bool selected) {
  if (num_results() <= 0)
    return;
  search_answer_container_view_->SetSelected(selected);
}

views::View* SearchResultAnswerCardView::GetSearchAnswerContainerViewForTest()
    const {
  return search_answer_container_view_;
}

}  // namespace app_list
