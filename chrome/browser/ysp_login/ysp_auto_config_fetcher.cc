//ysp+ { auto get config }
#ifdef REDCORE
#include "chrome/browser/ysp_login/ysp_auto_config_fetcher.h"

#include <utility>

#include "base/bind.h"
#include "base/values.h"
#include "extensions/common/extension_urls.h"
#include "net/base/load_flags.h"
#include "net/base/mime_util.h"
#include "net/url_request/url_fetcher.h"
#include "net/url_request/url_request_status.h"
#include "content/public/browser/browser_thread.h"
#include "chrome/browser/ysp_login/ysp_login_manager.h"
#include "content/public/common/service_manager_connection.h"
#include "services/data_decoder/public/cpp/safe_json_parser.h"

namespace {

  const char kInvalidDataResponseError[] = "Invalid Data reponse";
  //const char kUploadContentType[] = "multipart/form-data";
  //const char kMultipartBoundary[] = "------xdcehKrkohmfeHiyreFnWifghoDl------";

}  // namespace

YSPAutoConfigFetcher::YSPAutoConfigFetcher(
  net::URLRequestContextGetter* request_context)
  : request_context_(request_context) {
}

YSPAutoConfigFetcher::~YSPAutoConfigFetcher() {}

void YSPAutoConfigFetcher::StartGetConfigVersion(const std::string& server_url, const std::vector<std::string> header_list, bool auto_fetch) {
  server_url_ = server_url;
  header_list_ = header_list;
  auto_fetch_ = auto_fetch;

  content::BrowserThread::PostTask(
    content::BrowserThread::UI, FROM_HERE,
    base::Bind(&YSPAutoConfigFetcher::DoStartGetConfigVersion,
      base::Unretained(this)));
}

void YSPAutoConfigFetcher::DoStartGetConfigVersion() {
  std::string url = server_url_;
  std::string access_token = YSPLoginManager::GetInstance()->GetAccessToken();
  GURL requestUrl(url);
  if (!requestUrl.is_valid()) {
    return;
  }
  data_fetcher_ =
    net::URLFetcher::Create(requestUrl, net::URLFetcher::GET, this);
  data_fetcher_->SetRequestContext(request_context_);
  data_fetcher_->SetExtraRequestHeaders("Connection: close");
  if (!header_list_.empty()) {
    std::vector<std::string>::iterator iter = header_list_.begin();
    for (; iter != header_list_.end(); ++iter)
      data_fetcher_->AddExtraRequestHeader(*iter);
  }
  data_fetcher_->Start();
}

void YSPAutoConfigFetcher::OnJsonParseSuccess(
  std::unique_ptr<base::Value> parsed_json) {
  if (!parsed_json->is_dict()) {
    OnJsonParseFailure(kInvalidDataResponseError);
    return;
  }
  YSPLoginManager::GetInstance()->OnAutoConfigParseSuccess(std::unique_ptr<base::DictionaryValue>(
    static_cast<base::DictionaryValue*>(parsed_json.release())), auto_fetch_);
}

void YSPAutoConfigFetcher::OnJsonParseFailure(
  const std::string& error) {
  DLOG(INFO) << "Get config version failed!(" << error << ")";
}

void YSPAutoConfigFetcher::OnURLFetchComplete(const net::URLFetcher* source) {
  CHECK_EQ(data_fetcher_.get(), source);

  std::unique_ptr<net::URLFetcher> fetcher(
    std::move(data_fetcher_));

  if (!fetcher->GetStatus().is_success() ||
    fetcher->GetResponseCode() != 200) {
    DLOG(INFO) << "Get config version failed!";
    return;
  }

  std::string json_data;
  fetcher->GetResponseAsString(&json_data);
  DLOG(INFO) << "Response Data: " << json_data;

  // The parser will call us back via one of the callbacks.
  data_decoder::SafeJsonParser::Parse(
    content::ServiceManagerConnection::GetForProcess()->GetConnector(),
    json_data,
    base::Bind(&YSPAutoConfigFetcher::OnJsonParseSuccess, AsWeakPtr()),
    base::Bind(&YSPAutoConfigFetcher::OnJsonParseFailure, AsWeakPtr()));
}
#endif  // REDCORE
