#ifdef REDCORE
//ysp+

#ifndef CHROME_BROWSER_YSP_UPDATE_YS_UPDATE_MANAGER_H_
#define CHROME_BROWSER_YSP_UPDATE_YS_UPDATE_MANAGER_H_

#include <memory.h>
#include <string.h>
//#include <vector.h>

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

// Singleton class
class YSPUpdateManager : public base::SupportsWeakPtr<YSPUpdateManager>,
                         public base::RefCounted<YSPUpdateManager>,
                         public YSPUpdateFetcherDelegate,
                         public download::DownloadItem::Observer {
public:
  YSPUpdateManager();

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

  void OnAutoUpdateDownload(std::unique_ptr<base::DictionaryValue> response_data, bool enable = true);
  void StartDownload(const GURL& package_url, base::FilePath updateFilePath);

private:
  ~YSPUpdateManager() override;
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
