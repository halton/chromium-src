//YSP+ { Resource Replace }

#ifndef CONTENT_BROWSER_YSP_RESOURCE_REPLACE_INTERCEPTOR_H_
#define CONTENT_BROWSER_YSP_RESOURCE_REPLACE_INTERCEPTOR_H_

#include <stdint.h>

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "base/values.h"
#include "content/common/content_export.h"
#include "content/public/common/resource_type.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_delegate.h"
#include "net/url_request/url_request_interceptor.h"

class GURL;

namespace net {
class URLRequest;
class URLRequestFileJob;
}

namespace content {
class AppCacheRequestHandler;
class AppCacheServiceImpl;
class webcontents;

// An interceptor to hijack requests and potentially service them out of
// the appcache.
class CONTENT_EXPORT YSPResourceReplaceInterceptor : public net::URLRequestInterceptor {
 public:
  YSPResourceReplaceInterceptor();
  ~YSPResourceReplaceInterceptor() override;
  static void SetResourceReplaceValue(const std::string& resourceReplace);
  static void SetValueFormPostTask(const std::string& resourceReplace);

 protected:
  // Override from net::URLRequestInterceptor:
  net::URLRequestJob* MaybeInterceptRequest(
      net::URLRequest* request,
      net::NetworkDelegate* network_delegate) const override;

 private:
   //static scoped_ptr<base::DictionaryValue> resourceReplace_;
   static YSPResourceReplaceInterceptor* GetHandler(net::URLRequest* request);
   std::string ResourceReplaceCompared(const std::string url) const;

  DISALLOW_COPY_AND_ASSIGN(YSPResourceReplaceInterceptor);
};

}  // namespace content

#endif  // CONTENT_BROWSER_YSP_RESOURCE_REPLACE_INTERCEPTOR_H_
