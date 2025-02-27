// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/safe_browsing/chrome_cleaner/chrome_cleaner_dialog_controller_impl_win.h"

#include <memory>

#include "base/memory/ptr_util.h"
#include "base/memory/ref_counted.h"
#include "base/run_loop.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/safe_browsing/chrome_cleaner/chrome_cleaner_controller_win.h"
#include "chrome/browser/safe_browsing/chrome_cleaner/mock_chrome_cleaner_controller_win.h"
#include "chrome/browser/safe_browsing/chrome_cleaner/srt_field_trial_win.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_finder.h"
#include "chrome/browser/ui/browser_window.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "components/component_updater/pref_names.h"
#include "components/keep_alive_registry/keep_alive_types.h"
#include "components/keep_alive_registry/scoped_keep_alive.h"
#include "components/variations/variations_params_manager.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace safe_browsing {
namespace {

using ::testing::_;
using ::testing::InvokeWithoutArgs;
using ::testing::StrictMock;
using ::testing::Return;

constexpr char kSRTPromptGroup[] = "SRTGroup";

class MockChromeCleanerPromptDelegate : public ChromeCleanerPromptDelegate {
 public:
  MOCK_METHOD3(ShowChromeCleanerPrompt,
               void(Browser* browser,
                    ChromeCleanerDialogController* dialog_controller,
                    ChromeCleanerController* cleaner_controller));
};

// Parameters for this test:
//  - const char* old_seed_: The old "Seed" Finch parameter saved in prefs.
//  - const char* incoming_seed_: The new "Seed" Finch parameter.
class ChromeCleanerPromptUserTest
    : public InProcessBrowserTest,
      public ::testing::WithParamInterface<
          testing::tuple<const char*, const char*>> {
 public:
  ChromeCleanerPromptUserTest() {
    std::tie(old_seed_, incoming_seed_) = GetParam();
  }

  void SetUpCommandLine(base::CommandLine* command_line) override {
    variations::testing::VariationParamsManager::AppendVariationParams(
        kSRTPromptTrial, kSRTPromptGroup, {{"Seed", incoming_seed_}},
        command_line);
  }

  void SetUpInProcessBrowserTestFixture() override {
// dialog_controller_ expects that the cleaner controller would be on
// scanning state.
#if DCHECK_IS_ON()
    EXPECT_CALL(mock_cleaner_controller_, state())
        .WillOnce(Return(ChromeCleanerController::State::kScanning));
#endif
    EXPECT_CALL(mock_cleaner_controller_, AddObserver(_));
    dialog_controller_ =
        new ChromeCleanerDialogControllerImpl(&mock_cleaner_controller_);
    dialog_controller_->SetPromptDelegateForTests(&mock_delegate_);
  }

  void SetUpOnMainThread() override {
    chrome::FindLastActive()->profile()->GetPrefs()->SetString(
        prefs::kSwReporterPromptSeed, old_seed_);
  }

  void TearDownOnMainThread() override {
    bool expect_seed_changed =
        !incoming_seed_.empty() && (incoming_seed_ != old_seed_);
    EXPECT_EQ(expect_seed_changed ? incoming_seed_ : old_seed_,
              chrome::FindLastActive()->profile()->GetPrefs()->GetString(
                  prefs::kSwReporterPromptSeed));
  }

 protected:
  MockChromeCleanerController mock_cleaner_controller_;
  ChromeCleanerDialogControllerImpl* dialog_controller_;
  StrictMock<MockChromeCleanerPromptDelegate> mock_delegate_;

  std::string old_seed_;
  std::string incoming_seed_;
};

IN_PROC_BROWSER_TEST_P(ChromeCleanerPromptUserTest,
                       OnInfectedBrowserAvailable) {
  EXPECT_CALL(mock_delegate_, ShowChromeCleanerPrompt(_, _, _)).Times(1);
  dialog_controller_->OnInfected(false, ChromeCleanerScannerResults());
}

IN_PROC_BROWSER_TEST_P(ChromeCleanerPromptUserTest,
                       DISABLED_OnInfectedBrowserNotAvailable) {
  browser()->window()->Minimize();
  base::RunLoop().RunUntilIdle();
  dialog_controller_->OnInfected(false, ChromeCleanerScannerResults());

  base::RunLoop run_loop;
  // We only set the expectation here because we want to make sure that the
  // prompt is shown only when the window is restored.
  EXPECT_CALL(mock_delegate_, ShowChromeCleanerPrompt(_, _, _))
      .WillOnce(InvokeWithoutArgs([&run_loop]() { run_loop.Quit(); }));

  browser()->window()->Restore();
  run_loop.Run();
}

IN_PROC_BROWSER_TEST_P(ChromeCleanerPromptUserTest, AllBrowsersClosed) {
  std::unique_ptr<ScopedKeepAlive> keep_alive =
      base::MakeUnique<ScopedKeepAlive>(KeepAliveOrigin::BROWSER,
                                        KeepAliveRestartOption::DISABLED);

  CloseAllBrowsers();
  base::RunLoop().RunUntilIdle();
  dialog_controller_->OnInfected(false, ChromeCleanerScannerResults());

  base::RunLoop run_loop;
  // We only set the expectation here because we want to make sure that the
  // prompt is shown only when the window is restored.
  EXPECT_CALL(mock_delegate_, ShowChromeCleanerPrompt(_, _, _))
      .WillOnce(InvokeWithoutArgs([&run_loop]() { run_loop.Quit(); }));

  CreateBrowser(ProfileManager::GetActiveUserProfile());
  run_loop.Run();
}

INSTANTIATE_TEST_CASE_P(
    WithVaryingSeeds,
    ChromeCleanerPromptUserTest,
    ::testing::Combine(
        ::testing::Values("", "Seed1"),             // old_seed_
        ::testing::Values("", "Seed1", "Seed2")));  // incoming_seed

}  // namespace
}  // namespace safe_browsing
