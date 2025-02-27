// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/assist_ranker/base_predictor.h"

#include "base/feature_list.h"
#include "base/memory/ptr_util.h"
#include "components/assist_ranker/proto/ranker_example.pb.h"
#include "components/assist_ranker/proto/ranker_model.pb.h"
#include "components/assist_ranker/ranker_example_util.h"
#include "components/assist_ranker/ranker_model.h"
#include "services/metrics/public/cpp/ukm_entry_builder.h"
#include "services/metrics/public/cpp/ukm_recorder.h"
#include "url/gurl.h"

namespace assist_ranker {

BasePredictor::BasePredictor(const PredictorConfig& config) : config_(config) {
  // TODO(chrome-ranker-team): validate config.
  if (config_.field_trial) {
    is_query_enabled_ = base::FeatureList::IsEnabled(*config_.field_trial);
  } else {
    DVLOG(0) << "No field trial specified";
  }
}

BasePredictor::~BasePredictor() {}

void BasePredictor::LoadModel(std::unique_ptr<RankerModelLoader> model_loader) {
  if (!is_query_enabled_)
    return;

  if (model_loader_) {
    DVLOG(0) << "This predictor already has a model loader.";
    return;
  }
  // Take ownership of the model loader.
  model_loader_ = std::move(model_loader);
  // Kick off the initial model load.
  model_loader_->NotifyOfRankerActivity();
}

void BasePredictor::OnModelAvailable(
    std::unique_ptr<assist_ranker::RankerModel> model) {
  ranker_model_ = std::move(model);
  is_ready_ = Initialize();
}

bool BasePredictor::IsReady() {
  if (!is_ready_ && model_loader_)
    model_loader_->NotifyOfRankerActivity();

  return is_ready_;
}

void BasePredictor::LogFeatureToUkm(const std::string& feature_name,
                                    const Feature& feature,
                                    ukm::UkmEntryBuilder* ukm_builder) {
  if (!ukm_builder)
    return;

  if (!base::ContainsKey(*config_.feature_whitelist, feature_name)) {
    DVLOG(1) << "Feature not whitelisted: " << feature_name;
    return;
  }

  int feature_int_value;
  if (FeatureToInt(feature, &feature_int_value)) {
    DVLOG(3) << "Logging: " << feature_name << ": " << feature_int_value;
    ukm_builder->AddMetric(feature_name.c_str(), feature_int_value);
  } else {
    DVLOG(0) << "Could not convert feature to int: " << feature_name;
  }
}

void BasePredictor::LogExampleToUkm(const RankerExample& example,
                                    ukm::SourceId source_id) {
  if (config_.log_type != LOG_UKM) {
    DVLOG(0) << "Wrong log type in predictor config: " << config_.log_type;
    return;
  }

  if (!config_.feature_whitelist) {
    DVLOG(0) << "No whitelist specified.";
    return;
  }
  if (config_.feature_whitelist->empty()) {
    DVLOG(0) << "Empty whitelist, examples will not be logged.";
    return;
  }

  // Releasing the builder will trigger logging.
  std::unique_ptr<ukm::UkmEntryBuilder> builder =
      ukm::UkmRecorder::Get()->GetEntryBuilder(source_id, config_.logging_name);
  if (builder) {
    for (const auto& feature_kv : example.features()) {
      LogFeatureToUkm(feature_kv.first, feature_kv.second, builder.get());
    }
  } else {
    DVLOG(0) << "Could not get UKM Entry Builder.";
  }
}

std::string BasePredictor::GetModelName() const {
  return config_.model_name;
}

GURL BasePredictor::GetModelUrl() const {
  if (!config_.field_trial_url_param) {
    DVLOG(1) << "No URL specified.";
    return GURL();
  }

  return GURL(config_.field_trial_url_param->Get());
}

RankerExample BasePredictor::PreprocessExample(const RankerExample& example) {
  if (ranker_model_->proto().has_metadata() &&
      ranker_model_->proto().metadata().input_features_names_are_hex_hashes()) {
    return HashExampleFeatureNames(example);
  }
  return example;
}

}  // namespace assist_ranker
