//
//  ysp_lock_view.h
//  
//
//  Created by 刘 崴 on 18/5/17.
//
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
