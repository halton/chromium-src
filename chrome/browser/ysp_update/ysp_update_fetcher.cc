#ifdef REDCORE
//ysp+

#include "chrome/browser/ysp_update/ysp_update_fetcher.h"

#include <utility>
#include <string.h>

#include "base/bind.h"
#include "base/values.h"
#include "extensions/common/extension_urls.h"
#include "net/base/load_flags.h"
#include "net/base/mime_util.h"
#include "net/url_request/url_fetcher.h"
#include "net/url_request/url_request_status.h"
#include "content/public/browser/browser_thread.h"
#include "base/files/file_util.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/common/service_manager_connection.h"
#include "services/data_decoder/public/cpp/safe_json_parser.h"

#if defined(OS_MACOSX)
#define HEADER_OS "os: macos"
#elif defined(OS_WIN)
#define HEADER_OS "os: windows"
#elif defined(OS_LINUX)
#define HEADER_OS "os: linux"
#else
#error Unknown OS
#endif


namespace {

  const char kInvalidDataResponseError[] = "Invalid Data response";
  const char kUploadPath[] = "/client/v3/configuration/pc/"; //获取PC设置为自动升级

}  // namespace

YSPUpdateFetcher::YSPUpdateFetcher(
  YSPUpdateFetcherDelegate* delegate,
  net::URLRequestContextGetter* request_context)
  : delegate_(delegate),
    request_context_(request_context) {
}

YSPUpdateFetcher::~YSPUpdateFetcher() {}

void YSPUpdateFetcher::StartCheck(
  const std::string& server_url,
  const std::string& userId,
  const std::string& companyId,
  const std::string& accessToken) {
  server_url_ = server_url;
  companyId_ = companyId;
  userId_ = userId;
  accessToken_ = accessToken;

  content::BrowserThread::PostDelayedTask(
    content::BrowserThread::UI, FROM_HERE,
    base::Bind(&YSPUpdateFetcher::DoStartCheck,
      base::Unretained(this)),
      base::TimeDelta::FromMilliseconds(1000));
}

void YSPUpdateFetcher::DoStartCheck() {
  std::string url = server_url_ + kUploadPath + companyId_;
  GURL requestUrl(url);
  if (!requestUrl.is_valid()) {
    return;
  }
  std::string userHeader = "userid: " + userId_;
  std::string tokenHeader = "access-token: " + accessToken_;

  data_fetcher_ =
    net::URLFetcher::Create(requestUrl, net::URLFetcher::GET, this);
  data_fetcher_->SetRequestContext(request_context_);
  data_fetcher_->AddExtraRequestHeader(userHeader);
  data_fetcher_->AddExtraRequestHeader(tokenHeader);
  data_fetcher_->AddExtraRequestHeader(HEADER_OS);
  data_fetcher_->Start();
}

void YSPUpdateFetcher::OnJsonParseSuccess(
  std::unique_ptr<base::Value> parsed_json) {
  if (!parsed_json->is_dict()) {
    OnJsonParseFailure(kInvalidDataResponseError);
    return;
  }

  delegate_->OnUpdateResponseParseSuccess(
    std::unique_ptr<base::DictionaryValue>(
      static_cast<base::DictionaryValue*>(parsed_json.release())));
}

void YSPUpdateFetcher::OnJsonParseFailure(
  const std::string& error) {
  delegate_->OnUpdateResponseParseFailure(error);
}

void YSPUpdateFetcher::OnURLFetchComplete(const net::URLFetcher* source) {
  CHECK_EQ(data_fetcher_.get(), source);

  std::unique_ptr<net::URLFetcher> fetcher(
    std::move(data_fetcher_));

  if (!fetcher->GetStatus().is_success() ||
    fetcher->GetResponseCode() != 200) {
    delegate_->OnUpdateRequestFailure();
    return;
  }

  std::string json_data;
  fetcher->GetResponseAsString(&json_data);

  // The parser will call us back via one of the callbacks.
  data_decoder::SafeJsonParser::Parse(
    content::ServiceManagerConnection::GetForProcess()->GetConnector(),
    json_data,
    base::Bind(&YSPUpdateFetcher::OnJsonParseSuccess, AsWeakPtr()),
    base::Bind(&YSPUpdateFetcher::OnJsonParseFailure, AsWeakPtr()));
}

#endif
