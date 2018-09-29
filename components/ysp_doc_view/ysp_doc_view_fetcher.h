//ysp+
#ifdef REDCORE
#ifndef COMPONENTS_YSP_DOC_VIEW_YS_DOC_VIEW_FETCHER_H_
#define COMPONENTS_YSP_DOC_VIEW_YS_DOC_VIEW_FETCHER_H_

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

// YSPDocViewFetcherDelegate
class YSPDocViewFetcherDelegate {
public:
  virtual void OnDocViewRequestFailure() = 0;
  virtual void OnDocViewResponseParseSuccess(
    std::unique_ptr<base::DictionaryValue> response_data) = 0;
  virtual void OnDocViewResponseParseFailure(const std::string& error) = 0;

protected:
  virtual ~YSPDocViewFetcherDelegate() {}
};

// YSPDocViewFetcher
class YSPDocViewFetcher : public base::SupportsWeakPtr<YSPDocViewFetcher>,
  public net::URLFetcherDelegate {
public:
  YSPDocViewFetcher(YSPDocViewFetcherDelegate* delegate,
    net::URLRequestContextGetter* request_context);
  ~YSPDocViewFetcher() override;

  void Start(const std::string& uploadUrl, const base::FilePath& localPath);

private:
  void DoStart(const std::string& post_data);
  void OnJsonParseSuccess(std::unique_ptr<base::Value> parsed_json);
  void OnJsonParseFailure(const std::string& error);

  // net::URLFetcherDelegate overrides:
  void OnURLFetchComplete(const net::URLFetcher* source) override;

  YSPDocViewFetcherDelegate* delegate_;
  net::URLRequestContextGetter* request_context_;

  // For fetching JSON data.
  std::unique_ptr<net::URLFetcher> data_fetcher_;
  std::string upload_url_;

  DISALLOW_COPY_AND_ASSIGN(YSPDocViewFetcher);
};

#endif  // COMPONENTS_YSP_DOC_VIEW_YS_DOC_VIEW_FETCHER_H_
#endif // REDCORE
