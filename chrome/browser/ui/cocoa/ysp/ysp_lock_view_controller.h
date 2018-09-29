#ifndef ysp_lock_view_controller_h
#define ysp_lock_view_controller_h

#import <Cocoa/Cocoa.h>
#include "base/memory/scoped_ptr.h"
#include "base/mac/scoped_nsobject.h"
#import "chrome/browser/ui/cocoa/themed_window.h"

@class YSPLockView;

@interface YSPLockViewController : NSViewController<NSTextFieldDelegate> {
@private
  base::scoped_nsobject<YSPLockView> lockView_;
  base::scoped_nsobject<NSImage> image_;
  base::scoped_nsobject<NSImageView> avatar_;
  base::scoped_nsobject<NSTextField> title_;
  base::scoped_nsobject<NSTextField> userNameLabel_;
  base::scoped_nsobject<NSSecureTextField> password_;
  base::scoped_nsobject<NSView> passwordContainer_;
  base::scoped_nsobject<NSButton> enterButton_;
  base::scoped_nsobject<NSTextField> prompt_;
}

- (id)initWithView:(YSPLockView *)lockView;
- (void)windowImageShot;
- (void)layout;
- (void)show;
- (void)hide;
- (void)setTitle:(NSString *)title;
- (void)setPromptMessage:(NSString *)message;
- (void)submit:(NSString *)password;
@end

#endif