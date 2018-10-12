// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/ysp_download_activex_infobar_delegate.h"

#include "base/files/File_path.h"
#include "base/files/file_util.h"
#include "base/memory/weak_ptr.h"
#include "base/path_service.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/pref_names.h"
#include "chrome/grit/generated_resources.h"
#include "components/download/public/common/download_url_parameters.h"
#include "components/infobars/core/infobar.h"
#include "components/prefs/scoped_user_pref_update.h"
#include "content/public/browser/download_manager.h"
#include "ui/base/l10n/l10n_util.h"
#include "url/gurl.h"

DownloadActivexInfobarDelegate::DownloadActivexInfobarDelegate(
    BrowserView* browser_view,
    const ActivexDownloadInfo& info)
    : browser_view_(browser_view), activex_info_(info) {}

DownloadActivexInfobarDelegate::~DownloadActivexInfobarDelegate() {}

void DownloadActivexInfobarDelegate::Create(BrowserView* browser_view,
                                            const ActivexDownloadInfo& info) {
  if (browser_view == NULL)
    return;
  content::WebContents* webcontent = browser_view->GetActiveWebContents();
  if (webcontent == NULL)
    return;
  InfoBarService* infobar_service = NULL;
  infobar_service = InfoBarService::FromWebContents(webcontent);
  if (infobar_service == NULL)
    return;
  infobar_service->AddInfoBar(DownloadActivexInfobarDelegate::CreateInfoBar(
      std::unique_ptr<DownloadActivexInfobarDelegate>(
          new DownloadActivexInfobarDelegate(browser_view, info))));
}

void DownloadActivexInfobarDelegate::ClearAllActivexInfoBar(
    BrowserView* browser_view) {
  TabStripModel* pTabModel = NULL;
  pTabModel = browser_view->browser()->tab_strip_model();
  if (pTabModel == NULL)
    return;
  int count = pTabModel->count();
  int i = 0;
  for (i = 0; i < count; i++)  // Get All InfoBarService
  {
    content::WebContents* webcontent = pTabModel->GetWebContentsAt(i);
    if (webcontent == NULL)
      continue;
    InfoBarService* infobar_service = NULL;
    infobar_service = InfoBarService::FromWebContents(webcontent);
    if (infobar_service == NULL)
      continue;
    std::vector<infobars::InfoBar*> willRemoveInfoBar;
    size_t j = 0;
    for (j = 0; j < infobar_service->infobar_count();
         j++)  // Find All ActivexInfobar for every InfoBarService
    {
      infobars::InfoBar* bar = infobar_service->infobar_at(j);
      if (bar == NULL)
        continue;
      if (bar->delegate()->GetIdentifier() ==
          infobars::InfoBarDelegate::DOWNLOAD_ACTIVEX_INFOBAR_DELEGATE)
        willRemoveInfoBar.push_back(bar);
    }
    std::vector<infobars::InfoBar*>::iterator iter = willRemoveInfoBar.begin();
    for (; iter != willRemoveInfoBar.end(); iter++)  // Remove ActivexInfobar
    {
      infobar_service->RemoveInfoBar(*iter);
    }
  }
}

ActivexDownloadInfo::ActivexDownloadInfo() {
  display_name = L"";
  filename = L"";
  download_url = L"";
  md5 = L"";
}

ActivexDownloadInfo::ActivexDownloadInfo(const ActivexDownloadInfo& other) =
    default;
ActivexDownloadInfo::~ActivexDownloadInfo() {}

infobars::InfoBarDelegate::Type DownloadActivexInfobarDelegate::GetInfoBarType()
    const {
  return InfoBarDelegate::WARNING_TYPE;
}

// just for link error.
// gfx::VectorIcon& GetVectorIcon() const
// {   gfx::VectorIcon vector_icon;
//  return vector_icon;
// }

infobars::InfoBarDelegate::InfoBarIdentifier
DownloadActivexInfobarDelegate::GetIdentifier() const {
  return infobars::InfoBarDelegate::DOWNLOAD_ACTIVEX_INFOBAR_DELEGATE;
}

int DownloadActivexInfobarDelegate::GetIconId() const {
  return infobars::InfoBarDelegate::kNoIconID;
}

bool DownloadActivexInfobarDelegate::ShouldExpire(
    const NavigationDetails& details) const {
  return false;
}

bool DownloadActivexInfobarDelegate::EqualsDelegate(
    InfoBarDelegate* delegate) const {
  DownloadActivexInfobarDelegate* download_activex_delegate =
      delegate->AsDownloadActivexInfobarDelegate();
  return download_activex_delegate &&
         (download_activex_delegate->GetMessageText() == GetMessageText());
}

DownloadActivexInfobarDelegate*
DownloadActivexInfobarDelegate::AsDownloadActivexInfobarDelegate() {
  return this;
}

base::string16 DownloadActivexInfobarDelegate::GetMessageText() const {
  std::wstring txt = l10n_util::GetStringFUTF16(
      IDS_YSP_ACTIVEX_DOWNLOAD_INFOBAR_MESSAGE, activex_info_.display_name);
  return txt;
}

int DownloadActivexInfobarDelegate::GetButtons() const {
  return BUTTON_OK | BUTTON_CANCEL | BUTTON_NO_PROMPT;
}

base::string16 DownloadActivexInfobarDelegate::GetButtonLabel(
    InfoBarButton button) const {
  if (button == BUTTON_OK)
    return l10n_util::GetStringUTF16(IDS_YSP_ACTIVEX_DOWNLOAD_BUTTON_OK);
  else if (button == BUTTON_CANCEL)
    return l10n_util::GetStringUTF16(IDS_YSP_ACTIVEX_DOWNLOAD_BUTTON_CANCEL);
  else if (button == BUTTON_NO_PROMPT)
    return l10n_util::GetStringUTF16(IDS_YSP_ACTIVEX_DOWNLOAD_BUTTON_NO_PROMPT);
  return L"";
}

bool DownloadActivexInfobarDelegate::OKButtonTriggersUACPrompt() const {
  return false;
}

base::string16 DownloadActivexInfobarDelegate::GetLinkText() const {
  return base::string16();
}

// comment just for cmp
bool DownloadActivexInfobarDelegate::Accept() {
  // GURL gurl(activex_info_.download_url);
  // content::WebContents* webcontent = browser_view_->GetActiveWebContents();
  // if (webcontent == NULL)
  //  return false;
  // content::DownloadManager* manager =
  //  content::BrowserContext::GetDownloadManager(webcontent->GetBrowserContext());
  // if (manager == NULL)
  //  return false;
  // std::unique_ptr<download::DownloadUrlParameters>
  // downloadParam(download::DownloadUrlParameters::FromWebContents(webcontent,
  // gurl)); base::FilePath path; base::PathService::Get(chrome::DIR_USER_DATA,
  // &path); base::string16 strPath = path.value(); strPath += L"\\Activex\\";
  // strPath +=activex_info_.filename;
  // path=base::FilePath(strPath);
  // if (base::PathExists(path.DirName()) == false)
  //  base::CreateDirectory(path.DirName());
  // downloadParam->set_file_path(path);
  // downloadParam->set_callback(
  //  base::Bind(&DownloadActivexInfobarDelegate::DownloadStarted,
  //  base::Unretained(this)));
  // manager->DownloadUrl(std::move(downloadParam));
  return false;
}

bool DownloadActivexInfobarDelegate::Cancel() {
  return true;
}

bool DownloadActivexInfobarDelegate::NoPrompt() {
  if (activex_info_.md5 != L"") {
    std::string md5 = base::UTF16ToASCII(activex_info_.md5);
    PrefService* pref_service = g_browser_process->local_state();
    DictionaryPrefUpdate update(pref_service, prefs::kYSPActivexNoPromptInfo);
    update->SetBoolean(md5, true);
  }

  return true;
}

GURL DownloadActivexInfobarDelegate::GetLinkURL() const {
  return GURL();
}

bool DownloadActivexInfobarDelegate::LinkClicked(
    WindowOpenDisposition disposition) {
  infobar()->owner()->OpenURL(GetLinkURL(), disposition);
  return false;
}

void DownloadActivexInfobarDelegate::DownloadStarted(
    download::DownloadItem* item,
    download::DownloadInterruptReason interrupt_reason) {
  if (interrupt_reason != download::DOWNLOAD_INTERRUPT_REASON_NONE) {
    std::string err = "";
    err = DownloadInterruptReasonToString(interrupt_reason);
    LOG(WARNING) << "Download activex " << activex_info_.display_name
                 << " from " << activex_info_.download_url
                 << " failed reason: " << err;
    CloseInfobar();
    return;
  }
  item->set_is_update(true);
  item->SetOpenWhenComplete(true);
  // for cmp
  // item->SetIsTemporary(false);
  item->AddObserver(ActivexDownloadObserver::GetInstance());
  CloseInfobar();
}

void DownloadActivexInfobarDelegate::CloseInfobar() {
  content::WebContents* webcontent = browser_view_->GetActiveWebContents();
  InfoBarService* infobar_service = NULL;
  infobar_service = InfoBarService::FromWebContents(webcontent);
  infobar_service->RemoveInfoBar(infobar());
}

ActivexDownloadObserver* ActivexDownloadObserver::GetInstance() {
  return base::Singleton<ActivexDownloadObserver>::get();
}

void ActivexDownloadObserver::OnDownloadUpdated(
    download::DownloadItem* download) {
  download::DownloadItem::DownloadState state = download->GetState();
  switch (state) {
    case download::DownloadItem::IN_PROGRESS:
      break;
    case download::DownloadItem::COMPLETE:
      download->RemoveObserver(this);
      break;
    case download::DownloadItem::CANCELLED:
      download->RemoveObserver(this);
      break;
    case download::DownloadItem::INTERRUPTED:
      break;
    case download::DownloadItem::MAX_DOWNLOAD_STATE:
      break;
    default:
      break;
  }
}

void ActivexDownloadObserver::OnDownloadRemoved(
    download::DownloadItem* download) {
  download->RemoveObserver(this);
}

ActivexDownloadObserver::ActivexDownloadObserver() {}

ActivexDownloadObserver::~ActivexDownloadObserver() {}
