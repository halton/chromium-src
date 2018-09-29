#ifndef CHROME_BROWSER_YSP_LOGIN_YSP_US_REPORT_FETCHER_H_
#define CHROME_BROWSER_YSP_LOGIN_YSP_US_REPORT_FETCHER_H_

#include <memory>
#include <string>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "net/url_request/url_fetcher_delegate.h"
#include "url/gurl.h"

namespace base {
class Value;
class DictionaryValue;
} // namespace base

namespace net {
class URLFetcher;
class URLRequestContextGetter;
} // namespace net

// YSPUSReportFetcher
class YSPUSReportFetcher : public base::SupportsWeakPtr<YSPUSReportFetcher>,
                           public net::URLFetcherDelegate {
 public:
  YSPUSReportFetcher(net::URLRequestContextGetter* request_context);
  ~YSPUSReportFetcher() override;

  void StartReport(
      const std::string& server_url,
      const std::string& companyId,
      const base::string16& companyName,
      const std::string& departmentId,
      const base::string16& departmentName,
      const std::string& uuid,
      const base::string16& userName,
      const std::string& deviceId,
      const std::string& access_url);

 private:
  void DoStartReport(const std::string& post_data);
  void OnJsonParseSuccess(std::unique_ptr<base::Value> parsed_json);
  void OnJsonParseFailure(const std::string& error);

  // net::URLFetcherDelegate overrides:
  void OnURLFetchComplete(const net::URLFetcher* source) override;

  net::URLRequestContextGetter* request_context_;

  // For fetching JSON data.
  std::string server_url_;
  std::unique_ptr<net::URLFetcher> data_fetcher_;

  std::string last_access_url_;
  unsigned long long last_timestamp_;

  DISALLOW_COPY_AND_ASSIGN(YSPUSReportFetcher);
};

#endif  // CHROME_BROWSER_YSP_LOGIN_YSP_US_REPORT_FETCHER_H_
