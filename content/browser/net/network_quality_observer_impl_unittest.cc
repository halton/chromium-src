// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/net/network_quality_observer_impl.h"

#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"
#include "base/test/histogram_tester.h"
#include "base/time/time.h"
#include "net/nqe/effective_connection_type.h"
#include "net/nqe/network_quality_estimator_test_util.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace content {
namespace {

TEST(NetworkQualityObserverImplTest, TestObserverNotified) {
  base::MessageLoopForIO message_loop;

  net::TestNetworkQualityEstimator estimator;
  estimator.set_start_time_null_http_rtt(base::TimeDelta::FromMilliseconds(1));

  NetworkQualityObserverImpl observer(&estimator);
  // Give a chance for |observer| to register with the |estimator|.
  base::RunLoop().RunUntilIdle();
  // Run one main frame request to force recomputation of effective connection
  // type.
  estimator.RunOneRequest();
  base::RunLoop().RunUntilIdle();

  base::HistogramTester histogram_tester;
  estimator.set_start_time_null_http_rtt(
      base::TimeDelta::FromMilliseconds(500));
  // Run one main frame request to force recomputation of effective connection
  // type.
  estimator.RunOneRequest();
  base::RunLoop().RunUntilIdle();
  // RTT changed from 1 msec to 500 msec.
  histogram_tester.ExpectBucketCount(
      "NQE.ContentObserver.NetworkQualityMeaningfullyChanged", 1, 1);

  estimator.set_start_time_null_http_rtt(
      base::TimeDelta::FromMilliseconds(625));
  estimator.RunOneRequest();
  base::RunLoop().RunUntilIdle();
  // RTT changed from 500 msec to 625 msec.
  histogram_tester.ExpectBucketCount(
      "NQE.ContentObserver.NetworkQualityMeaningfullyChanged", 1, 2);

  estimator.set_start_time_null_http_rtt(
      base::TimeDelta::FromMilliseconds(626));
  estimator.RunOneRequest();
  base::RunLoop().RunUntilIdle();
  // RTT changed from 625 msec to 626 msec which is not a meaningful change.
  histogram_tester.ExpectBucketCount(
      "NQE.ContentObserver.NetworkQualityMeaningfullyChanged", 1, 2);
  EXPECT_LE(1, histogram_tester.GetBucketCount(
                   "NQE.ContentObserver.NetworkQualityMeaningfullyChanged", 0));
}

}  // namespace
}  // namespace content