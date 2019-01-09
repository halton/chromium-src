// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/ysp_doc_view/ysp_doc_view_manager.h"

#include "base/values.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/ysp_login/ysp_login_manager.h"

namespace {

YSPDocViewManager* g_instance = nullptr;

}  // namespace

YSPDocViewManager::YSPDocViewManager()
    : doc_fetcher_(nullptr),
      delegate_(nullptr),
      download_item_(nullptr) {
}

YSPDocViewManager::~YSPDocViewManager() {
  if (doc_fetcher_) {
    delete doc_fetcher_;
    doc_fetcher_ = nullptr;
  }
}

// static
YSPDocViewManager* YSPDocViewManager::GetInstance() {
  if (!g_instance) {
    g_instance = new YSPDocViewManager;
  }

  return g_instance;
}

void YSPDocViewManager::RequestDocView(const std::string& url,
                                       const base::FilePath& local_path) {
  DLOG(INFO) << "YSPDocViewManager::RequestDocView with file: "
             << local_path.value();
  if (!doc_fetcher_) {
    doc_fetcher_ = new YSPDocViewFetcher(
        this, g_browser_process->system_request_context());
  }

  if (doc_fetcher_)
    doc_fetcher_->Start(url, local_path);
}

// YSPDocViewFetcherDelegate:
void YSPDocViewManager::OnDocViewRequestFailure() {
  DLOG(INFO) << "YSPDocViewManager::OnDocViewRequestFailure";
  if (delegate_)
    delegate_->OnDocViewRequestFailure();
}

void YSPDocViewManager::OnDocViewResponseParseSuccess(
    std::unique_ptr<base::DictionaryValue> response_data) {
  if (!response_data)
    return;

  int result = -1;
  response_data->GetInteger("result", &result);
  DLOG(INFO) << "YSPDocViewManager::OnDocViewResponseParseSuccess result: "
             << result;
  if (result != 0)
    return;

  base::ListValue* data_list = nullptr;
  std::string doc_url;
  if (!response_data->GetList("data", &data_list) ||
      !data_list || !data_list->GetString(0, &doc_url)) {
    return;
  }

  DLOG(INFO) << "YSPDocViewManager::OnDocViewResponseParseSuccess doc url: "
             << doc_url;
  if (delegate_)
    delegate_->OnDocViewRequestSuccess(download_item_, doc_url);

  if (download_item_) {
    download_item_->DeleteFile(
        base::Bind(&YSPDocViewManager::DownloadItemRemoved,
                   base::Unretained(this)));
  }
}

void YSPDocViewManager::OnDocViewResponseParseFailure(
    const std::string& error) {
  DLOG(INFO) << "YSPDocViewManager::OnDocViewResponseParseFailure";
  if (delegate_)
    delegate_->OnDocViewRequestFailure();
}

// download::DownloadItem::Observer:
void YSPDocViewManager::OnDownloadUpdated(download::DownloadItem* download) {
  DLOG(INFO) << "YSPDocViewManager::OnDownloadUpdated";
  if (!download || !download->IsDone())
    return;

  if (download_item_ == download) {
    DLOG(INFO) << "YSPDocViewManager::OnDownloadUpdated viewing.";
    return;
  }

  download_item_ = download;
  base::FilePath file_path = download->GetTargetFilePath();
  DLOG(INFO) << "YSPDocViewManager::OnDownloadUpdated file: "
             << file_path.value();
  if (IsDocViewType(file_path))
    RequestDocView(online_url_, file_path);
}

void YSPDocViewManager::OnDownloadRemoved(download::DownloadItem* download) {
  if (download_item_)
    download_item_->RemoveObserver(this);

  download_item_ = nullptr;
}

void YSPDocViewManager::DownloadItemRemoved(bool success) {
  DLOG(INFO) << "YSPDocViewManager::DownloadItemRemoved: " << success;
  if (download_item_)
    download_item_->RemoveObserver(this);

  download_item_ = nullptr;
}

bool YSPDocViewManager::IsDocViewType(const base::FilePath& file_path) {
  if (!YSPLoginManager::GetInstance()->GetPreviewDocOnlineEnable())
    return false;

  base::DictionaryValue* prewview_doc_online =
      YSPLoginManager::GetInstance()->GetPreviewDocOnline();
  if (!prewview_doc_online && prewview_doc_online->empty())
    return false;

  prewview_doc_online->GetString("url", &online_url_);
  base::ListValue* format_list = nullptr;
  if (!prewview_doc_online->GetList("format", &format_list))
    return false;

  for (size_t i = 0; i < format_list->GetSize(); i++) {
    base::FilePath::StringType format;
    format_list->GetString(i, &format);
    base::FilePath::StringType file_ext = FILE_PATH_LITERAL(".") + format;
    if (!format.empty() && file_path.MatchesExtension(file_ext))
      return true;
  }
  // if (file_path.MatchesExtension(FILE_PATH_LITERAL(".xlsx")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".xls")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".ppt")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".pptx")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".doc")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".docx")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".rtf")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".eio")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".uof")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".uos")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".xml")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".txt")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".dat")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".log")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".wps")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".dps")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".et")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".zip")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".rar")) ||
  // file_path.MatchesExtension(FILE_PATH_LITERAL(".pdf"))) {
  // result = true;
  //}
  DLOG(INFO) << "YSPDocViewManager::IsDocViewType: "
             << " of file: " << file_path.value();
  return true;
}
