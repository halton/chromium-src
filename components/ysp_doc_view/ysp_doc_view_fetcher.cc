// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/ysp_doc_view/ysp_doc_view_fetcher.h"

#include <string.h>

#include "base/files/file_util.h"
#include "base/values.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/common/service_manager_connection.h"
#include "net/base/mime_util.h"
#include "net/url_request/url_fetcher.h"
#include "services/data_decoder/public/cpp/safe_json_parser.h"

namespace {

const char kInvalidDataResponseError[] = "Invalid Data response";
const char kUploadUrl[] = "/dcs.web/upload";
const char kUploadContentType[] = "multipart/form-data";
const char kMultipartBoundary[] = "------xdcehKrkohmfeHiwreFiAVfchoDd------";

}  // namespace

YSPDocViewFetcher::YSPDocViewFetcher(
    YSPDocViewFetcherDelegate* delegate,
    net::URLRequestContextGetter* request_context)
    : delegate_(delegate),
      request_context_(request_context) {
}

YSPDocViewFetcher::~YSPDocViewFetcher() {
}

void YSPDocViewFetcher::Start(const std::string& upload_url,
                              const base::FilePath& local_path) {
  upload_url_ = upload_url;
  if (!base::PathExists(local_path))
    return;

  std::string file_data;
  if (!base::ReadFileToString(local_path, &file_data))
    return;

  std::string convert_type = "1";
  if (local_path.MatchesExtension(FILE_PATH_LITERAL(".pdf"))) {
    convert_type = "14";
  }
  std::string post_data;
  // build form data
  net::AddMultipartValueForUpload("convertType", convert_type,
                                  kMultipartBoundary, "", &post_data);
  // file header
  post_data.append("--");
  post_data.append(kMultipartBoundary);
  post_data.append("\r\nContent-Disposition: form-data; name=\"file");
  post_data.append("\"; filename=\"");
  post_data.append(local_path.BaseName().AsUTF8Unsafe());
  post_data.append("\"\r\nContent-Type: application/octet-stream\r\n\r\n");
  // file data
  post_data.append(file_data.data(), file_data.size());
  post_data.append("\r\n");

  net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);

  content::BrowserThread::PostTask(
      content::BrowserThread::UI, FROM_HERE,
      base::Bind(&YSPDocViewFetcher::DoStart, base::Unretained(this),
                 post_data));
}

void YSPDocViewFetcher::DoStart(const std::string& post_data) {
  std::string content_type = kUploadContentType;
  content_type.append("; boundary=");
  content_type.append(kMultipartBoundary);

  GURL requestUrl(upload_url_ + kUploadUrl);
  if (!requestUrl.is_valid())
    return;

  data_fetcher_ = net::URLFetcher::Create(
      requestUrl, net::URLFetcher::POST, this);
  data_fetcher_->SetRequestContext(request_context_);
  data_fetcher_->SetUploadData(content_type, post_data);
  data_fetcher_->Start();
}

void YSPDocViewFetcher::OnJsonParseSuccess(
    std::unique_ptr<base::Value> parsed_json) {
  if (!parsed_json->is_dict()) {
    OnJsonParseFailure(kInvalidDataResponseError);
    return;
  }

  delegate_->OnDocViewResponseParseSuccess(
      std::unique_ptr<base::DictionaryValue>(
          static_cast<base::DictionaryValue*>(parsed_json.release())));
}

void YSPDocViewFetcher::OnJsonParseFailure(const std::string& error) {
  delegate_->OnDocViewResponseParseFailure(error);
}

void YSPDocViewFetcher::OnURLFetchComplete(const net::URLFetcher* source) {
  CHECK_EQ(data_fetcher_.get(), source);

  std::unique_ptr<net::URLFetcher> fetcher(std::move(data_fetcher_));

  if (!fetcher->GetStatus().is_success() ||
      fetcher->GetResponseCode() != 200) {
    delegate_->OnDocViewRequestFailure();
    return;
  }

  std::string json_data;
  fetcher->GetResponseAsString(&json_data);

  // The parser will call us back via one of the callbacks.
  data_decoder::SafeJsonParser::Parse(
      content::ServiceManagerConnection::GetForProcess()->GetConnector(),
      json_data,
      base::Bind(&YSPDocViewFetcher::OnJsonParseSuccess, AsWeakPtr()),
      base::Bind(&YSPDocViewFetcher::OnJsonParseFailure, AsWeakPtr()));
}
