// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/geolocation/GeoNotifier.h"

#include "modules/geolocation/Geolocation.h"
#include "modules/geolocation/PositionError.h"
#include "modules/geolocation/PositionOptions.h"
#include "platform/Histogram.h"
#include "platform/wtf/Assertions.h"
#include "public/platform/TaskType.h"

namespace blink {

GeoNotifier::GeoNotifier(Geolocation* geolocation,
                         V8PositionCallback* success_callback,
                         V8PositionErrorCallback* error_callback,
                         const PositionOptions& options)
    : geolocation_(geolocation),
      success_callback_(success_callback),
      error_callback_(error_callback),
      options_(options),
      timer_(
          geolocation->GetDocument()->GetTaskRunner(TaskType::kMiscPlatformAPI),
          this,
          &GeoNotifier::TimerFired),
      use_cached_position_(false) {
  DCHECK(geolocation_);
  DCHECK(success_callback_);

  DEFINE_STATIC_LOCAL(CustomCountHistogram, timeout_histogram,
                      ("Geolocation.Timeout", 0,
                       1000 * 60 * 10 /* 10 minute max */, 20 /* buckets */));
  timeout_histogram.Count(options_.timeout());
}

void GeoNotifier::Trace(blink::Visitor* visitor) {
  visitor->Trace(geolocation_);
  visitor->Trace(success_callback_);
  visitor->Trace(error_callback_);
  visitor->Trace(fatal_error_);
}

void GeoNotifier::TraceWrappers(const ScriptWrappableVisitor* visitor) const {
  visitor->TraceWrappers(success_callback_);
  visitor->TraceWrappers(error_callback_);
}

void GeoNotifier::SetFatalError(PositionError* error) {
  // If a fatal error has already been set, stick with it. This makes sure that
  // when permission is denied, this is the error reported, as required by the
  // spec.
  if (fatal_error_)
    return;

  fatal_error_ = error;
  // An existing timer may not have a zero timeout.
  timer_.Stop();
  timer_.StartOneShot(TimeDelta(), FROM_HERE);
}

void GeoNotifier::SetUseCachedPosition() {
  use_cached_position_ = true;
  timer_.StartOneShot(TimeDelta(), FROM_HERE);
}

void GeoNotifier::RunSuccessCallback(Geoposition* position) {
  success_callback_->InvokeAndReportException(nullptr, position);
}

void GeoNotifier::RunErrorCallback(PositionError* error) {
  if (error_callback_)
    error_callback_->InvokeAndReportException(nullptr, error);
}

void GeoNotifier::StartTimer() {
  timer_.StartOneShot(options_.timeout() / 1000.0, FROM_HERE);
}

void GeoNotifier::StopTimer() {
  timer_.Stop();
}

void GeoNotifier::TimerFired(TimerBase*) {
  timer_.Stop();

  // As the timer fires asynchronously, it's possible that the execution context
  // has already gone.  Check it first.
  if (!geolocation_->GetExecutionContext()) {
    return;  // Do not invoke anything because of no execution context.
  }
  // TODO(yukishiino): Remove this check once we understand the cause.
  // https://crbug.com/792604
  CHECK(!geolocation_->GetExecutionContext()->IsContextDestroyed());

  // Test for fatal error first. This is required for the case where the
  // LocalFrame is disconnected and requests are cancelled.
  if (fatal_error_) {
    RunErrorCallback(fatal_error_);
    // This will cause this notifier to be deleted.
    geolocation_->FatalErrorOccurred(this);
    return;
  }

  if (use_cached_position_) {
    // Clear the cached position flag in case this is a watch request, which
    // will continue to run.
    use_cached_position_ = false;
    geolocation_->RequestUsesCachedPosition(this);
    return;
  }

  if (error_callback_) {
    error_callback_->InvokeAndReportException(
        nullptr,
        PositionError::Create(PositionError::kTimeout, "Timeout expired"));
  }

  DEFINE_STATIC_LOCAL(CustomCountHistogram, timeout_expired_histogram,
                      ("Geolocation.TimeoutExpired", 0,
                       1000 * 60 * 10 /* 10 minute max */, 20 /* buckets */));
  timeout_expired_histogram.Count(options_.timeout());

  geolocation_->RequestTimedOut(this);
}

}  // namespace blink
