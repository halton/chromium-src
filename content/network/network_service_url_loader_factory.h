// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_NETWORK_NETWORK_SERVICE_URL_LOADER_FACTORY_H_
#define CONTENT_NETWORK_NETWORK_SERVICE_URL_LOADER_FACTORY_H_

#include "base/macros.h"
#include "content/public/common/url_loader_factory.mojom.h"
#include "net/traffic_annotation/network_traffic_annotation.h"

namespace content {

class NetworkContext;

// This class is an implementation of mojom::URLLoaderFactory that creates
// a mojom::URLLoader.
class NetworkServiceURLLoaderFactory : public mojom::URLLoaderFactory {
 public:
  // NOTE: |context| must outlive this instance.
  NetworkServiceURLLoaderFactory(NetworkContext* context, uint32_t process_id);

  ~NetworkServiceURLLoaderFactory() override;

  // mojom::URLLoaderFactory implementation.
  void CreateLoaderAndStart(mojom::URLLoaderRequest request,
                            int32_t routing_id,
                            int32_t request_id,
                            uint32_t options,
                            const network::ResourceRequest& url_request,
                            mojom::URLLoaderClientPtr client,
                            const net::MutableNetworkTrafficAnnotationTag&
                                traffic_annotation) override;
  void Clone(mojom::URLLoaderFactoryRequest request) override;

 private:
  // Not owned.
  NetworkContext* context_;
  uint32_t process_id_;

  DISALLOW_COPY_AND_ASSIGN(NetworkServiceURLLoaderFactory);
};

}  // namespace content

#endif  // CONTENT_NETWORK_NETWORK_SERVICE_URL_LOADER_FACTORY_H_
