//ysp+
#ifdef REDCORE
#include "components/ysp_doc_view/ysp_doc_view_manager.h"

#include <utility>
#include "base/values.h"
#include "base/path_service.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/json/json_file_value_serializer.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/browser/browser_process.h"
#include "content/public/browser/browser_thread.h"
#include "components/prefs/pref_service.h"
#include "chrome/common/pref_names.h"
#include "base/native_library.h"
#if defined(OS_WIN)
#include <Iphlpapi.h>
#include <comdef.h>
#include "setupapi.h"
#endif
#include "base/json/json_writer.h"
#include "base/guid.h"
#include "base/base64.h"
#include "net/url_request/url_request_context.h"
#include "chrome/browser/ysp_login/ysp_login_manager.h"

#if defined(OS_WIN)
#pragma comment(lib, "iphlpapi.lib")
#pragma comment (lib,"netapi32")
#pragma comment(lib, "setupapi")
#endif

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

//static
YSPDocViewManager* YSPDocViewManager::GetInstance() {
  if (!g_instance) {
    g_instance = new YSPDocViewManager;
  }
  return g_instance;
}

void YSPDocViewManager::RequestDocView(
  const std::string& url,
  const base::FilePath& localPath) {
  DLOG(INFO) << "YSPDocViewManager::RequestDocView with file: " << localPath.value();
  if (!doc_fetcher_) {
    doc_fetcher_ =
      new YSPDocViewFetcher(this,
        g_browser_process->system_request_context());
  }
  if (doc_fetcher_)
    doc_fetcher_->Start(url, localPath);
}

// YSPDocViewFetcherDelegate:
void YSPDocViewManager::OnDocViewRequestFailure() {
  DLOG(INFO) << "YSPDocViewManager::OnDocViewRequestFailure";
  if (delegate_)
    delegate_->OnDocViewRequestFailure();
}

void YSPDocViewManager::OnDocViewResponseParseSuccess(
  std::unique_ptr<base::DictionaryValue> response_data) {
  DLOG(INFO) << "YSPDocViewManager::OnDocViewResponseParseSuccess";
  if (response_data) {
    int result = -1;
    response_data->GetInteger("result", &result);
    DLOG(INFO) << "YSPDocViewManager::OnDocViewResponseParseSuccess result: " << result;
    if (result != 0)
      return;

    base::ListValue* dataList = nullptr;
    if (response_data->GetList("data", &dataList)) {
      DLOG(INFO) << "YSPDocViewManager::OnDocViewResponseParseSuccess get data list";
      std::string docUrl;
      if (dataList && dataList->GetString(0, &docUrl)) {
        DLOG(INFO) << "YSPDocViewManager::OnDocViewResponseParseSuccess doc url: " << docUrl;
        if(delegate_)
          delegate_->OnDocViewRequestSuccess(download_item_, docUrl);
        if (download_item_) {
          download_item_->DeleteFile(
            base::Bind(&YSPDocViewManager::DownloadItemRemoved, base::Unretained(this)));
        }
      }
    }
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
  if (download && download->IsDone()) {
    if (download_item_ == download) {
      DLOG(INFO) << "YSPDocViewManager::OnDownloadUpdated viewing.";
      return;
    }

    download_item_ = download;
    base::FilePath filePath = download->GetTargetFilePath();
    DLOG(INFO) << "YSPDocViewManager::OnDownloadUpdated file: " << filePath.value();
    if (IsDocViewType(filePath)) {
      RequestDocView(online_url_, filePath);
    }
  }
}

void YSPDocViewManager::OnDownloadRemoved(download::DownloadItem* download) {
  if(download_item_)
    download_item_->RemoveObserver(this);
  download_item_ = nullptr;
}

void YSPDocViewManager::DownloadItemRemoved(bool success) {
  DLOG(INFO) << "YSPDocViewManager::DownloadItemRemoved: " << success;
  if(download_item_)
    download_item_->RemoveObserver(this);
  download_item_ = nullptr;
}

bool YSPDocViewManager::IsDocViewType(const base::FilePath& filePath) {
  bool result = false;
  if (!YSPLoginManager::GetInstance()->GetPreviewDocOnlineEnable())
    return result;
  base::DictionaryValue* previewDocOnline = YSPLoginManager::GetInstance()->GetPreviewDocOnline();
  if (previewDocOnline || !previewDocOnline->empty()) {
    std::string onlineUrl = "";
    previewDocOnline->GetString("url", &onlineUrl);
    online_url_ = onlineUrl;
    base::ListValue* formatList = nullptr;
    if (previewDocOnline->GetList("format", &formatList)) {
      for (size_t i = 0; i < formatList->GetSize(); i++) {
        base::FilePath::StringType format;
        formatList->GetString(i, &format);
        base::FilePath::StringType file_ext = FILE_PATH_LITERAL(".") + format;
        if (!format.empty() && filePath.MatchesExtension(file_ext))
          return true;
      }
    }
  }

  //if (filePath.MatchesExtension(FILE_PATH_LITERAL(".xlsx")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".xls")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".ppt")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".pptx")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".doc")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".docx")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".rtf")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".eio")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".uof")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".uos")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".xml")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".txt")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".dat")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".log")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".wps")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".dps")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".et")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".zip")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".rar")) ||
   // filePath.MatchesExtension(FILE_PATH_LITERAL(".pdf"))) {
   // result = true;
  //}
  DLOG(INFO) << "YSPDocViewManager::IsDocViewType: " << result << " of file: " << filePath.value();
  return result;
}
#endif // REDCORE
