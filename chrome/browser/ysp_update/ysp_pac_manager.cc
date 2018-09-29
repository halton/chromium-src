#ifdef IE_REDCORE
//ysp+

#include "chrome/browser/ysp_update/ysp_pac_manager.h"

#include <utility>
#include "base/values.h"
#include "base/path_service.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/json/json_file_value_serializer.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/browser/browser_process.h"
#include "content/public/browser/browser_thread.h"
#include "components/prefs/pref_service.h"
#include "chrome/common/pref_names.h"
#include "base/native_library.h"
#if defined(OS_WIN)
#include <Iphlpapi.h>
#include <comdef.h>
#include "setupapi.h"
#endif
#include "base/json/json_writer.h"
#include "base/guid.h"
#include "base/base64.h"
#include "net/url_request/url_request_context.h"
#include "base/version.h"
#include "components/version_info/version_info.h"
#include "content/public/browser/browser_context.h"
#include "components/download/public/common/download_url_parameters.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/download_manager.h"
#include "chrome/browser/ysp_login/ysp_login_manager.h"

#if defined(OS_WIN)
#pragma comment(lib, "iphlpapi.lib")
#pragma comment (lib,"netapi32")
#pragma comment(lib, "setupapi")
#endif

namespace {

  YSPPACManager* g_instance = nullptr;

  base::FilePath FilePathOfPACFile(std::string filename) {
    base::FilePath path;
    base::PathService::Get(chrome::DIR_USER_DATA, &path);
    path = path.Append(FILE_PATH_LITERAL("\\"));
    path = path.AppendASCII(filename);
    return path;
  }

}  // namespace

YSPPACManager::YSPPACManager()
: updating_(false),
  updated_(false) {
}

YSPPACManager::~YSPPACManager() {
}

//static
YSPPACManager* YSPPACManager::GetInstance() {
  if (!g_instance) {
    g_instance = new YSPPACManager;
  }
  return g_instance;
}

void YSPPACManager::RequestPAC(content::WebContents* webContents) {
  DLOG(INFO) << "YSPPACManager::RequestPAC";
  //if(updating_ || updated_)
  //  return;
  updating_ = true;

  web_contents_ = webContents;
  base::string16 pac_url = YSPLoginManager::GetInstance()->GetPACFileURL();
  if(!pac_url.empty()) {
    GURL url(pac_url);
    if(url.is_valid()) {
      StartDownload(url);
      return;
    }
  }

  DLOG(INFO) << "YSPPACManager::RequestPAC empty or invalid url";
  base::FilePath path;
  YSPLoginManager::GetInstance()->UpdatePACSettings(path);
  updating_ = false;
  updated_ = true;
}

// download::DownloadItem::Observer:
void YSPPACManager::OnDownloadUpdated(download::DownloadItem* download) {
  DLOG(INFO) << "YSPPACManager::OnDownloadUpdated";
  download::DownloadItem::DownloadState state = download->GetState();
  switch (state) {
    case download::DownloadItem::IN_PROGRESS:
      return;
    case download::DownloadItem::COMPLETE: {
      download->RemoveObserver(this);
      base::FilePath filePath = download->GetTargetFilePath();
      DLOG(INFO) << "YSPPACManager::OnDownloadUpdated file: " << filePath.value();
      YSPLoginManager::GetInstance()->UpdatePACSettings(filePath);
      updating_ = false;
      updated_ = true;
      break;
    }
    case download::DownloadItem::CANCELLED: {
      updating_ = false;
      updated_ = true;
      break;
    }
    case download::DownloadItem::INTERRUPTED: {
      updating_ = false;
      updated_ = true;
      break;
    }
    case download::DownloadItem::MAX_DOWNLOAD_STATE: {
      NOTREACHED();
      return;
    }
  }
}

void YSPPACManager::OnDownloadRemoved(download::DownloadItem* download) {
  updating_ = false;
  if(download)
    download->RemoveObserver(this);
}

void YSPPACManager::StartDownload(const GURL& package_url) {
  if(!package_url.is_valid())
    return;
  DLOG(INFO) << "YSPPACManager::StartDownload";

  content::DownloadManager* download_manager =
      content::BrowserContext::GetDownloadManager(
          web_contents_->GetBrowserContext());
  std::unique_ptr<download::DownloadUrlParameters> download_parameters;
      //std::unique_ptr<download::DownloadUrlParameters> download_parameters(
      //    download::DownloadUrlParameters::CreateForWebContentsMainFrame(web_contents_,
      //                                                package_url));
  //download_parameters->set_suggested_name(L"aaaa.pac");
  std::string filename = package_url.ExtractFileName();
  if(filename.empty())
    filename = "ysp.pac";
  download_parameters->set_file_path(FilePathOfPACFile(filename));
  download_parameters->set_callback(
      base::Bind(&YSPPACManager::DownloadStarted, base::Unretained(this)));
  //RecordDownloadSource(DOWNLOAD_INITIATED_BY_PLUGIN_INSTALLER);
  download_manager->DownloadUrl(std::move(download_parameters));
}

void YSPPACManager::DownloadStarted(
    download::DownloadItem* item,
    download::DownloadInterruptReason interrupt_reason) {
  if (interrupt_reason != download::DOWNLOAD_INTERRUPT_REASON_NONE) {
    return;
  }
  item->set_is_update(true);
  //item->SetOpenWhenComplete(true);
  item->AddObserver(this);
}

#endif
