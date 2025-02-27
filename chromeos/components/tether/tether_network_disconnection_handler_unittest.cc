// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chromeos/components/tether/tether_network_disconnection_handler.h"

#include <memory>

#include "base/memory/ptr_util.h"
#include "base/run_loop.h"
#include "base/test/scoped_task_environment.h"
#include "base/test/test_simple_task_runner.h"
#include "chromeos/components/tether/fake_active_host.h"
#include "chromeos/components/tether/fake_disconnect_tethering_request_sender.h"
#include "chromeos/components/tether/fake_network_configuration_remover.h"
#include "chromeos/components/tether/fake_tether_session_completion_logger.h"
#include "chromeos/components/tether/network_configuration_remover.h"
#include "chromeos/components/tether/tether_session_completion_logger.h"
#include "chromeos/dbus/dbus_thread_manager.h"
#include "chromeos/network/network_state.h"
#include "chromeos/network/network_state_test.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/cros_system_api/dbus/shill/dbus-constants.h"

namespace chromeos {

namespace tether {

namespace {

const char kDeviceId[] = "deviceId";
const char kWifiNetworkGuid[] = "wifiNetworkGuid";
const char kTetherNetworkGuid[] = "tetherNetworkGuid";

std::string CreateConnectedWifiConfigurationJsonString() {
  std::stringstream ss;
  ss << "{"
     << "  \"GUID\": \"" << kWifiNetworkGuid << "\","
     << "  \"Type\": \"" << shill::kTypeWifi << "\","
     << "  \"State\": \"" << shill::kStateReady << "\""
     << "}";
  return ss.str();
}

}  // namespace

class TetherNetworkDisconnectionHandlerTest : public NetworkStateTest {
 protected:
  TetherNetworkDisconnectionHandlerTest() : NetworkStateTest() {}
  ~TetherNetworkDisconnectionHandlerTest() override = default;

  void SetUp() override {
    DBusThreadManager::Initialize();
    NetworkStateTest::SetUp();

    wifi_service_path_ =
        ConfigureService(CreateConnectedWifiConfigurationJsonString());

    fake_active_host_ = std::make_unique<FakeActiveHost>();
    fake_disconnect_tethering_request_sender_ =
        std::make_unique<FakeDisconnectTetheringRequestSender>();
    fake_network_configuration_remover_ =
        std::make_unique<FakeNetworkConfigurationRemover>();
    fake_tether_session_completion_logger_ =
        std::make_unique<FakeTetherSessionCompletionLogger>();

    handler_ = base::WrapUnique(new TetherNetworkDisconnectionHandler(
        fake_active_host_.get(), network_state_handler(),
        fake_network_configuration_remover_.get(),
        fake_disconnect_tethering_request_sender_.get(),
        fake_tether_session_completion_logger_.get()));

    test_task_runner_ = base::MakeRefCounted<base::TestSimpleTaskRunner>();
    handler_->SetTaskRunnerForTesting(test_task_runner_);
  }

  void TearDown() override {
    // Delete handler before the NetworkStateHandler and |fake_active_host_|.
    handler_.reset();
    ShutdownNetworkState();
    NetworkStateTest::TearDown();
    DBusThreadManager::Shutdown();
  }

  void NotifyDisconnected() {
    SetServiceProperty(wifi_service_path_, std::string(shill::kStateProperty),
                       base::Value(shill::kStateIdle));
  }

  void SetWiFiTechnologyStateEnabled(bool enabled) {
    network_state_handler()->SetTechnologyEnabled(
        chromeos::NetworkTypePattern::WiFi(), enabled,
        chromeos::network_handler::ErrorCallback());
    base::RunLoop().RunUntilIdle();
  }

  void VerifyDisconnectionNotYetHandled() {
    EXPECT_EQ(
        std::string(),
        fake_network_configuration_remover_->last_removed_wifi_network_guid());
    EXPECT_EQ(
        std::vector<std::string>(),
        fake_disconnect_tethering_request_sender_->device_ids_sent_requests());
    EXPECT_EQ(ActiveHost::ActiveHostStatus::CONNECTED,
              fake_active_host_->GetActiveHostStatus());
    EXPECT_EQ(nullptr, fake_tether_session_completion_logger_
                           ->last_session_completion_reason());
  }

  void VerifyDisconnectionHandled(
      const TetherSessionCompletionLogger::SessionCompletionReason reason) {
    EXPECT_EQ(
        kWifiNetworkGuid,
        fake_network_configuration_remover_->last_removed_wifi_network_guid());
    EXPECT_EQ(
        std::vector<std::string>{kDeviceId},
        fake_disconnect_tethering_request_sender_->device_ids_sent_requests());
    EXPECT_EQ(ActiveHost::ActiveHostStatus::DISCONNECTED,
              fake_active_host_->GetActiveHostStatus());
    EXPECT_EQ(reason, *fake_tether_session_completion_logger_
                           ->last_session_completion_reason());
  }

  const base::test::ScopedTaskEnvironment scoped_task_environment_;

  std::string wifi_service_path_;

  std::unique_ptr<FakeActiveHost> fake_active_host_;
  std::unique_ptr<FakeDisconnectTetheringRequestSender>
      fake_disconnect_tethering_request_sender_;
  std::unique_ptr<FakeTetherSessionCompletionLogger>
      fake_tether_session_completion_logger_;
  std::unique_ptr<FakeNetworkConfigurationRemover>
      fake_network_configuration_remover_;
  scoped_refptr<base::TestSimpleTaskRunner> test_task_runner_;

  std::unique_ptr<TetherNetworkDisconnectionHandler> handler_;

 private:
  DISALLOW_COPY_AND_ASSIGN(TetherNetworkDisconnectionHandlerTest);
};

TEST_F(TetherNetworkDisconnectionHandlerTest,
       TestConnectAndDisconnect_WiFiEnabled) {
  SetWiFiTechnologyStateEnabled(true);

  // Connect to the network. |handler_| should start tracking the connection.
  fake_active_host_->SetActiveHostConnecting(kDeviceId, kTetherNetworkGuid);
  fake_active_host_->SetActiveHostConnected(kDeviceId, kTetherNetworkGuid,
                                            kWifiNetworkGuid);

  // Now, disconnect the Wi-Fi network. This should result in
  // |fake_active_host_| becoming disconnected.
  NotifyDisconnected();

  // Should not be handled until the handler task is run.
  VerifyDisconnectionNotYetHandled();

  test_task_runner_->RunUntilIdle();
  VerifyDisconnectionHandled(TetherSessionCompletionLogger::
                                 SessionCompletionReason::CONNECTION_DROPPED);
}

TEST_F(TetherNetworkDisconnectionHandlerTest,
       TestConnectAndDisconnect_WiFiDisabled) {
  SetWiFiTechnologyStateEnabled(true);

  // Connect to the network. |handler_| should start tracking the connection.
  fake_active_host_->SetActiveHostConnecting(kDeviceId, kTetherNetworkGuid);
  fake_active_host_->SetActiveHostConnected(kDeviceId, kTetherNetworkGuid,
                                            kWifiNetworkGuid);

  // Now, simulate Wi-Fi being disabled.
  SetWiFiTechnologyStateEnabled(false);

  std::unique_ptr<NetworkState> network =
      std::make_unique<NetworkState>(kWifiNetworkGuid);
  network->SetGuid(kWifiNetworkGuid);
  handler_->NetworkConnectionStateChanged(network.get());

  // Should not be handled until the handler task is run.
  VerifyDisconnectionNotYetHandled();

  test_task_runner_->RunUntilIdle();
  VerifyDisconnectionHandled(
      TetherSessionCompletionLogger::SessionCompletionReason::WIFI_DISABLED);
}

}  // namespace tether

}  // namespace chromeos
