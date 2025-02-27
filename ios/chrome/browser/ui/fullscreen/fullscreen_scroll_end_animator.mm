// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "ios/chrome/browser/ui/fullscreen/fullscreen_scroll_end_animator.h"

#include <math.h>
#include <algorithm>

#include "base/logging.h"
#include "base/memory/ptr_util.h"
#import "ios/chrome/common/material_timing.h"
#include "ui/gfx/geometry/cubic_bezier.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

@interface FullscreenScrollEndAnimator () {
  // The bezier backing the timing curve.
  std::unique_ptr<gfx::CubicBezier> _bezier;
  // The current progress value that was recorded when the animator was stopped.
  CGFloat _progressUponStopping;
}
@end

@implementation FullscreenScrollEndAnimator
@synthesize startProgress = _startProgress;
@synthesize finalProgress = _finalProgress;

- (instancetype)initWithStartProgress:(CGFloat)startProgress {
  // Control points for Material Design CurveEaseOut curve.
  UICubicTimingParameters* timingParams = [[UICubicTimingParameters alloc]
      initWithControlPoint1:CGPointMake(0.0, 0.0)
              controlPoint2:CGPointMake(0.2, 0.1)];
  DCHECK_GE(startProgress, 0.0);
  DCHECK_LE(startProgress, 1.0);
  CGFloat finalProgress = roundf(startProgress);
  // Scale the duration by the progress delta traversed in this animation.
  // Since |finalProgress - startProgress| <= 0.5, the delta is multiplied by
  // 2.0.
  NSTimeInterval duration =
      2.0 * fabs(finalProgress - startProgress) * ios::material::kDuration1;
  self = [super initWithDuration:duration timingParameters:timingParams];
  if (self) {
    DCHECK_GE(startProgress, 0.0);
    DCHECK_LE(startProgress, 1.0);
    _startProgress = startProgress;
    _finalProgress = roundf(_startProgress);
    _bezier = base::MakeUnique<gfx::CubicBezier>(
        timingParams.controlPoint1.x, timingParams.controlPoint1.y,
        timingParams.controlPoint2.x, timingParams.controlPoint2.y);
  }
  return self;
}

#pragma mark Accessors

- (CGFloat)currentProgress {
  if (self.state == UIViewAnimatingStateStopped)
    return _progressUponStopping;
  CGFloat interpolationFraction = _bezier->Solve(self.fractionComplete);
  CGFloat range = self.finalProgress - self.startProgress;
  return self.startProgress + interpolationFraction * range;
}

#pragma mark Public

- (CGFloat)progressForAnimatingPosition:(UIViewAnimatingPosition)position {
  switch (position) {
    case UIViewAnimatingPositionStart:
      return self.startProgress;
    case UIViewAnimatingPositionEnd:
      return self.finalProgress;
    case UIViewAnimatingPositionCurrent:
      return self.currentProgress;
  }
}

#pragma mark UIViewAnimating

- (void)stopAnimation:(BOOL)withoutFinishing {
  // Record the progress value when transitioning from the active to stopped
  // state.  This allows |currentProgress| to return the correct value after
  // stopping, as |fractionComplete| is reset to 0.0 for stopped animators.
  if (self.state == UIViewAnimatingStateActive)
    _progressUponStopping = self.currentProgress;
  [super stopAnimation:withoutFinishing];
}

#pragma mark UIViewPropertyAnimator

- (BOOL)isInterruptible {
  return YES;
}

@end
