// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// ysp+ { login }
#ifdef REDCORE
#include "chrome/browser/ysp_login/ysp_company_info_fetcher.h"

#include <utility>

#include "base/bind.h"
#include "base/values.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/common/service_manager_connection.h"
#include "extensions/common/extension_urls.h"
#include "net/base/load_flags.h"
#include "net/base/mime_util.h"
#include "net/url_request/url_fetcher.h"
#include "net/url_request/url_request_status.h"
#include "services/data_decoder/public/cpp/safe_json_parser.h"

namespace {

const char kInvalidDataResponseError[] = "Invalid Data reponse";
const char kUploadPath[] = "/v2/companyInfo";
const char kUploadContentType[] = "multipart/form-data";
const char kMultipartBoundary[] = "------xdcehKrkohmfeHiyreFTWjfshUDd------";

}  // namespace

YSPCompanyInfoFetcher::YSPCompanyInfoFetcher(
    YSPCompanyInfoFetcherDelegate* delegate,
    net::URLRequestContextGetter* request_context)
    : delegate_(delegate), request_context_(request_context) {}

YSPCompanyInfoFetcher::~YSPCompanyInfoFetcher() {}

void YSPCompanyInfoFetcher::StartGetCompanyInfo(const std::string& server_url,
                                                const std::string& cid) {
  server_url_ = server_url;
  std::string post_data;
  // build form data
  net::AddMultipartValueForUpload("cid", cid, kMultipartBoundary, "",
                                  &post_data);
  net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);

  content::BrowserThread::PostTask(
      content::BrowserThread::UI, FROM_HERE,
      base::Bind(&YSPCompanyInfoFetcher::DoStartGetCompanyInfo,
                 base::Unretained(this), post_data));
}

void YSPCompanyInfoFetcher::DoStartGetCompanyInfo(
    const std::string& post_data) {
  std::string content_type = kUploadContentType;
  content_type.append("; boundary=");
  content_type.append(kMultipartBoundary);

  std::string url = server_url_ + kUploadPath;
  data_fetcher_ =
      net::URLFetcher::Create(GURL(url), net::URLFetcher::POST, this);
  data_fetcher_->SetRequestContext(request_context_);
  data_fetcher_->SetUploadData(content_type, post_data);
  data_fetcher_->Start();
}

void YSPCompanyInfoFetcher::OnJsonParseSuccess(
    std::unique_ptr<base::Value> parsed_json) {
  if (!parsed_json->is_dict()) {
    OnJsonParseFailure(kInvalidDataResponseError);
    return;
  }

  delegate_->OnCompanyInfoResponseParseSuccess(
      std::unique_ptr<base::DictionaryValue>(
          static_cast<base::DictionaryValue*>(parsed_json.release())));
}

void YSPCompanyInfoFetcher::OnJsonParseFailure(const std::string& error) {
  delegate_->OnCompanyInfoResponseParseFailure(error);
}

void YSPCompanyInfoFetcher::OnURLFetchComplete(const net::URLFetcher* source) {
  CHECK_EQ(data_fetcher_.get(), source);

  std::unique_ptr<net::URLFetcher> fetcher(std::move(data_fetcher_));

  if (!fetcher->GetStatus().is_success() || fetcher->GetResponseCode() != 200) {
    delegate_->OnCompanyInfoRequestFailure();
    return;
  }

  std::string json_data;
  fetcher->GetResponseAsString(&json_data);

  // The parser will call us back via one of the callbacks.
  data_decoder::SafeJsonParser::Parse(
      content::ServiceManagerConnection::GetForProcess()->GetConnector(),
      json_data,
      base::Bind(&YSPCompanyInfoFetcher::OnJsonParseSuccess, AsWeakPtr()),
      base::Bind(&YSPCompanyInfoFetcher::OnJsonParseFailure, AsWeakPtr()));
}
#endif  // REDCORE
