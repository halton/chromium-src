// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_YSP_DOWNLOAD_ACTIVEX_INFOBAR_DELEGATE_H_
#define CHROME_BROWSER_UI_VIEWS_YSP_DOWNLOAD_ACTIVEX_INFOBAR_DELEGATE_H_

#include <string>

#include "base/location.h"
#include "base/macros.h"
#include "base/memory/singleton.h"
#include "base/memory/weak_ptr.h"
#include "base/metrics/histogram_macros.h"
#include "chrome/browser/infobars/infobar_service.h"
#include "chrome/grit/generated_resources.h"
#include "components/download/public/common/download_interrupt_reasons.h"
#include "components/download/public/common/download_item.h"
#include "components/infobars/core/infobar.h"
#include "components/infobars/core/infobar_delegate.h"
#include "content/public/browser/web_contents.h"

class BrowserView;

struct ActivexDownloadInfo {
  std::wstring display_name;
  std::wstring filename;
  std::wstring download_url;
  std::wstring md5;
  ActivexDownloadInfo();
  ~ActivexDownloadInfo();
  ActivexDownloadInfo(const ActivexDownloadInfo& other);
};

class DownloadActivexInfobarDelegate : public infobars::InfoBarDelegate {
 public:
  enum InfoBarButton {
    BUTTON_NONE = 0,
    BUTTON_OK = 1 << 0,
    BUTTON_CANCEL = 1 << 1,
    BUTTON_NO_PROMPT = 1 << 2,
  };

  DownloadActivexInfobarDelegate(BrowserView* browser_view,
                                 const ActivexDownloadInfo& info);
  ~DownloadActivexInfobarDelegate() override;

  static void Create(BrowserView* browser_view,
                     const ActivexDownloadInfo& info);
  static void ClearAllActivexInfoBar(BrowserView* browser_view);

  base::string16 GetMessageText() const;
  int GetButtons() const;
  bool OKButtonTriggersUACPrompt() const;
  base::string16 GetButtonLabel(InfoBarButton button) const;
  base::string16 GetLinkText() const;
  bool Accept();
  bool Cancel();
  bool NoPrompt();
  GURL GetLinkURL() const;
  bool LinkClicked(WindowOpenDisposition disposition);

 private:
  static std::unique_ptr<infobars::InfoBar> CreateInfoBar(
      std::unique_ptr<DownloadActivexInfobarDelegate> delegate);

  // InfoBarDelegate:
  Type GetInfoBarType() const override;
  infobars::InfoBarDelegate::InfoBarIdentifier GetIdentifier() const override;
  int GetIconId() const override;
  // gfx::VectorIcon& GetVectorIcon() const override;
  bool ShouldExpire(const NavigationDetails& details) const override;
  bool EqualsDelegate(infobars::InfoBarDelegate* delegate) const override;
  DownloadActivexInfobarDelegate* AsDownloadActivexInfobarDelegate() override;

  void DownloadStarted(download::DownloadItem* item,
                       download::DownloadInterruptReason interrupt_reason);
  void CloseInfobar();

 private:
  BrowserView* browser_view_;
  ActivexDownloadInfo activex_info_;
};

class ActivexDownloadObserver : public download::DownloadItem::Observer {
 public:
  static ActivexDownloadObserver* GetInstance();

  // content::DownloadItem::Observer:
  void OnDownloadUpdated(download::DownloadItem* download) override;
  void OnDownloadRemoved(download::DownloadItem* download) override;

 private:
  friend struct base::DefaultSingletonTraits<ActivexDownloadObserver>;
  ActivexDownloadObserver();
  ~ActivexDownloadObserver() override;
};

#endif  // CHROME_BROWSER_UI_VIEWS_YSP_DOWNLOAD_ACTIVEX_INFOBAR_DELEGATE_H_
