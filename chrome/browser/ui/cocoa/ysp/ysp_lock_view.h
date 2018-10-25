// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

//
//  ysp_lock_view.h
//  
//  Created by 刘 崴 on 18/5/17.
//

#ifndef ysp_lock_view_h
#define ysp_lock_view_h

#import <Cocoa/Cocoa.h>
#include "base/memory/scoped_ptr.h"
#include "base/mac/scoped_nsobject.h"
#import "chrome/browser/ui/cocoa/themed_window.h"

@class yspAccountController;

@interface YSPLockView : NSView<NSTextFieldDelegate> {
  base::scoped_nsobject<NSImage> image_;
  yspAccountController * controller_;
}

- (void)setImage:(NSImage *)image;

@end


#endif /* ysp_lock_view_h */
