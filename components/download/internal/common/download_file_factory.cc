// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/download/public/common/download_file_factory.h"

#include <utility>

#include "components/download/public/common/download_file_impl.h"

#ifdef IE_REDCORE
#include "components/download/public/common/download_file_ie.h"
#endif

namespace download {

DownloadFileFactory::~DownloadFileFactory() {}

DownloadFile* DownloadFileFactory::CreateFile(
    std::unique_ptr<DownloadSaveInfo> save_info,
    const base::FilePath& default_downloads_directory,
    std::unique_ptr<InputStream> stream,
    uint32_t download_id,
    base::WeakPtr<DownloadDestinationObserver> observer) {
  return new DownloadFileImpl(std::move(save_info), default_downloads_directory,
                              std::move(stream), download_id, observer);
}

#ifdef IE_REDCORE
IEDownloadFileFactory::~IEDownloadFileFactory() {}

DownloadFile* IEDownloadFileFactory::CreateFile(
    std::unique_ptr<DownloadSaveInfo> save_info,
    const base::FilePath& default_downloads_directory,
    const GURL& url,
    const GURL& referrer_url,
    bool calculate_hash,
    base::WeakPtr<IE::IEDownloader> pDownloader,
    const net::NetLogWithSource& bound_net_log,
    base::WeakPtr<DownloadDestinationObserver> observer) {
  return new IE::DownloadFileIE(
      std::move(save_info), default_downloads_directory, url, referrer_url,
      calculate_hash, bound_net_log, observer, std::move(pDownloader));
}
#endif

}  // namespace download
