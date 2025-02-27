// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/variations/pref_names.h"

namespace variations {
namespace prefs {

// base64-encoded compressed serialized form of the variations seed protobuf.
const char kVariationsCompressedSeed[] = "variations_compressed_seed";

// The latest country code received by the VariationsService for evaluating
// studies.
const char kVariationsCountry[] = "variations_country";

// The number of times that Chrome has crashed before successfully fetching a
// new seed. Used to determine whether to fall back to a "safe" seed.
const char kVariationsCrashStreak[] = "variations_crash_streak";

// The number of times that the VariationsService has failed to fetch a new
// seed. Used to determine whether to fall back to a "safe" seed.
const char kVariationsFailedToFetchSeedStreak[] =
    "variations_failed_to_fetch_seed_streak";

// 64-bit integer serialization of the base::Time from the last successful seed
// fetch (i.e. when the Variations server responds with 200 or 304).
const char kVariationsLastFetchTime[] = "variations_last_fetch_time";

// Pair of <Chrome version string, country code string> representing the country
// used for filtering permanent consistency studies until the next time Chrome
// is updated.
const char kVariationsPermanentConsistencyCountry[] =
    "variations_permanent_consistency_country";

// A serialized PermutedEntropyCache protobuf, used as a cache to avoid
// recomputing permutations.
const char kVariationsPermutedEntropyCache[] =
    "user_experience_metrics.permuted_entropy_cache";

// String for the restrict parameter to be appended to the variations URL.
const char kVariationsRestrictParameter[] = "variations_restrict_parameter";

// The last known "safe" variations seed, stored as the result of compressing
// the base64-encoded serialized form of the variations seed protobuf. Empty if
// there is no known "safe" seed. A seed is deemed "safe" if, while the seed is
// active, it has been observed to be possible to reach the variations server
// and download a new seed. Design doc:
// https://docs.google.com/document/d/17UN2pLSa5JZqk8f3LeYZIftXewxqcITotgalTrJvGSY
const char kVariationsSafeCompressedSeed[] = "variations_safe_compressed_seed";

// The 64-bit integer serialization of the base::Time at which the last known
// "safe" seed was received. An empty/default-constructed base::Time time if
// there is no known "safe" seed.
const char kVariationsSafeSeedDate[] = "variations_safe_seed_date";

// The active client locale that was successfully used in association with the
// last known "safe" seed.
const char kVariationsSafeSeedLocale[] = "variations_safe_seed_locale";

// The country code used by the VariationsService for evaluating permanent
// consistency studies, that was successfully used in association with the last
// known "safe" seed.
const char kVariationsSafeSeedPermanentConsistencyCountry[] =
    "variations_safe_seed_permanent_consistency_country";

// The country code received by the VariationsService for evaluating studies,
// that was successfully used in association with the last known "safe" seed.
// This is the country code used for session consistency studies.
const char kVariationsSafeSeedSessionConsistencyCountry[] =
    "variations_safe_seed_session_consistency_country";

// The digital signature of the last known "safe" variations seed's binary data,
// base64-encoded. Empty if there is no known "safe" seed.
const char kVariationsSafeSeedSignature[] = "variations_safe_seed_signature";

// 64-bit integer serialization of the base::Time from the last seed received.
const char kVariationsSeedDate[] = "variations_seed_date";

// Digital signature of the binary variations seed data, base64-encoded.
const char kVariationsSeedSignature[] = "variations_seed_signature";

}  // namespace prefs
}  // namespace metrics
