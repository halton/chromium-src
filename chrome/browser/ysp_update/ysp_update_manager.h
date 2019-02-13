// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef REDCORE
// ysp+

#ifndef CHROME_BROWSER_YSP_UPDATE_YS_UPDATE_MANAGER_H_
#define CHROME_BROWSER_YSP_UPDATE_YS_UPDATE_MANAGER_H_

#include <memory.h>
#include <string.h>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "chrome/browser/ysp_update/ysp_update_fetcher.h"
#include "components/download/public/common/download_interrupt_reasons.h"
#include "components/download/public/common/download_item.h"

namespace net {
class URLRequestContext;
}

namespace content {
class WebContents;
}

namespace base {
template <typename T>
struct DefaultSingletonTraits;
}

// Singleton class
class YSPUpdateManager : public base::SupportsWeakPtr<YSPUpdateManager>,
                         public YSPUpdateFetcherDelegate,
                         public download::DownloadItem::Observer {
 public:
  static YSPUpdateManager* GetInstance();

  void RequestUpdate(content::WebContents* webContents,
                     const std::string& server_url,
                     const std::string& userId,
                     const std::string& companyId,
                     const std::string& accessToken);

  // YSPUpdateFetcherDelegate:
  void OnUpdateRequestFailure() override;
  void OnUpdateResponseParseSuccess(
      std::unique_ptr<base::DictionaryValue> response_data) override;
  void OnUpdateResponseParseFailure(const std::string& error) override;

  // download::DownloadItem::Observer:
  void OnDownloadUpdated(download::DownloadItem* download) override;
  void OnDownloadRemoved(download::DownloadItem* download) override;

  void OnAutoUpdateDownload(const std::string& update_data);
  void OnAutoUpdateDownload(
      std::unique_ptr<base::DictionaryValue> response_data,
      bool enable = true);
  void StartDownload(const GURL& package_url, base::FilePath updateFilePath);

 private:
  enum UpdateType {
    REMIND_UPDATE = 1,
    AUTO_UPDATE,
  };

  struct UpdateInfo {
    UpdateInfo();
    ~UpdateInfo();
    std::string file_name;
    std::string file_md5;
    std::string file_url;
    std::string version_str;
    std::string platform_type;
    int update_type;
  };

  YSPUpdateManager();
  ~YSPUpdateManager() override;
  friend struct base::DefaultSingletonTraits<YSPUpdateManager>;

  void DoAutoUpdateDownload(const UpdateInfo& update_info,
                            std::unique_ptr<base::DictionaryValue> response_data,
                            bool enable = true);
  void DownloadStarted(download::DownloadItem* item,
                       download::DownloadInterruptReason interrupt_reason);

  bool started_;
  YSPUpdateFetcher* update_fetcher_;
  content::WebContents* web_contents_;

  DISALLOW_COPY_AND_ASSIGN(YSPUpdateManager);
};

#if defined(OS_MACOSX)
void PrepareUpdate(std::string path);
#endif
#endif  // CHROME_BROWSER_YSP_UPDATE_YS_UPDATE_MANAGER_H_

#endif
