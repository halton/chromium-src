// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// ysp+ { login }
#ifdef REDCORE
#ifndef CHROME_BROWSER_YSP_LOGIN_YSP_LOGIN_FETCHER_H_
#define CHROME_BROWSER_YSP_LOGIN_YSP_LOGIN_FETCHER_H_

#include <memory>
#include <string>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "chrome/browser/ysp_login/ysp_auto_config_fetcher.h"  // ysp+ { auto get config }
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

// YSPLoginFetcherDelegate
class YSPLoginFetcherDelegate {
 public:
  virtual void OnLoginRequestFailure(const std::string& error) = 0;
  virtual void OnLoginResponseParseSuccess(
      std::unique_ptr<base::DictionaryValue> response_data) = 0;
  virtual void OnLoginResponseParseFailure(const std::string& error) = 0;

 protected:
  virtual ~YSPLoginFetcherDelegate() {}
};

// YSPLoginFetcher
class YSPLoginFetcher : public base::SupportsWeakPtr<YSPLoginFetcher>,
                        public net::URLFetcherDelegate {
 public:
  YSPLoginFetcher(YSPLoginFetcherDelegate* delegate,
                  net::URLRequestContextGetter* request_context);
  ~YSPLoginFetcher() override;

  void StartLogin(const std::string& server_url,
                  const std::string& domain_name,
                  const std::string& uuid,
                  const std::string& password,
                  const std::vector<std::string> header_list,
                  const std::string& device_info);

 private:
  void DoAutoGetConfig();
  void DoStartLogin(const std::string& post_data);
  void OnJsonParseSuccess(std::unique_ptr<base::Value> parsed_json);
  void OnJsonParseFailure(const std::string& error);

  // net::URLFetcherDelegate overrides:
  void OnURLFetchComplete(const net::URLFetcher* source) override;

  YSPLoginFetcherDelegate* delegate_;
  net::URLRequestContextGetter* request_context_;

  // For fetching JSON data.
  std::string server_url_;
  std::string post_data_;  // ysp+ { auto get config }
  std::unique_ptr<net::URLFetcher> data_fetcher_;
  std::string auto_config_flags_old_ = "";    // ysp+ { auto get config }
  static std::string auto_config_flags_now_;  // ysp+ { auto get config }
  int frequency = 0;                          // ysp+ { auto get config }
  static bool auto_get_config_;               // ysp+ { auto get config }
  std::vector<std::string> header_list_;

  DISALLOW_COPY_AND_ASSIGN(YSPLoginFetcher);
};

#endif  // CHROME_BROWSER_YSP_LOGIN_YSP_LOGIN_FETCHER_H_
#endif  // REDCORE
