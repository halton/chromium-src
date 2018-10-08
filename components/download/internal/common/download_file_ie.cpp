
#ifdef REDCORE
#ifdef IE_REDCORE
#include "components/download/public/common/download_file_ie.h"

#include <string>
#include <utility>

#include "base/bind.h"
#include "base/files/file_util.h"
#include "base/strings/stringprintf.h"
#include "base/time/time.h"
#include "base/values.h"
#include "components/download/public/common/download_create_info.h"
#include "components/download/public/common/download_interrupt_reasons_utils.h"
#include "components/download/public/common/download_stats.h"
#include "components/download/public/common/download_destination_observer.h"
#include "content/public/browser/browser_thread.h"

#include "content/common/IE/IEVersion.h"

namespace IE {
  const int kMaxRenameRetries = 3;
  const int kInitialRenameRetryDelayMs = 200;
  const int kUpdatePeriodMs = 500;

  DownloadFileIE::DownloadFileIE(
      std::unique_ptr<download::DownloadSaveInfo> save_info,
      const base::FilePath& default_downloads_directory,
      const GURL& url,
      const GURL& referrer_url,
      bool calculate_hash,
      const net::NetLogWithSource & bound_net_log, 
      base::WeakPtr<download::DownloadDestinationObserver> observer,
      base::WeakPtr<IEDownloader> pDownloader)
      : file_(download::DownloadItem::kInvalidId),
        defaultDir(default_downloads_directory),
        downloadDestObserver(observer),
      pIEDownloader(pDownloader),
      weakFactory(this) {
  }

  DownloadFileIE::~DownloadFileIE()
  {
  }

  void DownloadFileIE::IE_Initialize(const InitializeCallback & callback)
  {
    // download::DownloadInterruptReason result =
    //   file_.Initialize(defaultDir);
    // Author: Webb 
    // just for compile 
    // if (result != download::DOWNLOAD_INTERRUPT_REASON_NONE)
    // {
    //   content::BrowserThread::PostTask(
    //     content::BrowserThread::UI, FROM_HERE, callback);
    // }
    // content::BrowserThread::PostTask(content::BrowserThread::UI, FROM_HERE,
    //     base::Bind(&IEDownloader::RegisterCallback, pIEDownloader,
    //       base::Bind(&DownloadFileIE::OnDataReceived, weakFactory.GetWeakPtr())));

    // content::BrowserThread::PostTask(content::BrowserThread::UI, FROM_HERE,
    //   base::Bind(callback, download::DOWNLOAD_INTERRUPT_REASON_NONE));
  }

void DownloadFileIE::Initialize(InitializeCallback initialize_callback,
                          const CancelRequestCallback& cancel_request_callback,
                          const download::DownloadItem::ReceivedSlices& received_slices,
                          bool is_parallelizable){}
void DownloadFileIE::AddInputStream(std::unique_ptr<download::InputStream> stream,
                              int64_t offset,
                              int64_t length) {}
  void DownloadFileIE::RenameAndUniquify(const base::FilePath & full_path, const RenameCompletionCallback & callback)
  {
    RenameWithRetryInternal(full_path, UNIQUIFY, kMaxRenameRetries, base::TimeTicks(), callback);
  }

void DownloadFileIE::RenameAndAnnotate(const base::FilePath& full_path,
                                 const std::string& client_guid,
                                 const GURL& source_url,
                                 const GURL& referrer_url,
                                 const DownloadFileIE::RenameCompletionCallback& callback){
      RenameWithRetryInternal(full_path,
      ANNOTATE_WITH_SOURCE_INFORMATION,
      kMaxRenameRetries,
      base::TimeTicks(),
      callback);
  }
void DownloadFileIE::SetPotentialFileLength(int64_t length){}

  void DownloadFileIE::Detach()
  {
    file_.Detach();
  }

  void DownloadFileIE::Pause() {}
  void DownloadFileIE::Resume() {}
  void DownloadFileIE::Cancel()
  {
    file_.Cancel();
    // content::BrowserThread::PostTask(content::BrowserThread::UI, FROM_HERE,
    //   base::Bind(&IEDownloader::FinishDownload,pIEDownloader,true));
  }

   const base::FilePath& DownloadFileIE::FullPath() const
  { 
    return file_.full_path();
  }

  bool DownloadFileIE::InProgress() const
  {
    return file_.in_progress();
  }

  int64_t DownloadFileIE::CurrentSpeed() const
  {
    return rateEstimator.GetCountPerSecond();
  }

  // bool DownloadFileIE::GetHash(std::string * hash)
  // {
  //   return file_.GetHash(hash);
  // }

  // std::string DownloadFileIE::GetHashState()
  // {
  //   return file_.GetHashState();
  // }

  // void DownloadFileIE::SetClientGuid(const std::string & guid)
  // {
  //   file_.SetClientGuid(guid);
  // }

  bool DownloadFileIE::IsIEDownload()
  {
    return true;
  }

  void DownloadFileIE::OnDataReceived(int status, const char* data, int64_t len)
  {
    if (updateTimer.get()==NULL)
    {
      updateTimer.reset(new base::RepeatingTimer());
      updateTimer->Start(FROM_HERE,
        base::TimeDelta::FromMilliseconds(kUpdatePeriodMs),
        this, &DownloadFileIE::SendUpdate);
    }
    if (status == (int)DOWNLOADINGDATA && data && len>0)
    {
      file_.AppendDataToFile(data, len);
      rateEstimator.Increment(len);
    }
    else if (status == (int)ENDDOWNLOAD)
    {
      if (data && len>0)
        file_.AppendDataToFile(data, len);
      file_.Finish();
      SendUpdate();
      updateTimer.reset();

      weakFactory.InvalidateWeakPtrs();
      std::string hash="";
      // if (!GetHash(&hash) || file_.IsEmptyHash(hash))
      //   hash.clear();
      // content::BrowserThread::PostTask(
      //   content::BrowserThread::UI, FROM_HERE,
      //   base::Bind(
      //     &download::DownloadDestinationObserver::DestinationCompleted,
      //     downloadDestObserver, hash));

      // content::BrowserThread::PostTask(content::BrowserThread::UI, FROM_HERE,
      //   base::Bind(&IEDownloader::FinishDownload, pIEDownloader, false));
    }
    else if (status == (int)DOWNLOADERROR)
    {
      // file_.FinishWithError();
      updateTimer.reset();
      weakFactory.InvalidateWeakPtrs();

      SendUpdate();
      // content::BrowserThread::PostTask(
      //   content::BrowserThread::UI, FROM_HERE,
      //   base::Bind(&DownloadDestinationObserver::DestinationError,
      //     downloadDestObserver, DOWNLOAD_INTERRUPT_REASON_NETWORK_FAILED));

      // content::BrowserThread::PostTask(content::BrowserThread::UI, FROM_HERE,
      //   base::Bind(&IEDownloader::FinishDownload, pIEDownloader, false));
    }
  }

  void DownloadFileIE::RenameWithRetryInternal(const base::FilePath & full_path, RenameOption option,
                                        int retries_left, base::TimeTicks time_of_first_failure,
                                        const RenameCompletionCallback & callback)
  {
    // DCHECK_CURRENTLY_ON(content::BrowserThread::IO);

    base::FilePath new_path(full_path);

    if ((option & UNIQUIFY) && full_path != file_.full_path())
    {
      int uniquifier =
        base::GetUniquePathNumber(new_path, base::FilePath::StringType());
      if (uniquifier > 0)
        new_path = new_path.InsertBeforeExtensionASCII(base::StringPrintf(" (%d)", uniquifier));
    }

    download::DownloadInterruptReason reason = file_.Rename(new_path);

    // Attempt to retry the rename if possible. If the rename failed and the
    // subsequent open also failed, then in_progress() would be false. We don't
    // try to retry renames if the in_progress() was false to begin with since we
    // have less assurance that the file at file_.full_path() was the one we were
    // working with.
    if (ShouldRetryFailedRename(reason) && file_.in_progress() && retries_left > 0)
    {
      // int attempt_number = kMaxRenameRetries - retries_left;
      // content::BrowserThread::PostDelayedTask(content::BrowserThread::IO, FROM_HERE,
      //   base::Bind(&DownloadFileIE::RenameWithRetryInternal,
      //     weakFactory.GetWeakPtr(),
      //     full_path,  option, --retries_left,
      //     time_of_first_failure.is_null() ? base::TimeTicks::Now() : time_of_first_failure,
      //     callback),
      //   GetRetryDelayForFailedRename(attempt_number));
      return;
    }

    // if (!time_of_first_failure.is_null())
    //   RecordDownloadFileRenameResultAfterRetry(
    //     base::TimeTicks::Now() - time_of_first_failure, reason);

    if (reason == download::DOWNLOAD_INTERRUPT_REASON_NONE &&
      (option & ANNOTATE_WITH_SOURCE_INFORMATION)) {
      // Doing the annotation after the rename rather than before leaves
      // a very small window during which the file has the final name but
      // hasn't been marked with the Mark Of The Web.  However, it allows
      // anti-virus scanners on Windows to actually see the data
      // (http://crbug.com/127999) under the correct name (which is information
      // it uses).

      // comment this just for compiling
      // reason = file_.AnnotateWithSourceInformation();
    }

    if (reason != download::DOWNLOAD_INTERRUPT_REASON_NONE)
    {
      // Make sure our information is updated, since we're about to
      // error out.
      SendUpdate();

      new_path.clear();
    }

    // content::BrowserThread::PostTask(
    //   content::BrowserThread::UI, FROM_HERE,
    //   base::Bind(callback, reason, new_path));
  }

  bool DownloadFileIE::ShouldRetryFailedRename(download::DownloadInterruptReason reason)
  {
    return reason == download::DOWNLOAD_INTERRUPT_REASON_FILE_TRANSIENT_ERROR;
  }

  base::TimeDelta DownloadFileIE::GetRetryDelayForFailedRename(int attempt_number)
  {
    DCHECK_GE(attempt_number, 0);
    // |delay| starts at kInitialRenameRetryDelayMs and increases by a factor of
    // 2 at each subsequent retry. Assumes that |retries_left| starts at
    // kMaxRenameRetries. Also assumes that kMaxRenameRetries is less than the
    // number of bits in an int.
    return base::TimeDelta::FromMilliseconds(kInitialRenameRetryDelayMs) *
      (1 << attempt_number);
  }

  // comment just for compiling
  void DownloadFileIE::SendUpdate()
  {
    // content::BrowserThread::PostTask(content::BrowserThread::UI, FROM_HERE,
    //                     base::Bind(&download::DownloadDestinationObserver::DestinationUpdate,
    //                     downloadDestObserver, file_.bytes_so_far(), CurrentSpeed(),
    //                     GetHashState()));
  }

} //namespace IE
#endif
#endif
