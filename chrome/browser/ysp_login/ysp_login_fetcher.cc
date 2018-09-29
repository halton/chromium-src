//ysp+ { login }
#ifdef REDCORE
#include "chrome/browser/ysp_login/ysp_login_fetcher.h"

#include <utility>

#include "base/bind.h"
#include "base/values.h"
#include "extensions/common/extension_urls.h"
#include "net/base/load_flags.h"
#include "net/base/mime_util.h"
#include "net/url_request/url_fetcher.h"
#include "net/url_request/url_request_status.h"
#include "content/public/browser/browser_thread.h"
#include "chrome/browser/ysp_login/ysp_login_manager.h" //ysp+ { auto get config }
#include "chrome/browser/browser_process.h" //ysp+ { auto get config }
#include "content/public/common/service_manager_connection.h"
#include "services/data_decoder/public/cpp/safe_json_parser.h"

namespace {

  const char kInvalidDataResponseError[] = "Invalid Data reponse";
  const char kLoginUploadPath[] = "/client/v3/login";
//  const char kAutoGetConfigPath[] = "/client/v3/version/";
  const char kUploadContentType[] = "multipart/form-data";
  const char kMultipartBoundary[] = "------xdcehKrkohmfeHiyreFnWifghoDl------";

}  // namespace
std::string YSPLoginFetcher::autoConfigFlags_now = ""; //ysp+ { auto get config }
bool YSPLoginFetcher::autoGetConfig_ = true; //ysp+ { auto get config }
YSPLoginFetcher::YSPLoginFetcher(
  YSPLoginFetcherDelegate* delegate,
  net::URLRequestContextGetter* request_context)
  : delegate_(delegate),
  request_context_(request_context) {
  autoConfigFlags_now.assign(""); //ysp+ { auto get config }
  autoGetConfig_ = true; //ysp+ { auto get config }
}

YSPLoginFetcher::~YSPLoginFetcher() {}

void YSPLoginFetcher::StartLogin(
  const std::string& server_url,
  const std::string& domainName,
  const std::string& uuid,
  const std::string& password,
  const std::vector<std::string> header_list,
  const std::string& deviceInfo) {
  server_url_ = server_url;
  header_list_ = header_list;
  std::string post_data;
  // build form data
  net::AddMultipartValueForUpload("domainName", domainName, kMultipartBoundary, "", &post_data);
  net::AddMultipartValueForUpload("loginName", uuid, kMultipartBoundary, "", &post_data);
  net::AddMultipartValueForUpload("password", password, kMultipartBoundary, "", &post_data);
  net::AddMultipartValueForUpload("deviceInfo", deviceInfo, kMultipartBoundary, "", &post_data);
  net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);
  post_data_ = post_data; //ysp+ { auto get config }

  content::BrowserThread::PostTask(
    content::BrowserThread::UI, FROM_HERE,
    base::Bind(&YSPLoginFetcher::DoStartLogin,
      base::Unretained(this), post_data));
}
//ysp+ { auto get config

void YSPLoginFetcher::DoAutoGetConfig() {
  // build form data
  bool loginstatus = YSPLoginManager::GetInstance()->GetAutoLoginStatus();
  if (loginstatus) {
    content::BrowserThread::PostDelayedTask(
      content::BrowserThread::UI, FROM_HERE,
      base::Bind(&YSPLoginFetcher::DoAutoGetConfig,
        base::Unretained(this)), base::TimeDelta::FromSeconds(60));
  }
  else {
    autoGetConfig_ = true;
  }
  if (frequency == 0) {
    int delta_time = YSPLoginManager::GetInstance()->GetAutoConfigDelta();
    YSPLoginManager::GetInstance()->GetAutoConfigfetcher();
    frequency = delta_time / 60 - 1;
  }
  else if (frequency > 0) {
    frequency--;
  }
  else
    frequency = 0;
}
//ysp+ } /*auto get config*/
void YSPLoginFetcher::DoStartLogin(const std::string& post_data) {
  std::string content_type = kUploadContentType;
  content_type.append("; boundary=");
  content_type.append(kMultipartBoundary);

  std::string url = server_url_ + kLoginUploadPath;
  GURL requestUrl(url);
  if (!requestUrl.is_valid()) {
    return;
  }
  data_fetcher_ =
    net::URLFetcher::Create(requestUrl, net::URLFetcher::POST, this);
  data_fetcher_->SetRequestContext(request_context_);
  data_fetcher_->SetUploadData(content_type, post_data);
  if (!header_list_.empty()) {
    std::vector<std::string>::iterator iter = header_list_.begin();
    for (; iter != header_list_.end(); ++iter)
      data_fetcher_->AddExtraRequestHeader(*iter);
  }
  data_fetcher_->Start();
}

void YSPLoginFetcher::OnJsonParseSuccess(
  std::unique_ptr<base::Value> parsed_json) {
  if (!parsed_json->is_dict()) {
    OnJsonParseFailure(kInvalidDataResponseError);
    return;
  }

  delegate_->OnLoginResponseParseSuccess(
    std::unique_ptr<base::DictionaryValue>(
      static_cast<base::DictionaryValue*>(parsed_json.release())));
}

void YSPLoginFetcher::OnJsonParseFailure(
  const std::string& error) {
  delegate_->OnLoginResponseParseFailure(error);
}

void YSPLoginFetcher::OnURLFetchComplete(const net::URLFetcher* source) {
  CHECK_EQ(data_fetcher_.get(), source);

  std::unique_ptr<net::URLFetcher> fetcher(
    std::move(data_fetcher_));

  // TODO: to handle the 302 response code
  if (!fetcher->GetStatus().is_success() ||
    fetcher->GetResponseCode() != 200) {
    char buffer[10];
    sprintf(buffer, "%d", fetcher->GetResponseCode());
    const std::string error(buffer);
    delegate_->OnLoginRequestFailure(error);
    return;
  }

  std::string json_data;
  fetcher->GetResponseAsString(&json_data);
  DLOG(INFO) << "login fetcher response data: [" << json_data << "]";

  // The parser will call us back via one of the callbacks.
  data_decoder::SafeJsonParser::Parse(
    content::ServiceManagerConnection::GetForProcess()->GetConnector(),
    json_data,
    base::Bind(&YSPLoginFetcher::OnJsonParseSuccess, AsWeakPtr()),
    base::Bind(&YSPLoginFetcher::OnJsonParseFailure, AsWeakPtr()));
}
#endif //REDCORE
