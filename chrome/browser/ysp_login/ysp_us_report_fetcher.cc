#include "chrome/browser/ysp_login/ysp_us_report_fetcher.h"

#include <utility>

#include "base/bind.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "components/version_info/version_info.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/common/service_manager_connection.h"
#include "extensions/common/extension_urls.h"
#include "services/data_decoder/public/cpp/safe_json_parser.h"
#include "net/base/load_flags.h"
#include "net/base/mime_util.h"
#include "net/url_request/url_fetcher.h"
#include "net/url_request/url_request_status.h"

namespace {

const char kInvalidDataResponseError[] = "Invalid Data reponse";
const char kUploadPath[] = "/v2/addPcStatistics";
const char kUploadContentType[] = "multipart/form-data";
const char kMultipartBoundary[] = "------xdcehKrkohmfetirroFpWefrhsDu------";

}  // namespace

YSPUSReportFetcher::YSPUSReportFetcher(
  net::URLRequestContextGetter* request_context)
  : request_context_(request_context) {
}

YSPUSReportFetcher::~YSPUSReportFetcher() {
}

void YSPUSReportFetcher::StartReport(
    const std::string& server_url,
    const std::string& companyId,
    const base::string16& companyName,
    const std::string& departmentId,
    const base::string16& departmentName,
    const std::string& uuid,
    const base::string16& userName,
    const std::string& deviceId,
    const std::string& access_url) {

  unsigned long long tm = base::Time::Now().ToJavaTime();
  if((tm - last_timestamp_) < 500 && access_url == last_access_url_) {
    last_timestamp_ = tm;
    return;
  }
  last_timestamp_ = tm;
  last_access_url_ = access_url;

  server_url_ = server_url;
  std::string post_data;
  std::string sObj;
  std::string version = version_info::GetVersionNumber();

  static char timestamp[64];
  snprintf(timestamp, sizeof(timestamp), "%lld", tm);

  // build data
  sObj += "[{\"companyId\":\"";
  sObj += companyId;
  sObj += "\",\"uuid\":\"";
  sObj += uuid;
  sObj += "\",\"departmentId\":\"";
  sObj += departmentId;
  sObj += "\",\"userName\":\"";
  sObj += base::UTF16ToUTF8(userName);
  sObj += "\",\"companyName\":\"";
  sObj += base::UTF16ToUTF8(companyName);
  sObj += "\",\"departmentName\":\"";
  sObj += base::UTF16ToUTF8(departmentName);
  sObj += "\",\"deviceId\":\"";
  sObj += deviceId;
  sObj += "\",\"url\":\"";
  sObj += access_url;
  sObj += "\",\"time\":";
  sObj += timestamp;
  sObj += ",\"version\":\"";
  sObj += version;
  sObj += "\"}]";

  net::AddMultipartValueForUpload("sObj", sObj, kMultipartBoundary, "", &post_data);
  net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);

  content::BrowserThread::PostTask(
      content::BrowserThread::UI, FROM_HERE,
      base::Bind(&YSPUSReportFetcher::DoStartReport,
                 base::Unretained(this), post_data));
}

void YSPUSReportFetcher::DoStartReport(const std::string& post_data) {
  std::string content_type = kUploadContentType;
  content_type.append("; boundary=");
  content_type.append(kMultipartBoundary);

  std::string url = server_url_ + kUploadPath;
  GURL requestUrl(url);
  if (!requestUrl.is_valid()) {
    return;
  }
  data_fetcher_ =
      net::URLFetcher::Create(requestUrl, net::URLFetcher::POST, this);
  data_fetcher_->SetRequestContext(request_context_);
  data_fetcher_->SetUploadData(content_type, post_data);
  data_fetcher_->Start();
}

void YSPUSReportFetcher::OnJsonParseSuccess(
  std::unique_ptr<base::Value> parsed_json) {
  if (!parsed_json->is_dict()) {
    OnJsonParseFailure(kInvalidDataResponseError);
    return;
  }
}

void YSPUSReportFetcher::OnJsonParseFailure(
  const std::string& error) {
}

void YSPUSReportFetcher::OnURLFetchComplete(const net::URLFetcher* source) {
  CHECK_EQ(data_fetcher_.get(), source);

  std::unique_ptr<net::URLFetcher> fetcher(std::move(data_fetcher_));

  if (!fetcher->GetStatus().is_success() ||
      fetcher->GetResponseCode() != 200) {
    return;
  }

  std::string json_data;
  fetcher->GetResponseAsString(&json_data);

  // The parser will call us back via one of the callbacks.
  data_decoder::SafeJsonParser::Parse(
      content::ServiceManagerConnection::GetForProcess()->GetConnector(),
      json_data,
      base::Bind(&YSPUSReportFetcher::OnJsonParseSuccess, AsWeakPtr()),
      base::Bind(&YSPUSReportFetcher::OnJsonParseFailure, AsWeakPtr()));
}
