#ifndef _YSP_AVATAR_CONTROLLER_H_ 
#define _YSP_AVATAR_CONTROLLER_H_ 

#import <Cocoa/Cocoa.h>
#include "base/memory/scoped_ptr.h"
#include "base/mac/scoped_nsobject.h"
#import "chrome/browser/ui/cocoa/themed_window.h"
#import "chrome/browser/ui/cocoa/ysp/ysp_lock_view.h"
#import "chrome/browser/ui/cocoa/ysp/ysp_lock_view_controller.h"


class Browser;
class LoginObserver;
@class CrTrackingArea;
@class CustomWindowControlsView;

@interface YSPTitleView : NSView<ThemedWindowDrawing> {
  
}
@end

@interface yspAccountController : NSViewController {
@private
  //scoped_nsobject<NSImage> image_;
  //NSString *userName_;
  base::scoped_nsobject<NSView> titleView_;
  base::scoped_nsobject<YSPLockView> lockView_;
  base::scoped_nsobject<NSTextField> greetings_;
  base::scoped_nsobject<NSButton> avatarButton_;
  base::scoped_nsobject<NSButton> lockButton_;
  base::scoped_nsobject<NSTextField> title_;

  base::scoped_nsobject<YSPLockViewController> lockViewController_;

  scoped_ptr<LoginObserver> loginObserver_;
  Browser* browser_;
  
  // move from TabStripController
  base::scoped_nsobject<CustomWindowControlsView> customWindowControls_;
  base::scoped_nsobject<CrTrackingArea> customWindowControlsTrackingArea_;
}

-(id)initWithView:(NSView *)titleView
         lockView:(NSView *)lockView
          browser:(Browser *)browser;

#if 0
- (id)initWithView:(NSTextField *)userName
            avatar:(NSButton *)avatar
           browser:(Browser *)browser;
#endif
- (void)layout:(NSRect&)frame;

- (void)login:(NSString *)userName;
        //image:(NSImage *)image;
- (void)logout;
-(NSView*) titleView;

// ysp: move from tab strip controller
// Adds custom traffic light buttons to the tab strip. Idempotent.
- (void)addCustomWindowControls;

// Removes custom traffic light buttons from the tab strip. Idempotent.
- (void)removeCustomWindowControls;

@end

#endif
