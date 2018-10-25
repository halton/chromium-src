// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#ifndef COMPONENTS_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_FILE_FACTORY_H_
#define COMPONENTS_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_FILE_FACTORY_H_

#include <memory>
#include <vector>

#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "components/download/public/common/download_export.h"
#include "components/download/public/common/input_stream.h"
#include "net/log/net_log_with_source.h"
#include "url/gurl.h"

#if defined(REDCORE) && defined(IE_REDCORE)
namespace IE {
class IEDownloader;
}
#endif

namespace download {
class DownloadDestinationObserver;
class DownloadFile;
struct DownloadSaveInfo;

class COMPONENTS_DOWNLOAD_EXPORT DownloadFileFactory {
 public:
  virtual ~DownloadFileFactory();

  virtual DownloadFile* CreateFile(
      std::unique_ptr<DownloadSaveInfo> save_info,
      const base::FilePath& default_downloads_directory,
      std::unique_ptr<InputStream> stream,
      uint32_t download_id,
      base::WeakPtr<DownloadDestinationObserver> observer);
};

#if defined(REDCORE) && defined(IE_REDCORE)
class COMPONENTS_DOWNLOAD_EXPORT IEDownloadFileFactory {
 public:
  virtual ~IEDownloadFileFactory();

  virtual DownloadFile* CreateFile(
      std::unique_ptr<DownloadSaveInfo> save_info,
      const base::FilePath& default_downloads_directory,
      const GURL& url,
      const GURL& referrer_url,
      bool calculate_hash,
      base::WeakPtr<IE::IEDownloader> pDownloader,
      const net::NetLogWithSource& bound_net_log,
      base::WeakPtr<DownloadDestinationObserver> observer);
};
#endif

}  // namespace download

#endif  // COMPONENTS_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_FILE_FACTORY_H_
