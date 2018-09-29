//ysp+ { auto get config }

#ifdef REDCORE
#ifndef CHROME_BROWSER_YSP_LOGIN_YSP_AUTO_CONFIG_FETCHER_H_
#define CHROME_BROWSER_YSP_LOGIN_YSP_AUTO_CONFIG_FETCHER_H_

#include <memory>
#include <string>
#include <vector>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "net/url_request/url_fetcher_delegate.h"
#include "url/gurl.h"
#include "chrome/browser/ysp_login/ysp_login_fetcher.h"

namespace base {
  class Value;
  class DictionaryValue;
}

namespace net {
  class URLFetcher;
  class URLRequestContextGetter;
}

// YSPAutoConfigFetcher
class YSPAutoConfigFetcher : public base::SupportsWeakPtr<YSPAutoConfigFetcher>,
  public net::URLFetcherDelegate {
public:
  YSPAutoConfigFetcher(net::URLRequestContextGetter* request_context);
  ~YSPAutoConfigFetcher() override;

  void StartGetConfigVersion(const std::string& server_url, const std::vector<std::string> header_list, bool auto_fetch);

private:
  void DoStartGetConfigVersion();
  void OnJsonParseSuccess(std::unique_ptr<base::Value> parsed_json);
  void OnJsonParseFailure(const std::string& error);

  // net::URLFetcherDelegate overrides:
  void OnURLFetchComplete(const net::URLFetcher* source) override;

  //YSPAutoConfigFetcherDelegate* delegate_;
  net::URLRequestContextGetter* request_context_;

  // For fetching JSON data.
  std::string server_url_;
  std::string userId_;
  std::vector<std::string> header_list_;
  std::unique_ptr<net::URLFetcher> data_fetcher_;
  bool auto_fetch_;

  DISALLOW_COPY_AND_ASSIGN(YSPAutoConfigFetcher);
};

#endif  // CHROME_BROWSER_YSP_LOGIN_YS_AUTO_CONFIG_FETCHER_H_
#endif  // REDCORE
