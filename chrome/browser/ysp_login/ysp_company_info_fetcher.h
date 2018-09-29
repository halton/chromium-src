//ysp+ { login }
#ifdef REDCORE
#ifndef CHROME_BROWSER_YSP_LOGIN_YSP_COMPANY_INFO_FETCHER_H_
#define CHROME_BROWSER_YSP_LOGIN_YSP_COMPANY_INFO_FETCHER_H_

#include <memory>
#include <string>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "net/url_request/url_fetcher_delegate.h"
#include "url/gurl.h"

namespace base {
  class Value;
  class DictionaryValue;
}

namespace net {
  class URLFetcher;
  class URLRequestContextGetter;
}

// YSPCompanyInfoFetcherDelegate
class YSPCompanyInfoFetcherDelegate {
public:
  virtual void OnCompanyInfoRequestFailure() = 0;
  virtual void OnCompanyInfoResponseParseSuccess(
    std::unique_ptr<base::DictionaryValue> response_data) = 0;
  virtual void OnCompanyInfoResponseParseFailure(const std::string& error) = 0;

protected:
  virtual ~YSPCompanyInfoFetcherDelegate() {}
};

// YSPCompanyInfoFetcher
class YSPCompanyInfoFetcher : public base::SupportsWeakPtr<YSPCompanyInfoFetcher>,
  public net::URLFetcherDelegate {
public:
  YSPCompanyInfoFetcher(YSPCompanyInfoFetcherDelegate* delegate,
    net::URLRequestContextGetter* request_context);
  ~YSPCompanyInfoFetcher() override;

  void StartGetCompanyInfo(
    const std::string& serverUrl,
    const std::string& cid);

private:
  void DoStartGetCompanyInfo(const std::string& post_data);
  void OnJsonParseSuccess(std::unique_ptr<base::Value> parsed_json);
  void OnJsonParseFailure(const std::string& error);

  // net::URLFetcherDelegate overrides:
  void OnURLFetchComplete(const net::URLFetcher* source) override;

  YSPCompanyInfoFetcherDelegate* delegate_;
  net::URLRequestContextGetter* request_context_;

  // For fetching JSON data.
  std::unique_ptr<net::URLFetcher> data_fetcher_;
  std::string server_url_;

  DISALLOW_COPY_AND_ASSIGN(YSPCompanyInfoFetcher);
};

#endif  // CHROME_BROWSER_YSP_LOGIN_YS_COMPANY_INFO_FETCHER_H_
#endif  // REDCORE
