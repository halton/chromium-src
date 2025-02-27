// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_WEB_VIEW_INTERNAL_CWV_WEB_VIEW_INTERNAL_H_
#define IOS_WEB_VIEW_INTERNAL_CWV_WEB_VIEW_INTERNAL_H_

#import "ios/web_view/public/cwv_web_view.h"

@interface CWVWebView ()

// This is called by the associated CWVWebViewConfiguration in order to shut
// down cleanly. See CWVWebViewConfiguration's |shutDown| method for more info.
- (void)shutDown;

@end

#endif  // IOS_WEB_VIEW_INTERNAL_CWV_WEB_VIEW_INTERNAL_H_
