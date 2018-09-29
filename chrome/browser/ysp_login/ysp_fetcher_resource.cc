//ysp+ { fetcher resource }
#ifdef REDCORE
#include "chrome/browser/ysp_login/ysp_fetcher_resource.h"

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
  const char kUploadContentType[] = "multipart/form-data";
  const char kMultipartBoundary[] = "------xdcehKrkohmfeHiyreFnWifghoDl------";

}  // namespace
YSPFetcherResource::YSPFetcherResource(
  YSPFetcherResourceDelegate* delegate,
  net::URLRequestContextGetter* request_context)
  : delegate_(delegate),
    request_context_(request_context) {
}

YSPFetcherResource::~YSPFetcherResource() {}

void YSPFetcherResource::StarFetcherResource(
  const net::URLFetcher::RequestType& request_type,
  const std::string& server_url,
  const std::vector<std::string> header_list,
  const std::string& post_data,
  bool auto_fetch) {
  server_url_ = server_url;
  header_list_ = header_list;
  request_type_ = request_type;
  auto_fetch_ = auto_fetch;
  loading = true;

  content::BrowserThread::PostTask(
    content::BrowserThread::UI, FROM_HERE,
    base::Bind(&YSPFetcherResource::DoStartFetcherResource,
      base::Unretained(this), post_data));
}

void YSPFetcherResource::DoStartFetcherResource(const std::string& post_data) {
  GURL requestUrl(server_url_);
  if (!requestUrl.is_valid()) {
    return;
  }
  data_fetcher_ =
    net::URLFetcher::Create(requestUrl, request_type_, this);
  data_fetcher_->SetRequestContext(request_context_);
  if (request_type_ == net::URLFetcher::POST || request_type_ == net::URLFetcher::PUT) {
    std::string content_type = kUploadContentType;
    content_type.append("; boundary=");
    content_type.append(kMultipartBoundary);
    data_fetcher_->SetUploadData(content_type, post_data);
  }
  if (!header_list_.empty()) {
    std::vector<std::string>::iterator iter = header_list_.begin();
    for (; iter != header_list_.end(); ++iter)
      data_fetcher_->AddExtraRequestHeader(*iter);
  }
  data_fetcher_->Start();
}

void YSPFetcherResource::OnJsonParseSuccess(
  std::unique_ptr<base::Value> parsed_json) {
  if (!parsed_json->is_dict()) {
    OnJsonParseFailure(kInvalidDataResponseError);
    return;
  }
  delegate_->OnFetcherResourceResponseParseSuccess(original_url_,
    std::unique_ptr<base::DictionaryValue>(
      static_cast<base::DictionaryValue*>(parsed_json.release())), auto_fetch_);
  loading = false;
}

void YSPFetcherResource::OnJsonParseFailure(
  const std::string& error) {
  delegate_->OnFetcherResourceResponseParseFailure(original_url_, auto_fetch_, error);
  loading = false;
}

void YSPFetcherResource::OnURLFetchComplete(const net::URLFetcher* source) {
  CHECK_EQ(data_fetcher_.get(), source);

  std::unique_ptr<net::URLFetcher> fetcher(
    std::move(data_fetcher_));
  original_url_ = fetcher->GetOriginalURL();
  DLOG(INFO) << "original_url: " << original_url_;
  if (!fetcher->GetStatus().is_success() ||
    fetcher->GetResponseCode() != 200) {
    char buffer[10];
    sprintf(buffer, "%d",fetcher->GetResponseCode());
    const std::string error(buffer);
    delegate_->OnFetcherResourceRequestFailure(original_url_, auto_fetch_, error);
    return;
  }

  std::string json_data;
  fetcher->GetResponseAsString(&json_data);

  // The parser will call us back via one of the callbacks.
  data_decoder::SafeJsonParser::Parse(
    content::ServiceManagerConnection::GetForProcess()->GetConnector(),
    json_data,
    base::Bind(&YSPFetcherResource::OnJsonParseSuccess, AsWeakPtr()),
    base::Bind(&YSPFetcherResource::OnJsonParseFailure, AsWeakPtr()));
  loading = false;
}
#endif  // REDCORE
