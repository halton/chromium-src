#ifdef REDCORE
//ysp+

#ifndef CHROME_BROWSER_YSP_UPDATE_YS_UPDATE_FETCHER_H_
#define CHROME_BROWSER_YSP_UPDATE_YS_UPDATE_FETCHER_H_

#include <memory>
#include <string>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "base/files/file_path.h"
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

// YSPUpdateFetcherDelegate
class YSPUpdateFetcherDelegate {
public:
  virtual void OnUpdateRequestFailure() = 0;
  virtual void OnUpdateResponseParseSuccess(
    std::unique_ptr<base::DictionaryValue> response_data) = 0;
  virtual void OnUpdateResponseParseFailure(const std::string& error) = 0;

protected:
  virtual ~YSPUpdateFetcherDelegate() {}
};

// YSPUpdateFetcher
class YSPUpdateFetcher : public base::SupportsWeakPtr<YSPUpdateFetcher>,
  public net::URLFetcherDelegate {
public:
  YSPUpdateFetcher(YSPUpdateFetcherDelegate* delegate,
    net::URLRequestContextGetter* request_context);
  ~YSPUpdateFetcher() override;

  void StartCheck(const std::string& server_url,
                  const std::string& userId,
                  const std::string& companyId,
				  const std::string& accessToken);

private:
  void DoStartCheck();
  void OnJsonParseSuccess(std::unique_ptr<base::Value> parsed_json);
  void OnJsonParseFailure(const std::string& error);

  // net::URLFetcherDelegate overrides:
  void OnURLFetchComplete(const net::URLFetcher* source) override;

  YSPUpdateFetcherDelegate* delegate_;
  net::URLRequestContextGetter* request_context_;

  // For fetching JSON data.
  std::string server_url_;
  std::string companyId_, userId_, accessToken_;
  std::unique_ptr<net::URLFetcher> data_fetcher_;

  DISALLOW_COPY_AND_ASSIGN(YSPUpdateFetcher);
};

#endif  // CHROME_BROWSER_YSP_UPDATE_YS_UPDATE_FETCHER_H_

#endif
