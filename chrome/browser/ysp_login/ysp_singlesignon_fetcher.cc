//YSP+ { SingleSignOn config }
#ifdef REDCORE
#include "chrome/browser/ysp_login/ysp_singlesignon_fetcher.h"

#include <utility>

#include "base/bind.h"
#include "base/values.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/common/service_manager_connection.h"
#include "extensions/common/extension_urls.h"
#include "net/base/load_flags.h"
#include "net/base/mime_util.h"
#include "net/url_request/url_fetcher.h"
#include "net/url_request/url_request_status.h"
#include "services/data_decoder/public/cpp/safe_json_parser.h"

namespace {

const char kInvalidDataResponseError[] = "Invalid Data reponse";
const char kUploadPath[] = "/u/ssos/token";
const char kUploadContentType[] = "multipart/form-data";
const char kMultipartBoundary[] = "------xdcehKrkohmfeHiyreFTWjfshUDd------";

}  // namespace

YSPSingleSignOnFetcher::YSPSingleSignOnFetcher(
  YSPSingleSignOnFetcherDelegate* delegate,
  net::URLRequestContextGetter* request_context)
  : delegate_(delegate),
  request_context_(request_context) {
}

YSPSingleSignOnFetcher::~YSPSingleSignOnFetcher() {}

void YSPSingleSignOnFetcher::StartGetSingleSignOn(
  const std::string& server_url,
  const std::string& companyId,
  const std::string& uuid,
  const std::string& password) {
  server_url_ = server_url;
  std::string post_data;
  // build form data
  net::AddMultipartValueForUpload("companyId", companyId, kMultipartBoundary, "", &post_data);
  net::AddMultipartValueForUpload("uuid", uuid, kMultipartBoundary, "", &post_data);
  net::AddMultipartValueForUpload("password", password, kMultipartBoundary, "", &post_data);
  net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);

  content::BrowserThread::PostTask(
    content::BrowserThread::UI, FROM_HERE,
    base::Bind(&YSPSingleSignOnFetcher::DoStartGetSingleSignOn,
      base::Unretained(this), post_data));
}

void YSPSingleSignOnFetcher::DoStartGetSingleSignOn(const std::string& post_data) {
  std::string content_type = kUploadContentType;
  content_type.append("; boundary=");
  content_type.append(kMultipartBoundary);

  std::string url = server_url_ +kUploadPath;
  data_fetcher_ =
    net::URLFetcher::Create(GURL(url), net::URLFetcher::POST, this);
  data_fetcher_->SetRequestContext(request_context_);
  data_fetcher_->SetUploadData(content_type, post_data);
  data_fetcher_->Start();
}

void YSPSingleSignOnFetcher::OnJsonParseSuccess(
  std::unique_ptr<base::Value> parsed_json) {
  if (!parsed_json->is_dict()) {
    OnJsonParseFailure(kInvalidDataResponseError);
    return;
  }

  delegate_->OnSingleSignOnResponseParseSuccess(
    std::unique_ptr<base::DictionaryValue>(
      static_cast<base::DictionaryValue*>(parsed_json.release())));
}

void YSPSingleSignOnFetcher::OnJsonParseFailure(
  const std::string& error) {
  delegate_->OnSingleSignOnResponseParseFailure(error);
}

void YSPSingleSignOnFetcher::OnURLFetchComplete(const net::URLFetcher* source) {
  CHECK_EQ(data_fetcher_.get(), source);

  std::unique_ptr<net::URLFetcher> fetcher(
    std::move(data_fetcher_));

  if (!fetcher->GetStatus().is_success() ||
    fetcher->GetResponseCode() != 200) {
    delegate_->OnSingleSignOnRequestFailure();
    return;
  }

  std::string value_data;
  fetcher->GetResponseAsString(&value_data);

  // The parser will call us back via one of the callbacks.
  data_decoder::SafeJsonParser::Parse(
    content::ServiceManagerConnection::GetForProcess()->GetConnector(),
    value_data,
    base::Bind(&YSPSingleSignOnFetcher::OnJsonParseSuccess, AsWeakPtr()),
    base::Bind(&YSPSingleSignOnFetcher::OnJsonParseFailure, AsWeakPtr()));
}
#endif //REDCORE
