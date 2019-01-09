// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_YSP_DOC_VIEW_YS_DOC_VIEW_MANAGER_H_
#define COMPONENTS_YSP_DOC_VIEW_YS_DOC_VIEW_MANAGER_H_

#include <string>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "components/download/public/common/download_item.h"
#include "components/ysp_doc_view/ysp_doc_view_fetcher.h"

namespace net {
class URLRequestContext;
}

// YSPDocViewManagerDelegate
class YSPDocViewManagerDelegate {
 public:
  virtual void OnDocViewRequestFailure() = 0;
  virtual void OnDocViewRequestSuccess(download::DownloadItem* download,
                                       const std::string& doc_url) = 0;

 protected:
  virtual ~YSPDocViewManagerDelegate() {}
};

// Singleton class
class YSPDocViewManager : public base::SupportsWeakPtr<YSPDocViewManager>,
                          public base::RefCounted<YSPDocViewManager>,
                          public YSPDocViewFetcherDelegate,
                          public download::DownloadItem::Observer {
 public:
  YSPDocViewManager();

  static YSPDocViewManager* GetInstance();

  void SetDelegate(YSPDocViewManagerDelegate* d) { delegate_ = d; }

  // YSPDocViewFetcherDelegate:
  void OnDocViewRequestFailure() override;
  void OnDocViewResponseParseSuccess(
      std::unique_ptr<base::DictionaryValue> response_data) override;
  void OnDocViewResponseParseFailure(const std::string& error) override;

  // download::DownloadItem::Observer:
  void OnDownloadUpdated(download::DownloadItem* download) override;
  void OnDownloadRemoved(download::DownloadItem* download) override;

  void DownloadItemRemoved(bool success);

  bool IsDocViewType(const base::FilePath& file_path);

 private:
  ~YSPDocViewManager() override;
  void RequestDocView(const std::string& url, const base::FilePath& local_path);

  YSPDocViewFetcher* doc_fetcher_;
  YSPDocViewManagerDelegate* delegate_;
  download::DownloadItem* download_item_;
  std::string online_url_;

  DISALLOW_COPY_AND_ASSIGN(YSPDocViewManager);
};

#endif  // COMPONENTS_YSP_DOC_VIEW_YS_DOC_VIEW_MANAGER_H_
