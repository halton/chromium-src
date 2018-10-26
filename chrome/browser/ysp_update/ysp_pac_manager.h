// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef REDCORE
// ysp+

#ifndef CHROME_BROWSER_YSP_UPDATE_YS_PAC_MANAGER_H_
#define CHROME_BROWSER_YSP_UPDATE_YS_PAC_MANAGER_H_

#include <memory>
#include <string>
#include <vector>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "components/download/public/common/download_interrupt_reasons.h"
#include "components/download/public/common/download_item.h"

namespace net {
class URLRequestContext;
}

namespace content {
class WebContents;
}

// Singleton class
class YSPPACManager : public base::SupportsWeakPtr<YSPPACManager>,
                      public base::RefCounted<YSPPACManager>,
                      public download::DownloadItem::Observer {
 public:
  YSPPACManager();

  static YSPPACManager* GetInstance();

  void RequestPAC(content::WebContents* webContents);

  // download::DownloadItem::Observer:
  void OnDownloadUpdated(download::DownloadItem* download) override;
  void OnDownloadRemoved(download::DownloadItem* download) override;

 private:
  ~YSPPACManager() override;
  void StartDownload(const GURL& package_url);
  void DownloadStarted(download::DownloadItem* item,
                       download::DownloadInterruptReason interrupt_reason);

  bool updating_;
  bool updated_;
  content::WebContents* web_contents_;

  DISALLOW_COPY_AND_ASSIGN(YSPPACManager);
};

#endif  // CHROME_BROWSER_YSP_UPDATE_YS_PAC_MANAGER_H_

#endif
