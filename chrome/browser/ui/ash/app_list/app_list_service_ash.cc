// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/ash/app_list/app_list_service_ash.h"

#include <string>
#include <utility>

#include "ash/app_list/app_list_presenter_delegate.h"
#include "ash/app_list/app_list_presenter_delegate_factory.h"
#include "ash/shell.h"
#include "base/files/file_path.h"
#include "base/memory/ptr_util.h"
#include "base/memory/singleton.h"
#include "build/build_config.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/app_list/app_list_view_delegate.h"
#include "chrome/browser/ui/app_list/start_page_service.h"
#include "chrome/browser/ui/ash/app_list/app_list_controller_ash.h"
#include "chrome/browser/ui/ash/app_list/app_list_presenter_delegate_mus.h"
#include "chrome/browser/ui/ash/app_list/app_list_presenter_service.h"
#include "chrome/browser/ui/ash/ash_util.h"
#include "chrome/browser/ui/ash/launcher/chrome_launcher_controller.h"
#include "chrome/browser/ui/ash/session_util.h"
#include "ui/app_list/app_list_features.h"
#include "ui/app_list/app_list_switches.h"
#include "ui/app_list/presenter/app_list_presenter_delegate_factory.h"
#include "ui/app_list/presenter/app_list_presenter_impl.h"
#include "ui/app_list/presenter/app_list_view_delegate_factory.h"
#include "ui/app_list/views/app_list_main_view.h"
#include "ui/app_list/views/app_list_view.h"
#include "ui/app_list/views/contents_view.h"
#include "ui/display/display.h"
#include "ui/display/screen.h"

namespace {

class ViewDelegateFactoryImpl : public app_list::AppListViewDelegateFactory {
 public:
  explicit ViewDelegateFactoryImpl(AppListServiceImpl* factory)
      : factory_(factory) {}
  ~ViewDelegateFactoryImpl() override {}

  // app_list::AppListViewDelegateFactory:
  app_list::AppListViewDelegate* GetDelegate() override {
    return factory_->GetViewDelegate(
        Profile::FromBrowserContext(GetActiveBrowserContext()));
  }

 private:
  AppListServiceImpl* factory_;

  DISALLOW_COPY_AND_ASSIGN(ViewDelegateFactoryImpl);
};

class AppListPresenterDelegateFactoryMus
    : public app_list::AppListPresenterDelegateFactory {
 public:
  explicit AppListPresenterDelegateFactoryMus(
      std::unique_ptr<app_list::AppListViewDelegateFactory>
          view_delegate_factory)
      : view_delegate_factory_(std::move(view_delegate_factory)) {}

  ~AppListPresenterDelegateFactoryMus() override {}

  std::unique_ptr<app_list::AppListPresenterDelegate> GetDelegate(
      app_list::AppListPresenterImpl* presenter) override {
    return base::MakeUnique<AppListPresenterDelegateMus>(
        presenter, view_delegate_factory_.get());
  }

 private:
  std::unique_ptr<app_list::AppListViewDelegateFactory> view_delegate_factory_;

  DISALLOW_COPY_AND_ASSIGN(AppListPresenterDelegateFactoryMus);
};

int64_t GetDisplayIdToShowAppListOn() {
  return display::Screen::GetScreen()
      ->GetDisplayNearestWindow(ash::Shell::GetRootWindowForNewWindows())
      .id();
}

}  // namespace

// static
AppListServiceAsh* AppListServiceAsh::GetInstance() {
  return base::Singleton<AppListServiceAsh,
                         base::LeakySingletonTraits<AppListServiceAsh>>::get();
}

AppListServiceAsh::AppListServiceAsh() {
  std::unique_ptr<app_list::AppListPresenterDelegateFactory> factory;
  if (ash_util::IsRunningInMash()) {
    factory = base::MakeUnique<AppListPresenterDelegateFactoryMus>(
        base::MakeUnique<ViewDelegateFactoryImpl>(this));
  } else {
    factory = base::MakeUnique<ash::AppListPresenterDelegateFactory>(
        base::MakeUnique<ViewDelegateFactoryImpl>(this));
  }
  app_list_presenter_ =
      base::MakeUnique<app_list::AppListPresenterImpl>(std::move(factory));
  controller_delegate_ =
      base::MakeUnique<AppListControllerDelegateAsh>(app_list_presenter_.get());
  app_list_presenter_service_ = base::MakeUnique<AppListPresenterService>();
}

AppListServiceAsh::~AppListServiceAsh() {}

app_list::AppListPresenterImpl* AppListServiceAsh::GetAppListPresenter() {
  return app_list_presenter_.get();
}

void AppListServiceAsh::Init(Profile* initial_profile) {
  app_list_presenter_service_->Init();

  // Ensure the StartPageService is created here. This early initialization is
  // necessary to allow the WebContents to load before the app list is shown.
  app_list::StartPageService* service =
      app_list::StartPageService::Get(initial_profile);
  if (service)
    service->Init();
}

void AppListServiceAsh::OnProfileWillBeRemoved(
    const base::FilePath& profile_path) {}

void AppListServiceAsh::ShowAndSwitchToState(
    app_list::AppListModel::State state) {
  bool app_list_was_open = true;
  app_list::AppListView* app_list_view = app_list_presenter_->GetView();
  if (!app_list_view) {
    // TODO(calamity): This may cause the app list to show briefly before the
    // state change. If this becomes an issue, add the ability to ash::Shell to
    // load the app list without showing it.
    app_list_presenter_->Show(GetDisplayIdToShowAppListOn());
    app_list_was_open = false;
    app_list_view = app_list_presenter_->GetView();
    DCHECK(app_list_view);
  }

  if (state == app_list::AppListModel::INVALID_STATE)
    return;

  app_list::ContentsView* contents_view =
      app_list_view->app_list_main_view()->contents_view();
  contents_view->SetActiveState(state, app_list_was_open /* animate */);
}

base::FilePath AppListServiceAsh::GetProfilePath(
    const base::FilePath& user_data_dir) {
  return ChromeLauncherController::instance()->profile()->GetPath();
}

void AppListServiceAsh::ShowForProfile(Profile* /*default_profile*/) {
  // This may not work correctly if the profile passed in is different from the
  // one the ash Shell is currently using.
  app_list_presenter_->Show(GetDisplayIdToShowAppListOn());
}

void AppListServiceAsh::ShowForAppInstall(Profile* profile,
                                          const std::string& extension_id,
                                          bool start_discovery_tracking) {
  if (app_list::features::IsFullscreenAppListEnabled())
    return;
  ShowAndSwitchToState(app_list::AppListModel::STATE_APPS);
  AppListServiceImpl::ShowForAppInstall(profile, extension_id,
                                        start_discovery_tracking);
}

bool AppListServiceAsh::IsAppListVisible() const {
  return app_list_presenter_->GetTargetVisibility();
}

void AppListServiceAsh::DismissAppList() {
  app_list_presenter_->Dismiss();
}

void AppListServiceAsh::EnableAppList(Profile* initial_profile,
                                      AppListEnableSource enable_source) {}

gfx::NativeWindow AppListServiceAsh::GetAppListWindow() {
  return app_list_presenter_->GetWindow();
}

Profile* AppListServiceAsh::GetCurrentAppListProfile() {
  return ChromeLauncherController::instance()->profile();
}

AppListControllerDelegate* AppListServiceAsh::GetControllerDelegate() {
  return controller_delegate_.get();
}

void AppListServiceAsh::CreateForProfile(Profile* default_profile) {}

void AppListServiceAsh::DestroyAppList() {
  // On Ash, the app list is torn down whenever it is dismissed, so just ensure
  // that it is dismissed.
  DismissAppList();
}

// static
AppListService* AppListService::Get() {
  return AppListServiceAsh::GetInstance();
}

// static
void AppListService::InitAll(Profile* initial_profile,
                             const base::FilePath& profile_path) {
  AppListServiceAsh::GetInstance()->Init(initial_profile);
}
