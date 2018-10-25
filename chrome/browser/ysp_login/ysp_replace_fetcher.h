// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// YSP+ { Resource Replace
#ifdef REDCORE
#ifndef CHROME_BROWSER_YSP_LOGIN_YSP_REPLACE_FETCHER_H_
#define CHROME_BROWSER_YSP_LOGIN_YSP_REPLACE_FETCHER_H_

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

// YSPReplaceFetcherDelegate
class YSPReplaceFetcherDelegate {
 public:
  virtual void OnReplaceRequestFailure(const std::string& error) = 0;
  virtual void OnReplaceResponseParseSuccess(const std::string& response_data,
                                             const std::string& path_url) = 0;
  virtual void OnReplaceResponseParseFailure(const std::string& error) = 0;

 protected:
  virtual ~YSPReplaceFetcherDelegate() {}
};

// YSPReplaceFetcher
class YSPReplaceFetcher : public base::SupportsWeakPtr<YSPReplaceFetcher>,
                          public net::URLFetcherDelegate {
 public:
  YSPReplaceFetcher(YSPReplaceFetcherDelegate* delegate,
                    net::URLRequestContextGetter* request_context);
  ~YSPReplaceFetcher() override;

  void StartGetReplace(const std::string& server_url,
                       const std::string& cid,
                       const std::string& path_url);

 private:
  void DoStartGetReplace(const std::string& post_data);
  // void OnJsonParseSuccess(std::unique_ptr<base::Value> parsed_json);
  void OnValueParseSuccess(const std::string& value_data);
  void OnValueParseFailure(const std::string& error);

  // net::URLFetcherDelegate overrides:
  void OnURLFetchComplete(const net::URLFetcher* source) override;

  YSPReplaceFetcherDelegate* delegate_;
  net::URLRequestContextGetter* request_context_;

  // For fetching JSON data.
  std::unique_ptr<net::URLFetcher> data_fetcher_;
  std::string server_url_;
  std::string path_url_;

  DISALLOW_COPY_AND_ASSIGN(YSPReplaceFetcher);
};

#endif  // CHROME_BROWSER_YSP_LOGIN_YSP_REPLACE_FETCHER_H_
#       // YSP+ }
#endif  // REDCORE
