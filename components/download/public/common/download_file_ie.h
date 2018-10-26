// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_FILE_IE_H_
#define COMPONENTS_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_FILE_IE_H_

#include "components/download/public/common/download_file.h"

#include <stddef.h>
#include <stdint.h>

#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "components/download/public/common/base_file.h"
#include "components/download/public/common/download_destination_observer.h"
#include "components/download/public/common/rate_estimator.h"
#include "components/download/public/common/download_save_info.h"
#include "components/download/public/common/download_item.h"
#include "components/download/public/common/download_file.h"
#include "components/download/public/common/input_stream.h"
#include "components/download/public/common/download_save_info.h"
#include "net/log/net_log.h"
#include "net/log/net_log_with_source.h"

namespace IE {

class IEDownloader;

class COMPONENTS_DOWNLOAD_EXPORT DownloadFileIE
    : public download::DownloadFile {
 public:
  DownloadFileIE(std::unique_ptr<download::DownloadSaveInfo> save_info,
                 const base::FilePath& default_downloads_directory,
                 const GURL& url,
                 const GURL& referrer_url,
                 bool calculate_hash,
                 const net::NetLogWithSource& bound_net_log,
                 base::WeakPtr<download::DownloadDestinationObserver> observer,
                 base::WeakPtr<IEDownloader> download_ptr);
  ~DownloadFileIE() override;

  // DownloadFile
  void IE_Initialize(const InitializeCallback& callback);
  void Initialize(InitializeCallback initialize_callback,
                  const CancelRequestCallback& cancel_request_callback,
                  const download::DownloadItem::ReceivedSlices& received_slices,
                  bool is_parallelizable) override;
  void AddInputStream(std::unique_ptr<download::InputStream> stream,
                      int64_t offset,
                      int64_t length) override;
  void RenameAndUniquify(const base::FilePath& full_path,
                         const RenameCompletionCallback& callback) override;

  void RenameAndAnnotate(const base::FilePath& full_path,
                         const std::string& client_guid,
                         const GURL& source_url,
                         const GURL& referrer_url,
                         const RenameCompletionCallback& callback) override;
  void SetPotentialFileLength(int64_t length) override;
  void Pause() override;
  void Resume() override;
  void Detach() override;

  void Cancel() override;

  const base::FilePath& FullPath() const override;

  bool InProgress() const override;

  int64_t CurrentSpeed() const;

  bool GetHash(std::string* hash);

  std::string GetHashState();

  void SetClientGuid(const std::string& guid);

  bool IsIEDownload() override;

  void OnDataReceived(int status, const char* data, int64_t len);

 private:
  // Options for RenameWithRetryInternal.
  enum RenameOption {
    UNIQUIFY = 1 << 0,  // If there's already a file on disk that conflicts with
                        // |new_path|, try to create a unique file by appending
                        // a uniquifier.
    ANNOTATE_WITH_SOURCE_INFORMATION = 1 << 1
  };

  void RenameWithRetryInternal(const base::FilePath& full_path,
                               RenameOption option,
                               int retries_left,
                               base::TimeTicks time_of_first_failure,
                               const RenameCompletionCallback& callback);
  bool ShouldRetryFailedRename(download::DownloadInterruptReason reason);
  base::TimeDelta GetRetryDelayForFailedRename(int attempt_number);
  void SendUpdate();

 private:
  download::BaseFile file_;
  base::FilePath default_dir_;
  base::FilePath file_path_;
  base::WeakPtr<download::DownloadDestinationObserver> download_dest_observer_;
  std::unique_ptr<base::RepeatingTimer> update_timer_;
  download::RateEstimator rate_estimator_;
  base::WeakPtr<IEDownloader> ie_downloader_;
  base::WeakPtrFactory<DownloadFileIE> self_;
};

// Call IEDownloader function on file thread
class IEDownloader {
 public:
  typedef base::Callback<void(int, const char*, int64_t)> DataReceivedCallback;

  virtual ~IEDownloader() {}

  virtual void RegisterCallback(const DataReceivedCallback& sink_callback) = 0;
  virtual void FinishDownload(bool isCancel) = 0;
  virtual base::WeakPtr<IEDownloader> GetWeakPtr() = 0;
  virtual bool IsDownloading() = 0;
};

}  // namespace IE

#endif  // COMPONENTS_DOWNLOAD_PUBLIC_COMMON_DOWNLOAD_FILE_IE_H_
