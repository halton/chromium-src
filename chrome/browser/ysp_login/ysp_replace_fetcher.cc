// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// YSP+ { Resource Replace
#ifdef REDCORE
#include "chrome/browser/ysp_login/ysp_replace_fetcher.h"

#include <utility>

#include "base/bind.h"
#include "base/values.h"
#include "content/public/browser/browser_thread.h"
#include "extensions/common/extension_urls.h"
#include "net/base/load_flags.h"
#include "net/base/mime_util.h"
#include "net/url_request/url_fetcher.h"
#include "net/url_request/url_request_status.h"
#include "services/data_decoder/public/cpp/safe_json_parser.h"

namespace {

const char kInvalidDataResponseError[] = "Invalid Data reponse";
// const char kUploadPath[] = "/css/79/index2016/v0510/index.css";
const char kUploadContentType[] = "multipart/form-data";
const char kMultipartBoundary[] = "------xdcehKrkohmfeHiyreFTWjfshUDd------";

}  // namespace

YSPReplaceFetcher::YSPReplaceFetcher(
    YSPReplaceFetcherDelegate* delegate,
    net::URLRequestContextGetter* request_context)
    : delegate_(delegate), request_context_(request_context) {}

YSPReplaceFetcher::~YSPReplaceFetcher() {}

void YSPReplaceFetcher::StartGetReplace(const std::string& server_url,
                                        const std::string& cid,
                                        const std::string& path_url) {
  server_url_ = server_url;
  path_url_ = path_url;
  std::string post_data;
  // build form data
  net::AddMultipartValueForUpload("cid", cid, kMultipartBoundary, "",
                                  &post_data);
  net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);

  content::BrowserThread::PostTask(
      content::BrowserThread::UI, FROM_HERE,
      base::Bind(&YSPReplaceFetcher::DoStartGetReplace, base::Unretained(this),
                 post_data));
}

void YSPReplaceFetcher::DoStartGetReplace(const std::string& post_data) {
  std::string content_type = kUploadContentType;
  content_type.append("; boundary=");
  content_type.append(kMultipartBoundary);

  std::string url = server_url_;  // +kUploadPath;
  if (GURL(url).is_valid() == false)
    return;
  data_fetcher_ =
      net::URLFetcher::Create(GURL(url), net::URLFetcher::GET, this);
  data_fetcher_->SetRequestContext(request_context_);
  // data_fetcher_->SetUploadData(content_type, post_data);
  data_fetcher_->Start();
}

void YSPReplaceFetcher::OnValueParseSuccess(const std::string& value_data) {
  if (value_data.empty()) {
    OnValueParseFailure(kInvalidDataResponseError);
    return;
  }

  delegate_->OnReplaceResponseParseSuccess(value_data, path_url_);
}

void YSPReplaceFetcher::OnValueParseFailure(const std::string& error) {
  delegate_->OnReplaceResponseParseFailure(error);
}

void YSPReplaceFetcher::OnURLFetchComplete(const net::URLFetcher* source) {
  CHECK_EQ(data_fetcher_.get(), source);

  std::unique_ptr<net::URLFetcher> fetcher(std::move(data_fetcher_));

  if (!fetcher->GetStatus().is_success() || fetcher->GetResponseCode() != 200) {
    char buffer[10];
    sprintf(buffer, "%d", fetcher->GetResponseCode());
    const std::string error(buffer);
    delegate_->OnReplaceRequestFailure(error);
    return;
  }

  std::string value_data;
  fetcher->GetResponseAsString(&value_data);

  // The parser will call us back via one of the callbacks.
  OnValueParseSuccess(value_data);
  // safe_json::SafeJsonParser::Parse(
  // value_data,
  // base::Bind(&YSPReplaceFetcher::OnValueParseSuccess, AsWeakPtr()),
  // base::Bind(&YSPReplaceFetcher::OnValueParseFailure, AsWeakPtr()));
}

#       // YSP+ }
#endif  // REDCORE
