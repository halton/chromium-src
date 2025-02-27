// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/service_worker/service_worker_url_loader_job.h"

#include <utility>

#include "base/optional.h"
#include "content/browser/service_worker/service_worker_version.h"
#include "content/browser/url_loader_factory_getter.h"
#include "content/common/service_worker/service_worker_loader_helpers.h"
#include "content/common/service_worker/service_worker_utils.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/resource_request_info.h"
#include "content/public/common/content_features.h"
#include "content/public/common/content_switches.h"
#include "services/network/public/interfaces/fetch_api.mojom.h"

namespace content {

// This class waits for completion of a stream response from the service worker.
// It calls ServiceWorkerURLLoader::CommitComplete() upon completion of the
// response.
class ServiceWorkerURLLoaderJob::StreamWaiter
    : public blink::mojom::ServiceWorkerStreamCallback {
 public:
  StreamWaiter(
      ServiceWorkerURLLoaderJob* owner,
      scoped_refptr<ServiceWorkerVersion> streaming_version,
      blink::mojom::ServiceWorkerStreamCallbackRequest callback_request)
      : owner_(owner),
        streaming_version_(streaming_version),
        binding_(this, std::move(callback_request)) {
    streaming_version_->OnStreamResponseStarted();
    binding_.set_connection_error_handler(
        base::BindOnce(&StreamWaiter::OnAborted, base::Unretained(this)));
  }
  ~StreamWaiter() override { streaming_version_->OnStreamResponseFinished(); }

  // Implements mojom::ServiceWorkerStreamCallback.
  void OnCompleted() override {
    // Destroys |this|.
    owner_->CommitCompleted(net::OK);
  }
  void OnAborted() override {
    // Destroys |this|.
    owner_->CommitCompleted(net::ERR_ABORTED);
  }

 private:
  ServiceWorkerURLLoaderJob* owner_;
  scoped_refptr<ServiceWorkerVersion> streaming_version_;
  mojo::Binding<blink::mojom::ServiceWorkerStreamCallback> binding_;

  DISALLOW_COPY_AND_ASSIGN(StreamWaiter);
};

ServiceWorkerURLLoaderJob::ServiceWorkerURLLoaderJob(
    LoaderCallback callback,
    Delegate* delegate,
    const network::ResourceRequest& resource_request,
    scoped_refptr<URLLoaderFactoryGetter> url_loader_factory_getter)
    : loader_callback_(std::move(callback)),
      delegate_(delegate),
      resource_request_(resource_request),
      url_loader_factory_getter_(std::move(url_loader_factory_getter)),
      binding_(this),
      weak_factory_(this) {
  DCHECK(ServiceWorkerUtils::IsMainResourceType(
      static_cast<ResourceType>(resource_request.resource_type)));
  DCHECK_EQ(network::mojom::FetchRequestMode::kNavigate,
            resource_request_.fetch_request_mode);
  DCHECK_EQ(network::mojom::FetchCredentialsMode::kInclude,
            resource_request_.fetch_credentials_mode);
  DCHECK_EQ(network::mojom::FetchRedirectMode::kManual,
            resource_request_.fetch_redirect_mode);
  response_head_.load_timing.request_start = base::TimeTicks::Now();
  response_head_.load_timing.request_start_time = base::Time::Now();
}

ServiceWorkerURLLoaderJob::~ServiceWorkerURLLoaderJob() = default;

void ServiceWorkerURLLoaderJob::FallbackToNetwork() {
  response_type_ = ResponseType::FALLBACK_TO_NETWORK;
  // This could be called multiple times in some cases because we simply
  // call this synchronously here and don't wait for a separate async
  // StartRequest cue like what URLRequestJob case does.
  // TODO(kinuko): Make sure this is ok or we need to make this async.
  if (!loader_callback_.is_null())
    std::move(loader_callback_).Run(StartLoaderCallback());
}

void ServiceWorkerURLLoaderJob::FallbackToNetworkOrRenderer() {
  // TODO(kinuko): Implement this. Now we always fallback to network.
  FallbackToNetwork();
}

void ServiceWorkerURLLoaderJob::ForwardToServiceWorker() {
  response_type_ = ResponseType::FORWARD_TO_SERVICE_WORKER;
  StartRequest();
}

bool ServiceWorkerURLLoaderJob::ShouldFallbackToNetwork() {
  return response_type_ == ResponseType::FALLBACK_TO_NETWORK;
}

ui::PageTransition ServiceWorkerURLLoaderJob::GetPageTransition() {
  NOTIMPLEMENTED();
  return ui::PAGE_TRANSITION_LINK;
}

size_t ServiceWorkerURLLoaderJob::GetURLChainSize() const {
  NOTIMPLEMENTED();
  return 0;
}

void ServiceWorkerURLLoaderJob::FailDueToLostController() {
  NOTIMPLEMENTED();
}

void ServiceWorkerURLLoaderJob::Cancel() {
  status_ = Status::kCancelled;
  weak_factory_.InvalidateWeakPtrs();
  fetch_dispatcher_.reset();
  stream_waiter_.reset();

  url_loader_client_->OnComplete(
      network::URLLoaderCompletionStatus(net::ERR_ABORTED));
  url_loader_client_.reset();
  DeleteIfNeeded();
}

bool ServiceWorkerURLLoaderJob::WasCanceled() const {
  return status_ == Status::kCancelled;
}

void ServiceWorkerURLLoaderJob::DetachedFromRequest() {
  detached_from_request_ = true;
  DeleteIfNeeded();
}

void ServiceWorkerURLLoaderJob::StartRequest() {
  DCHECK_EQ(ResponseType::FORWARD_TO_SERVICE_WORKER, response_type_);
  DCHECK_EQ(Status::kNotStarted, status_);
  status_ = Status::kStarted;

  ServiceWorkerMetrics::URLRequestJobResult result =
      ServiceWorkerMetrics::REQUEST_JOB_ERROR_BAD_DELEGATE;
  ServiceWorkerVersion* active_worker =
      delegate_->GetServiceWorkerVersion(&result);
  if (!active_worker) {
    ReturnNetworkError();
    return;
  }

  // Dispatch the fetch event.
  fetch_dispatcher_ = std::make_unique<ServiceWorkerFetchDispatcher>(
      std::make_unique<network::ResourceRequest>(resource_request_),
      active_worker, base::nullopt /* timeout */,
      net::NetLogWithSource() /* TODO(scottmg): net log? */,
      base::BindOnce(&ServiceWorkerURLLoaderJob::DidPrepareFetchEvent,
                     weak_factory_.GetWeakPtr(),
                     base::WrapRefCounted(active_worker)),
      base::BindOnce(&ServiceWorkerURLLoaderJob::DidDispatchFetchEvent,
                     weak_factory_.GetWeakPtr()));
  did_navigation_preload_ =
      fetch_dispatcher_->MaybeStartNavigationPreloadWithURLLoader(
          resource_request_, url_loader_factory_getter_.get(),
          base::BindOnce(&base::DoNothing /* TODO(crbug/762357): metrics? */));
  response_head_.service_worker_start_time = base::TimeTicks::Now();
  response_head_.load_timing.send_start = base::TimeTicks::Now();
  response_head_.load_timing.send_end = base::TimeTicks::Now();
  fetch_dispatcher_->Run();
}

void ServiceWorkerURLLoaderJob::CommitResponseHeaders() {
  DCHECK_EQ(Status::kStarted, status_);
  DCHECK(url_loader_client_.is_bound());
  status_ = Status::kSentHeader;
  url_loader_client_->OnReceiveResponse(response_head_, ssl_info_,
                                        nullptr /* downloaded_file */);
}

void ServiceWorkerURLLoaderJob::CommitCompleted(int error_code) {
  DCHECK_LT(status_, Status::kCompleted);
  DCHECK(url_loader_client_.is_bound());
  status_ = Status::kCompleted;

  // |stream_waiter_| calls this when done.
  stream_waiter_.reset();

  url_loader_client_->OnComplete(
      network::URLLoaderCompletionStatus(error_code));
}

void ServiceWorkerURLLoaderJob::ReturnNetworkError() {
  DCHECK(!url_loader_client_.is_bound());
  DCHECK(loader_callback_);
  std::move(loader_callback_)
      .Run(base::BindOnce(&ServiceWorkerURLLoaderJob::StartErrorResponse,
                          weak_factory_.GetWeakPtr()));
}

void ServiceWorkerURLLoaderJob::DidPrepareFetchEvent(
    scoped_refptr<ServiceWorkerVersion> version) {
  response_head_.service_worker_ready_time = base::TimeTicks::Now();
}

void ServiceWorkerURLLoaderJob::DidDispatchFetchEvent(
    ServiceWorkerStatusCode status,
    ServiceWorkerFetchDispatcher::FetchEventResult fetch_result,
    const ServiceWorkerResponse& response,
    blink::mojom::ServiceWorkerStreamHandlePtr body_as_stream,
    blink::mojom::BlobPtr body_as_blob,
    scoped_refptr<ServiceWorkerVersion> version) {
  ServiceWorkerMetrics::URLRequestJobResult result =
      ServiceWorkerMetrics::REQUEST_JOB_ERROR_BAD_DELEGATE;
  if (!delegate_->RequestStillValid(&result)) {
    ReturnNetworkError();
    return;
  }

  if (status != SERVICE_WORKER_OK) {
    delegate_->MainResourceLoadFailed();
    FallbackToNetwork();
    return;
  }

  if (fetch_result ==
      ServiceWorkerFetchDispatcher::FetchEventResult::kShouldFallback) {
    // TODO(kinuko): Check if this needs to fallback to the renderer.
    FallbackToNetwork();
    return;
  }

  DCHECK_EQ(fetch_result,
            ServiceWorkerFetchDispatcher::FetchEventResult::kGotResponse);

  // A response with status code 0 is Blink telling us to respond with
  // network error.
  if (response.status_code == 0) {
    ReturnNetworkError();
    return;
  }

  // Creates a new HttpResponseInfo using the the ServiceWorker script's
  // HttpResponseInfo to show HTTPS padlock.
  // TODO(horo): When we support mixed-content (HTTP) no-cors requests from a
  // ServiceWorker, we have to check the security level of the responses.
  const net::HttpResponseInfo* main_script_http_info =
      version->GetMainScriptHttpResponseInfo();
  DCHECK(main_script_http_info);
  ssl_info_ = main_script_http_info->ssl_info;

  std::move(loader_callback_)
      .Run(base::BindOnce(&ServiceWorkerURLLoaderJob::StartResponse,
                          weak_factory_.GetWeakPtr(), response, version,
                          std::move(body_as_stream), std::move(body_as_blob)));
}

void ServiceWorkerURLLoaderJob::StartResponse(
    const ServiceWorkerResponse& response,
    scoped_refptr<ServiceWorkerVersion> version,
    blink::mojom::ServiceWorkerStreamHandlePtr body_as_stream,
    blink::mojom::BlobPtr body_as_blob,
    mojom::URLLoaderRequest request,
    mojom::URLLoaderClientPtr client) {
  DCHECK(!binding_.is_bound());
  DCHECK(!url_loader_client_.is_bound());
  binding_.Bind(std::move(request));
  binding_.set_connection_error_handler(base::BindOnce(
      &ServiceWorkerURLLoaderJob::Cancel, base::Unretained(this)));
  url_loader_client_ = std::move(client);

  ServiceWorkerLoaderHelpers::SaveResponseInfo(response, &response_head_);
  ServiceWorkerLoaderHelpers::SaveResponseHeaders(
      response.status_code, response.status_text, response.headers,
      &response_head_);

  response_head_.did_service_worker_navigation_preload =
      did_navigation_preload_;
  response_head_.load_timing.receive_headers_end = base::TimeTicks::Now();

  // Handle a redirect response. ComputeRedirectInfo returns non-null redirect
  // info if the given response is a redirect.
  base::Optional<net::RedirectInfo> redirect_info =
      ServiceWorkerLoaderHelpers::ComputeRedirectInfo(
          resource_request_, response_head_,
          ssl_info_ && ssl_info_->token_binding_negotiated);
  if (redirect_info) {
    response_head_.encoded_data_length = 0;
    url_loader_client_->OnReceiveRedirect(*redirect_info, response_head_);
    // Our client is the navigation loader, which will start a new URLLoader for
    // the redirect rather than calling FollowRedirect(), so we're done here.
    status_ = Status::kCompleted;
    return;
  }

  // We have a non-redirect response. Send the headers to the client.
  CommitResponseHeaders();

  // Handle a stream response body.
  if (!body_as_stream.is_null() && body_as_stream->stream.is_valid()) {
    stream_waiter_ = std::make_unique<StreamWaiter>(
        this, std::move(version), std::move(body_as_stream->callback_request));
    url_loader_client_->OnStartLoadingResponseBody(
        std::move(body_as_stream->stream));
    // StreamWaiter will call CommitCompleted() when done.
    return;
  }

  // Handle a blob response body.
  if (body_as_blob) {
    body_as_blob_ = std::move(body_as_blob);
    mojo::ScopedDataPipeConsumerHandle data_pipe;
    int error = ServiceWorkerLoaderHelpers::ReadBlobResponseBody(
        &body_as_blob_, resource_request_.headers,
        base::BindOnce(&ServiceWorkerURLLoaderJob::OnBlobReadingComplete,
                       weak_factory_.GetWeakPtr()),
        &data_pipe);
    if (error != net::OK) {
      CommitCompleted(error);
      return;
    }
    url_loader_client_->OnStartLoadingResponseBody(std::move(data_pipe));
    // We continue in OnBlobReadingComplete().
    return;
  }

  // The response has no body.
  CommitCompleted(net::OK);
}

void ServiceWorkerURLLoaderJob::StartErrorResponse(
    mojom::URLLoaderRequest request,
    mojom::URLLoaderClientPtr client) {
  DCHECK_EQ(Status::kStarted, status_);
  DCHECK(!url_loader_client_.is_bound());
  url_loader_client_ = std::move(client);
  CommitCompleted(net::ERR_FAILED);
}

// URLLoader implementation----------------------------------------

void ServiceWorkerURLLoaderJob::FollowRedirect() {
  NOTIMPLEMENTED();
}

void ServiceWorkerURLLoaderJob::ProceedWithResponse() {
  // TODO(arthursonzogni): Implement this for navigation requests if the
  // ServiceWorker service is enabled before the Network Service.
  NOTREACHED();
}

void ServiceWorkerURLLoaderJob::SetPriority(net::RequestPriority priority,
                                            int32_t intra_priority_value) {
  NOTIMPLEMENTED();
}

void ServiceWorkerURLLoaderJob::PauseReadingBodyFromNet() {}

void ServiceWorkerURLLoaderJob::ResumeReadingBodyFromNet() {}

void ServiceWorkerURLLoaderJob::OnBlobReadingComplete(int net_error) {
  CommitCompleted(net_error);
  body_as_blob_.reset();
}

void ServiceWorkerURLLoaderJob::DeleteIfNeeded() {
  if (!binding_.is_bound() && detached_from_request_)
    delete this;
}

}  // namespace content
