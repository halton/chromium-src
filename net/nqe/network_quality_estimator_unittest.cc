// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/nqe/network_quality_estimator.h"

#include <stddef.h>
#include <stdint.h>

#include <cmath>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "base/logging.h"
#include "base/macros.h"
#include "base/metrics/histogram_samples.h"
#include "base/optional.h"
#include "base/run_loop.h"
#include "base/strings/string_number_conversions.h"
#include "base/test/histogram_tester.h"
#include "base/test/simple_test_tick_clock.h"
#include "base/threading/platform_thread.h"
#include "base/time/time.h"
#include "build/build_config.h"
#include "net/base/load_flags.h"
#include "net/base/network_change_notifier.h"
#include "net/http/http_response_headers.h"
#include "net/http/http_response_info.h"
#include "net/http/http_status_code.h"
#include "net/log/test_net_log.h"
#include "net/nqe/effective_connection_type.h"
#include "net/nqe/effective_connection_type_observer.h"
#include "net/nqe/external_estimate_provider.h"
#include "net/nqe/network_quality_estimator_test_util.h"
#include "net/nqe/network_quality_observation.h"
#include "net/nqe/network_quality_observation_source.h"
#include "net/nqe/observation_buffer.h"
#include "net/nqe/rtt_throughput_estimates_observer.h"
#include "net/socket/socket_performance_watcher.h"
#include "net/socket/socket_performance_watcher_factory.h"
#include "net/traffic_annotation/network_traffic_annotation_test_helper.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_test_util.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

namespace {

// Verifies that the number of samples in the bucket with minimum value
// |bucket_min| in |histogram| are at least |expected_min_count_samples|.
void ExpectBucketCountAtLeast(base::HistogramTester* histogram_tester,
                              const std::string& histogram,
                              int32_t bucket_min,
                              int32_t expected_min_count_samples) {
  std::vector<base::Bucket> buckets =
      histogram_tester->GetAllSamples(histogram);
  int actual_count_samples = 0;
  for (const auto& bucket : buckets) {
    if (bucket.min == bucket_min)
      actual_count_samples += bucket.count;
  }
  EXPECT_LE(expected_min_count_samples, actual_count_samples)
      << " histogram=" << histogram << " bucket_min=" << bucket_min
      << " expected_min_count_samples=" << expected_min_count_samples;
}

}  // namespace

namespace net {

namespace {

class TestEffectiveConnectionTypeObserver
    : public EffectiveConnectionTypeObserver {
 public:
  std::vector<EffectiveConnectionType>& effective_connection_types() {
    return effective_connection_types_;
  }

  // EffectiveConnectionTypeObserver implementation:
  void OnEffectiveConnectionTypeChanged(EffectiveConnectionType type) override {
    effective_connection_types_.push_back(type);
  }

 private:
  std::vector<EffectiveConnectionType> effective_connection_types_;
};

class TestRTTAndThroughputEstimatesObserver
    : public RTTAndThroughputEstimatesObserver {
 public:
  TestRTTAndThroughputEstimatesObserver()
      : http_rtt_(nqe::internal::InvalidRTT()),
        transport_rtt_(nqe::internal::InvalidRTT()),
        downstream_throughput_kbps_(nqe::internal::INVALID_RTT_THROUGHPUT),
        notifications_received_(0) {}

  // RTTAndThroughputEstimatesObserver implementation:
  void OnRTTOrThroughputEstimatesComputed(
      base::TimeDelta http_rtt,
      base::TimeDelta transport_rtt,
      int32_t downstream_throughput_kbps) override {
    http_rtt_ = http_rtt;
    transport_rtt_ = transport_rtt;
    downstream_throughput_kbps_ = downstream_throughput_kbps;
    notifications_received_++;
  }

  int notifications_received() const { return notifications_received_; }

  base::TimeDelta http_rtt() const { return http_rtt_; }
  base::TimeDelta transport_rtt() const { return transport_rtt_; }
  int32_t downstream_throughput_kbps() const {
    return downstream_throughput_kbps_;
  }

 private:
  base::TimeDelta http_rtt_;
  base::TimeDelta transport_rtt_;
  int32_t downstream_throughput_kbps_;
  int notifications_received_;
};

class TestRTTObserver : public NetworkQualityEstimator::RTTObserver {
 public:
  struct Observation {
    Observation(int32_t ms,
                const base::TimeTicks& ts,
                NetworkQualityObservationSource src)
        : rtt_ms(ms), timestamp(ts), source(src) {}
    int32_t rtt_ms;
    base::TimeTicks timestamp;
    NetworkQualityObservationSource source;
  };

  std::vector<Observation>& observations() { return observations_; }

  // RttObserver implementation:
  void OnRTTObservation(int32_t rtt_ms,
                        const base::TimeTicks& timestamp,
                        NetworkQualityObservationSource source) override {
    observations_.push_back(Observation(rtt_ms, timestamp, source));
  }

  // Returns the last received RTT observation that has source set to |source|.
  base::TimeDelta last_rtt(NetworkQualityObservationSource source) {
    for (std::vector<Observation>::reverse_iterator i = observations_.rbegin();
         i != observations_.rend(); ++i) {
      Observation observation = *i;
      if (observation.source == source)
        return base::TimeDelta::FromMilliseconds(observation.rtt_ms);
    }
    return nqe::internal::InvalidRTT();
  }

 private:
  std::vector<Observation> observations_;
};

class TestThroughputObserver
    : public NetworkQualityEstimator::ThroughputObserver {
 public:
  struct Observation {
    Observation(int32_t kbps,
                const base::TimeTicks& ts,
                NetworkQualityObservationSource src)
        : throughput_kbps(kbps), timestamp(ts), source(src) {}
    int32_t throughput_kbps;
    base::TimeTicks timestamp;
    NetworkQualityObservationSource source;
  };

  std::vector<Observation>& observations() { return observations_; }

  // ThroughputObserver implementation:
  void OnThroughputObservation(
      int32_t throughput_kbps,
      const base::TimeTicks& timestamp,
      NetworkQualityObservationSource source) override {
    observations_.push_back(Observation(throughput_kbps, timestamp, source));
  }

 private:
  std::vector<Observation> observations_;
};

}  // namespace

TEST(NetworkQualityEstimatorTest, TestKbpsRTTUpdates) {
  base::HistogramTester histogram_tester;
  // Enable requests to local host to be used for network quality estimation.
  std::map<std::string, std::string> variation_params;
  variation_params["throughput_min_requests_in_flight"] = "1";
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params);

  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_UNKNOWN, "test");
  histogram_tester.ExpectUniqueSample("NQE.CachedNetworkQualityAvailable",
                                      false, 2);

  base::TimeDelta rtt;
  int32_t kbps;
  EXPECT_FALSE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_FALSE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));

  TestDelegate test_delegate;
  TestURLRequestContext context(true);
  context.set_network_quality_estimator(&estimator);
  context.Init();

  std::unique_ptr<URLRequest> request(
      context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                            &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
  request->SetLoadFlags(request->load_flags() | LOAD_MAIN_FRAME_DEPRECATED);
  request->Start();
  base::RunLoop().Run();

  // Both RTT and downstream throughput should be updated.
  base::TimeDelta http_rtt;
  EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &http_rtt));
  EXPECT_EQ(http_rtt, estimator.GetHttpRTT().value());
  EXPECT_TRUE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
  EXPECT_EQ(kbps, estimator.GetDownstreamThroughputKbps().value());
  base::TimeDelta transport_rtt;
  EXPECT_FALSE(estimator.GetTransportRTT());
  EXPECT_FALSE(estimator.GetRecentTransportRTT(base::TimeTicks(),
                                               &transport_rtt, nullptr));

  // Verify the contents of the net log.
  EXPECT_LE(
      2, estimator.GetEntriesCount(NetLogEventType::NETWORK_QUALITY_CHANGED));
  EXPECT_EQ(http_rtt.InMilliseconds(),
            estimator.GetNetLogLastIntegerValue(
                NetLogEventType::NETWORK_QUALITY_CHANGED, "http_rtt_ms"));
  EXPECT_EQ(-1,
            estimator.GetNetLogLastIntegerValue(
                NetLogEventType::NETWORK_QUALITY_CHANGED, "transport_rtt_ms"));
  EXPECT_EQ(kbps, estimator.GetNetLogLastIntegerValue(
                      NetLogEventType::NETWORK_QUALITY_CHANGED,
                      "downstream_throughput_kbps"));

  // Check UMA histograms.
  histogram_tester.ExpectUniqueSample(
      "NQE.MainFrame.EffectiveConnectionType",
      EffectiveConnectionType::EFFECTIVE_CONNECTION_TYPE_UNKNOWN, 1);
  histogram_tester.ExpectUniqueSample("NQE.EstimateAvailable.MainFrame.RTT", 0,
                                      1);
  histogram_tester.ExpectUniqueSample(
      "NQE.EstimateAvailable.MainFrame.TransportRTT", 0, 1);
  histogram_tester.ExpectUniqueSample("NQE.EstimateAvailable.MainFrame.Kbps", 0,
                                      1);
  EXPECT_LE(1u,
            histogram_tester.GetAllSamples("NQE.RTT.OnECTComputation").size());
  EXPECT_LE(1u,
            histogram_tester.GetAllSamples("NQE.Kbps.OnECTComputation").size());

  histogram_tester.ExpectBucketCount(
      "NQE.RTT.ObservationSource", NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP, 1);
  histogram_tester.ExpectBucketCount(
      "NQE.Kbps.ObservationSource", NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP, 1);
  histogram_tester.ExpectTotalCount("NQE.Kbps.RawObservation.Http", 1);

  std::unique_ptr<URLRequest> request2(
      context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                            &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
  request2->SetLoadFlags(request2->load_flags() | LOAD_MAIN_FRAME_DEPRECATED);
  request2->Start();
  base::RunLoop().Run();
  histogram_tester.ExpectTotalCount("NQE.MainFrame.EffectiveConnectionType", 2);
  histogram_tester.ExpectBucketCount("NQE.EstimateAvailable.MainFrame.RTT", 1,
                                     1);
  histogram_tester.ExpectUniqueSample(
      "NQE.EstimateAvailable.MainFrame.TransportRTT", 0, 2);
  histogram_tester.ExpectBucketCount("NQE.EstimateAvailable.MainFrame.Kbps", 1,
                                     1);

  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, "test-1");
  histogram_tester.ExpectUniqueSample("NQE.CachedNetworkQualityAvailable",
                                      false, 3);
  histogram_tester.ExpectTotalCount("NQE.RatioMedianRTT.WiFi", 0);

  EXPECT_FALSE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_FALSE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));

  // Verify that metrics are logged correctly on main-frame requests.
  histogram_tester.ExpectTotalCount("NQE.MainFrame.RTT.Percentile50", 1);
  histogram_tester.ExpectTotalCount("NQE.MainFrame.TransportRTT.Percentile50",
                                    0);
  histogram_tester.ExpectTotalCount("NQE.MainFrame.Kbps.Percentile50", 1);

  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, std::string());
  histogram_tester.ExpectUniqueSample("NQE.CachedNetworkQualityAvailable",
                                      false, 4);

  EXPECT_FALSE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_FALSE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));

  std::unique_ptr<URLRequest> request3(
      context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                            &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
  request3->SetLoadFlags(request2->load_flags() | LOAD_MAIN_FRAME_DEPRECATED);
  request3->Start();
  base::RunLoop().Run();
  histogram_tester.ExpectBucketCount(
      "NQE.MainFrame.EffectiveConnectionType",
      EffectiveConnectionType::EFFECTIVE_CONNECTION_TYPE_UNKNOWN, 2);
  histogram_tester.ExpectTotalCount("NQE.MainFrame.EffectiveConnectionType", 3);
  histogram_tester.ExpectBucketCount("NQE.EstimateAvailable.MainFrame.RTT", 0,
                                     2);
  histogram_tester.ExpectBucketCount("NQE.EstimateAvailable.MainFrame.RTT", 1,
                                     1);
  histogram_tester.ExpectUniqueSample(
      "NQE.EstimateAvailable.MainFrame.TransportRTT", 0, 3);
  histogram_tester.ExpectBucketCount("NQE.EstimateAvailable.MainFrame.Kbps", 0,
                                     2);
  histogram_tester.ExpectBucketCount("NQE.EstimateAvailable.MainFrame.Kbps", 1,
                                     1);

  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_UNKNOWN, "test");
  histogram_tester.ExpectBucketCount("NQE.CachedNetworkQualityAvailable", false,
                                     4);
}

// Tests that the network quality estimator writes and reads network quality
// from the cache store correctly.
TEST(NetworkQualityEstimatorTest, Caching) {
  for (NetworkChangeNotifier::ConnectionType connection_type :
       {NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI,
        NetworkChangeNotifier::ConnectionType::CONNECTION_ETHERNET}) {
    base::HistogramTester histogram_tester;
    std::map<std::string, std::string> variation_params;
    variation_params["throughput_min_requests_in_flight"] = "1";
    variation_params["add_default_platform_observations"] = "false";
    TestNetworkQualityEstimator estimator(variation_params);

    const std::string connection_id =
        connection_type ==
                NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI
            ? "test"
            : "";

    estimator.SimulateNetworkChange(connection_type, connection_id);
    histogram_tester.ExpectUniqueSample("NQE.CachedNetworkQualityAvailable",
                                        false, 2);

    base::TimeDelta rtt;
    int32_t kbps;
    EXPECT_FALSE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
    EXPECT_FALSE(
        estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));

    TestDelegate test_delegate;
    TestURLRequestContext context(true);
    context.set_network_quality_estimator(&estimator);
    context.Init();

    // Start two requests so that the network quality is added to cache store at
    // the beginning of the second request from the network traffic observed
    // from the first request.
    for (size_t i = 0; i < 2; ++i) {
      std::unique_ptr<URLRequest> request(
          context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                                &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
      request->SetLoadFlags(request->load_flags() | LOAD_MAIN_FRAME_DEPRECATED);
      request->Start();
      base::RunLoop().Run();
    }
    histogram_tester.ExpectUniqueSample("NQE.RTT.ObservationSource",
                                        NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP,
                                        2);

    base::RunLoop().RunUntilIdle();

    // Both RTT and downstream throughput should be updated.
    EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
    EXPECT_EQ(rtt, estimator.GetHttpRTT().value());
    EXPECT_TRUE(
        estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
    EXPECT_EQ(kbps, estimator.GetDownstreamThroughputKbps().value());
    EXPECT_NE(EFFECTIVE_CONNECTION_TYPE_UNKNOWN,
              estimator.GetEffectiveConnectionType());
    EXPECT_FALSE(
        estimator.GetRecentTransportRTT(base::TimeTicks(), &rtt, nullptr));
    EXPECT_FALSE(estimator.GetTransportRTT());

    histogram_tester.ExpectBucketCount("NQE.CachedNetworkQualityAvailable",
                                       false, 2);

    // Add the observers before changing the network type.
    TestEffectiveConnectionTypeObserver observer;
    estimator.AddEffectiveConnectionTypeObserver(&observer);
    TestRTTObserver rtt_observer;
    estimator.AddRTTObserver(&rtt_observer);
    TestThroughputObserver throughput_observer;
    estimator.AddThroughputObserver(&throughput_observer);

    // |observer| should be notified as soon as it is added.
    base::RunLoop().RunUntilIdle();
    EXPECT_EQ(1U, observer.effective_connection_types().size());

    int num_net_log_entries =
        estimator.GetEntriesCount(NetLogEventType::NETWORK_QUALITY_CHANGED);
    EXPECT_LE(2, num_net_log_entries);

    estimator.SimulateNetworkChange(connection_type, connection_id);
    histogram_tester.ExpectBucketCount(
        "NQE.RTT.ObservationSource",
        NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP_CACHED_ESTIMATE, 1);
    histogram_tester.ExpectBucketCount(
        "NQE.RTT.ObservationSource",
        NETWORK_QUALITY_OBSERVATION_SOURCE_TRANSPORT_CACHED_ESTIMATE, 1);
    histogram_tester.ExpectTotalCount("NQE.RTT.ObservationSource", 4);

    histogram_tester.ExpectBucketCount(
        "NQE.Kbps.ObservationSource",
        NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP_CACHED_ESTIMATE, 1);
    histogram_tester.ExpectTotalCount(
        "NQE.Kbps.RawObservation.HttpCachedEstimate", 1);

    // Verify the contents of the net log.
    EXPECT_LE(
        1, estimator.GetEntriesCount(NetLogEventType::NETWORK_QUALITY_CHANGED) -
               num_net_log_entries);
    EXPECT_NE(-1, estimator.GetNetLogLastIntegerValue(
                      NetLogEventType::NETWORK_QUALITY_CHANGED, "http_rtt_ms"));
    EXPECT_NE(
        -1, estimator.GetNetLogLastIntegerValue(
                NetLogEventType::NETWORK_QUALITY_CHANGED, "transport_rtt_ms"));
    EXPECT_NE(-1, estimator.GetNetLogLastIntegerValue(
                      NetLogEventType::NETWORK_QUALITY_CHANGED,
                      "downstream_throughput_kbps"));
    EXPECT_EQ(GetNameForEffectiveConnectionType(
                  estimator.GetEffectiveConnectionType()),
              estimator.GetNetLogLastStringValue(
                  NetLogEventType::NETWORK_QUALITY_CHANGED,
                  "effective_connection_type"));

    histogram_tester.ExpectBucketCount("NQE.CachedNetworkQualityAvailable",
                                       true, 1);
    histogram_tester.ExpectTotalCount("NQE.CachedNetworkQualityAvailable", 3);
    base::RunLoop().RunUntilIdle();

    // Verify that the cached network quality was read, and observers were
    // notified. |observer| must be notified once right after it was added, and
    // once again after the cached network quality was read.
    EXPECT_LE(2U, observer.effective_connection_types().size());
    EXPECT_EQ(estimator.GetEffectiveConnectionType(),
              observer.effective_connection_types().back());
    EXPECT_EQ(2U, rtt_observer.observations().size());
    EXPECT_EQ(1U, throughput_observer.observations().size());
  }
}

// Tests that the network quality estimator does not read the network quality
// from the cache store when caching is not enabled.
TEST(NetworkQualityEstimatorTest, CachingDisabled) {
  base::HistogramTester histogram_tester;
  std::map<std::string, std::string> variation_params;
  // Do not set |persistent_cache_reading_enabled| variation param.
  variation_params["persistent_cache_reading_enabled"] = "false";
  variation_params["throughput_min_requests_in_flight"] = "1";
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params);

  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, "test");
  histogram_tester.ExpectTotalCount("NQE.CachedNetworkQualityAvailable", 0);

  base::TimeDelta rtt;
  int32_t kbps;
  EXPECT_FALSE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_FALSE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));

  TestDelegate test_delegate;
  TestURLRequestContext context(true);
  context.set_network_quality_estimator(&estimator);
  context.Init();

  // Start two requests so that the network quality is added to cache store at
  // the beginning of the second request from the network traffic observed from
  // the first request.
  for (size_t i = 0; i < 2; ++i) {
    std::unique_ptr<URLRequest> request(
        context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                              &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
    request->SetLoadFlags(request->load_flags() | LOAD_MAIN_FRAME_DEPRECATED);
    request->Start();
    base::RunLoop().Run();
  }

  base::RunLoop().RunUntilIdle();

  // Both RTT and downstream throughput should be updated.
  EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_EQ(rtt, estimator.GetHttpRTT().value());
  EXPECT_TRUE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
  EXPECT_EQ(kbps, estimator.GetDownstreamThroughputKbps().value());
  EXPECT_NE(EFFECTIVE_CONNECTION_TYPE_UNKNOWN,
            estimator.GetEffectiveConnectionType());
  EXPECT_FALSE(
      estimator.GetRecentTransportRTT(base::TimeTicks(), &rtt, nullptr));
  EXPECT_FALSE(estimator.GetTransportRTT());

  histogram_tester.ExpectTotalCount("NQE.CachedNetworkQualityAvailable", 0);

  // Add the observers before changing the network type.
  TestRTTObserver rtt_observer;
  estimator.AddRTTObserver(&rtt_observer);
  TestThroughputObserver throughput_observer;
  estimator.AddThroughputObserver(&throughput_observer);

  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_2G, "test");

  histogram_tester.ExpectTotalCount("NQE.CachedNetworkQualityAvailable", 0);
  base::RunLoop().RunUntilIdle();

  // Verify that the cached network quality was read, and observers were
  // notified. |observer| must be notified once right after it was added, and
  // once again after the cached network quality was read.
  EXPECT_EQ(0U, rtt_observer.observations().size());
  EXPECT_EQ(0U, throughput_observer.observations().size());
}

TEST(NetworkQualityEstimatorTest, QuicObservations) {
  base::HistogramTester histogram_tester;
  std::map<std::string, std::string> variation_params;
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params);
  estimator.OnUpdatedTransportRTTAvailable(
      SocketPerformanceWatcherFactory::PROTOCOL_TCP,
      base::TimeDelta::FromMilliseconds(10), base::nullopt);
  estimator.OnUpdatedTransportRTTAvailable(
      SocketPerformanceWatcherFactory::PROTOCOL_QUIC,
      base::TimeDelta::FromMilliseconds(10), base::nullopt);
  histogram_tester.ExpectBucketCount("NQE.RTT.ObservationSource",
                                     NETWORK_QUALITY_OBSERVATION_SOURCE_TCP, 1);
  histogram_tester.ExpectBucketCount(
      "NQE.RTT.ObservationSource", NETWORK_QUALITY_OBSERVATION_SOURCE_QUIC, 1);
  histogram_tester.ExpectTotalCount("NQE.RTT.ObservationSource", 2);
}

TEST(NetworkQualityEstimatorTest, StoreObservations) {
  std::map<std::string, std::string> variation_params;
  variation_params["throughput_min_requests_in_flight"] = "1";
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params);

  base::TimeDelta rtt;
  int32_t kbps;
  EXPECT_FALSE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_FALSE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));

  TestDelegate test_delegate;
  TestURLRequestContext context(true);
  context.set_network_quality_estimator(&estimator);
  context.Init();

  const size_t kMaxObservations = 10;
  for (size_t i = 0; i < kMaxObservations; ++i) {
    std::unique_ptr<URLRequest> request(
        context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                              &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
    request->Start();
    base::RunLoop().Run();
    EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
    EXPECT_TRUE(
        estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
  }

  // Verify that the stored observations are cleared on network change.
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, "test-2");
  EXPECT_FALSE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_FALSE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
}

// This test notifies NetworkQualityEstimator of received data. Next,
// throughput and RTT percentiles are checked for correctness by doing simple
// verifications.
TEST(NetworkQualityEstimatorTest, ComputedPercentiles) {
  std::map<std::string, std::string> variation_params;
  variation_params["throughput_min_requests_in_flight"] = "1";
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params);

  EXPECT_EQ(nqe::internal::InvalidRTT(),
            estimator.GetRTTEstimateInternal(
                base::TimeTicks(),
                base::Optional<NetworkQualityEstimator::Statistic>(),
                nqe::internal::ObservationCategory::kHttp, 100, nullptr));
  EXPECT_EQ(nqe::internal::INVALID_RTT_THROUGHPUT,
            estimator.GetDownlinkThroughputKbpsEstimateInternal(
                base::TimeTicks(), 100));

  TestDelegate test_delegate;
  TestURLRequestContext context(true);
  context.set_network_quality_estimator(&estimator);
  context.Init();

  for (size_t i = 0; i < 10U; ++i) {
    std::unique_ptr<URLRequest> request(
        context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                              &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
    request->Start();
    base::RunLoop().Run();
  }

  // Verify the percentiles through simple tests.
  for (int i = 0; i <= 100; ++i) {
    EXPECT_GT(estimator.GetDownlinkThroughputKbpsEstimateInternal(
                  base::TimeTicks(), i),
              0);
    EXPECT_LT(estimator.GetRTTEstimateInternal(
                  base::TimeTicks(),
                  base::Optional<NetworkQualityEstimator::Statistic>(),
                  nqe::internal::ObservationCategory::kHttp, i, nullptr),
              base::TimeDelta::Max());

    if (i != 0) {
      // Throughput percentiles are in decreasing order.
      EXPECT_LE(estimator.GetDownlinkThroughputKbpsEstimateInternal(
                    base::TimeTicks(), i),
                estimator.GetDownlinkThroughputKbpsEstimateInternal(
                    base::TimeTicks(), i - 1));

      // RTT percentiles are in increasing order.
      EXPECT_GE(estimator.GetRTTEstimateInternal(
                    base::TimeTicks(),
                    base::Optional<NetworkQualityEstimator::Statistic>(),
                    nqe::internal::ObservationCategory::kHttp, i, nullptr),
                estimator.GetRTTEstimateInternal(
                    base::TimeTicks(),
                    base::Optional<NetworkQualityEstimator::Statistic>(),
                    nqe::internal::ObservationCategory::kHttp, i - 1, nullptr));
    }
  }
}

// Verifies that the observers receive the notifications when default estimates
// are added to the observations.
TEST(NetworkQualityEstimatorTest, DefaultObservations) {
  base::HistogramTester histogram_tester;

  TestEffectiveConnectionTypeObserver effective_connection_type_observer;
  TestRTTAndThroughputEstimatesObserver rtt_throughput_estimates_observer;
  TestRTTObserver rtt_observer;
  TestThroughputObserver throughput_observer;
  std::map<std::string, std::string> variation_params;
  TestNetworkQualityEstimator estimator(
      nullptr, variation_params, false, false,
      std::make_unique<BoundTestNetLog>());

  // Default observations should be added when constructing the |estimator|.
  histogram_tester.ExpectBucketCount(
      "NQE.RTT.ObservationSource",
      NETWORK_QUALITY_OBSERVATION_SOURCE_DEFAULT_HTTP_FROM_PLATFORM, 1);
  histogram_tester.ExpectBucketCount(
      "NQE.RTT.ObservationSource",
      NETWORK_QUALITY_OBSERVATION_SOURCE_DEFAULT_TRANSPORT_FROM_PLATFORM, 1);
  histogram_tester.ExpectBucketCount(
      "NQE.Kbps.ObservationSource",
      NETWORK_QUALITY_OBSERVATION_SOURCE_DEFAULT_HTTP_FROM_PLATFORM, 1);
  histogram_tester.ExpectTotalCount("NQE.Kbps.RawObservation.HttpPlatform", 1);
  histogram_tester.ExpectTotalCount("NQE.RTT.ObservationSource", 2);
  histogram_tester.ExpectTotalCount("NQE.RTT.RawObservation.HttpPlatform", 1);
  histogram_tester.ExpectTotalCount("NQE.RTT.RawObservation.TransportPlatform",
                                    1);
  histogram_tester.ExpectTotalCount("NQE.Kbps.ObservationSource", 1);

  // Default observations should be added on connection change.
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_UNKNOWN, "unknown-1");
  histogram_tester.ExpectBucketCount(
      "NQE.RTT.ObservationSource",
      NETWORK_QUALITY_OBSERVATION_SOURCE_DEFAULT_HTTP_FROM_PLATFORM, 2);
  histogram_tester.ExpectBucketCount(
      "NQE.RTT.ObservationSource",
      NETWORK_QUALITY_OBSERVATION_SOURCE_DEFAULT_TRANSPORT_FROM_PLATFORM, 2);
  histogram_tester.ExpectBucketCount(
      "NQE.Kbps.ObservationSource",
      NETWORK_QUALITY_OBSERVATION_SOURCE_DEFAULT_HTTP_FROM_PLATFORM, 2);
  histogram_tester.ExpectTotalCount("NQE.Kbps.RawObservation.HttpPlatform", 2);
  histogram_tester.ExpectTotalCount("NQE.RTT.ObservationSource", 4);
  histogram_tester.ExpectTotalCount("NQE.RTT.RawObservation.HttpPlatform", 2);
  histogram_tester.ExpectTotalCount("NQE.RTT.RawObservation.TransportPlatform",
                                    2);
  histogram_tester.ExpectTotalCount("NQE.Kbps.ObservationSource", 2);

  base::TimeDelta rtt;
  int32_t kbps;

  // Default estimates should be available.
  EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(115), rtt);
  EXPECT_EQ(rtt, estimator.GetHttpRTT().value());
  EXPECT_TRUE(
      estimator.GetRecentTransportRTT(base::TimeTicks(), &rtt, nullptr));
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(55), rtt);
  EXPECT_EQ(rtt, estimator.GetTransportRTT().value());
  EXPECT_TRUE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
  EXPECT_EQ(1961, kbps);
  EXPECT_EQ(kbps, estimator.GetDownstreamThroughputKbps().value());

  estimator.AddEffectiveConnectionTypeObserver(
      &effective_connection_type_observer);
  estimator.AddRTTAndThroughputEstimatesObserver(
      &rtt_throughput_estimates_observer);
  estimator.AddRTTObserver(&rtt_observer);
  estimator.AddThroughputObserver(&throughput_observer);

  // Simulate network change to 3G. Default estimates should be available.
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_3G, "test-3");
  EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  // Taken from network_quality_estimator_params.cc.
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(273), rtt);
  EXPECT_EQ(rtt, estimator.GetHttpRTT().value());
  EXPECT_TRUE(
      estimator.GetRecentTransportRTT(base::TimeTicks(), &rtt, nullptr));
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(209), rtt);
  EXPECT_EQ(rtt, estimator.GetTransportRTT());
  EXPECT_TRUE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
  EXPECT_EQ(749, kbps);
  EXPECT_EQ(kbps, estimator.GetDownstreamThroughputKbps().value());

  EXPECT_NE(EFFECTIVE_CONNECTION_TYPE_UNKNOWN,
            estimator.GetEffectiveConnectionType());
  EXPECT_EQ(
      1U,
      effective_connection_type_observer.effective_connection_types().size());
  EXPECT_NE(
      EFFECTIVE_CONNECTION_TYPE_UNKNOWN,
      effective_connection_type_observer.effective_connection_types().front());

  // Verify the contents of the net log.
  EXPECT_LE(
      3, estimator.GetEntriesCount(NetLogEventType::NETWORK_QUALITY_CHANGED));
  EXPECT_NE(
      GetNameForEffectiveConnectionType(EFFECTIVE_CONNECTION_TYPE_UNKNOWN),
      estimator.GetNetLogLastStringValue(
          NetLogEventType::NETWORK_QUALITY_CHANGED,
          "effective_connection_type"));

  EXPECT_EQ(4, rtt_throughput_estimates_observer.notifications_received());
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(273),
            rtt_throughput_estimates_observer.http_rtt());
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(209),
            rtt_throughput_estimates_observer.transport_rtt());
  EXPECT_EQ(749,
            rtt_throughput_estimates_observer.downstream_throughput_kbps());

  EXPECT_EQ(2U, rtt_observer.observations().size());
  EXPECT_EQ(273, rtt_observer.observations().at(0).rtt_ms);
  EXPECT_EQ(NETWORK_QUALITY_OBSERVATION_SOURCE_DEFAULT_HTTP_FROM_PLATFORM,
            rtt_observer.observations().at(0).source);
  EXPECT_EQ(209, rtt_observer.observations().at(1).rtt_ms);
  EXPECT_EQ(NETWORK_QUALITY_OBSERVATION_SOURCE_DEFAULT_TRANSPORT_FROM_PLATFORM,
            rtt_observer.observations().at(1).source);

  EXPECT_EQ(1U, throughput_observer.observations().size());
  EXPECT_EQ(749, throughput_observer.observations().at(0).throughput_kbps);
  EXPECT_EQ(NETWORK_QUALITY_OBSERVATION_SOURCE_DEFAULT_HTTP_FROM_PLATFORM,
            throughput_observer.observations().at(0).source);
}

// Verifies that the default observations are added to the set of observations.
// If default observations are overridden using field trial parameters, verify
// that the overriding values are used.
TEST(NetworkQualityEstimatorTest, DefaultObservationsOverridden) {
  std::map<std::string, std::string> variation_params;
  variation_params["Unknown.DefaultMedianKbps"] = "100";
  variation_params["WiFi.DefaultMedianKbps"] = "200";
  variation_params["2G.DefaultMedianKbps"] = "300";

  variation_params["Unknown.DefaultMedianRTTMsec"] = "1000";
  variation_params["WiFi.DefaultMedianRTTMsec"] = "2000";
  // Negative variation value should not be used.
  variation_params["2G.DefaultMedianRTTMsec"] = "-5";

  variation_params["Unknown.DefaultMedianTransportRTTMsec"] = "500";
  variation_params["WiFi.DefaultMedianTransportRTTMsec"] = "1000";
  // Negative variation value should not be used.
  variation_params["2G.DefaultMedianTransportRTTMsec"] = "-5";

  TestNetworkQualityEstimator estimator(
      nullptr, variation_params, false, false,
      std::make_unique<BoundTestNetLog>());
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_UNKNOWN, "unknown-1");

  base::TimeDelta rtt;
  int32_t kbps;

  EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(1000), rtt);
  EXPECT_EQ(rtt, estimator.GetHttpRTT().value());
  EXPECT_TRUE(
      estimator.GetRecentTransportRTT(base::TimeTicks(), &rtt, nullptr));
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(500), rtt);
  EXPECT_EQ(rtt, estimator.GetTransportRTT().value());
  EXPECT_TRUE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
  EXPECT_EQ(100, kbps);
  EXPECT_EQ(kbps, estimator.GetDownstreamThroughputKbps().value());

  // Simulate network change to Wi-Fi.
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, "test-1");
  EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(2000), rtt);
  EXPECT_EQ(rtt, estimator.GetHttpRTT().value());
  EXPECT_TRUE(
      estimator.GetRecentTransportRTT(base::TimeTicks(), &rtt, nullptr));
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(1000), rtt);
  EXPECT_EQ(rtt, estimator.GetTransportRTT().value());
  EXPECT_TRUE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
  EXPECT_EQ(200, kbps);
  EXPECT_EQ(kbps, estimator.GetDownstreamThroughputKbps().value());

  // Simulate network change to 2G. Only the Kbps default estimate should be
  // available.
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_2G, "test-2");
  EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  // Taken from network_quality_estimator_params.cc.
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(1726), rtt);
  EXPECT_EQ(rtt, estimator.GetHttpRTT().value());
  EXPECT_TRUE(
      estimator.GetRecentTransportRTT(base::TimeTicks(), &rtt, nullptr));
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(1531), rtt);
  EXPECT_EQ(rtt, estimator.GetTransportRTT().value());
  EXPECT_TRUE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
  EXPECT_EQ(300, kbps);
  EXPECT_EQ(kbps, estimator.GetDownstreamThroughputKbps().value());

  // Simulate network change to 3G. Default estimates should be available.
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_3G, "test-3");
  EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(273), rtt);
  EXPECT_EQ(rtt, estimator.GetHttpRTT().value());
  EXPECT_TRUE(
      estimator.GetRecentTransportRTT(base::TimeTicks(), &rtt, nullptr));
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(209), rtt);
  EXPECT_EQ(rtt, estimator.GetTransportRTT().value());
  EXPECT_TRUE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
  EXPECT_EQ(749, kbps);
  EXPECT_EQ(kbps, estimator.GetDownstreamThroughputKbps().value());
}

// Tests that |GetEffectiveConnectionType| returns
// EFFECTIVE_CONNECTION_TYPE_OFFLINE when the device is currently offline.
TEST(NetworkQualityEstimatorTest, Offline) {
  std::map<std::string, std::string> variation_params;
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params);

  const struct {
    NetworkChangeNotifier::ConnectionType connection_type;
    EffectiveConnectionType expected_connection_type;
  } tests[] = {
      {NetworkChangeNotifier::CONNECTION_2G, EFFECTIVE_CONNECTION_TYPE_UNKNOWN},
      {NetworkChangeNotifier::CONNECTION_NONE,
       EFFECTIVE_CONNECTION_TYPE_OFFLINE},
      {NetworkChangeNotifier::CONNECTION_3G, EFFECTIVE_CONNECTION_TYPE_UNKNOWN},
  };

  for (const auto& test : tests) {
    estimator.SimulateNetworkChange(test.connection_type, "test");
    EXPECT_EQ(test.expected_connection_type,
              estimator.GetEffectiveConnectionType());
  }
}

// Tests that |GetEffectiveConnectionType| returns correct connection type when
// only RTT thresholds are specified in the variation params.
TEST(NetworkQualityEstimatorTest, ObtainThresholdsOnlyRTT) {
  std::map<std::string, std::string> variation_params;

  variation_params["Offline.ThresholdMedianHttpRTTMsec"] = "4000";
  variation_params["Slow2G.ThresholdMedianHttpRTTMsec"] = "2000";
  variation_params["2G.ThresholdMedianHttpRTTMsec"] = "1000";
  variation_params["3G.ThresholdMedianHttpRTTMsec"] = "500";

  TestNetworkQualityEstimator estimator(variation_params);

  // Simulate the connection type as Wi-Fi so that GetEffectiveConnectionType
  // does not return Offline if the device is offline.
  estimator.SimulateNetworkChange(NetworkChangeNotifier::CONNECTION_WIFI,
                                  "test");

  const struct {
    int32_t rtt_msec;
    EffectiveConnectionType expected_conn_type;
  } tests[] = {
      {5000, EFFECTIVE_CONNECTION_TYPE_OFFLINE},
      {4000, EFFECTIVE_CONNECTION_TYPE_OFFLINE},
      {3000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {2000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {1500, EFFECTIVE_CONNECTION_TYPE_2G},
      {1000, EFFECTIVE_CONNECTION_TYPE_2G},
      {700, EFFECTIVE_CONNECTION_TYPE_3G},
      {500, EFFECTIVE_CONNECTION_TYPE_3G},
      {400, EFFECTIVE_CONNECTION_TYPE_4G},
      {300, EFFECTIVE_CONNECTION_TYPE_4G},
      {200, EFFECTIVE_CONNECTION_TYPE_4G},
      {100, EFFECTIVE_CONNECTION_TYPE_4G},
      {20, EFFECTIVE_CONNECTION_TYPE_4G},
  };

  for (const auto& test : tests) {
    estimator.set_start_time_null_http_rtt(
        base::TimeDelta::FromMilliseconds(test.rtt_msec));
    estimator.set_recent_http_rtt(
        base::TimeDelta::FromMilliseconds(test.rtt_msec));
    estimator.set_start_time_null_downlink_throughput_kbps(INT32_MAX);
    estimator.set_recent_downlink_throughput_kbps(INT32_MAX);
    // Run one main frame request to force recomputation of effective connection
    // type.
    estimator.RunOneRequest();
    EXPECT_EQ(test.expected_conn_type, estimator.GetEffectiveConnectionType());
  }
}

// Tests that default transport RTT thresholds for different effective
// connection types are correctly set.
TEST(NetworkQualityEstimatorTest, DefaultTransportRTTBasedThresholds) {
  const struct {
    bool override_defaults_using_variation_params;
    int32_t transport_rtt_msec;
    EffectiveConnectionType expected_conn_type;
  } tests[] = {
      // When the variation params do not override connection thresholds,
      // default values should be used.
      {false, 5000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {false, 4000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {false, 3000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {false, 2000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {false, 1500, EFFECTIVE_CONNECTION_TYPE_2G},
      {false, 1000, EFFECTIVE_CONNECTION_TYPE_3G},
      {false, 100, EFFECTIVE_CONNECTION_TYPE_4G},
      {false, 20, EFFECTIVE_CONNECTION_TYPE_4G},
      // Override default thresholds using variation params.
      {true, 5000, EFFECTIVE_CONNECTION_TYPE_OFFLINE},
      {true, 4000, EFFECTIVE_CONNECTION_TYPE_OFFLINE},
      {true, 3000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {true, 2000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {true, 1500, EFFECTIVE_CONNECTION_TYPE_2G},
      {true, 1000, EFFECTIVE_CONNECTION_TYPE_2G},
      {true, 20, EFFECTIVE_CONNECTION_TYPE_4G},
  };

  for (const auto& test : tests) {
    std::map<std::string, std::string> variation_params;
    variation_params["effective_connection_type_algorithm"] =
        "TransportRTTOrDownstreamThroughput";
    if (test.override_defaults_using_variation_params) {
      variation_params["Offline.ThresholdMedianTransportRTTMsec"] = "4000";
      variation_params["Slow2G.ThresholdMedianTransportRTTMsec"] = "2000";
      variation_params["2G.ThresholdMedianTransportRTTMsec"] = "1000";
    }

    TestNetworkQualityEstimator estimator(variation_params);

    // Simulate the connection type as Wi-Fi so that GetEffectiveConnectionType
    // does not return Offline if the device is offline.
    estimator.SimulateNetworkChange(NetworkChangeNotifier::CONNECTION_WIFI,
                                    "test");

    estimator.set_start_time_null_transport_rtt(
        base::TimeDelta::FromMilliseconds(test.transport_rtt_msec));
    estimator.set_recent_transport_rtt(
        base::TimeDelta::FromMilliseconds(test.transport_rtt_msec));
    estimator.set_start_time_null_downlink_throughput_kbps(INT32_MAX);
    estimator.set_recent_downlink_throughput_kbps(INT32_MAX);
    // Run one main frame request to force recomputation of effective connection
    // type.
    estimator.RunOneRequest();
    EXPECT_EQ(test.expected_conn_type, estimator.GetEffectiveConnectionType());
  }
}

// Tests that default HTTP RTT thresholds for different effective
// connection types are correctly set.
TEST(NetworkQualityEstimatorTest, DefaultHttpRTTBasedThresholds) {
  const struct {
    bool override_defaults_using_variation_params;
    int32_t http_rtt_msec;
    EffectiveConnectionType expected_conn_type;
  } tests[] = {
      // When the variation params do not override connection thresholds,
      // default values should be used.
      {false, 5000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {false, 4000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {false, 3000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {false, 2000, EFFECTIVE_CONNECTION_TYPE_2G},
      {false, 1500, EFFECTIVE_CONNECTION_TYPE_2G},
      {false, 1000, EFFECTIVE_CONNECTION_TYPE_3G},
      {false, 100, EFFECTIVE_CONNECTION_TYPE_4G},
      {false, 20, EFFECTIVE_CONNECTION_TYPE_4G},
      // Override default thresholds using variation params.
      {true, 5000, EFFECTIVE_CONNECTION_TYPE_OFFLINE},
      {true, 4000, EFFECTIVE_CONNECTION_TYPE_OFFLINE},
      {true, 3000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {true, 2000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {true, 1500, EFFECTIVE_CONNECTION_TYPE_2G},
      {true, 1000, EFFECTIVE_CONNECTION_TYPE_2G},
      {true, 20, EFFECTIVE_CONNECTION_TYPE_4G},
  };

  for (const auto& test : tests) {
    std::map<std::string, std::string> variation_params;
    if (test.override_defaults_using_variation_params) {
      variation_params["Offline.ThresholdMedianHttpRTTMsec"] = "4000";
      variation_params["Slow2G.ThresholdMedianHttpRTTMsec"] = "2000";
      variation_params["2G.ThresholdMedianHttpRTTMsec"] = "1000";
    }

    TestNetworkQualityEstimator estimator(variation_params);

    // Simulate the connection type as Wi-Fi so that GetEffectiveConnectionType
    // does not return Offline if the device is offline.
    estimator.SimulateNetworkChange(NetworkChangeNotifier::CONNECTION_WIFI,
                                    "test");

    estimator.set_start_time_null_http_rtt(
        base::TimeDelta::FromMilliseconds(test.http_rtt_msec));
    estimator.set_recent_http_rtt(
        base::TimeDelta::FromMilliseconds(test.http_rtt_msec));
    estimator.set_start_time_null_downlink_throughput_kbps(INT32_MAX);
    estimator.set_recent_downlink_throughput_kbps(INT32_MAX);
    // Run one main frame request to force recomputation of effective connection
    // type.
    estimator.RunOneRequest();
    EXPECT_EQ(test.expected_conn_type, estimator.GetEffectiveConnectionType());
  }
}

// Tests that |GetEffectiveConnectionType| returns correct connection type when
// only transport RTT thresholds are specified in the variation params.
#if defined(OS_IOS)
// Flaky on iOS: crbug.com/672917.
#define MAYBE_ObtainThresholdsOnlyTransportRTT \
  DISABLED_ObtainThresholdsOnlyTransportRTT
#else
#define MAYBE_ObtainThresholdsOnlyTransportRTT ObtainThresholdsOnlyTransportRTT
#endif
TEST(NetworkQualityEstimatorTest, MAYBE_ObtainThresholdsOnlyTransportRTT) {
  std::map<std::string, std::string> variation_params;
  variation_params["effective_connection_type_algorithm"] =
      "TransportRTTOrDownstreamThroughput";

  variation_params["Offline.ThresholdMedianTransportRTTMsec"] = "4000";
  variation_params["Slow2G.ThresholdMedianTransportRTTMsec"] = "2000";
  variation_params["2G.ThresholdMedianTransportRTTMsec"] = "1000";
  variation_params["3G.ThresholdMedianTransportRTTMsec"] = "500";

  TestNetworkQualityEstimator estimator(variation_params);

  // Simulate the connection type as Wi-Fi so that GetEffectiveConnectionType
  // does not return Offline if the device is offline.
  estimator.SimulateNetworkChange(NetworkChangeNotifier::CONNECTION_WIFI,
                                  "test");

  const struct {
    int32_t transport_rtt_msec;
    EffectiveConnectionType expected_conn_type;
  } tests[] = {
      {5000, EFFECTIVE_CONNECTION_TYPE_OFFLINE},
      {4000, EFFECTIVE_CONNECTION_TYPE_OFFLINE},
      {3000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {2000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {1500, EFFECTIVE_CONNECTION_TYPE_2G},
      {1000, EFFECTIVE_CONNECTION_TYPE_2G},
      {700, EFFECTIVE_CONNECTION_TYPE_3G},
      {500, EFFECTIVE_CONNECTION_TYPE_3G},
      {400, EFFECTIVE_CONNECTION_TYPE_4G},
      {300, EFFECTIVE_CONNECTION_TYPE_4G},
      {200, EFFECTIVE_CONNECTION_TYPE_4G},
      {100, EFFECTIVE_CONNECTION_TYPE_4G},
      {20, EFFECTIVE_CONNECTION_TYPE_4G},
  };

  for (const auto& test : tests) {
    estimator.set_start_time_null_transport_rtt(
        base::TimeDelta::FromMilliseconds(test.transport_rtt_msec));
    estimator.set_recent_transport_rtt(
        base::TimeDelta::FromMilliseconds(test.transport_rtt_msec));
    estimator.set_start_time_null_downlink_throughput_kbps(INT32_MAX);
    estimator.set_recent_downlink_throughput_kbps(INT32_MAX);
    // Run one main frame request to force recomputation of effective connection
    // type.
    estimator.RunOneRequest();
    EXPECT_EQ(test.expected_conn_type, estimator.GetEffectiveConnectionType());
  }
}

// Tests that |GetEffectiveConnectionType| returns correct connection type when
// both HTTP RTT and throughput thresholds are specified in the variation
// params.
TEST(NetworkQualityEstimatorTest, ObtainThresholdsHttpRTTandThroughput) {
  std::map<std::string, std::string> variation_params;

  variation_params["Offline.ThresholdMedianHttpRTTMsec"] = "4000";
  variation_params["Slow2G.ThresholdMedianHttpRTTMsec"] = "2000";
  variation_params["2G.ThresholdMedianHttpRTTMsec"] = "1000";
  variation_params["3G.ThresholdMedianHttpRTTMsec"] = "500";

  variation_params["Offline.ThresholdMedianKbps"] = "10";
  variation_params["Slow2G.ThresholdMedianKbps"] = "100";
  variation_params["2G.ThresholdMedianKbps"] = "300";
  variation_params["3G.ThresholdMedianKbps"] = "500";

  TestNetworkQualityEstimator estimator(variation_params);

  // Simulate the connection type as Wi-Fi so that GetEffectiveConnectionType
  // does not return Offline if the device is offline.
  estimator.SimulateNetworkChange(NetworkChangeNotifier::CONNECTION_WIFI,
                                  "test");

  const struct {
    int32_t rtt_msec;
    int32_t downlink_throughput_kbps;
    EffectiveConnectionType expected_conn_type;
  } tests[] = {
      // Set RTT to a very low value to observe the effect of throughput.
      // Throughput is the bottleneck.
      {1, 5, EFFECTIVE_CONNECTION_TYPE_OFFLINE},
      {1, 10, EFFECTIVE_CONNECTION_TYPE_OFFLINE},
      {1, 50, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {1, 100, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {1, 150, EFFECTIVE_CONNECTION_TYPE_2G},
      {1, 300, EFFECTIVE_CONNECTION_TYPE_2G},
      {1, 400, EFFECTIVE_CONNECTION_TYPE_3G},
      {1, 500, EFFECTIVE_CONNECTION_TYPE_3G},
      {1, 700, EFFECTIVE_CONNECTION_TYPE_4G},
      {1, 1000, EFFECTIVE_CONNECTION_TYPE_4G},
      {1, 1500, EFFECTIVE_CONNECTION_TYPE_4G},
      {1, 2500, EFFECTIVE_CONNECTION_TYPE_4G},
      // Set both RTT and throughput. RTT is the bottleneck.
      {3000, 25000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {700, 25000, EFFECTIVE_CONNECTION_TYPE_3G},
  };

  for (const auto& test : tests) {
    estimator.set_start_time_null_http_rtt(
        base::TimeDelta::FromMilliseconds(test.rtt_msec));
    estimator.set_recent_http_rtt(
        base::TimeDelta::FromMilliseconds(test.rtt_msec));
    estimator.set_start_time_null_downlink_throughput_kbps(
        test.downlink_throughput_kbps);
    estimator.set_recent_downlink_throughput_kbps(
        test.downlink_throughput_kbps);
    // Run one main frame request to force recomputation of effective connection
    // type.
    estimator.RunOneRequest();
    EXPECT_EQ(test.expected_conn_type, estimator.GetEffectiveConnectionType());
  }
}

// Tests that |GetEffectiveConnectionType| returns correct connection type when
// both transport RTT and throughput thresholds are specified in the variation
// params.
TEST(NetworkQualityEstimatorTest, ObtainThresholdsTransportRTTandThroughput) {
  std::map<std::string, std::string> variation_params;
  variation_params["effective_connection_type_algorithm"] =
      "TransportRTTOrDownstreamThroughput";

  variation_params["Offline.ThresholdMedianTransportRTTMsec"] = "4000";
  variation_params["Slow2G.ThresholdMedianTransportRTTMsec"] = "2000";
  variation_params["2G.ThresholdMedianTransportRTTMsec"] = "1000";
  variation_params["3G.ThresholdMedianTransportRTTMsec"] = "500";

  variation_params["Offline.ThresholdMedianKbps"] = "10";
  variation_params["Slow2G.ThresholdMedianKbps"] = "100";
  variation_params["2G.ThresholdMedianKbps"] = "300";
  variation_params["3G.ThresholdMedianKbps"] = "500";

  TestNetworkQualityEstimator estimator(variation_params);

  // Simulate the connection type as Wi-Fi so that GetEffectiveConnectionType
  // does not return Offline if the device is offline.
  estimator.SimulateNetworkChange(NetworkChangeNotifier::CONNECTION_WIFI,
                                  "test");

  const struct {
    int32_t transport_rtt_msec;
    int32_t downlink_throughput_kbps;
    EffectiveConnectionType expected_conn_type;
  } tests[] = {
      // Set RTT to a very low value to observe the effect of throughput.
      // Throughput is the bottleneck.
      {1, 5, EFFECTIVE_CONNECTION_TYPE_OFFLINE},
      {1, 10, EFFECTIVE_CONNECTION_TYPE_OFFLINE},
      {1, 50, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {1, 100, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {1, 150, EFFECTIVE_CONNECTION_TYPE_2G},
      {1, 300, EFFECTIVE_CONNECTION_TYPE_2G},
      {1, 400, EFFECTIVE_CONNECTION_TYPE_3G},
      {1, 500, EFFECTIVE_CONNECTION_TYPE_3G},
      {1, 700, EFFECTIVE_CONNECTION_TYPE_4G},
      {1, 1000, EFFECTIVE_CONNECTION_TYPE_4G},
      {1, 1500, EFFECTIVE_CONNECTION_TYPE_4G},
      {1, 2500, EFFECTIVE_CONNECTION_TYPE_4G},
      // Set both RTT and throughput. RTT is the bottleneck.
      {3000, 25000, EFFECTIVE_CONNECTION_TYPE_SLOW_2G},
      {700, 25000, EFFECTIVE_CONNECTION_TYPE_3G},
  };

  for (const auto& test : tests) {
    estimator.set_start_time_null_transport_rtt(
        base::TimeDelta::FromMilliseconds(test.transport_rtt_msec));
    estimator.set_recent_transport_rtt(
        base::TimeDelta::FromMilliseconds(test.transport_rtt_msec));
    estimator.set_start_time_null_downlink_throughput_kbps(
        test.downlink_throughput_kbps);
    estimator.set_recent_downlink_throughput_kbps(
        test.downlink_throughput_kbps);
    // Run one main frame request to force recomputation of effective connection
    // type.
    estimator.RunOneRequest();
    EXPECT_EQ(test.expected_conn_type, estimator.GetEffectiveConnectionType());
  }
}

TEST(NetworkQualityEstimatorTest, TestGetMetricsSince) {
  std::map<std::string, std::string> variation_params;

  const base::TimeDelta rtt_threshold_3g =
      base::TimeDelta::FromMilliseconds(30);
  const base::TimeDelta rtt_threshold_4g = base::TimeDelta::FromMilliseconds(1);

  variation_params["3G.ThresholdMedianHttpRTTMsec"] =
      base::IntToString(rtt_threshold_3g.InMilliseconds());
  variation_params["HalfLifeSeconds"] = "300000";
  variation_params["add_default_platform_observations"] = "false";

  TestNetworkQualityEstimator estimator(variation_params);
  base::TimeTicks now = base::TimeTicks::Now();
  base::TimeTicks old = now - base::TimeDelta::FromMilliseconds(1);
  ASSERT_NE(old, now);

  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, "test");

  const int32_t old_downlink_kbps = 1;
  const base::TimeDelta old_url_rtt = base::TimeDelta::FromMilliseconds(1);
  const base::TimeDelta old_tcp_rtt = base::TimeDelta::FromMilliseconds(10);

  DCHECK_LT(old_url_rtt, rtt_threshold_3g);
  DCHECK_LT(old_tcp_rtt, rtt_threshold_3g);

  // First sample has very old timestamp.
  for (size_t i = 0; i < 2; ++i) {
    estimator.http_downstream_throughput_kbps_observations_.AddObservation(
        NetworkQualityEstimator::Observation(
            old_downlink_kbps, old, INT32_MIN,
            NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP));
    estimator.http_rtt_ms_observations_.AddObservation(
        NetworkQualityEstimator::Observation(
            old_url_rtt.InMilliseconds(), old, INT32_MIN,
            NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP));
    estimator.transport_rtt_ms_observations_.AddObservation(
        NetworkQualityEstimator::Observation(
            old_tcp_rtt.InMilliseconds(), old, INT32_MIN,
            NETWORK_QUALITY_OBSERVATION_SOURCE_TCP));
  }

  const int32_t new_downlink_kbps = 100;
  const base::TimeDelta new_url_rtt = base::TimeDelta::FromMilliseconds(100);
  const base::TimeDelta new_tcp_rtt = base::TimeDelta::FromMilliseconds(1000);

  DCHECK_NE(old_downlink_kbps, new_downlink_kbps);
  DCHECK_NE(old_url_rtt, new_url_rtt);
  DCHECK_NE(old_tcp_rtt, new_tcp_rtt);
  DCHECK_GT(new_url_rtt, rtt_threshold_3g);
  DCHECK_GT(new_tcp_rtt, rtt_threshold_3g);
  DCHECK_GT(new_url_rtt, rtt_threshold_4g);
  DCHECK_GT(new_tcp_rtt, rtt_threshold_4g);

  estimator.http_downstream_throughput_kbps_observations_.AddObservation(
      NetworkQualityEstimator::Observation(
          new_downlink_kbps, now, INT32_MIN,
          NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP));
  estimator.http_rtt_ms_observations_.AddObservation(
      NetworkQualityEstimator::Observation(
          new_url_rtt.InMilliseconds(), now, INT32_MIN,
          NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP));
  estimator.transport_rtt_ms_observations_.AddObservation(
      NetworkQualityEstimator::Observation(
          new_tcp_rtt.InMilliseconds(), now, INT32_MIN,
          NETWORK_QUALITY_OBSERVATION_SOURCE_TCP));

  const struct {
    base::TimeTicks start_timestamp;
    bool expect_network_quality_available;
    base::TimeDelta expected_http_rtt;
    base::TimeDelta expected_transport_rtt;
    int32_t expected_downstream_throughput;
    EffectiveConnectionType expected_effective_connection_type;
  } tests[] = {
      {now + base::TimeDelta::FromSeconds(10), false,
       base::TimeDelta::FromMilliseconds(0),
       base::TimeDelta::FromMilliseconds(0), 0, EFFECTIVE_CONNECTION_TYPE_4G},
      {now, true, new_url_rtt, new_tcp_rtt, new_downlink_kbps,
       EFFECTIVE_CONNECTION_TYPE_3G},
      {old - base::TimeDelta::FromMicroseconds(500), true, old_url_rtt,
       old_tcp_rtt, old_downlink_kbps, EFFECTIVE_CONNECTION_TYPE_4G},

  };
  for (const auto& test : tests) {
    base::TimeDelta http_rtt;
    base::TimeDelta transport_rtt;
    int32_t downstream_throughput_kbps;
    EXPECT_EQ(test.expect_network_quality_available,
              estimator.GetRecentHttpRTT(test.start_timestamp, &http_rtt));
    EXPECT_EQ(test.expect_network_quality_available,
              estimator.GetRecentTransportRTT(test.start_timestamp,
                                              &transport_rtt, nullptr));
    EXPECT_EQ(test.expect_network_quality_available,
              estimator.GetRecentDownlinkThroughputKbps(
                  test.start_timestamp, &downstream_throughput_kbps));

    if (test.expect_network_quality_available) {
      EXPECT_EQ(test.expected_http_rtt, http_rtt);
      EXPECT_EQ(test.expected_transport_rtt, transport_rtt);
      EXPECT_EQ(test.expected_downstream_throughput,
                downstream_throughput_kbps);
      EXPECT_EQ(
          test.expected_effective_connection_type,
          estimator.GetRecentEffectiveConnectionType(test.start_timestamp));
    }
  }
}

// An external estimate provider that does not have a valid RTT or throughput
// estimate.
class InvalidExternalEstimateProvider : public ExternalEstimateProvider {
 public:
  InvalidExternalEstimateProvider() : update_count_(0) {}
  ~InvalidExternalEstimateProvider() override = default;

  void SetUpdatedEstimateDelegate(UpdatedEstimateDelegate* delegate) override {}

  void Update() const override { update_count_++; }

  size_t update_count() const { return update_count_; }

 private:
  void ClearCachedEstimate() override {}

  mutable size_t update_count_;

  DISALLOW_COPY_AND_ASSIGN(InvalidExternalEstimateProvider);
};

// Tests if the RTT value from external estimate provider is discarded if the
// external estimate provider is invalid.
TEST(NetworkQualityEstimatorTest, InvalidExternalEstimateProvider) {
  base::HistogramTester histogram_tester;
  InvalidExternalEstimateProvider* invalid_external_estimate_provider =
      new InvalidExternalEstimateProvider();
  std::unique_ptr<ExternalEstimateProvider> external_estimate_provider(
      invalid_external_estimate_provider);

  std::map<std::string, std::string> variation_params;
  variation_params["add_default_platform_observations"] = "false";

  TestNetworkQualityEstimator estimator(variation_params,
                                        std::move(external_estimate_provider));
  estimator.SimulateNetworkChange(net::NetworkChangeNotifier::CONNECTION_WIFI,
                                  "test");

  base::TimeDelta rtt;
  int32_t kbps;
  EXPECT_EQ(1U, invalid_external_estimate_provider->update_count());
  EXPECT_FALSE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_FALSE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
  histogram_tester.ExpectTotalCount("NQE.ExternalEstimateProviderStatus", 2);

  histogram_tester.ExpectBucketCount(
      "NQE.ExternalEstimateProviderStatus",
      1 /* EXTERNAL_ESTIMATE_PROVIDER_STATUS_AVAILABLE */, 1);
  histogram_tester.ExpectBucketCount(
      "NQE.ExternalEstimateProviderStatus",
      2 /* EXTERNAL_ESTIMATE_PROVIDER_STATUS_QUERIED */, 1);
  histogram_tester.ExpectTotalCount("NQE.ExternalEstimateProvider.RTT", 0);
  histogram_tester.ExpectTotalCount(
      "NQE.ExternalEstimateProvider.DownlinkBandwidth", 0);
}

class TestExternalEstimateProvider : public ExternalEstimateProvider {
 public:
  TestExternalEstimateProvider(base::TimeDelta rtt,
                               int32_t downstream_throughput_kbps)
      : delegate_(nullptr),
        should_notify_delegate_(true),
        rtt_(rtt),
        downstream_throughput_kbps_(downstream_throughput_kbps),
        update_count_(0),
        cached_estimate_cleared_(false) {}
  ~TestExternalEstimateProvider() override = default;

  void SetUpdatedEstimateDelegate(UpdatedEstimateDelegate* delegate) override {
    delegate_ = delegate;
  }

  void set_should_notify_delegate(bool should_notify_delegate) {
    should_notify_delegate_ = should_notify_delegate;
  }

  void Update() const override {
    update_count_++;
    if (!should_notify_delegate_)
      return;
    delegate_->OnUpdatedEstimateAvailable(rtt_, downstream_throughput_kbps_);
  }

  size_t update_count() const { return update_count_; }

  bool cached_estimate_cleared() { return cached_estimate_cleared_; }

 private:
  void ClearCachedEstimate() override { cached_estimate_cleared_ = true; }

  UpdatedEstimateDelegate* delegate_;

  bool should_notify_delegate_;

  // RTT and downstream throughput estimates.
  const base::TimeDelta rtt_;
  const int32_t downstream_throughput_kbps_;

  mutable size_t update_count_;

  // True if the cached estimate has been cleared.
  bool cached_estimate_cleared_;

  DISALLOW_COPY_AND_ASSIGN(TestExternalEstimateProvider);
};

// Tests if the external estimate provider is called in the constructor and
// on network change notification.
TEST(NetworkQualityEstimatorTest, TestExternalEstimateProvider) {
  base::HistogramTester histogram_tester;
  const base::TimeDelta external_estimate_provider_rtt =
      base::TimeDelta::FromMilliseconds(1);
  const int32_t external_estimate_provider_downstream_throughput = 100;

  TestExternalEstimateProvider* test_external_estimate_provider =
      new TestExternalEstimateProvider(
          external_estimate_provider_rtt,
          external_estimate_provider_downstream_throughput);
  std::unique_ptr<ExternalEstimateProvider> external_estimate_provider(
      test_external_estimate_provider);
  std::map<std::string, std::string> variation_params;
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params,
                                        std::move(external_estimate_provider));
  EXPECT_FALSE(test_external_estimate_provider->cached_estimate_cleared());
  estimator.SimulateNetworkChange(net::NetworkChangeNotifier::CONNECTION_WIFI,
                                  "test");
  EXPECT_TRUE(test_external_estimate_provider->cached_estimate_cleared());

  base::TimeDelta rtt;
  int32_t kbps;
  EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_EQ(rtt, estimator.GetHttpRTT().value());
  EXPECT_FALSE(
      estimator.GetRecentTransportRTT(base::TimeTicks(), &rtt, nullptr));
  EXPECT_FALSE(estimator.GetTransportRTT());
  EXPECT_TRUE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
  EXPECT_EQ(kbps, estimator.GetDownstreamThroughputKbps().value());

  histogram_tester.ExpectTotalCount("NQE.ExternalEstimateProviderStatus", 5);

  histogram_tester.ExpectBucketCount(
      "NQE.ExternalEstimateProviderStatus",
      1 /* EXTERNAL_ESTIMATE_PROVIDER_STATUS_AVAILABLE */, 1);
  histogram_tester.ExpectBucketCount(
      "NQE.ExternalEstimateProviderStatus",
      2 /* EXTERNAL_ESTIMATE_PROVIDER_STATUS_QUERIED */, 1);
  histogram_tester.ExpectBucketCount(
      "NQE.ExternalEstimateProviderStatus",
      4 /* EXTERNAL_ESTIMATE_PROVIDER_STATUS_CALLBACK */, 1);
  histogram_tester.ExpectBucketCount(
      "NQE.ExternalEstimateProviderStatus",
      5 /* EXTERNAL_ESTIMATE_PROVIDER_STATUS_RTT_AVAILABLE */, 1);
  histogram_tester.ExpectBucketCount(
      "NQE.ExternalEstimateProviderStatus",
      6 /* EXTERNAL_ESTIMATE_PROVIDER_STATUS_DOWNLINK_BANDWIDTH_AVAILABLE */,
      1);
  histogram_tester.ExpectUniqueSample("NQE.ExternalEstimateProvider.RTT", 1, 1);
  histogram_tester.ExpectUniqueSample(
      "NQE.ExternalEstimateProvider.DownlinkBandwidth", 100, 1);
  histogram_tester.ExpectBucketCount(
      "NQE.RTT.ObservationSource",
      NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP_EXTERNAL_ESTIMATE, 1);
  histogram_tester.ExpectTotalCount(
      "NQE.RTT.RawObservation.HttpExternalEstimate", 1);
  histogram_tester.ExpectBucketCount(
      "NQE.Kbps.ObservationSource",
      NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP_EXTERNAL_ESTIMATE, 1);
  histogram_tester.ExpectTotalCount(
      "NQE.Kbps.RawObservation.HttpExternalEstimate", 1);

  EXPECT_EQ(1U, test_external_estimate_provider->update_count());

  // Change network type to WiFi. Number of queries to External estimate
  // provider must increment.
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, "test-1");
  EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_TRUE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
  EXPECT_EQ(2U, test_external_estimate_provider->update_count());

  test_external_estimate_provider->set_should_notify_delegate(false);
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_2G, "test-2");
  EXPECT_EQ(3U, test_external_estimate_provider->update_count());
  // Estimates are unavailable because external estimate provider never
  // notifies network quality estimator of the updated estimates.
  EXPECT_FALSE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_FALSE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
}

// Tests if the estimate from the external estimate provider is merged with the
// observations collected from the HTTP requests.
TEST(NetworkQualityEstimatorTest, TestExternalEstimateProviderMergeEstimates) {
  const base::TimeDelta external_estimate_provider_rtt =
      base::TimeDelta::FromMilliseconds(10 * 1000);
  const int32_t external_estimate_provider_downstream_throughput = 100 * 1000;
  TestExternalEstimateProvider* test_external_estimate_provider =
      new TestExternalEstimateProvider(
          external_estimate_provider_rtt,
          external_estimate_provider_downstream_throughput);
  std::unique_ptr<ExternalEstimateProvider> external_estimate_provider(
      test_external_estimate_provider);

  std::map<std::string, std::string> variation_params;
  variation_params["throughput_min_requests_in_flight"] = "1";
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params,
                                        std::move(external_estimate_provider));
  estimator.SimulateNetworkChange(net::NetworkChangeNotifier::CONNECTION_WIFI,
                                  "test");

  base::TimeDelta rtt;
  // Estimate provided by network quality estimator should match the estimate
  // provided by external estimate provider.
  EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_EQ(external_estimate_provider_rtt, rtt);

  int32_t kbps;
  EXPECT_TRUE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
  EXPECT_EQ(external_estimate_provider_downstream_throughput, kbps);

  TestDelegate test_delegate;
  TestURLRequestContext context(true);
  context.set_network_quality_estimator(&estimator);
  context.Init();

  for (size_t i = 0; i < 2; ++i) {
    // Start 2 requests to ensure that the RTT estimate computed by the network
    // quality estimator takes into account the RTT observations from the
    // external estimate provider as well as organic observations.
    std::unique_ptr<URLRequest> request(
        context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                              &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
    request->Start();
    base::RunLoop().Run();
  }

  EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_NE(external_estimate_provider_rtt, rtt);

  EXPECT_TRUE(
      estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
  EXPECT_NE(external_estimate_provider_downstream_throughput, kbps);
}

// Tests if the throughput observation is taken correctly when local and network
// requests do not overlap.
TEST(NetworkQualityEstimatorTest, TestThroughputNoRequestOverlap) {
  base::HistogramTester histogram_tester;
  std::map<std::string, std::string> variation_params;
  variation_params["throughput_min_requests_in_flight"] = "1";
  variation_params["add_default_platform_observations"] = "false";

  static const struct {
    bool allow_small_localhost_requests;
  } tests[] = {
      {
          false,
      },
      {
          true,
      },
  };

  for (const auto& test : tests) {
    TestNetworkQualityEstimator estimator(
        std::unique_ptr<net::ExternalEstimateProvider>(), variation_params,
        test.allow_small_localhost_requests,
        test.allow_small_localhost_requests,
        std::make_unique<BoundTestNetLog>());

    base::TimeDelta rtt;
    EXPECT_FALSE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
    int32_t kbps;
    EXPECT_FALSE(
        estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));

    TestDelegate test_delegate;
    TestURLRequestContext context(true);
    context.set_network_quality_estimator(&estimator);
    context.Init();

    std::unique_ptr<URLRequest> request(
        context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                              &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
    request->SetLoadFlags(request->load_flags() | LOAD_MAIN_FRAME_DEPRECATED);
    request->Start();
    base::RunLoop().Run();

    EXPECT_EQ(test.allow_small_localhost_requests,
              estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
    EXPECT_EQ(
        test.allow_small_localhost_requests,
        estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(), &kbps));
  }
}

#if defined(OS_IOS)
// Flaky on iOS: crbug.com/672917.
#define MAYBE_TestEffectiveConnectionTypeObserver \
  DISABLED_TestEffectiveConnectionTypeObserver
#else
#define MAYBE_TestEffectiveConnectionTypeObserver \
  TestEffectiveConnectionTypeObserver
#endif

// Tests that the effective connection type is computed at the specified
// interval, and that the observers are notified of any change.
TEST(NetworkQualityEstimatorTest, MAYBE_TestEffectiveConnectionTypeObserver) {
  base::HistogramTester histogram_tester;
  base::SimpleTestTickClock tick_clock;

  TestEffectiveConnectionTypeObserver observer;
  std::map<std::string, std::string> variation_params;
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params);
  estimator.AddEffectiveConnectionTypeObserver(&observer);
  // |observer| may be notified as soon as it is added. Run the loop to so that
  // the notification to |observer| is finished.
  base::RunLoop().RunUntilIdle();
  estimator.SetTickClockForTesting(&tick_clock);

  TestDelegate test_delegate;
  TestURLRequestContext context(true);
  context.set_network_quality_estimator(&estimator);
  context.Init();

  EXPECT_EQ(0U, observer.effective_connection_types().size());

  estimator.set_start_time_null_http_rtt(
      base::TimeDelta::FromMilliseconds(1500));
  estimator.set_start_time_null_downlink_throughput_kbps(100000);

  tick_clock.Advance(base::TimeDelta::FromMinutes(60));

  std::unique_ptr<URLRequest> request(
      context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                            &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
  request->SetLoadFlags(request->load_flags() | LOAD_MAIN_FRAME_DEPRECATED);
  request->Start();
  base::RunLoop().Run();
  EXPECT_EQ(1U, observer.effective_connection_types().size());
  EXPECT_LE(
      1, estimator.GetEntriesCount(NetLogEventType::NETWORK_QUALITY_CHANGED));

  // Verify the contents of the net log.
  EXPECT_EQ(GetNameForEffectiveConnectionType(EFFECTIVE_CONNECTION_TYPE_2G),
            estimator.GetNetLogLastStringValue(
                NetLogEventType::NETWORK_QUALITY_CHANGED,
                "effective_connection_type"));
  EXPECT_EQ(1500, estimator.GetNetLogLastIntegerValue(
                      NetLogEventType::NETWORK_QUALITY_CHANGED, "http_rtt_ms"));
  EXPECT_EQ(-1,
            estimator.GetNetLogLastIntegerValue(
                NetLogEventType::NETWORK_QUALITY_CHANGED, "transport_rtt_ms"));
  EXPECT_EQ(100000, estimator.GetNetLogLastIntegerValue(
                        NetLogEventType::NETWORK_QUALITY_CHANGED,
                        "downstream_throughput_kbps"));

  histogram_tester.ExpectUniqueSample("NQE.MainFrame.EffectiveConnectionType",
                                      EFFECTIVE_CONNECTION_TYPE_2G, 1);

  // Next request should not trigger recomputation of effective connection type
  // since there has been no change in the clock.
  std::unique_ptr<URLRequest> request2(
      context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                            &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
  request2->SetLoadFlags(request->load_flags() | LOAD_MAIN_FRAME_DEPRECATED);
  request2->Start();
  base::RunLoop().Run();
  EXPECT_EQ(1U, observer.effective_connection_types().size());

  // Change in connection type should send out notification to the observers.
  estimator.set_start_time_null_http_rtt(
      base::TimeDelta::FromMilliseconds(500));
  estimator.SimulateNetworkChange(NetworkChangeNotifier::CONNECTION_WIFI,
                                  "test");
  EXPECT_EQ(2U, observer.effective_connection_types().size());

  // A change in effective connection type does not trigger notification to the
  // observers, since it is not accompanied by any new observation or a network
  // change event.
  estimator.set_start_time_null_http_rtt(
      base::TimeDelta::FromMilliseconds(100));
  EXPECT_EQ(2U, observer.effective_connection_types().size());

  TestEffectiveConnectionTypeObserver observer_2;
  estimator.AddEffectiveConnectionTypeObserver(&observer_2);
  EXPECT_EQ(0U, observer_2.effective_connection_types().size());
  base::RunLoop().RunUntilIdle();
  // |observer_2| must be notified as soon as it is added.
  EXPECT_EQ(1U, observer_2.effective_connection_types().size());

  // |observer_3| should not be notified since it unregisters before the
  // message loop is run.
  TestEffectiveConnectionTypeObserver observer_3;
  estimator.AddEffectiveConnectionTypeObserver(&observer_3);
  EXPECT_EQ(0U, observer_3.effective_connection_types().size());
  estimator.RemoveEffectiveConnectionTypeObserver(&observer_3);
  base::RunLoop().RunUntilIdle();
  EXPECT_EQ(0U, observer_3.effective_connection_types().size());
}

// Tests that the transport RTT is used for computing the HTTP RTT.
TEST(NetworkQualityEstimatorTest, TestTransportRttUsedForHttpRttComputation) {
  const struct {
    base::TimeDelta http_rtt;
    base::TimeDelta transport_rtt;
    std::string lower_bound_http_rtt_transport_rtt_multiplier;
    std::string upper_bound_http_rtt_transport_rtt_multiplier;
    base::TimeDelta expected_http_rtt;
    EffectiveConnectionType expected_type;
  } tests[] = {
      {
          base::TimeDelta::FromMilliseconds(100),
          base::TimeDelta::FromMilliseconds(200), "", "",
          base::TimeDelta::FromMilliseconds(200), EFFECTIVE_CONNECTION_TYPE_4G,
      },
      {
          base::TimeDelta::FromMilliseconds(100),
          base::TimeDelta::FromMilliseconds(200), "0.01", "",
          base::TimeDelta::FromMilliseconds(100), EFFECTIVE_CONNECTION_TYPE_4G,
      },
      {
          base::TimeDelta::FromMilliseconds(100),
          base::TimeDelta::FromMilliseconds(200), "1.0", "",
          base::TimeDelta::FromMilliseconds(200), EFFECTIVE_CONNECTION_TYPE_4G,
      },
      {
          base::TimeDelta::FromMilliseconds(100),
          base::TimeDelta::FromMilliseconds(200), "1.5", "",
          base::TimeDelta::FromMilliseconds(300), EFFECTIVE_CONNECTION_TYPE_3G,
      },
      {
          base::TimeDelta::FromMilliseconds(100),
          base::TimeDelta::FromMilliseconds(200), "2.0", "",
          base::TimeDelta::FromMilliseconds(400), EFFECTIVE_CONNECTION_TYPE_3G,
      },
      {
          base::TimeDelta::FromMilliseconds(100),
          base::TimeDelta::FromMilliseconds(200), "2.0", "5.0",
          base::TimeDelta::FromMilliseconds(400), EFFECTIVE_CONNECTION_TYPE_3G,
      },
      {
          base::TimeDelta::FromMilliseconds(100),
          base::TimeDelta::FromMilliseconds(200), "10.0", "",
          base::TimeDelta::FromMilliseconds(2000), EFFECTIVE_CONNECTION_TYPE_2G,
      },
      {
          base::TimeDelta::FromMilliseconds(200),
          base::TimeDelta::FromMilliseconds(100), "10.0", "",
          base::TimeDelta::FromMilliseconds(1000), EFFECTIVE_CONNECTION_TYPE_3G,
      },
      {
          base::TimeDelta::FromMilliseconds(200),
          base::TimeDelta::FromMilliseconds(5), "10.0", "",
          base::TimeDelta::FromMilliseconds(200), EFFECTIVE_CONNECTION_TYPE_4G,
      },
      {
          base::TimeDelta::FromMilliseconds(200),
          base::TimeDelta::FromMilliseconds(5), "10.0", "20.0",
          base::TimeDelta::FromMilliseconds(100), EFFECTIVE_CONNECTION_TYPE_4G,
      },
      {
          base::TimeDelta::FromMilliseconds(100),
          base::TimeDelta::FromMilliseconds(200), "foobar", "",
          base::TimeDelta::FromMilliseconds(200), EFFECTIVE_CONNECTION_TYPE_4G,
      },
      {
          base::TimeDelta::FromMilliseconds(100),
          base::TimeDelta::FromMilliseconds(4000), "", "",
          base::TimeDelta::FromMilliseconds(4000),
          EFFECTIVE_CONNECTION_TYPE_SLOW_2G,
      },
      {
          base::TimeDelta::FromMilliseconds(100),
          base::TimeDelta::FromMilliseconds(4000), "1.0", "10.0",
          base::TimeDelta::FromMilliseconds(4000),
          EFFECTIVE_CONNECTION_TYPE_SLOW_2G,
      },
      {
          base::TimeDelta::FromMilliseconds(4000),
          base::TimeDelta::FromMilliseconds(100), "", "",
          base::TimeDelta::FromMilliseconds(4000),
          EFFECTIVE_CONNECTION_TYPE_SLOW_2G,
      },
      {
          base::TimeDelta::FromMilliseconds(4000),
          base::TimeDelta::FromMilliseconds(100), "1.0", "10.0",
          base::TimeDelta::FromMilliseconds(1000), EFFECTIVE_CONNECTION_TYPE_3G,
      },
      {
          base::TimeDelta::FromMilliseconds(1),
          base::TimeDelta::FromMilliseconds(100), "1.0", "10.0",
          base::TimeDelta::FromMilliseconds(100), EFFECTIVE_CONNECTION_TYPE_4G,
      },
      {
          base::TimeDelta::FromMilliseconds(500),
          base::TimeDelta::FromMilliseconds(100), "1.0", "10.0",
          base::TimeDelta::FromMilliseconds(500), EFFECTIVE_CONNECTION_TYPE_3G,
      },
  };

  for (const auto& test : tests) {
    std::map<std::string, std::string> variation_params;
    variation_params["lower_bound_http_rtt_transport_rtt_multiplier"] =
        test.lower_bound_http_rtt_transport_rtt_multiplier;
    variation_params["upper_bound_http_rtt_transport_rtt_multiplier"] =
        test.upper_bound_http_rtt_transport_rtt_multiplier;
    variation_params["add_default_platform_observations"] = "false";
    TestNetworkQualityEstimator estimator(variation_params);

    base::SimpleTestTickClock tick_clock;
    tick_clock.Advance(base::TimeDelta::FromSeconds(1));
    estimator.SetTickClockForTesting(&tick_clock);

    estimator.set_start_time_null_http_rtt(test.http_rtt);
    estimator.set_start_time_null_transport_rtt(test.transport_rtt);

    // Minimum number of transport RTT samples that should be present before
    // transport RTT estimate can be used to clamp the HTTP RTT.
    estimator.SetTransportRTTAtastECTSampleCount(
        estimator.params()->http_rtt_transport_rtt_min_count());

    // Add one observation to ensure ECT is not computed for each request.
    estimator.AddAndNotifyObserversOfRTT(NetworkQualityEstimator::Observation(
        test.http_rtt.InMilliseconds(), tick_clock.NowTicks(), INT32_MIN,
        NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP));

    estimator.RunOneRequest();
    EXPECT_EQ(test.expected_http_rtt, estimator.GetHttpRTT());
    EXPECT_EQ(test.transport_rtt, estimator.GetTransportRTT());
    EXPECT_EQ(test.expected_type, estimator.GetEffectiveConnectionType())
        << test.lower_bound_http_rtt_transport_rtt_multiplier;
  }
}

// Tests that the network quality is computed at the specified interval, and
// that the network quality observers are notified of any change.
TEST(NetworkQualityEstimatorTest, TestRTTAndThroughputEstimatesObserver) {
  base::HistogramTester histogram_tester;
  base::SimpleTestTickClock tick_clock;

  TestRTTAndThroughputEstimatesObserver observer;
  std::map<std::string, std::string> variation_params;
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params);
  estimator.AddRTTAndThroughputEstimatesObserver(&observer);
  estimator.SetTickClockForTesting(&tick_clock);

  TestDelegate test_delegate;
  TestURLRequestContext context(true);
  context.set_network_quality_estimator(&estimator);
  context.Init();

  EXPECT_EQ(nqe::internal::InvalidRTT(), observer.http_rtt());
  EXPECT_EQ(nqe::internal::InvalidRTT(), observer.transport_rtt());
  EXPECT_EQ(nqe::internal::INVALID_RTT_THROUGHPUT,
            observer.downstream_throughput_kbps());
  int notifications_received = observer.notifications_received();
  EXPECT_EQ(0, notifications_received);

  base::TimeDelta http_rtt(base::TimeDelta::FromMilliseconds(100));
  base::TimeDelta transport_rtt(base::TimeDelta::FromMilliseconds(200));
  int32_t downstream_throughput_kbps(300);
  estimator.set_start_time_null_http_rtt(http_rtt);
  estimator.set_start_time_null_transport_rtt(transport_rtt);
  estimator.set_start_time_null_downlink_throughput_kbps(
      downstream_throughput_kbps);
  tick_clock.Advance(base::TimeDelta::FromMinutes(60));

  std::unique_ptr<URLRequest> request(
      context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                            &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
  request->Start();
  base::RunLoop().Run();
  EXPECT_EQ(http_rtt, observer.http_rtt());
  EXPECT_EQ(transport_rtt, observer.transport_rtt());
  EXPECT_EQ(downstream_throughput_kbps, observer.downstream_throughput_kbps());
  EXPECT_LE(1, observer.notifications_received() - notifications_received);
  notifications_received = observer.notifications_received();

  // The next request should not trigger recomputation of RTT or throughput
  // since there has been no change in the clock.
  std::unique_ptr<URLRequest> request2(
      context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                            &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
  request2->Start();
  base::RunLoop().Run();
  EXPECT_LE(1, observer.notifications_received() - notifications_received);
  notifications_received = observer.notifications_received();

  // A change in the connection type should send out notification to the
  // observers.
  estimator.SimulateNetworkChange(NetworkChangeNotifier::CONNECTION_WIFI,
                                  "test");
  EXPECT_EQ(http_rtt, observer.http_rtt());
  EXPECT_EQ(transport_rtt, observer.transport_rtt());
  EXPECT_EQ(downstream_throughput_kbps, observer.downstream_throughput_kbps());
  EXPECT_LE(1, observer.notifications_received() - notifications_received);
  notifications_received = observer.notifications_received();

  // A change in effective connection type does not trigger notification to the
  // observers, since it is not accompanied by any new observation or a network
  // change event.
  estimator.set_start_time_null_http_rtt(
      base::TimeDelta::FromMilliseconds(10000));
  estimator.set_start_time_null_http_rtt(base::TimeDelta::FromMilliseconds(1));
  EXPECT_EQ(0, observer.notifications_received() - notifications_received);

  TestRTTAndThroughputEstimatesObserver observer_2;
  estimator.AddRTTAndThroughputEstimatesObserver(&observer_2);
  EXPECT_EQ(nqe::internal::InvalidRTT(), observer_2.http_rtt());
  EXPECT_EQ(nqe::internal::InvalidRTT(), observer_2.transport_rtt());
  EXPECT_EQ(nqe::internal::INVALID_RTT_THROUGHPUT,
            observer_2.downstream_throughput_kbps());
  base::RunLoop().RunUntilIdle();
  EXPECT_NE(nqe::internal::InvalidRTT(), observer_2.http_rtt());
  EXPECT_NE(nqe::internal::InvalidRTT(), observer_2.transport_rtt());
  EXPECT_NE(nqe::internal::INVALID_RTT_THROUGHPUT,
            observer_2.downstream_throughput_kbps());

  // |observer_3| should not be notified because it is unregisters before the
  // message loop is run.
  TestRTTAndThroughputEstimatesObserver observer_3;
  estimator.AddRTTAndThroughputEstimatesObserver(&observer_3);
  EXPECT_EQ(nqe::internal::InvalidRTT(), observer_3.http_rtt());
  EXPECT_EQ(nqe::internal::InvalidRTT(), observer_3.transport_rtt());
  EXPECT_EQ(nqe::internal::INVALID_RTT_THROUGHPUT,
            observer_3.downstream_throughput_kbps());
  estimator.RemoveRTTAndThroughputEstimatesObserver(&observer_3);
  base::RunLoop().RunUntilIdle();
  EXPECT_EQ(nqe::internal::InvalidRTT(), observer_3.http_rtt());
  EXPECT_EQ(nqe::internal::InvalidRTT(), observer_3.transport_rtt());
  EXPECT_EQ(nqe::internal::INVALID_RTT_THROUGHPUT,
            observer_3.downstream_throughput_kbps());
}

// Tests that the effective connection type is computed on every RTT
// observation if the last computed effective connection type was unknown.
TEST(NetworkQualityEstimatorTest, UnknownEffectiveConnectionType) {
  base::SimpleTestTickClock tick_clock;

  TestEffectiveConnectionTypeObserver observer;
  std::map<std::string, std::string> variation_params;
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params);
  estimator.SetTickClockForTesting(&tick_clock);
  estimator.AddEffectiveConnectionTypeObserver(&observer);
  tick_clock.Advance(base::TimeDelta::FromMinutes(60));

  size_t expected_effective_connection_type_notifications = 0;
  estimator.set_recent_effective_connection_type(
      EFFECTIVE_CONNECTION_TYPE_UNKNOWN);
  // Run one main frame request to force recomputation of effective connection
  // type.
  estimator.RunOneRequest();
  estimator.SimulateNetworkChange(NetworkChangeNotifier::CONNECTION_WIFI,
                                  "test");

  NetworkQualityEstimator::Observation rtt_observation(
      5000, tick_clock.NowTicks(), INT32_MIN,
      NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP);

  for (size_t i = 0; i < 10; ++i) {
    estimator.AddAndNotifyObserversOfRTT(rtt_observation);
    EXPECT_EQ(expected_effective_connection_type_notifications,
              observer.effective_connection_types().size());
  }
  estimator.set_recent_effective_connection_type(
      EFFECTIVE_CONNECTION_TYPE_SLOW_2G);
  // Even though there are 10 RTT samples already available, the addition of one
  // more RTT sample should trigger recomputation of the effective connection
  // type since the last computed effective connection type was unknown.
  estimator.AddAndNotifyObserversOfRTT(NetworkQualityEstimator::Observation(
      5000, tick_clock.NowTicks(), INT32_MIN,
      NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP));
  ++expected_effective_connection_type_notifications;
  EXPECT_EQ(expected_effective_connection_type_notifications,
            observer.effective_connection_types().size());
}

// Tests that the effective connection type is computed regularly depending
// on the number of RTT and bandwidth samples.
TEST(NetworkQualityEstimatorTest,
     AdaptiveRecomputationEffectiveConnectionType) {
  base::HistogramTester histogram_tester;
  base::SimpleTestTickClock tick_clock;

  TestEffectiveConnectionTypeObserver observer;
  std::map<std::string, std::string> variation_params;
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params);
  estimator.SetTickClockForTesting(&tick_clock);
  estimator.SimulateNetworkChange(NetworkChangeNotifier::CONNECTION_WIFI,
                                  "test");
  estimator.AddEffectiveConnectionTypeObserver(&observer);
  // |observer| may be notified as soon as it is added. Run the loop to so that
  // the notification to |observer| is finished.
  base::RunLoop().RunUntilIdle();

  TestDelegate test_delegate;
  TestURLRequestContext context(true);
  context.set_network_quality_estimator(&estimator);
  context.Init();

  EXPECT_EQ(0U, observer.effective_connection_types().size());

  estimator.set_recent_effective_connection_type(EFFECTIVE_CONNECTION_TYPE_2G);
  tick_clock.Advance(base::TimeDelta::FromMinutes(60));

  std::unique_ptr<URLRequest> request(
      context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                            &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
  request->SetLoadFlags(request->load_flags() | LOAD_MAIN_FRAME_DEPRECATED);
  request->Start();
  base::RunLoop().Run();
  EXPECT_EQ(1U, observer.effective_connection_types().size());
  histogram_tester.ExpectUniqueSample("NQE.MainFrame.EffectiveConnectionType",
                                      EFFECTIVE_CONNECTION_TYPE_2G, 1);
  histogram_tester.ExpectUniqueSample("NQE.EstimateAvailable.MainFrame.RTT", 0,
                                      1);
  histogram_tester.ExpectUniqueSample(
      "NQE.EstimateAvailable.MainFrame.TransportRTT", 0, 1);
  histogram_tester.ExpectUniqueSample("NQE.EstimateAvailable.MainFrame.Kbps", 0,
                                      1);
  EXPECT_LE(1u,
            histogram_tester
                .GetAllSamples("NQE.EffectiveConnectionType.OnECTComputation")
                .size());

  size_t expected_effective_connection_type_notifications = 1;
  EXPECT_EQ(expected_effective_connection_type_notifications,
            observer.effective_connection_types().size());

  EXPECT_EQ(expected_effective_connection_type_notifications,
            (estimator.http_rtt_ms_observations_.Size() +
             estimator.transport_rtt_ms_observations_.Size()));

  // Increase the number of RTT observations. Every time the number of RTT
  // observations is more than doubled, effective connection type must be
  // recomputed and notified to observers.
  for (size_t repetition = 0; repetition < 2; ++repetition) {
    // Change the effective connection type so that the observers are
    // notified when the effective connection type is recomputed.
    if (repetition % 2 == 0) {
      estimator.set_recent_effective_connection_type(
          EFFECTIVE_CONNECTION_TYPE_SLOW_2G);
    } else {
      estimator.set_recent_effective_connection_type(
          EFFECTIVE_CONNECTION_TYPE_3G);
    }
    size_t rtt_observations_count =
        (estimator.http_rtt_ms_observations_.Size() +
         estimator.transport_rtt_ms_observations_.Size()) *
        0.5;
    // Increase the number of RTT observations to more than twice the number
    // of current observations. This should trigger recomputation of
    // effective connection type.
    for (size_t i = 0; i < rtt_observations_count + 1; ++i) {
      estimator.AddAndNotifyObserversOfRTT(NetworkQualityEstimator::Observation(
          5000, tick_clock.NowTicks(), INT32_MIN,
          NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP));

      if (i == rtt_observations_count) {
        // Effective connection type must be recomputed since the number of RTT
        // samples are now more than twice the number of RTT samples that were
        // available when effective connection type was last computed.
        ++expected_effective_connection_type_notifications;
      }
      EXPECT_EQ(expected_effective_connection_type_notifications,
                observer.effective_connection_types().size());
    }
  }
}

TEST(NetworkQualityEstimatorTest, TestRttThroughputObservers) {
  TestRTTObserver rtt_observer;
  TestThroughputObserver throughput_observer;

  std::map<std::string, std::string> variation_params;
  variation_params["throughput_min_requests_in_flight"] = "1";
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params);

  estimator.AddRTTObserver(&rtt_observer);
  estimator.AddThroughputObserver(&throughput_observer);

  TestDelegate test_delegate;
  TestURLRequestContext context(true);
  context.set_network_quality_estimator(&estimator);
  context.Init();

  EXPECT_EQ(0U, rtt_observer.observations().size());
  EXPECT_EQ(0U, throughput_observer.observations().size());
  base::TimeTicks then = base::TimeTicks::Now();

  std::unique_ptr<URLRequest> request(
      context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                            &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
  request->SetLoadFlags(request->load_flags() | LOAD_MAIN_FRAME_DEPRECATED);
  request->Start();
  base::RunLoop().Run();

  std::unique_ptr<URLRequest> request2(
      context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                            &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
  request2->SetLoadFlags(request->load_flags() | LOAD_MAIN_FRAME_DEPRECATED);
  request2->Start();
  base::RunLoop().Run();

  // Both RTT and downstream throughput should be updated.
  base::TimeDelta rtt;
  EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));

  int32_t throughput;
  EXPECT_TRUE(estimator.GetRecentDownlinkThroughputKbps(base::TimeTicks(),
                                                        &throughput));

  EXPECT_EQ(2U, rtt_observer.observations().size());
  EXPECT_EQ(2U, throughput_observer.observations().size());
  for (const auto& observation : rtt_observer.observations()) {
    EXPECT_LE(0, observation.rtt_ms);
    EXPECT_LE(0, (observation.timestamp - then).InMilliseconds());
    EXPECT_EQ(NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP, observation.source);
  }
  for (const auto& observation : throughput_observer.observations()) {
    EXPECT_LE(0, observation.throughput_kbps);
    EXPECT_LE(0, (observation.timestamp - then).InMilliseconds());
    EXPECT_EQ(NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP, observation.source);
  }

  EXPECT_FALSE(
      estimator.GetRecentTransportRTT(base::TimeTicks(), &rtt, nullptr));

  // Verify that observations from TCP and QUIC are passed on to the observers.
  base::TimeDelta tcp_rtt(base::TimeDelta::FromMilliseconds(1));
  base::TimeDelta quic_rtt(base::TimeDelta::FromMilliseconds(2));

  // Use a public IP address so that the socket watcher runs the RTT callback.
  IPAddressList ip_list;
  IPAddress ip_address;
  ASSERT_TRUE(ip_address.AssignFromIPLiteral("157.0.0.1"));
  ip_list.push_back(ip_address);
  AddressList address_list =
      AddressList::CreateFromIPAddressList(ip_list, "canonical.example.com");

  std::unique_ptr<SocketPerformanceWatcher> tcp_watcher =
      estimator.GetSocketPerformanceWatcherFactory()
          ->CreateSocketPerformanceWatcher(
              SocketPerformanceWatcherFactory::PROTOCOL_TCP, address_list);

  std::unique_ptr<SocketPerformanceWatcher> quic_watcher =
      estimator.GetSocketPerformanceWatcherFactory()
          ->CreateSocketPerformanceWatcher(
              SocketPerformanceWatcherFactory::PROTOCOL_QUIC, address_list);

  tcp_watcher->OnUpdatedRTTAvailable(tcp_rtt);
  // First RTT sample from QUIC connections is dropped, but the second RTT
  // notification should not be dropped.
  quic_watcher->OnUpdatedRTTAvailable(quic_rtt);
  quic_watcher->OnUpdatedRTTAvailable(quic_rtt);

  base::RunLoop().RunUntilIdle();

  EXPECT_EQ(4U, rtt_observer.observations().size());
  EXPECT_EQ(2U, throughput_observer.observations().size());

  EXPECT_EQ(tcp_rtt.InMilliseconds(), rtt_observer.observations().at(2).rtt_ms);
  EXPECT_EQ(quic_rtt.InMilliseconds(),
            rtt_observer.observations().at(3).rtt_ms);

  EXPECT_TRUE(
      estimator.GetRecentTransportRTT(base::TimeTicks(), &rtt, nullptr));
}

TEST(NetworkQualityEstimatorTest, TestGlobalSocketWatcherThrottle) {
  base::SimpleTestTickClock tick_clock;
  tick_clock.Advance(base::TimeDelta::FromSeconds(1));

  std::map<std::string, std::string> variation_params;
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params);
  estimator.SetTickClockForTesting(&tick_clock);

  TestRTTObserver rtt_observer;
  estimator.AddRTTObserver(&rtt_observer);

  const base::TimeDelta tcp_rtt(base::TimeDelta::FromMilliseconds(1));

  TestDelegate test_delegate;
  TestURLRequestContext context(true);
  context.set_network_quality_estimator(&estimator);
  context.Init();

  // Use a public IP address so that the socket watcher runs the RTT callback.
  IPAddressList ip_list;
  IPAddress ip_address;
  ASSERT_TRUE(ip_address.AssignFromIPLiteral("157.0.0.1"));
  ip_list.push_back(ip_address);
  AddressList address_list =
      AddressList::CreateFromIPAddressList(ip_list, "canonical.example.com");
  std::unique_ptr<SocketPerformanceWatcher> tcp_watcher =
      estimator.GetSocketPerformanceWatcherFactory()
          ->CreateSocketPerformanceWatcher(
              SocketPerformanceWatcherFactory::PROTOCOL_TCP, address_list);

  EXPECT_EQ(0U, rtt_observer.observations().size());
  EXPECT_TRUE(tcp_watcher->ShouldNotifyUpdatedRTT());
  std::unique_ptr<URLRequest> request(
      context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                            &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
  request->SetLoadFlags(request->load_flags() | LOAD_MAIN_FRAME_DEPRECATED);
  request->Start();
  base::RunLoop().Run();
  EXPECT_EQ(1U, rtt_observer.observations().size());
  EXPECT_TRUE(tcp_watcher->ShouldNotifyUpdatedRTT());

  tcp_watcher->OnUpdatedRTTAvailable(tcp_rtt);
  base::RunLoop().RunUntilIdle();
  EXPECT_FALSE(tcp_watcher->ShouldNotifyUpdatedRTT());
  EXPECT_EQ(2U, rtt_observer.observations().size());
  // Advancing the clock should make it possible to notify new RTT
  // notifications.
  tick_clock.Advance(
      estimator.params()->socket_watchers_min_notification_interval());
  EXPECT_TRUE(tcp_watcher->ShouldNotifyUpdatedRTT());

  EXPECT_EQ(tcp_rtt.InMilliseconds(), rtt_observer.observations().at(1).rtt_ms);
  base::TimeDelta rtt;
  EXPECT_TRUE(
      estimator.GetRecentTransportRTT(base::TimeTicks(), &rtt, nullptr));
}

// TestTCPSocketRTT requires kernel support for tcp_info struct, and so it is
// enabled only on certain platforms.
#if defined(TCP_INFO) || defined(OS_LINUX) || defined(OS_ANDROID)
#define MAYBE_TestTCPSocketRTT TestTCPSocketRTT
#else
#define MAYBE_TestTCPSocketRTT DISABLED_TestTCPSocketRTT
#endif
// Tests that the TCP socket notifies the Network Quality Estimator of TCP RTTs,
// which in turn notifies registered RTT observers.
TEST(NetworkQualityEstimatorTest, MAYBE_TestTCPSocketRTT) {
  base::SimpleTestTickClock tick_clock;
  tick_clock.Advance(base::TimeDelta::FromSeconds(1));

  base::HistogramTester histogram_tester;
  TestRTTObserver rtt_observer;

  std::map<std::string, std::string> variation_params;
  variation_params["persistent_cache_reading_enabled"] = "true";
  variation_params["throughput_min_requests_in_flight"] = "1";
  TestNetworkQualityEstimator estimator(
      nullptr, variation_params, true, true,
      std::make_unique<BoundTestNetLog>());
  estimator.SetTickClockForTesting(&tick_clock);
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_2G, "test");

  estimator.AddRTTObserver(&rtt_observer);
  // |observer| may be notified as soon as it is added. Run the loop to so that
  // the notification to |observer| is finished.
  base::RunLoop().RunUntilIdle();

  TestDelegate test_delegate;
  TestURLRequestContext context(true);
  context.set_network_quality_estimator(&estimator);

  std::unique_ptr<HttpNetworkSession::Context> session_context(
      new HttpNetworkSession::Context);
  // |estimator| should be notified of TCP RTT observations.
  session_context->socket_performance_watcher_factory =
      estimator.GetSocketPerformanceWatcherFactory();
  context.set_http_network_session_context(std::move(session_context));
  context.Init();

  EXPECT_EQ(0U, rtt_observer.observations().size());
  base::TimeDelta rtt;
  EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_EQ(rtt, estimator.GetHttpRTT().value());
  EXPECT_TRUE(
      estimator.GetRecentTransportRTT(base::TimeTicks(), &rtt, nullptr));
  EXPECT_EQ(rtt, estimator.GetTransportRTT().value());

  // Send two requests. Verify that the completion of each request generates at
  // least one TCP RTT observation.
  const size_t num_requests = 2;
  for (size_t i = 0; i < num_requests; ++i) {
    size_t before_count_tcp_rtt_observations = 0;
    for (const auto& observation : rtt_observer.observations()) {
      if (observation.source == NETWORK_QUALITY_OBSERVATION_SOURCE_TCP)
        ++before_count_tcp_rtt_observations;
    }

    std::unique_ptr<URLRequest> request(
        context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                              &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
    request->SetLoadFlags(request->load_flags() | LOAD_MAIN_FRAME_DEPRECATED);
    request->Start();
    tick_clock.Advance(
        estimator.params()->socket_watchers_min_notification_interval());

    base::RunLoop().Run();

    size_t after_count_tcp_rtt_observations = 0;
    for (const auto& observation : rtt_observer.observations()) {
      if (observation.source == NETWORK_QUALITY_OBSERVATION_SOURCE_TCP)
        ++after_count_tcp_rtt_observations;
    }
    // At least one notification should be received per socket performance
    // watcher.
    EXPECT_LE(1U, after_count_tcp_rtt_observations -
                      before_count_tcp_rtt_observations)
        << i;
  }
  EXPECT_TRUE(estimator.GetRecentHttpRTT(base::TimeTicks(), &rtt));
  EXPECT_NE(nqe::internal::InvalidRTT(), estimator.GetHttpRTT().value());
  EXPECT_TRUE(
      estimator.GetRecentTransportRTT(base::TimeTicks(), &rtt, nullptr));
  EXPECT_EQ(rtt, estimator.GetTransportRTT().value());

  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, "test-1");

  // Verify that metrics are logged correctly on main-frame requests.
  histogram_tester.ExpectTotalCount("NQE.MainFrame.TransportRTT.Percentile50",
                                    num_requests);
  histogram_tester.ExpectUniqueSample("NQE.EstimateAvailable.MainFrame.RTT", 1,
                                      num_requests);
  histogram_tester.ExpectUniqueSample(
      "NQE.EstimateAvailable.MainFrame.TransportRTT", 1, num_requests);
  histogram_tester.ExpectUniqueSample("NQE.EstimateAvailable.MainFrame.Kbps", 1,
                                      num_requests);

  histogram_tester.ExpectTotalCount("NQE.MainFrame.EffectiveConnectionType",
                                    num_requests);
  histogram_tester.ExpectBucketCount("NQE.MainFrame.EffectiveConnectionType",
                                     EFFECTIVE_CONNECTION_TYPE_UNKNOWN, 0);
  ExpectBucketCountAtLeast(&histogram_tester, "NQE.RTT.ObservationSource",
                           NETWORK_QUALITY_OBSERVATION_SOURCE_TCP, 1);
  ExpectBucketCountAtLeast(&histogram_tester, "NQE.Kbps.ObservationSource",
                           NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP, 1);
  EXPECT_LE(1u,
            histogram_tester
                .GetAllSamples("NQE.EffectiveConnectionType.OnECTComputation")
                .size());
  EXPECT_LE(1u,
            histogram_tester.GetAllSamples("NQE.TransportRTT.OnECTComputation")
                .size());
  EXPECT_LE(1u,
            histogram_tester.GetAllSamples("NQE.RTT.OnECTComputation").size());

  histogram_tester.ExpectBucketCount(
      "NQE.Kbps.ObservationSource",
      NETWORK_QUALITY_OBSERVATION_SOURCE_TRANSPORT_CACHED_ESTIMATE, 0);

  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_2G, "test");
  histogram_tester.ExpectBucketCount(
      "NQE.RTT.ObservationSource",
      NETWORK_QUALITY_OBSERVATION_SOURCE_TRANSPORT_CACHED_ESTIMATE, 1);

  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, "test-1");
  histogram_tester.ExpectBucketCount(
      "NQE.RTT.ObservationSource",
      NETWORK_QUALITY_OBSERVATION_SOURCE_TRANSPORT_CACHED_ESTIMATE, 2);
}

#if defined(OS_IOS)
// Flaky on iOS when |accuracy_recording_delay| is non-zero.
#define MAYBE_RecordAccuracy DISABLED_RecordAccuracy
#else
#define MAYBE_RecordAccuracy RecordAccuracy
#endif
// Tests if the NQE accuracy metrics are recorded properly.
TEST(NetworkQualityEstimatorTest, MAYBE_RecordAccuracy) {
  const int expected_rtt_msec = 500;
  const int expected_downstream_throughput_kbps = 2000;

  const base::TimeDelta accuracy_recording_delays[] = {
      base::TimeDelta::FromSeconds(0), base::TimeDelta::FromSeconds(1),
  };

  const struct {
    base::TimeDelta rtt;
    base::TimeDelta recent_rtt;
    int32_t downstream_throughput_kbps;
    int32_t recent_downstream_throughput_kbps;
    EffectiveConnectionType effective_connection_type;
    EffectiveConnectionType recent_effective_connection_type;
  } tests[] = {
      {base::TimeDelta::FromMilliseconds(expected_rtt_msec),
       base::TimeDelta::FromMilliseconds(expected_rtt_msec),
       expected_downstream_throughput_kbps, expected_downstream_throughput_kbps,
       EFFECTIVE_CONNECTION_TYPE_3G, EFFECTIVE_CONNECTION_TYPE_3G},
      {
          base::TimeDelta::FromMilliseconds(expected_rtt_msec + 1000),
          base::TimeDelta::FromMilliseconds(expected_rtt_msec),
          expected_downstream_throughput_kbps - 1,
          expected_downstream_throughput_kbps, EFFECTIVE_CONNECTION_TYPE_2G,
          EFFECTIVE_CONNECTION_TYPE_3G,
      },
      {
          base::TimeDelta::FromMilliseconds(expected_rtt_msec - 400),
          base::TimeDelta::FromMilliseconds(expected_rtt_msec),
          expected_downstream_throughput_kbps + 1,
          expected_downstream_throughput_kbps, EFFECTIVE_CONNECTION_TYPE_4G,
          EFFECTIVE_CONNECTION_TYPE_3G,
      },
  };

  for (const auto& accuracy_recording_delay : accuracy_recording_delays) {
    for (const auto& test : tests) {
      base::SimpleTestTickClock tick_clock;
      tick_clock.Advance(base::TimeDelta::FromSeconds(1));

      std::unique_ptr<ExternalEstimateProvider> external_estimate_provider(
          new TestExternalEstimateProvider(test.rtt, 0));

      std::map<std::string, std::string> variation_params;
      TestNetworkQualityEstimator estimator(
          variation_params, std::move(external_estimate_provider));

      estimator.SetTickClockForTesting(&tick_clock);
      estimator.SimulateNetworkChange(
          NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, "test-1");
      tick_clock.Advance(base::TimeDelta::FromSeconds(1));

      std::vector<base::TimeDelta> accuracy_recording_intervals;
      accuracy_recording_intervals.push_back(accuracy_recording_delay);
      estimator.SetAccuracyRecordingIntervals(accuracy_recording_intervals);

      // RTT is higher than threshold. Network is slow.
      // Network was predicted to be slow and actually was slow.
      estimator.set_start_time_null_http_rtt(test.rtt);
      estimator.set_recent_http_rtt(test.recent_rtt);
      estimator.set_rtt_estimate_internal(test.recent_rtt);
      estimator.set_start_time_null_transport_rtt(test.rtt);
      estimator.set_recent_transport_rtt(test.recent_rtt);
      estimator.set_start_time_null_downlink_throughput_kbps(
          test.downstream_throughput_kbps);
      estimator.set_recent_downlink_throughput_kbps(
          test.recent_downstream_throughput_kbps);

      base::HistogramTester histogram_tester;

      TestDelegate test_delegate;
      TestURLRequestContext context(true);
      context.set_network_quality_estimator(&estimator);
      context.Init();

      // Start a main-frame request which should cause network quality estimator
      // to record accuracy UMA.
      std::unique_ptr<URLRequest> request(
          context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                                &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
      request->SetLoadFlags(request->load_flags() | LOAD_MAIN_FRAME_DEPRECATED);
      request->Start();
      base::RunLoop().Run();

      if (accuracy_recording_delay != base::TimeDelta()) {
        tick_clock.Advance(accuracy_recording_delay);

        // Sleep for some time to ensure that the delayed task is posted.
        base::PlatformThread::Sleep(accuracy_recording_delay * 2);
        base::RunLoop().RunUntilIdle();
      }

      const int rtt_diff = std::abs(test.rtt.InMilliseconds() -
                                    test.recent_rtt.InMilliseconds());
      const int kbps_diff = std::abs(test.downstream_throughput_kbps -
                                     test.recent_downstream_throughput_kbps);
      const int ect_diff = std::abs(test.effective_connection_type -
                                    test.recent_effective_connection_type);

      const std::string rtt_sign_suffix_with_zero_samples =
          test.rtt.InMilliseconds() - test.recent_rtt.InMilliseconds() >= 0
              ? "Negative"
              : "Positive";
      const std::string kbps_sign_suffix_with_zero_samples =
          test.downstream_throughput_kbps -
                      test.recent_downstream_throughput_kbps >=
                  0
              ? "Negative"
              : "Positive";

      const std::string rtt_sign_suffix_with_one_sample =
          rtt_sign_suffix_with_zero_samples == "Positive" ? "Negative"
                                                          : "Positive";
      const std::string ect_sign_suffix_with_zero_samples =
          test.rtt.InMilliseconds() - test.recent_rtt.InMilliseconds() > 0
              ? "Positive"
              : "Negative";

      const std::string kbps_sign_suffix_with_one_sample =
          kbps_sign_suffix_with_zero_samples == "Positive" ? "Negative"
                                                           : "Positive";
      const std::string ect_sign_suffix_with_one_sample =
          ect_sign_suffix_with_zero_samples == "Positive" ? "Negative"
                                                          : "Positive";
      const std::string interval_value =
          base::IntToString(accuracy_recording_delay.InSeconds());

      histogram_tester.ExpectUniqueSample(
          "NQE.Accuracy.DownstreamThroughputKbps.EstimatedObservedDiff." +
              kbps_sign_suffix_with_one_sample + "." + interval_value +
              ".1260_2540",
          kbps_diff, 1);
      histogram_tester.ExpectTotalCount(
          "NQE.Accuracy.DownstreamThroughputKbps.EstimatedObservedDiff." +
              kbps_sign_suffix_with_zero_samples + "." + interval_value +
              ".1260_2540",
          0);

      histogram_tester.ExpectUniqueSample(
          "NQE.Accuracy.EffectiveConnectionType.EstimatedObservedDiff." +
              ect_sign_suffix_with_one_sample + "." + interval_value + ".3G",
          ect_diff, 1);
      histogram_tester.ExpectTotalCount(
          "NQE.Accuracy.EffectiveConnectionType.EstimatedObservedDiff." +
              ect_sign_suffix_with_zero_samples + "." + interval_value + ".3G",
          0);

      histogram_tester.ExpectUniqueSample(
          "NQE.Accuracy.HttpRTT.EstimatedObservedDiff." +
              rtt_sign_suffix_with_one_sample + "." + interval_value +
              ".300_620",
          rtt_diff, 1);
      histogram_tester.ExpectTotalCount(
          "NQE.Accuracy.HttpRTT.EstimatedObservedDiff." +
              rtt_sign_suffix_with_zero_samples + "." + interval_value +
              ".300_620",
          0);

      histogram_tester.ExpectUniqueSample(
          "NQE.Accuracy.TransportRTT.EstimatedObservedDiff." +
              rtt_sign_suffix_with_one_sample + "." + interval_value +
              ".300_620",
          rtt_diff, 1);
      histogram_tester.ExpectTotalCount(
          "NQE.Accuracy.TransportRTT.EstimatedObservedDiff." +
              rtt_sign_suffix_with_zero_samples + "." + interval_value +
              ".300_620",
          0);

      histogram_tester.ExpectUniqueSample(
          "NQE.ExternalEstimateProvider.RTT.Accuracy.EstimatedObservedDiff." +
              rtt_sign_suffix_with_one_sample + "." + interval_value +
              ".300_620",
          rtt_diff, 1);
      histogram_tester.ExpectTotalCount(
          "NQE.ExternalEstimateProvider.RTT.Accuracy.EstimatedObservedDiff." +
              rtt_sign_suffix_with_zero_samples + "." + interval_value +
              ".300_620",
          0);
    }
  }
}

TEST(NetworkQualityEstimatorTest, TestRecordNetworkIDAvailability) {
  base::HistogramTester histogram_tester;
  TestNetworkQualityEstimator estimator;

  // The NetworkID is recorded as available on Wi-Fi connection.
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, "test-1");
  histogram_tester.ExpectUniqueSample("NQE.NetworkIdAvailable", 1, 1);

  // The histogram is not recorded on an unknown connection.
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_UNKNOWN, "");
  histogram_tester.ExpectTotalCount("NQE.NetworkIdAvailable", 1);

  // The NetworkID is recorded as not being available on a Wi-Fi connection
  // with an empty SSID.
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, "");
  histogram_tester.ExpectBucketCount("NQE.NetworkIdAvailable", 0, 1);
  histogram_tester.ExpectTotalCount("NQE.NetworkIdAvailable", 2);

  // The NetworkID is recorded as being available on a Wi-Fi connection.
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, "test-1");
  histogram_tester.ExpectBucketCount("NQE.NetworkIdAvailable", 1, 2);
  histogram_tester.ExpectTotalCount("NQE.NetworkIdAvailable", 3);

  // The NetworkID is recorded as being available on a cellular connection.
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_2G, "test-1");
  histogram_tester.ExpectBucketCount("NQE.NetworkIdAvailable", 1, 3);
  histogram_tester.ExpectTotalCount("NQE.NetworkIdAvailable", 4);
}

class TestNetworkQualitiesCacheObserver
    : public nqe::internal::NetworkQualityStore::NetworkQualitiesCacheObserver {
 public:
  TestNetworkQualitiesCacheObserver()
      : network_id_(net::NetworkChangeNotifier::CONNECTION_UNKNOWN,
                    std::string(),
                    INT32_MIN),
        notification_received_(0) {}
  ~TestNetworkQualitiesCacheObserver() override = default;

  void OnChangeInCachedNetworkQuality(
      const nqe::internal::NetworkID& network_id,
      const nqe::internal::CachedNetworkQuality& cached_network_quality)
      override {
    network_id_ = network_id;
    notification_received_++;
  }

  size_t get_notification_received_and_reset() {
    size_t notification_received = notification_received_;
    notification_received_ = 0;
    return notification_received;
  }

  nqe::internal::NetworkID network_id() const { return network_id_; }

 private:
  nqe::internal::NetworkID network_id_;
  size_t notification_received_;
  DISALLOW_COPY_AND_ASSIGN(TestNetworkQualitiesCacheObserver);
};

TEST(NetworkQualityEstimatorTest, CacheObserver) {
  TestNetworkQualitiesCacheObserver observer;
  TestNetworkQualityEstimator estimator;

  // Add |observer| as a persistent caching observer.
  estimator.AddNetworkQualitiesCacheObserver(&observer);

  estimator.set_recent_effective_connection_type(EFFECTIVE_CONNECTION_TYPE_3G);
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_UNKNOWN, "test3g");
  estimator.RunOneRequest();
  EXPECT_EQ(4u, observer.get_notification_received_and_reset());
  EXPECT_EQ("test3g", observer.network_id().id);

  estimator.set_recent_effective_connection_type(EFFECTIVE_CONNECTION_TYPE_2G);
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_2G, "test2g");
  // One notification should be received for the previous network
  // ("test3g") right before the connection change event. The second
  // notification should be received for the second network ("test2g").
  base::RunLoop().RunUntilIdle();
  EXPECT_EQ(2u, observer.get_notification_received_and_reset());
  estimator.RunOneRequest();
  EXPECT_EQ("test2g", observer.network_id().id);

  estimator.set_recent_effective_connection_type(EFFECTIVE_CONNECTION_TYPE_4G);
  // Start multiple requests, but there should be only one notification
  // received, since the effective connection type does not change.
  estimator.RunOneRequest();
  estimator.RunOneRequest();
  estimator.RunOneRequest();
  EXPECT_EQ(1u, observer.get_notification_received_and_reset());

  estimator.set_recent_effective_connection_type(EFFECTIVE_CONNECTION_TYPE_2G);
  estimator.RunOneRequest();
  EXPECT_EQ(1u, observer.get_notification_received_and_reset());

  // Remove |observer|, and it should not receive any notifications.
  estimator.RemoveNetworkQualitiesCacheObserver(&observer);
  estimator.set_recent_effective_connection_type(EFFECTIVE_CONNECTION_TYPE_3G);
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_2G, "test2g");
  EXPECT_EQ(0u, observer.get_notification_received_and_reset());
  estimator.RunOneRequest();
  EXPECT_EQ(0u, observer.get_notification_received_and_reset());
}

// Tests that the value of the effective connection type can be forced through
// field trial parameters.
TEST(NetworkQualityEstimatorTest,
     ForceEffectiveConnectionTypeThroughFieldTrial) {
  for (int i = 0; i < EFFECTIVE_CONNECTION_TYPE_LAST; ++i) {
    EffectiveConnectionType ect_type = static_cast<EffectiveConnectionType>(i);
    std::map<std::string, std::string> variation_params;
    variation_params[kForceEffectiveConnectionType] =
        GetNameForEffectiveConnectionType(
            static_cast<EffectiveConnectionType>(i));
    TestNetworkQualityEstimator estimator(variation_params);

    TestEffectiveConnectionTypeObserver ect_observer;
    estimator.AddEffectiveConnectionTypeObserver(&ect_observer);
    TestRTTAndThroughputEstimatesObserver rtt_throughput_observer;
    estimator.AddRTTAndThroughputEstimatesObserver(&rtt_throughput_observer);
    // |observer| may be notified as soon as it is added. Run the loop to so
    // that the notification to |observer| is finished.
    base::RunLoop().RunUntilIdle();

    TestDelegate test_delegate;
    TestURLRequestContext context(true);
    context.set_network_quality_estimator(&estimator);
    context.Init();

    if (ect_type == EFFECTIVE_CONNECTION_TYPE_UNKNOWN) {
      EXPECT_EQ(0U, ect_observer.effective_connection_types().size());
    } else {
      EXPECT_EQ(1U, ect_observer.effective_connection_types().size());
    }

    std::unique_ptr<URLRequest> request(
        context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                              &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
    request->SetLoadFlags(request->load_flags() | LOAD_MAIN_FRAME_DEPRECATED);
    request->Start();
    base::RunLoop().Run();

    EXPECT_EQ(i, estimator.GetEffectiveConnectionType());

    size_t expected_count =
        ect_type == EFFECTIVE_CONNECTION_TYPE_UNKNOWN ? 0 : 1;
    ASSERT_EQ(expected_count, ect_observer.effective_connection_types().size());
    if (expected_count == 1) {
      EffectiveConnectionType last_notified_type =
          ect_observer.effective_connection_types().at(
              ect_observer.effective_connection_types().size() - 1);
      EXPECT_EQ(i, last_notified_type);

      if (ect_type == EFFECTIVE_CONNECTION_TYPE_UNKNOWN ||
          ect_type == EFFECTIVE_CONNECTION_TYPE_OFFLINE) {
        EXPECT_EQ(nqe::internal::InvalidRTT(),
                  rtt_throughput_observer.http_rtt());
        EXPECT_EQ(nqe::internal::InvalidRTT(),
                  rtt_throughput_observer.transport_rtt());
        EXPECT_EQ(nqe::internal::INVALID_RTT_THROUGHPUT,
                  rtt_throughput_observer.downstream_throughput_kbps());
      } else {
        EXPECT_EQ(estimator.params_->TypicalNetworkQuality(ect_type).http_rtt(),
                  rtt_throughput_observer.http_rtt());
        EXPECT_EQ(
            estimator.params_->TypicalNetworkQuality(ect_type).transport_rtt(),
            rtt_throughput_observer.transport_rtt());
        EXPECT_EQ(estimator.params_->TypicalNetworkQuality(ect_type)
                      .downstream_throughput_kbps(),
                  rtt_throughput_observer.downstream_throughput_kbps());
      }
    }
  }
}

// Test that the typical network qualities are set correctly.
TEST(NetworkQualityEstimatorTest, TypicalNetworkQualities) {
  const struct {
    bool use_transport_rtt;
  } tests[] = {
      {
          false,
      },
      {
          true,
      },
  };

  for (const auto& test : tests) {
    std::map<std::string, std::string> variation_params;
    if (test.use_transport_rtt) {
      variation_params["effective_connection_type_algorithm"] =
          "TransportRTTOrDownstreamThroughput";
    }
    TestNetworkQualityEstimator estimator(variation_params);
    TestDelegate test_delegate;
    TestURLRequestContext context(true);
    context.set_network_quality_estimator(&estimator);
    context.Init();

    for (size_t effective_connection_type = EFFECTIVE_CONNECTION_TYPE_SLOW_2G;
         effective_connection_type <= EFFECTIVE_CONNECTION_TYPE_4G;
         ++effective_connection_type) {
      // Set the RTT and throughput values to the typical values for
      // |effective_connection_type|. The effective connection type should be
      // computed as |effective_connection_type|.
      estimator.set_start_time_null_http_rtt(
          estimator.params_
              ->TypicalNetworkQuality(static_cast<EffectiveConnectionType>(
                  effective_connection_type))
              .http_rtt());
      estimator.set_start_time_null_transport_rtt(
          estimator.params_
              ->TypicalNetworkQuality(static_cast<EffectiveConnectionType>(
                  effective_connection_type))
              .transport_rtt());
      estimator.set_start_time_null_downlink_throughput_kbps(INT32_MAX);

      // Force recomputation of effective connection type by starting  a main
      // frame request.
      std::unique_ptr<URLRequest> request(
          context.CreateRequest(estimator.GetEchoURL(), DEFAULT_PRIORITY,
                                &test_delegate, TRAFFIC_ANNOTATION_FOR_TESTS));
      request->SetLoadFlags(request->load_flags() | LOAD_MAIN_FRAME_DEPRECATED);
      request->Start();
      base::RunLoop().Run();

      EXPECT_EQ(effective_connection_type,
                static_cast<size_t>(estimator.GetEffectiveConnectionType()));
    }
  }
}

// Verify that the cached network qualities from the prefs are correctly used.
TEST(NetworkQualityEstimatorTest, OnPrefsRead) {
  base::HistogramTester histogram_tester;

  // Construct the read prefs.
  std::map<nqe::internal::NetworkID, nqe::internal::CachedNetworkQuality>
      read_prefs;
  read_prefs[nqe::internal::NetworkID(NetworkChangeNotifier::CONNECTION_WIFI,
                                      "test_ect_2g", INT32_MIN)] =
      nqe::internal::CachedNetworkQuality(EFFECTIVE_CONNECTION_TYPE_2G);
  read_prefs[nqe::internal::NetworkID(NetworkChangeNotifier::CONNECTION_WIFI,
                                      "test_ect_slow_2g", INT32_MIN)] =
      nqe::internal::CachedNetworkQuality(EFFECTIVE_CONNECTION_TYPE_SLOW_2G);
  read_prefs[nqe::internal::NetworkID(NetworkChangeNotifier::CONNECTION_4G,
                                      "test_ect_4g", INT32_MIN)] =
      nqe::internal::CachedNetworkQuality(EFFECTIVE_CONNECTION_TYPE_4G);

  std::map<std::string, std::string> variation_params;
  variation_params["effective_connection_type_algorithm"] =
      "TransportRTTOrDownstreamThroughput";
  variation_params["persistent_cache_reading_enabled"] = "true";
  variation_params["add_default_platform_observations"] = "false";
  // Disable default platform values so that the effect of cached estimates
  // at the time of startup can be studied in isolation.
  TestNetworkQualityEstimator estimator(
      std::unique_ptr<net::ExternalEstimateProvider>(), variation_params, true,
      true, std::make_unique<BoundTestNetLog>());

  // Add observers.
  TestRTTObserver rtt_observer;
  TestThroughputObserver throughput_observer;
  TestRTTAndThroughputEstimatesObserver rtt_throughput_observer;
  TestEffectiveConnectionTypeObserver effective_connection_type_observer;
  estimator.AddRTTObserver(&rtt_observer);
  estimator.AddThroughputObserver(&throughput_observer);
  estimator.AddRTTAndThroughputEstimatesObserver(&rtt_throughput_observer);
  estimator.AddEffectiveConnectionTypeObserver(
      &effective_connection_type_observer);

  std::string network_name("test_ect_2g");

  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, network_name);
  EXPECT_EQ(0u, rtt_observer.observations().size());
  EXPECT_EQ(0u, throughput_observer.observations().size());
  EXPECT_LE(0, rtt_throughput_observer.notifications_received());

  // Simulate reading of prefs.
  estimator.OnPrefsRead(read_prefs);
  histogram_tester.ExpectUniqueSample("NQE.Prefs.ReadSize", read_prefs.size(),
                                      1);

  // Taken from network_quality_estimator_params.cc.
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(1800),
            rtt_observer.last_rtt(
                NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP_CACHED_ESTIMATE));
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(1500),
            rtt_observer.last_rtt(
                NETWORK_QUALITY_OBSERVATION_SOURCE_TRANSPORT_CACHED_ESTIMATE));
  EXPECT_EQ(1u, throughput_observer.observations().size());
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(1800),
            rtt_throughput_observer.http_rtt());
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(1500),
            rtt_throughput_observer.transport_rtt());
  EXPECT_EQ(75, rtt_throughput_observer.downstream_throughput_kbps());
  EXPECT_LE(
      1u,
      effective_connection_type_observer.effective_connection_types().size());
  // Compare the ECT stored in prefs with the observer's last entry.
  EXPECT_EQ(
      read_prefs[nqe::internal::NetworkID(
                     NetworkChangeNotifier::CONNECTION_WIFI, network_name,
                     INT32_MIN)]
          .effective_connection_type(),
      effective_connection_type_observer.effective_connection_types().back());

  // Change to a different connection type.
  network_name = "test_ect_slow_2g";
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, network_name);

  EXPECT_EQ(base::TimeDelta::FromMilliseconds(3600),
            rtt_observer.last_rtt(
                NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP_CACHED_ESTIMATE));
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(3000),
            rtt_observer.last_rtt(
                NETWORK_QUALITY_OBSERVATION_SOURCE_TRANSPORT_CACHED_ESTIMATE));
  EXPECT_EQ(2U, throughput_observer.observations().size());
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(3600),
            rtt_throughput_observer.http_rtt());
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(3000),
            rtt_throughput_observer.transport_rtt());
  EXPECT_EQ(40, rtt_throughput_observer.downstream_throughput_kbps());
  EXPECT_LE(
      2u,
      effective_connection_type_observer.effective_connection_types().size());
  // Compare with the last entry.
  EXPECT_EQ(
      read_prefs[nqe::internal::NetworkID(
                     NetworkChangeNotifier::CONNECTION_WIFI, network_name,
                     INT32_MIN)]
          .effective_connection_type(),
      effective_connection_type_observer.effective_connection_types().back());

  // Cleanup.
  estimator.RemoveRTTObserver(&rtt_observer);
  estimator.RemoveThroughputObserver(&throughput_observer);
  estimator.RemoveRTTAndThroughputEstimatesObserver(&rtt_throughput_observer);
  estimator.RemoveEffectiveConnectionTypeObserver(
      &effective_connection_type_observer);
}

// Verify that the cached network qualities from the prefs are not used if the
// reading of the network quality prefs is not enabled..
TEST(NetworkQualityEstimatorTest, OnPrefsReadWithReadingDisabled) {
  base::HistogramTester histogram_tester;

  // Construct the read prefs.
  std::map<nqe::internal::NetworkID, nqe::internal::CachedNetworkQuality>
      read_prefs;
  read_prefs[nqe::internal::NetworkID(NetworkChangeNotifier::CONNECTION_WIFI,
                                      "test_ect_2g", INT32_MIN)] =
      nqe::internal::CachedNetworkQuality(EFFECTIVE_CONNECTION_TYPE_2G);
  read_prefs[nqe::internal::NetworkID(NetworkChangeNotifier::CONNECTION_WIFI,
                                      "test_ect_slow_2g", INT32_MIN)] =
      nqe::internal::CachedNetworkQuality(EFFECTIVE_CONNECTION_TYPE_SLOW_2G);
  read_prefs[nqe::internal::NetworkID(NetworkChangeNotifier::CONNECTION_4G,
                                      "test_ect_4g", INT32_MIN)] =
      nqe::internal::CachedNetworkQuality(EFFECTIVE_CONNECTION_TYPE_4G);

  std::map<std::string, std::string> variation_params;
  variation_params["effective_connection_type_algorithm"] =
      "TransportRTTOrDownstreamThroughput";
  variation_params["persistent_cache_reading_enabled"] = "false";
  variation_params["add_default_platform_observations"] = "false";

  // Disable default platform values so that the effect of cached estimates
  // at the time of startup can be studied in isolation.
  TestNetworkQualityEstimator estimator(
      std::unique_ptr<net::ExternalEstimateProvider>(), variation_params, true,
      true, std::make_unique<BoundTestNetLog>());

  // Add observers.
  TestRTTObserver rtt_observer;
  TestThroughputObserver throughput_observer;
  TestRTTAndThroughputEstimatesObserver rtt_throughput_observer;
  TestEffectiveConnectionTypeObserver effective_connection_type_observer;
  estimator.AddRTTObserver(&rtt_observer);
  estimator.AddThroughputObserver(&throughput_observer);
  estimator.AddRTTAndThroughputEstimatesObserver(&rtt_throughput_observer);
  estimator.AddEffectiveConnectionTypeObserver(
      &effective_connection_type_observer);

  std::string network_name("test_ect_2g");

  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, network_name);
  EXPECT_EQ(0u, rtt_observer.observations().size());
  EXPECT_EQ(0u, throughput_observer.observations().size());
  EXPECT_LE(0, rtt_throughput_observer.notifications_received());

  // Simulate reading of prefs.
  estimator.OnPrefsRead(read_prefs);
  histogram_tester.ExpectUniqueSample("NQE.Prefs.ReadSize", read_prefs.size(),
                                      1);

  // Force read the network quality store from the store to verify that store
  // gets populated even if reading of prefs is not enabled.
  nqe::internal::CachedNetworkQuality cached_network_quality;
  EXPECT_TRUE(estimator.network_quality_store_->GetById(
      nqe::internal::NetworkID(NetworkChangeNotifier::CONNECTION_WIFI,
                               "test_ect_2g", INT32_MIN),
      &cached_network_quality));
  EXPECT_EQ(EFFECTIVE_CONNECTION_TYPE_2G,
            cached_network_quality.effective_connection_type());

  // Taken from network_quality_estimator_params.cc.
  EXPECT_EQ(nqe::internal::InvalidRTT(),
            rtt_observer.last_rtt(
                NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP_CACHED_ESTIMATE));
  EXPECT_EQ(nqe::internal::InvalidRTT(),
            rtt_observer.last_rtt(
                NETWORK_QUALITY_OBSERVATION_SOURCE_TRANSPORT_CACHED_ESTIMATE));
  EXPECT_EQ(0u, throughput_observer.observations().size());

  EXPECT_EQ(
      0u,
      effective_connection_type_observer.effective_connection_types().size());

  // Change to a different connection type.
  network_name = "test_ect_slow_2g";
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, network_name);

  EXPECT_EQ(nqe::internal::InvalidRTT(),
            rtt_observer.last_rtt(
                NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP_CACHED_ESTIMATE));
  EXPECT_EQ(nqe::internal::InvalidRTT(),
            rtt_observer.last_rtt(
                NETWORK_QUALITY_OBSERVATION_SOURCE_TRANSPORT_CACHED_ESTIMATE));
  EXPECT_EQ(0U, throughput_observer.observations().size());

  // Cleanup.
  estimator.RemoveRTTObserver(&rtt_observer);
  estimator.RemoveThroughputObserver(&throughput_observer);
  estimator.RemoveRTTAndThroughputEstimatesObserver(&rtt_throughput_observer);
  estimator.RemoveEffectiveConnectionTypeObserver(
      &effective_connection_type_observer);
}

// Tests that |ComputeBandwidthDelayProduct| calculates the
// BDP correctly and records histogram data.
TEST(NetworkQualityEstimatorTest, TestBDPComputation) {
  TestNetworkQualityEstimator estimator;
  base::HistogramTester histogram_tester;
  base::TimeTicks now = base::TimeTicks::Now();
  for (int i = 1; i <= std::pow(2, 10); i *= 2) {
    estimator.transport_rtt_ms_observations_.AddObservation(
        NetworkQualityEstimator::Observation(
            i, now, INT32_MIN, NETWORK_QUALITY_OBSERVATION_SOURCE_TCP));
  }
  for (int i = 1; i <= std::pow(3, 10); i *= 3) {
    estimator.http_downstream_throughput_kbps_observations_.AddObservation(
        NetworkQualityEstimator::Observation(
            i, now, INT32_MIN, NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP));
  }
  estimator.RunOneRequest();

  // Histograms must contain at least one entry each.
  EXPECT_GE(
      1u, histogram_tester
              .GetAllSamples("NQE.BDPComputationTransportRTT.OnECTComputation")
              .size());
  EXPECT_GE(1u, histogram_tester
                    .GetAllSamples("NQE.BDPComputationKbps.OnECTComputation")
                    .size());
  EXPECT_GE(
      1u,
      histogram_tester.GetAllSamples("NQE.BDPKbits.OnECTComputation").size());

  EXPECT_TRUE(estimator.GetBandwidthDelayProductKbits().has_value());
  EXPECT_EQ(estimator.GetBandwidthDelayProductKbits().value(),
            (int32_t)(std::pow(2, 2) * std::pow(3, 8) / 1000));
}

TEST(NetworkQualityEstimatorTest,
     TestComputeIncreaseInTransportRTTFullHostsOverlap) {
  base::SimpleTestTickClock tick_clock;
  tick_clock.Advance(base::TimeDelta::FromMinutes(1));

  std::map<std::string, std::string> variation_params;
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params);
  estimator.SetTickClockForTesting(&tick_clock);

  base::TimeTicks now = tick_clock.NowTicks();
  base::TimeTicks recent = now - base::TimeDelta::FromMilliseconds(2500);
  base::TimeTicks historical = now - base::TimeDelta::FromSeconds(20);

  // Add historical observations. The 0 percentile for |host| is |10 * host|
  // ms.
  for (int host = 1; host <= 3; ++host) {
    for (int rtt = 10 * host; rtt <= 10 * host + 20; ++rtt) {
      estimator.transport_rtt_ms_observations_.AddObservation(
          NetworkQualityEstimator::Observation(
              rtt, historical, INT32_MIN,
              NETWORK_QUALITY_OBSERVATION_SOURCE_TCP,
              static_cast<uint64_t>(host)));
    }
  }

  // Add recent observations. The 50 percentile for |host| is |10 * host + 10|
  // ms. The difference between them is expected to be 10 ms.
  for (int host = 1; host <= 3; ++host) {
    for (int rtt = 10 * host + 5; rtt <= 10 * host + 15; ++rtt) {
      estimator.transport_rtt_ms_observations_.AddObservation(
          NetworkQualityEstimator::Observation(
              rtt, recent, INT32_MIN, NETWORK_QUALITY_OBSERVATION_SOURCE_TCP,
              static_cast<uint64_t>(host)));
    }
  }

  EXPECT_EQ(10, estimator.ComputeIncreaseInTransportRTTForTests().value_or(0));
}

TEST(NetworkQualityEstimatorTest,
     TestComputeIncreaseInTransportRTTPartialHostsOverlap) {
  base::SimpleTestTickClock tick_clock;
  tick_clock.Advance(base::TimeDelta::FromMinutes(1));

  std::map<std::string, std::string> variation_params;
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params);
  estimator.SetTickClockForTesting(&tick_clock);

  base::TimeTicks now = tick_clock.NowTicks();
  base::TimeTicks recent = now - base::TimeDelta::FromMilliseconds(2500);
  base::TimeTicks historical = now - base::TimeDelta::FromSeconds(20);

  // Add historical observations for hosts 1 and 2 with minimum RTT as
  // |10 * host|.
  for (int host = 1; host <= 2; ++host) {
    for (int rtt = 10 * host; rtt <= 10 * host + 20; ++rtt) {
      estimator.AddAndNotifyObserversOfRTT(NetworkQualityEstimator::Observation(
          rtt, historical, INT32_MIN, NETWORK_QUALITY_OBSERVATION_SOURCE_TCP,
          static_cast<uint64_t>(host)));
    }
  }

  // Add recent observations, with median RTT as |10 + host| over the
  // historical minimum for hosts 2 and 3.
  for (int host = 2; host <= 3; ++host) {
    for (int rtt = 11 * host + 5; rtt <= 11 * host + 15; ++rtt) {
      estimator.AddAndNotifyObserversOfRTT(NetworkQualityEstimator::Observation(
          rtt, recent, INT32_MIN, NETWORK_QUALITY_OBSERVATION_SOURCE_TCP,
          static_cast<uint64_t>(host)));
    }
  }

  // Only host 2 should have contributed to the calculation. Hence, the median
  // should be |10 + 2 = 12|.
  EXPECT_EQ(12, estimator.ComputeIncreaseInTransportRTTForTests().value_or(0));
}

// Verifies that when the cached network qualities from the prefs are available,
// then estimates from the platform or the external estimate provider are not
// used.
TEST(NetworkQualityEstimatorTest,
     ObservationDiscardedIfCachedEstimateAvailable) {
  base::HistogramTester histogram_tester;

  // Construct the read prefs.
  std::map<nqe::internal::NetworkID, nqe::internal::CachedNetworkQuality>
      read_prefs;
  read_prefs[nqe::internal::NetworkID(NetworkChangeNotifier::CONNECTION_WIFI,
                                      "test_2g", INT32_MIN)] =
      nqe::internal::CachedNetworkQuality(EFFECTIVE_CONNECTION_TYPE_2G);

  std::map<std::string, std::string> variation_params;
  variation_params["effective_connection_type_algorithm"] =
      "TransportRTTOrDownstreamThroughput";
  variation_params["persistent_cache_reading_enabled"] = "true";
  variation_params["add_default_platform_observations"] = "false";
  // Disable default platform values so that the effect of cached estimates
  // at the time of startup can be studied in isolation.
  TestNetworkQualityEstimator estimator(
      std::unique_ptr<net::ExternalEstimateProvider>(), variation_params, true,
      true, std::make_unique<BoundTestNetLog>());

  // Add observers.
  TestRTTObserver rtt_observer;
  TestThroughputObserver throughput_observer;
  estimator.AddRTTObserver(&rtt_observer);
  estimator.AddThroughputObserver(&throughput_observer);

  std::string network_name("test_2g");

  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_WIFI, network_name);
  EXPECT_EQ(0u, rtt_observer.observations().size());
  EXPECT_EQ(0u, throughput_observer.observations().size());

  estimator.AddAndNotifyObserversOfRTT(nqe::internal::Observation(
      1, base::TimeTicks::Now(), base::Optional<int32_t>(),
      NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP_EXTERNAL_ESTIMATE));
  estimator.AddAndNotifyObserversOfThroughput(nqe::internal::Observation(
      1, base::TimeTicks::Now(), base::Optional<int32_t>(),
      NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP_EXTERNAL_ESTIMATE));
  EXPECT_EQ(1u, rtt_observer.observations().size());
  EXPECT_EQ(1u, throughput_observer.observations().size());
  EXPECT_EQ(1u, estimator.http_rtt_ms_observations_.Size());
  EXPECT_EQ(0u, estimator.transport_rtt_ms_observations_.Size());
  EXPECT_EQ(1u, estimator.http_downstream_throughput_kbps_observations_.Size());

  // Simulate reading of prefs.
  estimator.OnPrefsRead(read_prefs);
  histogram_tester.ExpectUniqueSample("NQE.Prefs.ReadSize", read_prefs.size(),
                                      1);

  // Taken from network_quality_estimator_params.cc.
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(1800),
            rtt_observer.last_rtt(
                NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP_CACHED_ESTIMATE));
  EXPECT_EQ(base::TimeDelta::FromMilliseconds(1500),
            rtt_observer.last_rtt(
                NETWORK_QUALITY_OBSERVATION_SOURCE_TRANSPORT_CACHED_ESTIMATE));
  EXPECT_EQ(3u, rtt_observer.observations().size());

  // RTT observation with source
  // NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP_EXTERNAL_ESTIMATE should be removed
  // from |estimator.rtt_ms_observations_| when a cached estimate is received.
  EXPECT_EQ(1u, estimator.http_rtt_ms_observations_.Size());
  EXPECT_EQ(1u, estimator.transport_rtt_ms_observations_.Size());

  // When a cached estimate is available, RTT observations from the external
  // estimate provider and platform must be discarded.
  estimator.AddAndNotifyObserversOfRTT(nqe::internal::Observation(
      1, base::TimeTicks::Now(), base::Optional<int32_t>(),
      NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP_EXTERNAL_ESTIMATE));
  estimator.AddAndNotifyObserversOfRTT(nqe::internal::Observation(
      1, base::TimeTicks::Now(), base::Optional<int32_t>(),
      NETWORK_QUALITY_OBSERVATION_SOURCE_DEFAULT_HTTP_FROM_PLATFORM));
  EXPECT_EQ(3u, rtt_observer.observations().size());
  EXPECT_EQ(1u, estimator.http_rtt_ms_observations_.Size());
  EXPECT_EQ(1u, estimator.transport_rtt_ms_observations_.Size());
  estimator.AddAndNotifyObserversOfRTT(nqe::internal::Observation(
      1, base::TimeTicks::Now(), base::Optional<int32_t>(),
      NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP));
  EXPECT_EQ(4u, rtt_observer.observations().size());
  EXPECT_EQ(2u, estimator.http_rtt_ms_observations_.Size());
  EXPECT_EQ(1u, estimator.transport_rtt_ms_observations_.Size());

  // When a cached estimate is available, throughput observations from the
  // external estimate provider and platform must be discarded.
  EXPECT_EQ(2u, throughput_observer.observations().size());
  // Throughput observation with source
  // NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP_EXTERNAL_ESTIMATE should be removed
  // from |estimator.downstream_throughput_kbps_observations_| when a cached
  // estimate is received.
  EXPECT_EQ(1u, estimator.http_downstream_throughput_kbps_observations_.Size());
  estimator.AddAndNotifyObserversOfThroughput(nqe::internal::Observation(
      1, base::TimeTicks::Now(), base::Optional<int32_t>(),
      NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP_EXTERNAL_ESTIMATE));
  estimator.AddAndNotifyObserversOfThroughput(nqe::internal::Observation(
      1, base::TimeTicks::Now(), base::Optional<int32_t>(),
      NETWORK_QUALITY_OBSERVATION_SOURCE_DEFAULT_HTTP_FROM_PLATFORM));
  EXPECT_EQ(2u, throughput_observer.observations().size());
  EXPECT_EQ(1u, estimator.http_downstream_throughput_kbps_observations_.Size());
  estimator.AddAndNotifyObserversOfThroughput(nqe::internal::Observation(
      1, base::TimeTicks::Now(), base::Optional<int32_t>(),
      NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP));
  EXPECT_EQ(3u, throughput_observer.observations().size());
  EXPECT_EQ(2u, estimator.http_downstream_throughput_kbps_observations_.Size());

  base::RunLoop().RunUntilIdle();
}

// Tests that the ECT is computed when more than N RTT samples have been
// received.
TEST(NetworkQualityEstimatorTest, MaybeComputeECTAfterNSamples) {
  base::SimpleTestTickClock tick_clock;
  tick_clock.Advance(base::TimeDelta::FromMinutes(1));

  std::map<std::string, std::string> variation_params;
  variation_params["add_default_platform_observations"] = "false";
  TestNetworkQualityEstimator estimator(variation_params);
  estimator.DisableOfflineCheckForTesting(true);
  base::RunLoop().RunUntilIdle();
  estimator.SetTickClockForTesting(&tick_clock);
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_UNKNOWN, "test");
  tick_clock.Advance(base::TimeDelta::FromMinutes(1));

  const base::TimeDelta rtt = base::TimeDelta::FromSeconds(1);
  uint64_t host = 1u;

  // Fill the observation buffer so that ECT computations are not triggered due
  // to observation buffer's size increasing to 1.5x.
  for (size_t i = 0; i < estimator.params()->observation_buffer_size(); ++i) {
    estimator.AddAndNotifyObserversOfRTT(NetworkQualityEstimator::Observation(
        rtt.InMilliseconds(), tick_clock.NowTicks(), INT32_MIN,
        NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP, host));
  }
  EXPECT_EQ(rtt, estimator.GetHttpRTT().value());
  tick_clock.Advance(base::TimeDelta::FromMinutes(60));

  const base::TimeDelta rtt_new = base::TimeDelta::FromSeconds(3);
  for (size_t i = 0;
       i < estimator.params()->count_new_observations_received_compute_ect();
       ++i) {
    estimator.AddAndNotifyObserversOfRTT(NetworkQualityEstimator::Observation(
        rtt_new.InMilliseconds(), tick_clock.NowTicks(), INT32_MIN,
        NETWORK_QUALITY_OBSERVATION_SOURCE_HTTP, host));
  }
  EXPECT_EQ(rtt_new, estimator.GetHttpRTT().value());
}

// Tests that the hanging request is correctly detected.
TEST(NetworkQualityEstimatorTest, HangingRequestUsingHttpOnly) {
  base::HistogramTester histogram_tester;

  std::map<std::string, std::string> variation_params;
  variation_params["add_default_platform_observations"] = "false";
  variation_params["hanging_request_http_rtt_upper_bound_http_rtt_multiplier"] =
      "6";
  variation_params["hanging_request_upper_bound_min_http_rtt_msec"] = "500";

  TestNetworkQualityEstimator estimator(variation_params);

  // 500 msec.
  const int32_t hanging_request_threshold =
      estimator.params()
          ->hanging_request_upper_bound_min_http_rtt()
          .InMilliseconds();

  estimator.set_start_time_null_http_rtt(base::TimeDelta::FromMilliseconds(5));
  base::RunLoop().RunUntilIdle();
  estimator.SimulateNetworkChange(
      NetworkChangeNotifier::ConnectionType::CONNECTION_UNKNOWN, "test");

  const struct {
    base::TimeDelta observed_http_rtt;
    std::string histogram_name;
  } tests[] = {
      {base::TimeDelta::FromMilliseconds(10),
       "NQE.RTT.NotAHangingRequest.HttpRTT"},
      {base::TimeDelta::FromMilliseconds(100),
       "NQE.RTT.NotAHangingRequest.MinHttpBound"},
      {base::TimeDelta::FromMilliseconds(hanging_request_threshold - 1),
       "NQE.RTT.NotAHangingRequest.MinHttpBound"},
      {base::TimeDelta::FromMilliseconds(hanging_request_threshold + 1),
       "NQE.RTT.HangingRequest"},
      {base::TimeDelta::FromMilliseconds(1000), "NQE.RTT.HangingRequest"},
  };

  for (const auto& test : tests) {
    EXPECT_EQ(
        test.observed_http_rtt.InMilliseconds() >= hanging_request_threshold,
        estimator.IsHangingRequest(test.observed_http_rtt));
    histogram_tester.ExpectBucketCount(
        test.histogram_name, test.observed_http_rtt.InMilliseconds(), 1);
  }

  // Verify total sample count in all histograms.
  histogram_tester.ExpectTotalCount("NQE.RTT.NotAHangingRequest.TransportRTT",
                                    0);
  histogram_tester.ExpectTotalCount("NQE.RTT.NotAHangingRequest.HttpRTT", 1);
  histogram_tester.ExpectTotalCount("NQE.RTT.NotAHangingRequest.MinHttpBound",
                                    2);
  histogram_tester.ExpectTotalCount("NQE.RTT.HangingRequest", 2);
}

TEST(NetworkQualityEstimatorTest, HangingRequestUsingTransportAndHttpOnly) {
  base::HistogramTester histogram_tester;

  std::map<std::string, std::string> variation_params;
  variation_params["add_default_platform_observations"] = "false";
  variation_params
      ["hanging_request_http_rtt_upper_bound_transport_rtt_multiplier"] = "8";
  variation_params["hanging_request_http_rtt_upper_bound_http_rtt_multiplier"] =
      "6";
  variation_params["hanging_request_upper_bound_min_http_rtt_msec"] = "500";

  const base::TimeDelta transport_rtt = base::TimeDelta::FromMilliseconds(100);

  TestNetworkQualityEstimator estimator(variation_params);

  // 800 msec.
  const int32_t hanging_request_threshold =
      transport_rtt.InMilliseconds() *
      estimator.params()
          ->hanging_request_http_rtt_upper_bound_transport_rtt_multiplier();

  estimator.DisableOfflineCheckForTesting(true);
  estimator.set_start_time_null_http_rtt(base::TimeDelta::FromMilliseconds(5));

  for (size_t i = 0; i < 100; ++i) {
    // Throw enough transport RTT samples so that transport RTT estimate is
    // recomputed.
    estimator.AddAndNotifyObserversOfRTT(NetworkQualityEstimator::Observation(
        transport_rtt.InMilliseconds(), base::TimeTicks::Now(), INT32_MIN,
        NETWORK_QUALITY_OBSERVATION_SOURCE_TCP, 0));
  }

  base::RunLoop().RunUntilIdle();
  EXPECT_EQ(transport_rtt, estimator.GetTransportRTT());

  const struct {
    base::TimeDelta observed_http_rtt;
    std::string histogram_name;
  } tests[] = {
      {base::TimeDelta::FromMilliseconds(100),
       "NQE.RTT.NotAHangingRequest.TransportRTT"},
      {base::TimeDelta::FromMilliseconds(500),
       "NQE.RTT.NotAHangingRequest.TransportRTT"},
      {base::TimeDelta::FromMilliseconds(hanging_request_threshold - 1),
       "NQE.RTT.NotAHangingRequest.TransportRTT"},
      {base::TimeDelta::FromMilliseconds(hanging_request_threshold + 1),
       "NQE.RTT.HangingRequest"},
      {base::TimeDelta::FromMilliseconds(1000), "NQE.RTT.HangingRequest"},
  };

  for (const auto& test : tests) {
    EXPECT_EQ(
        test.observed_http_rtt.InMilliseconds() >= hanging_request_threshold,
        estimator.IsHangingRequest(test.observed_http_rtt));
    histogram_tester.ExpectBucketCount(
        test.histogram_name, test.observed_http_rtt.InMilliseconds(), 1);
  }

  // Verify total sample count in all histograms.
  histogram_tester.ExpectTotalCount("NQE.RTT.NotAHangingRequest.TransportRTT",
                                    3);
  histogram_tester.ExpectTotalCount("NQE.RTT.NotAHangingRequest.HttpRTT", 0);
  histogram_tester.ExpectTotalCount("NQE.RTT.NotAHangingRequest.MinHttpBound",
                                    0);
  histogram_tester.ExpectTotalCount("NQE.RTT.HangingRequest", 2);
}

}  // namespace net
