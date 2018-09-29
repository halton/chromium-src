//ysp+ { login }
#ifdef REDCORE
#include "chrome/browser/ysp_login/ysp_iplookup_fetcher.h"

#include <utility>

#include "base/bind.h"
#include "base/values.h"
#include "extensions/common/extension_urls.h"
#include "net/base/load_flags.h"
#include "net/base/mime_util.h"
#include "net/url_request/url_fetcher.h"
#include "net/url_request/url_request_status.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/common/service_manager_connection.h"
#include "services/data_decoder/public/cpp/safe_json_parser.h"

namespace {

const char kInvalidDataResponseError[] = "Invalid Data reponse";
const char kReqUrl[] = "http://int.dpool.sina.com.cn/iplookup/iplookup.php?format=json";

}  // namespace

YSPIPLookupFetcher::YSPIPLookupFetcher(
  YSPIPLookupFetcherDelegate* delegate,
  net::URLRequestContextGetter* request_context)
  : delegate_(delegate),
  request_context_(request_context) {
}

YSPIPLookupFetcher::~YSPIPLookupFetcher() {}

void YSPIPLookupFetcher::Start() {
  content::BrowserThread::PostTask(
    content::BrowserThread::UI, FROM_HERE,
    base::Bind(&YSPIPLookupFetcher::DoStart, base::Unretained(this)));
}

void YSPIPLookupFetcher::DoStart() {
  data_fetcher_ =
    net::URLFetcher::Create(GURL(kReqUrl), net::URLFetcher::GET, this);
  data_fetcher_->SetRequestContext(request_context_);
  data_fetcher_->Start();
}

void YSPIPLookupFetcher::OnJsonParseSuccess(
  std::unique_ptr<base::Value> parsed_json) {
  if (!parsed_json->is_dict()) {
    OnJsonParseFailure(kInvalidDataResponseError);
    return;
  }

  delegate_->OnIPLookupResponseParseSuccess(
    std::unique_ptr<base::DictionaryValue>(
      static_cast<base::DictionaryValue*>(parsed_json.release())));
}

void YSPIPLookupFetcher::OnJsonParseFailure(
  const std::string& error) {
  delegate_->OnIPLookupResponseParseFailure(error);
}

void YSPIPLookupFetcher::OnURLFetchComplete(const net::URLFetcher* source) {
  CHECK_EQ(data_fetcher_.get(), source);

  std::unique_ptr<net::URLFetcher> fetcher(
    std::move(data_fetcher_));

  if (!fetcher->GetStatus().is_success() ||
    fetcher->GetResponseCode() != 200) {
    delegate_->OnIPLookupRequestFailure();
    return;
  }

  std::string json_data;
  fetcher->GetResponseAsString(&json_data);

  // The parser will call us back via one of the callbacks.
  data_decoder::SafeJsonParser::Parse(
    content::ServiceManagerConnection::GetForProcess()->GetConnector(),
    json_data,
    base::Bind(&YSPIPLookupFetcher::OnJsonParseSuccess, AsWeakPtr()),
    base::Bind(&YSPIPLookupFetcher::OnJsonParseFailure, AsWeakPtr()));
}
#endif //REDCORE
