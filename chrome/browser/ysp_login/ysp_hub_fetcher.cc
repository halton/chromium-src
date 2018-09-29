//ysp+ { login }
#ifdef REDCORE
#include "chrome/browser/ysp_login/ysp_hub_fetcher.h"

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

}  // namespace

YSPHubFetcher::YSPHubFetcher(
  YSPHubFetcherDelegate* delegate,
  net::URLRequestContextGetter* request_context)
  : delegate_(delegate),
  request_context_(request_context) {
}

YSPHubFetcher::~YSPHubFetcher() {}

void YSPHubFetcher::Start(const std::string& url) {
  url_ = url;
  content::BrowserThread::PostTask(
    content::BrowserThread::UI, FROM_HERE,
    base::Bind(&YSPHubFetcher::DoStart, base::Unretained(this)));
}

void YSPHubFetcher::DoStart() {
  data_fetcher_ =
    net::URLFetcher::Create(GURL(url_), net::URLFetcher::GET, this);
  data_fetcher_->SetRequestContext(request_context_);
  data_fetcher_->Start();
}

void YSPHubFetcher::OnJsonParseSuccess(
  std::unique_ptr<base::Value> parsed_json) {
  if (!parsed_json->is_dict()) {
    OnJsonParseFailure(kInvalidDataResponseError);
    return;
  }

  delegate_->OnHubResponseParseSuccess(
    std::unique_ptr<base::DictionaryValue>(
      static_cast<base::DictionaryValue*>(parsed_json.release())));
}

void YSPHubFetcher::OnJsonParseFailure(
  const std::string& error) {
  delegate_->OnHubResponseParseFailure(error);
}

void YSPHubFetcher::OnURLFetchComplete(const net::URLFetcher* source) {
  CHECK_EQ(data_fetcher_.get(), source);

  std::unique_ptr<net::URLFetcher> fetcher(
    std::move(data_fetcher_));

  if (!fetcher->GetStatus().is_success() ||
    fetcher->GetResponseCode() != 200) {
    delegate_->OnHubRequestFailure();
    return;
  }

  std::string json_data;
  fetcher->GetResponseAsString(&json_data);

  // The parser will call us back via one of the callbacks.
  data_decoder::SafeJsonParser::Parse(
      content::ServiceManagerConnection::GetForProcess()->GetConnector(),
      json_data,
      base::Bind(&YSPHubFetcher::OnJsonParseSuccess, AsWeakPtr()),
      base::Bind(&YSPHubFetcher::OnJsonParseFailure, AsWeakPtr()));
}
#endif  // REDCORE
