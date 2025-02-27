// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/offline_pages/core/prefetch/download_archives_task.h"

#include <algorithm>
#include <set>

#include "base/guid.h"
#include "base/numerics/safe_conversions.h"
#include "base/test/histogram_tester.h"
#include "base/test/scoped_feature_list.h"
#include "components/offline_pages/core/offline_page_feature.h"
#include "components/offline_pages/core/prefetch/store/prefetch_downloader_quota.h"
#include "components/offline_pages/core/prefetch/store/prefetch_store_test_util.h"
#include "components/offline_pages/core/prefetch/store/prefetch_store_utils.h"
#include "components/offline_pages/core/prefetch/task_test_base.h"
#include "components/offline_pages/core/prefetch/test_prefetch_downloader.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace offline_pages {

namespace {

const int64_t kSmallArchiveSize = 1LL * 1024 * 1024;
const int64_t kLargeArchiveSize =
    2 * PrefetchDownloaderQuota::GetMaxDailyQuotaBytes() / 3;

const PrefetchItem* FindPrefetchItemByOfflineId(
    const std::set<PrefetchItem>& items,
    int64_t offline_id) {
  auto found_it = std::find_if(items.begin(), items.end(),
                               [&offline_id](const PrefetchItem& i) -> bool {
                                 return i.offline_id == offline_id;
                               });
  if (found_it != items.end())
    return &(*found_it);
  return nullptr;
}

class DownloadArchivesTaskTest : public TaskTestBase {
 public:
  TestPrefetchDownloader* prefetch_downloader() {
    return &test_prefetch_downloader_;
  }

  int64_t InsertDummyItem();
  void InsertDummyItemInState(PrefetchItemState state);
  int64_t InsertItemToDownload(int64_t archive_size);

 private:
  TestPrefetchDownloader test_prefetch_downloader_;
};

int64_t DownloadArchivesTaskTest::InsertDummyItem() {
  PrefetchItem item = item_generator()->CreateItem(
      PrefetchItemState::SENT_GENERATE_PAGE_BUNDLE);
  store_util()->InsertPrefetchItem(item);
  return item.offline_id;
}

void DownloadArchivesTaskTest::InsertDummyItemInState(PrefetchItemState state) {
  store_util()->InsertPrefetchItem(item_generator()->CreateItem(state));
}

int64_t DownloadArchivesTaskTest::InsertItemToDownload(int64_t archive_size) {
  PrefetchItem item =
      item_generator()->CreateItem(PrefetchItemState::RECEIVED_BUNDLE);
  item.archive_body_length = archive_size;
  store_util()->InsertPrefetchItem(item);
  return item.offline_id;
}

TEST_F(DownloadArchivesTaskTest, StoreFailure) {
  store_util()->SimulateInitializationError();
  DownloadArchivesTask task(store(), prefetch_downloader());
  ExpectTaskCompletes(&task);
  task.Run();
  RunUntilIdle();
}

TEST_F(DownloadArchivesTaskTest, NoArchivesToDownload) {
  InsertDummyItemInState(PrefetchItemState::NEW_REQUEST);
  InsertDummyItemInState(PrefetchItemState::SENT_GENERATE_PAGE_BUNDLE);
  InsertDummyItemInState(PrefetchItemState::AWAITING_GCM);
  InsertDummyItemInState(PrefetchItemState::RECEIVED_GCM);
  InsertDummyItemInState(PrefetchItemState::SENT_GET_OPERATION);
  InsertDummyItemInState(PrefetchItemState::DOWNLOADING);
  InsertDummyItemInState(PrefetchItemState::DOWNLOADED);
  InsertDummyItemInState(PrefetchItemState::IMPORTING);
  InsertDummyItemInState(PrefetchItemState::FINISHED);
  InsertDummyItemInState(PrefetchItemState::ZOMBIE);

  std::set<PrefetchItem> items_before_run;
  EXPECT_EQ(10U, store_util()->GetAllItems(&items_before_run));

  DownloadArchivesTask task(store(), prefetch_downloader());
  base::HistogramTester histogram_tester;
  ExpectTaskCompletes(&task);
  task.Run();
  RunUntilIdle();

  std::set<PrefetchItem> items_after_run;
  EXPECT_EQ(10U, store_util()->GetAllItems(&items_after_run));

  EXPECT_EQ(items_before_run, items_after_run);

  histogram_tester.ExpectTotalCount(
      "OfflinePages.Prefetching.DownloadExpectedFileSize", 0);
}

TEST_F(DownloadArchivesTaskTest, SingleArchiveToDownload) {
  int64_t dummy_item_id = InsertDummyItem();
  int64_t download_item_id = InsertItemToDownload(kLargeArchiveSize);

  std::set<PrefetchItem> items_before_run;
  EXPECT_EQ(2U, store_util()->GetAllItems(&items_before_run));

  DownloadArchivesTask task(store(), prefetch_downloader());
  base::HistogramTester histogram_tester;
  ExpectTaskCompletes(&task);
  task.Run();
  RunUntilIdle();

  std::set<PrefetchItem> items_after_run;
  EXPECT_EQ(2U, store_util()->GetAllItems(&items_after_run));

  const PrefetchItem* dummy_item_before =
      FindPrefetchItemByOfflineId(items_before_run, dummy_item_id);
  const PrefetchItem* dummy_item_after =
      FindPrefetchItemByOfflineId(items_after_run, dummy_item_id);
  ASSERT_TRUE(dummy_item_before);
  ASSERT_TRUE(dummy_item_after);
  EXPECT_EQ(*dummy_item_before, *dummy_item_after);

  const PrefetchItem* download_item_before =
      FindPrefetchItemByOfflineId(items_before_run, download_item_id);
  EXPECT_EQ(0, download_item_before->download_initiation_attempts);

  const PrefetchItem* download_item =
      FindPrefetchItemByOfflineId(items_after_run, download_item_id);
  ASSERT_TRUE(download_item);
  EXPECT_EQ(PrefetchItemState::DOWNLOADING, download_item->state);
  EXPECT_EQ(1, download_item->download_initiation_attempts);
  // These times are created using base::Time::Now() in short distance from each
  // other, therefore putting *_LE was considered too.
  EXPECT_LT(download_item_before->freshness_time,
            download_item->freshness_time);

  std::map<std::string, std::string> requested_downloads =
      prefetch_downloader()->requested_downloads();
  auto it = requested_downloads.find(download_item->guid);
  ASSERT_TRUE(it != requested_downloads.end());
  EXPECT_EQ(it->second, download_item->archive_body_name);

  histogram_tester.ExpectUniqueSample(
      "OfflinePages.Prefetching.DownloadExpectedFileSize",
      kLargeArchiveSize / 1024, 1);
}

TEST_F(DownloadArchivesTaskTest, MultipleArchivesToDownload) {
  int64_t dummy_item_id = InsertDummyItem();
  int64_t download_item_id_1 = InsertItemToDownload(kSmallArchiveSize);
  int64_t download_item_id_2 = InsertItemToDownload(kSmallArchiveSize);

  std::set<PrefetchItem> items_before_run;
  EXPECT_EQ(3U, store_util()->GetAllItems(&items_before_run));

  DownloadArchivesTask task(store(), prefetch_downloader());
  base::HistogramTester histogram_tester;
  ExpectTaskCompletes(&task);
  task.Run();
  RunUntilIdle();

  std::set<PrefetchItem> items_after_run;
  EXPECT_EQ(3U, store_util()->GetAllItems(&items_after_run));

  const PrefetchItem* dummy_item_before =
      FindPrefetchItemByOfflineId(items_before_run, dummy_item_id);
  const PrefetchItem* dummy_item_after =
      FindPrefetchItemByOfflineId(items_after_run, dummy_item_id);
  ASSERT_TRUE(dummy_item_before);
  ASSERT_TRUE(dummy_item_after);
  EXPECT_EQ(*dummy_item_before, *dummy_item_after);

  const PrefetchItem* download_item_1 =
      FindPrefetchItemByOfflineId(items_after_run, download_item_id_1);
  ASSERT_TRUE(download_item_1);
  EXPECT_EQ(PrefetchItemState::DOWNLOADING, download_item_1->state);

  const PrefetchItem* download_item_2 =
      FindPrefetchItemByOfflineId(items_after_run, download_item_id_2);
  ASSERT_TRUE(download_item_2);
  EXPECT_EQ(PrefetchItemState::DOWNLOADING, download_item_2->state);

  std::map<std::string, std::string> requested_downloads =
      prefetch_downloader()->requested_downloads();
  EXPECT_EQ(2U, requested_downloads.size());

  auto it = requested_downloads.find(download_item_1->guid);
  ASSERT_TRUE(it != requested_downloads.end());
  EXPECT_EQ(it->second, download_item_1->archive_body_name);

  it = requested_downloads.find(download_item_2->guid);
  ASSERT_TRUE(it != requested_downloads.end());
  EXPECT_EQ(it->second, download_item_2->archive_body_name);

  histogram_tester.ExpectUniqueSample(
      "OfflinePages.Prefetching.DownloadExpectedFileSize",
      kSmallArchiveSize / 1024, 2);
}

TEST_F(DownloadArchivesTaskTest, MultipleLargeArchivesToDownload) {
  int64_t dummy_item_id = InsertDummyItem();
  // download_item_1 is expected to be fresher, therefore we create it second.
  int64_t download_item_id_2 = InsertItemToDownload(kLargeArchiveSize);
  int64_t download_item_id_1 = InsertItemToDownload(kLargeArchiveSize);

  std::set<PrefetchItem> items_before_run;
  EXPECT_EQ(3U, store_util()->GetAllItems(&items_before_run));

  DownloadArchivesTask task(store(), prefetch_downloader());
  base::HistogramTester histogram_tester;
  ExpectTaskCompletes(&task);
  task.Run();
  RunUntilIdle();

  std::set<PrefetchItem> items_after_run;
  EXPECT_EQ(3U, store_util()->GetAllItems(&items_after_run));

  const PrefetchItem* dummy_item_before =
      FindPrefetchItemByOfflineId(items_before_run, dummy_item_id);
  const PrefetchItem* dummy_item_after =
      FindPrefetchItemByOfflineId(items_after_run, dummy_item_id);
  ASSERT_TRUE(dummy_item_before);
  ASSERT_TRUE(dummy_item_after);
  EXPECT_EQ(*dummy_item_before, *dummy_item_after);

  const PrefetchItem* download_item_1 =
      FindPrefetchItemByOfflineId(items_after_run, download_item_id_1);
  ASSERT_TRUE(download_item_1);
  EXPECT_EQ(PrefetchItemState::DOWNLOADING, download_item_1->state);

  const PrefetchItem* download_item_2 =
      FindPrefetchItemByOfflineId(items_after_run, download_item_id_2);
  ASSERT_TRUE(download_item_2);
  EXPECT_EQ(PrefetchItemState::RECEIVED_BUNDLE, download_item_2->state);

  std::map<std::string, std::string> requested_downloads =
      prefetch_downloader()->requested_downloads();
  EXPECT_EQ(1U, requested_downloads.size());

  auto it = requested_downloads.find(download_item_1->guid);
  ASSERT_TRUE(it != requested_downloads.end());
  EXPECT_EQ(it->second, download_item_1->archive_body_name);

  histogram_tester.ExpectUniqueSample(
      "OfflinePages.Prefetching.DownloadExpectedFileSize",
      kLargeArchiveSize / 1024, 1);
}

TEST_F(DownloadArchivesTaskTest, TooManyArchivesToDownload) {
  // Create multiple archives.
  std::vector<int64_t> item_ids;
  const int total_items = DownloadArchivesTask::kMaxConcurrentDownloads + 2;
  // Create more than we allow to download in parallel and put then in the
  // |item_ids| in front.
  for (int i = 0; i < total_items; ++i)
    item_ids.insert(item_ids.begin(), InsertItemToDownload(kSmallArchiveSize));

  std::set<PrefetchItem> items_before_run;
  EXPECT_EQ(static_cast<size_t>(total_items),
            store_util()->GetAllItems(&items_before_run));

  DownloadArchivesTask task(store(), prefetch_downloader());
  base::HistogramTester histogram_tester;
  ExpectTaskCompletes(&task);
  task.Run();
  RunUntilIdle();

  std::set<PrefetchItem> items_after_run;
  EXPECT_EQ(static_cast<size_t>(total_items),
            store_util()->GetAllItems(&items_after_run));

  std::map<std::string, std::string> requested_downloads =
      prefetch_downloader()->requested_downloads();
  EXPECT_EQ(static_cast<size_t>(DownloadArchivesTask::kMaxConcurrentDownloads),
            requested_downloads.size());

  // First |kMaxConcurrentDownloads| should be started.
  for (int i = 0; i < DownloadArchivesTask::kMaxConcurrentDownloads; ++i) {
    const PrefetchItem* download_item =
        FindPrefetchItemByOfflineId(items_after_run, item_ids[i]);
    ASSERT_TRUE(download_item);
    EXPECT_EQ(PrefetchItemState::DOWNLOADING, download_item->state);

    auto it = requested_downloads.find(download_item->guid);
    ASSERT_TRUE(it != requested_downloads.end());
    EXPECT_EQ(it->second, download_item->archive_body_name);
  }

  // Remaining items shouldn't have been started.
  for (int i = DownloadArchivesTask::kMaxConcurrentDownloads; i < total_items;
       ++i) {
    const PrefetchItem* download_item_before =
        FindPrefetchItemByOfflineId(items_before_run, item_ids[i]);
    const PrefetchItem* download_item_after =
        FindPrefetchItemByOfflineId(items_after_run, item_ids[i]);
    ASSERT_TRUE(download_item_before);
    ASSERT_TRUE(download_item_after);
    EXPECT_EQ(*download_item_before, *download_item_after);
    EXPECT_EQ(PrefetchItemState::RECEIVED_BUNDLE, download_item_after->state);
  }

  histogram_tester.ExpectUniqueSample(
      "OfflinePages.Prefetching.DownloadExpectedFileSize",
      kSmallArchiveSize / 1024, DownloadArchivesTask::kMaxConcurrentDownloads);
}

TEST_F(DownloadArchivesTaskTest,
       ManyLargeArchivesToDownloadWithLimitlessEnabled) {
  // Enable limitless prefetching.
  base::test::ScopedFeatureList scoped_feature_list;
  scoped_feature_list.InitWithFeatures(
      {kPrefetchingOfflinePagesFeature,
       kOfflinePagesLimitlessPrefetchingFeature},
      {});

  // Check the concurrent downloads limit is greater for limitless.
  ASSERT_GT(DownloadArchivesTask::kMaxConcurrentDownloadsForLimitless,
            DownloadArchivesTask::kMaxConcurrentDownloads);

  // Create more archives than we allow to download in parallel with limitless
  // and put them in the fresher |item_ids| in front.
  const size_t max_concurrent_downloads = base::checked_cast<size_t>(
      DownloadArchivesTask::kMaxConcurrentDownloadsForLimitless);
  const size_t total_items = max_concurrent_downloads + 2;
  std::vector<int64_t> item_ids;
  for (size_t i = 0; i < total_items; ++i)
    item_ids.insert(item_ids.begin(), InsertItemToDownload(kLargeArchiveSize));

  std::set<PrefetchItem> items_before_run;
  EXPECT_EQ(total_items, store_util()->GetAllItems(&items_before_run));

  DownloadArchivesTask task(store(), prefetch_downloader());
  base::HistogramTester histogram_tester;
  ExpectTaskCompletes(&task);
  task.Run();
  RunUntilIdle();

  std::set<PrefetchItem> items_after_run;
  EXPECT_EQ(total_items, store_util()->GetAllItems(&items_after_run));

  std::map<std::string, std::string> requested_downloads =
      prefetch_downloader()->requested_downloads();
  EXPECT_EQ(max_concurrent_downloads, requested_downloads.size());

  // The freshest |kMaxConcurrentDownloadsForLimitless| items should be started
  // as with limitless enabled there's no download size restrictions.
  for (size_t i = 0; i < max_concurrent_downloads; ++i) {
    const PrefetchItem* download_item =
        FindPrefetchItemByOfflineId(items_after_run, item_ids[i]);
    ASSERT_TRUE(download_item);
    EXPECT_EQ(PrefetchItemState::DOWNLOADING, download_item->state);

    auto it = requested_downloads.find(download_item->guid);
    ASSERT_TRUE(it != requested_downloads.end());
    EXPECT_EQ(it->second, download_item->archive_body_name);
  }

  // Remaining items shouldn't have been started.
  for (size_t i = max_concurrent_downloads; i < total_items; ++i) {
    const PrefetchItem* download_item_before =
        FindPrefetchItemByOfflineId(items_before_run, item_ids[i]);
    const PrefetchItem* download_item_after =
        FindPrefetchItemByOfflineId(items_after_run, item_ids[i]);
    ASSERT_TRUE(download_item_before);
    ASSERT_TRUE(download_item_after);
    EXPECT_EQ(*download_item_before, *download_item_after);
    EXPECT_EQ(PrefetchItemState::RECEIVED_BUNDLE, download_item_after->state);
  }

  histogram_tester.ExpectUniqueSample(
      "OfflinePages.Prefetching.DownloadExpectedFileSize",
      kLargeArchiveSize / 1024, max_concurrent_downloads);
}

TEST_F(DownloadArchivesTaskTest, SingleArchiveSecondAttempt) {
  PrefetchItem item =
      item_generator()->CreateItem(PrefetchItemState::RECEIVED_BUNDLE);
  item.download_initiation_attempts = 1;
  item.freshness_time = base::Time::Now();
  item.guid = base::GenerateGUID();
  store_util()->InsertPrefetchItem(item);

  std::set<PrefetchItem> items_before_run;
  EXPECT_EQ(1U, store_util()->GetAllItems(&items_before_run));

  DownloadArchivesTask task(store(), prefetch_downloader());
  base::HistogramTester histogram_tester;
  ExpectTaskCompletes(&task);
  task.Run();
  RunUntilIdle();

  std::set<PrefetchItem> items_after_run;
  EXPECT_EQ(1U, store_util()->GetAllItems(&items_after_run));

  const PrefetchItem* download_item =
      FindPrefetchItemByOfflineId(items_after_run, item.offline_id);
  ASSERT_TRUE(download_item);
  EXPECT_EQ(PrefetchItemState::DOWNLOADING, download_item->state);
  EXPECT_EQ(2, download_item->download_initiation_attempts);
  EXPECT_EQ(item.archive_body_name, download_item->archive_body_name);
  // GUID expected to change between download attempts.
  EXPECT_NE(item.guid, download_item->guid);
  // Freshness time not expected to change after first attempt.
  EXPECT_EQ(item.freshness_time, download_item->freshness_time);

  std::map<std::string, std::string> requested_downloads =
      prefetch_downloader()->requested_downloads();
  auto it = requested_downloads.find(download_item->guid);
  ASSERT_TRUE(it != requested_downloads.end());
  EXPECT_EQ(it->second, download_item->archive_body_name);

  histogram_tester.ExpectUniqueSample(
      "OfflinePages.Prefetching.DownloadExpectedFileSize",
      item.archive_body_length / 1024, 1);
}

}  // namespace
}  // namespace offline_pages
