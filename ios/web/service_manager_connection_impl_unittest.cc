// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ios/web/service_manager_connection_impl.h"

#include "base/synchronization/waitable_event.h"
#include "ios/web/public/test/test_web_thread_bundle.h"
#include "ios/web/public/web_thread.h"
#include "services/service_manager/public/cpp/identity.h"
#include "services/service_manager/public/cpp/service.h"
#include "services/service_manager/public/interfaces/constants.mojom.h"
#include "services/service_manager/public/interfaces/service_factory.mojom.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/platform_test.h"

namespace web {
namespace {

constexpr char kTestServiceName[] = "test service";

std::unique_ptr<service_manager::Service> LaunchService(
    base::WaitableEvent* event) {
  event->Signal();
  return base::MakeUnique<service_manager::Service>();
}

}  // namespace

using ServiceManagerConnectionImplTest = PlatformTest;

TEST_F(ServiceManagerConnectionImplTest, ServiceLaunchThreading) {
  TestWebThreadBundle thread_bundle(
      TestWebThreadBundle::Options::REAL_IO_THREAD);
  // base::MessageLoop message_loop;
  // base::Thread io_thread("ServiceManagerConnectionImplTest IO Thread");
  // io_thread.Start();
  service_manager::mojom::ServicePtr service;
  ServiceManagerConnectionImpl connection_impl(
      mojo::MakeRequest(&service),
      WebThread::GetTaskRunnerForThread(WebThread::IO));
  ServiceManagerConnection& connection = connection_impl;
  service_manager::EmbeddedServiceInfo info;
  base::WaitableEvent event(base::WaitableEvent::ResetPolicy::MANUAL,
                            base::WaitableEvent::InitialState::NOT_SIGNALED);
  info.factory = base::Bind(&LaunchService, &event);
  info.task_runner = WebThread::GetTaskRunnerForThread(WebThread::IO);
  connection.AddEmbeddedService(kTestServiceName, info);
  connection.Start();
  service_manager::BindSourceInfo source_info(
      {service_manager::mojom::kServiceName,
       service_manager::mojom::kRootUserID},
      service_manager::CapabilitySet());
  service_manager::mojom::ServiceFactoryPtr factory;
  service->OnBindInterface(source_info,
                           service_manager::mojom::ServiceFactory::Name_,
                           mojo::MakeRequest(&factory).PassMessagePipe(),
                           base::Bind(&base::DoNothing));
  service_manager::mojom::ServicePtr created_service;
  service_manager::mojom::PIDReceiverPtr pid_receiver;
  mojo::MakeRequest(&pid_receiver);
  factory->CreateService(mojo::MakeRequest(&created_service), kTestServiceName,
                         std::move(pid_receiver));
  event.Wait();
}

}  // namespace web
