// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_APPCACHE_APPCACHE_URL_LOADER_JOB_H_
#define CONTENT_BROWSER_APPCACHE_APPCACHE_URL_LOADER_JOB_H_

#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/strings/string16.h"
#include "base/time/time.h"
#include "content/browser/appcache/appcache_entry.h"
#include "content/browser/appcache/appcache_job.h"
#include "content/browser/appcache/appcache_request_handler.h"
#include "content/browser/appcache/appcache_response.h"
#include "content/browser/appcache/appcache_storage.h"
#include "content/browser/loader/url_loader_request_handler.h"
#include "content/common/content_export.h"
#include "content/public/common/url_loader.mojom.h"
#include "mojo/public/cpp/bindings/binding.h"
#include "mojo/public/cpp/system/data_pipe.h"

namespace network {
class NetToMojoPendingBuffer;
}

namespace content {

class AppCacheRequest;
class AppCacheURLLoaderRequest;

// AppCacheJob wrapper for a mojom::URLLoader implementation which returns
// responses stored in the AppCache.
class CONTENT_EXPORT AppCacheURLLoaderJob : public AppCacheJob,
                                            public AppCacheStorage::Delegate,
                                            public mojom::URLLoader {
 public:
  ~AppCacheURLLoaderJob() override;

  // Sets up the bindings.
  void Start(mojom::URLLoaderRequest request, mojom::URLLoaderClientPtr client);

  // AppCacheJob overrides.
  bool IsStarted() const override;
  void DeliverAppCachedResponse(const GURL& manifest_url,
                                int64_t cache_id,
                                const AppCacheEntry& entry,
                                bool is_fallback) override;
  void DeliverNetworkResponse() override;
  void DeliverErrorResponse() override;
  AppCacheURLLoaderJob* AsURLLoaderJob() override;
  base::WeakPtr<AppCacheJob> GetWeakPtr() override;
  base::WeakPtr<AppCacheURLLoaderJob> GetDerivedWeakPtr();

  // mojom::URLLoader implementation:
  void FollowRedirect() override;
  void ProceedWithResponse() override;
  void SetPriority(net::RequestPriority priority,
                   int32_t intra_priority_value) override;
  void PauseReadingBodyFromNet() override;
  void ResumeReadingBodyFromNet() override;

  void DeleteIfNeeded();

 protected:
  // AppCacheRequestHandler::CreateJob() creates this instance.
  friend class AppCacheRequestHandler;

  AppCacheURLLoaderJob(AppCacheURLLoaderRequest* appcache_request,
                       AppCacheStorage* storage,
                       LoaderCallback loader_callback);

  // Invokes the loader callback which is expected to setup the mojo binding.
  void CallLoaderCallback();

  // AppCacheStorage::Delegate methods
  void OnResponseInfoLoaded(AppCacheResponseInfo* response_info,
                            int64_t response_id) override;

  // AppCacheResponseReader completion callback.
  void OnReadComplete(int result);

  // Callback invoked when the data pipe can be written to.
  void OnResponseBodyStreamReady(MojoResult result);

  // Mojo binding error handler.
  void OnConnectionError();

  void SendResponseInfo();
  void ReadMore();
  void NotifyCompleted(int error_code);

  base::WeakPtr<AppCacheStorage> storage_;

  // Time when the request started.
  base::TimeTicks start_time_tick_;

  // Timing information for the most recent request.  Its start times are
  // populated in DeliverAppCachedResponse().
  net::LoadTimingInfo load_timing_info_;

  GURL manifest_url_;
  int64_t cache_id_;
  AppCacheEntry entry_;
  bool is_fallback_;

  // Binds the URLLoaderClient with us.
  mojo::Binding<mojom::URLLoader> binding_;

  // The URLLoaderClient pointer. We call this interface with notifications
  // about the URL load
  mojom::URLLoaderClientPtr client_;

  // The data pipe used to transfer AppCache data to the client.
  mojo::DataPipe data_pipe_;
  mojo::ScopedDataPipeProducerHandle response_body_stream_;
  scoped_refptr<network::NetToMojoPendingBuffer> pending_write_;
  mojo::SimpleWatcher writable_handle_watcher_;

  // The Callback to be invoked in the network service land to indicate if
  // the resource request can be serviced via the AppCache.
  LoaderCallback loader_callback_;

  // The AppCacheRequest instance, used to inform the loader job about range
  // request headers. Not owned by this class.
  base::WeakPtr<AppCacheURLLoaderRequest> appcache_request_;

  bool is_deleting_soon_ = false;
  bool is_main_resource_load_;

  base::WeakPtrFactory<AppCacheURLLoaderJob> weak_factory_;
  DISALLOW_COPY_AND_ASSIGN(AppCacheURLLoaderJob);
};

}  // namespace content

#endif  // CONTENT_BROWSER_APPCACHE_APPCACHE_URL_LOADER_JOB_H_
