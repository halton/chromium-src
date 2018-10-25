// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//  ysp_lock_view.m
//
//  Created by 刘 崴 on 18/5/17.
//

#import <Foundation/Foundation.h>
#import <CoreImage/CoreImage.h>
#import "chrome/browser/ui/cocoa/ysp/ysp_lock_view.h"
#import "chrome/browser/ui/cocoa/ysp/ysp_account_controller.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/grit/theme_resources.h"
#include "ui/base/resource/resource_bundle.h"
#include "base/mac/sdk_forward_declarations.h"
#include "ui/base/l10n/l10n_util.h"
#include "base/strings/sys_string_conversions.h"

@interface NSImage (Partial)
- (void)drawInRect:(NSRect)rect;
@end

@implementation YSPLockView

- (void)mouseDown:(NSEvent *)theEvent {
  // do nothing
}

- (void)mouseUp:(NSEvent *)theEvent {
  // do nothing
}

- (BOOL)isOpaque {
  return YES;
}

- (void)setImage:(NSImage *)image
{
  image_.reset(image);
}

- (void)drawRect:(NSRect)dirtyRect {
  [image_ drawInRect:dirtyRect];
}

@end
