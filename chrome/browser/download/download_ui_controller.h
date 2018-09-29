// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_DOWNLOAD_DOWNLOAD_UI_CONTROLLER_H_
#define CHROME_BROWSER_DOWNLOAD_DOWNLOAD_UI_CONTROLLER_H_

#include <memory>
#include <set>

#include "base/macros.h"
#include "components/download/content/public/all_download_item_notifier.h"

#ifdef REDCORE
#include "components/ysp_doc_view/ysp_doc_view_manager.h"   //ysp+
#endif /*REDCORE*/

// This class handles the task of observing a single DownloadManager for
// notifying the UI when a new download should be displayed in the UI.
// It invokes the OnNewDownloadReady() method of hte Delegate when the
// target path is available for a new download.
#ifdef REDCORE
class DownloadUIController : public download::AllDownloadItemNotifier::Observer,
                             public YSPDocViewManagerDelegate {   //ysp+
#else
class DownloadUIController : public download::AllDownloadItemNotifier::Observer {
#endif /*REDCORE*/
 public:
  // The delegate is responsible for figuring out how to notify the UI.
  class Delegate {
   public:
    virtual ~Delegate();

    // This method is invoked to notify the UI of the new download |item|. Note
    // that |item| may be in any state by the time this method is invoked.
    virtual void OnNewDownloadReady(download::DownloadItem* item) = 0;
  };

  // |manager| is the download manager to observe for new downloads. If
  // |delegate.get()| is NULL, then the default delegate is constructed.
  //
  // On Android the default delegate notifies DownloadControllerAndroid. On
  // other platforms the target of the notification is a Browser object.
  //
  // Currently explicit delegates are only used for testing.
  DownloadUIController(content::DownloadManager* manager,
                       std::unique_ptr<Delegate> delegate);

  ~DownloadUIController() override;

#ifdef REDCORE
  // YSPDocViewManagerDelegate:
  void OnDocViewRequestFailure() override;
  void OnDocViewRequestSuccess(download::DownloadItem* download, const std::string& doc_url) override;
#endif /*REDCORE*/
 private:
  void OnDownloadCreated(content::DownloadManager* manager,
                         download::DownloadItem* item) override;
  void OnDownloadUpdated(content::DownloadManager* manager,
                         download::DownloadItem* item) override;

  download::AllDownloadItemNotifier download_notifier_;

  std::unique_ptr<Delegate> delegate_;

  DISALLOW_COPY_AND_ASSIGN(DownloadUIController);
};

#endif  // CHROME_BROWSER_DOWNLOAD_DOWNLOAD_UI_CONTROLLER_H_
