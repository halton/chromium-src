// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/download/public/common/download_url_parameters.h"

namespace download {

DownloadUrlParameters::DownloadUrlParameters(
    const GURL& url,
    net::URLRequestContextGetter* url_request_context_getter,
    const net::NetworkTrafficAnnotationTag& traffic_annotation)
    : DownloadUrlParameters(url,
                            -1,
                            -1,
                            -1,
                            url_request_context_getter,
                            traffic_annotation) {}

DownloadUrlParameters::DownloadUrlParameters(
    const GURL& url,
    int render_process_host_id,
    int render_view_host_routing_id,
    int render_frame_host_routing_id,
    net::URLRequestContextGetter* url_request_context_getter,
    const net::NetworkTrafficAnnotationTag& traffic_annotation)
    : content_initiated_(false),
      use_if_range_(true),
      method_("GET"),
      post_id_(-1),
      prefer_cache_(false),
      referrer_policy_(
          net::URLRequest::
              CLEAR_REFERRER_ON_TRANSITION_FROM_SECURE_TO_INSECURE),
      render_process_host_id_(render_process_host_id),
      render_view_host_routing_id_(render_view_host_routing_id),
      render_frame_host_routing_id_(render_frame_host_routing_id),
      url_request_context_getter_(url_request_context_getter),
      url_(url),
      do_not_prompt_for_login_(false),
      follow_cross_origin_redirects_(true),
      fetch_error_body_(false),
      transient_(false),
      traffic_annotation_(traffic_annotation),
#if defined(IE_REDCORE)
      download_source_(DownloadSource::UNKNOWN),
      use_ie_download_(false),
      ie_downloader_(nullptr),
      ie_response_header_(L"") {}
#else
      download_source_(DownloadSource::UNKNOWN) {}
#endif  // defined(IE_REDCORE)

DownloadUrlParameters::~DownloadUrlParameters() = default;

#if defined(IE_REDCORE)
void DownloadUrlParameters::SetIEDownloader(ie::IEDownloader* downloader) {
  ie_downloader_ = downloader;
  use_ie_download_ = true;
}

ie::IEDownloader* DownloadUrlParameters::IEDownloader() {
  return ie_downloader_;
}

bool DownloadUrlParameters::IsUseIEDownloader() {
  if (ie_downloader_ && use_ie_download_)
    return true;
  return false;
}

void DownloadUrlParameters::SetIEDownloadResponseheader(
    const std::wstring header) {
  ie_response_header_ = header;
}

std::wstring DownloadUrlParameters::IEDownloadResponseheader() {
  return ie_response_header_;
}
#endif  // defined(IE_REDCORE)

}  // namespace download
