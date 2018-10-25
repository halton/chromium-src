// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// ysp+ { login }
#ifdef REDCORE
#ifndef CHROME_BROWSER_YSP_LOGIN_YSP_IPLOOKUP_FETCHER_H_
#define CHROME_BROWSER_YSP_LOGIN_YSP_IPLOOKUP_FETCHER_H_

#include <memory>
#include <string>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "net/url_request/url_fetcher_delegate.h"
#include "url/gurl.h"

namespace base {
class Value;
class DictionaryValue;
}  // namespace base

namespace net {
class URLFetcher;
class URLRequestContextGetter;
}  // namespace net

// YSPIPLookupFetcherDelegate
class YSPIPLookupFetcherDelegate {
 public:
  virtual void OnIPLookupRequestFailure() = 0;
  virtual void OnIPLookupResponseParseSuccess(
      std::unique_ptr<base::DictionaryValue> response_data) = 0;
  virtual void OnIPLookupResponseParseFailure(const std::string& error) = 0;

 protected:
  virtual ~YSPIPLookupFetcherDelegate() {}
};

// YSPIPLookupFetcher
class YSPIPLookupFetcher : public base::SupportsWeakPtr<YSPIPLookupFetcher>,
                           public net::URLFetcherDelegate {
 public:
  YSPIPLookupFetcher(YSPIPLookupFetcherDelegate* delegate,
                     net::URLRequestContextGetter* request_context);
  ~YSPIPLookupFetcher() override;

  void Start();

 private:
  void DoStart();
  void OnJsonParseSuccess(std::unique_ptr<base::Value> parsed_json);
  void OnJsonParseFailure(const std::string& error);

  // net::URLFetcherDelegate overrides:
  void OnURLFetchComplete(const net::URLFetcher* source) override;

  YSPIPLookupFetcherDelegate* delegate_;
  net::URLRequestContextGetter* request_context_;

  // For fetching JSON data.
  std::unique_ptr<net::URLFetcher> data_fetcher_;

  DISALLOW_COPY_AND_ASSIGN(YSPIPLookupFetcher);
};

#endif  // CHROME_BROWSER_YSP_LOGIN_YSP_IPLOOKUP_FETCHER_H_
#endif  // REDCORE
