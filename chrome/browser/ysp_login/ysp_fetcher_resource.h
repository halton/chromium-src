//ysp+ { fetcher resource }
#ifdef REDCORE
#ifndef CHROME_BROWSER_YSP_LOGIN_YSP_FETCHER_RESOURCE_H_
#define CHROME_BROWSER_YSP_LOGIN_YSP_FETCHER_RESOURCE_H_

#include <memory>
#include <string>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "net/url_request/url_fetcher_delegate.h"
#include "net/url_request/url_fetcher.h"
#include "url/gurl.h"

namespace base {
  class Value;
  class DictionaryValue;
}

namespace net {
  class URLFetcher;
  class URLRequestContextGetter;
}

// YSPFetcherResourceDelegate
class YSPFetcherResourceDelegate {
public:
  virtual void OnFetcherResourceRequestFailure(const GURL& url, bool auto_fecth, const std::string& error) = 0;
  virtual void OnFetcherResourceResponseParseSuccess(const GURL& url,
    std::unique_ptr<base::DictionaryValue> response_data, bool auto_fecth) = 0;
  virtual void OnFetcherResourceResponseParseFailure(const GURL& url, bool auto_fecth, const std::string& error) = 0;

protected:
  virtual ~YSPFetcherResourceDelegate() {}
};

// YSPFetcherResource
class YSPFetcherResource : public base::SupportsWeakPtr<YSPFetcherResource>,
  public net::URLFetcherDelegate {
public:
  YSPFetcherResource(YSPFetcherResourceDelegate* delegate,
    net::URLRequestContextGetter* request_context);
  ~YSPFetcherResource() override;

  void StarFetcherResource(
    const net::URLFetcher::RequestType& request_type,
    const std::string& server_url,
    const std::vector<std::string> header_list,
    const std::string& post_data,
    bool auto_fetch);
  bool IsLoading() { return loading; }

private:
  void DoStartFetcherResource(const std::string& post_data);
  void OnJsonParseSuccess(std::unique_ptr<base::Value> parsed_json);
  void OnJsonParseFailure(const std::string& error);

  // net::URLFetcherDelegate overrides:
  void OnURLFetchComplete(const net::URLFetcher* source) override;

  YSPFetcherResourceDelegate* delegate_;
  net::URLRequestContextGetter* request_context_;

  // For fetching JSON data.
  std::string server_url_;
  GURL original_url_;
  std::vector<std::string> header_list_;
  net::URLFetcher::RequestType request_type_;
  std::unique_ptr<net::URLFetcher> data_fetcher_;
  bool auto_fetch_;
  bool loading = false;

  DISALLOW_COPY_AND_ASSIGN(YSPFetcherResource);
};

#endif  // CHROME_BROWSER_YSP_LOGIN_YSP_FETCHER_RESOURCE_H_
#endif  // REDCORE
