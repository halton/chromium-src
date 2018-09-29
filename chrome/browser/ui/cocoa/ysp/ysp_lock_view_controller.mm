//
//  ysp_lock_view.m
//  
//
//  Created by 刘 崴 on 18/5/17.
//
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
#include "components/ysp_login/ysp_login_manager.h"
#include "base/prefs/pref_service.h"
#include "chrome/browser/browser_process.h"
#include "chrome/common/pref_names.h"
#include "chrome/browser/ui/browser.h"



const CGFloat kAvatarSize = 70;

@interface NSColor (Partial)
+ (NSColor *)colorWithRed:(CGFloat)red green:(CGFloat)green blue:(CGFloat)blue alpha:(CGFloat)alpha;
+ (NSColor *)colorWithWhite:(CGFloat)white alpha:(CGFloat)alpha;
@end

@interface NSImage (Partial)
- (void)drawInRect:(NSRect)rect;
@end

@interface NSView(Partial)
- (void)layout;
@property (readonly) NSSize fittingSize;
@property (readonly) NSSize intrinsicContentSize;
@end

@interface YSPLockView (Private)
- (void)OnSubmitButtonPressed;
@end

@interface NSTextField (Partial)
@property (nullable, copy) NSString *placeholderString;
@end

@implementation YSPLockViewController

- (id)initWithView:(YSPLockView *)lockView
{
  self = [super init];
  //self = [super initWithFrame:frame];
  if (self) {
    lockView_.reset([lockView retain]);

    // info label
    title_.reset([[NSTextField alloc] init]);
    [title_ setStringValue:@""];
    [title_ setEditable:NO];
    [title_ setFont:[NSFont titleBarFontOfSize:14]];
    [title_ setBordered:NO];
    [title_ setTextColor:[NSColor colorWithWhite:1
                                           alpha:1]];

    [title_ setBackgroundColor:[NSColor colorWithRed:0.9
                                                  green:0.9
                                                   blue:0.1
                                                  alpha:0]];

    [lockView_ addSubview:title_];
    
    // avatar
    avatar_.reset([[NSImageView alloc] initWithFrame:NSMakeRect(0, 0, kAvatarSize, kAvatarSize)]);
    [avatar_ setWantsLayer:YES];
    [avatar_ layer].cornerRadius = kAvatarSize / 2;
    [avatar_ layer].masksToBounds = YES;
    NSButtonCell *cell = [avatar_ cell];
    cell.imageScaling = NSImageScaleAxesIndependently;

    ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
    [avatar_ setImage:rb.GetNativeImageNamed(IDR_YSP_LOGIN_AVATAR).ToNSImage()];
    [lockView_ addSubview:avatar_];
    
    // user name label
    userNameLabel_.reset([[NSTextField alloc] init]);
    [userNameLabel_ setEditable:NO];
    [userNameLabel_ setStringValue:@"用户名"];
    [userNameLabel_ setFont:[NSFont titleBarFontOfSize:14]];
    [userNameLabel_ setBordered:NO];
    [userNameLabel_ setTextColor:[NSColor colorWithWhite:1 alpha:1]];
    [userNameLabel_ setBackgroundColor:[NSColor colorWithWhite:1
                                                         alpha:0]];

    [lockView_ addSubview:userNameLabel_];
    
    // password input label
    passwordContainer_.reset([[NSView alloc] initWithFrame:NSMakeRect(0, 0, 320, 40)]);
    [passwordContainer_ setWantsLayer:YES];
    [passwordContainer_ layer].backgroundColor = [[NSColor colorWithWhite:1 alpha:1] CGColor];

    password_.reset([[NSSecureTextField alloc] init]);

    [password_ setFont:[NSFont titleBarFontOfSize:14]];
    NSSize size = [password_ intrinsicContentSize];
    [password_ setFrame:NSMakeRect(13, (40-size.height)/2, 168, size.height)];

    [password_ setEditable:YES];
    [password_ setPlaceholderString:base::SysUTF16ToNSString(l10n_util::GetStringUTF16(IDS_YSP_LOCK_BROWSER_ENTER_PASSWORD_TO_UNLOCK))];
    [password_ setBordered:NO];
    [password_ setFocusRingType:NSFocusRingTypeNone];
    [password_ setBackgroundColor:[NSColor colorWithWhite:1 alpha:1]];
    [password_ setTarget:self];
    [password_ setAction:@selector(OnSubmitButtonPressed)];
    [passwordContainer_ addSubview:password_];
    [password_ setDelegate:self];

    prompt_.reset([[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 100, 12)]);
    [prompt_ setEditable:NO];
    [prompt_ setBordered:NO];
    [prompt_ setHidden:YES];
    [prompt_ setFont:[NSFont titleBarFontOfSize:14]];
    [prompt_ setTextColor:[NSColor colorWithWhite:1 alpha:1]];
    [prompt_ setBackgroundColor:[NSColor colorWithWhite:1 alpha:0]];
    //[prompt_ setStringValue:base::SysUTF16ToNSString(l10n_util::GetStringUTF16(IDS_YSP_LOCK_BROWSER_REENTER_PASSWORD))];
    [lockView_ addSubview:prompt_];

    enterButton_.reset([[NSButton alloc] initWithFrame:NSMakeRect(290, 13, 16, 14)]);
    [enterButton_ setWantsLayer:YES];
    [enterButton_ setImage:rb.GetNativeImageNamed(IDR_YSP_LOCK_SCREEN_ENTER).ToNSImage()];
    [enterButton_ setBordered:NO];
    [[enterButton_ cell] setBackgroundColor:[NSColor colorWithWhite:1 alpha:1]];

    [enterButton_ setTarget:self];
    [enterButton_ setAction:@selector(OnSubmitButtonPressed)];
    [passwordContainer_ addSubview:enterButton_];

    [lockView_ addSubview:passwordContainer_];

    [self layout];
  }
  
  return self;
}

- (void)windowImageShot {
  CGWindowID windowID = (CGWindowID)[[lockView_ window] windowNumber];
  CGWindowImageOption imageOptions = kCGWindowImageBoundsIgnoreFraming | kCGWindowImageShouldBeOpaque | kCGWindowImageBestResolution;
  CGWindowListOption singleWindowListOptions = kCGWindowListOptionIncludingWindow;
  CGRect imageBounds = CGRectNull;
  
  CGImageRef windowImage = CGWindowListCreateImage(imageBounds, singleWindowListOptions, windowID, imageOptions);
  CIImage *inputImage = [[CIImage alloc] initWithCGImage:windowImage];
  CIFilter *filter = [CIFilter filterWithName:@"CIGaussianBlur"];
                                //keysAndValues:kCIInputImageKey, inputImage, kCIInputRadiusKey, 2.5, nil];
  [filter setDefaults];
  [filter setValue:inputImage forKey:kCIInputImageKey];
  [filter setValue:@12.0 forKey:kCIInputRadiusKey];
  CIImage *output_temp = [[filter valueForKey:@"outputImage"] imageByCroppingToRect:[inputImage extent]];
  
  // make background darker
  CIFilter *evFilter = [CIFilter filterWithName:@"CIExposureAdjust"];
  [evFilter setValue:output_temp forKey:kCIInputImageKey];
  [evFilter setValue:@-2.0 forKey:kCIInputEVKey];
  CIImage *output = [[evFilter valueForKey:@"outputImage"] imageByCroppingToRect:[inputImage extent]];

  NSCIImageRep *rep = [NSCIImageRep imageRepWithCIImage:output];
  //image_.reset([[NSImage alloc] initWithSize:rep.size]);
  NSImage * image = [[NSImage alloc] initWithSize:rep.size];
  [image addRepresentation:rep];
  [lockView_ setImage:image];
}

- (void)layout {
  CGFloat window_width  = lockView_.get().frame.size.width;
  CGFloat window_height = lockView_.get().frame.size.height;
  CGFloat y = window_height * 1 / 1.85;
  CGFloat x;
  
  // info label
  NSSize size = [title_ fittingSize];
  x = (window_width - size.width) / 2;
  [title_ setFrame:NSMakeRect(x, y, size.width, size.height)];
  y -= 39;

  // avatar
  y -= kAvatarSize;
  x = (window_width - kAvatarSize) / 2;
  [avatar_ setFrame:NSMakeRect(x, y, kAvatarSize, kAvatarSize)];
  
  y -= 20;
  
  // user name
  size = [userNameLabel_ fittingSize];
  x = (window_width - size.width) / 2;
  y -= size.height;
  [userNameLabel_ setFrame:NSMakeRect(x, y, size.width, size.height)];
  
  y -= 20;
  
  // password & enter button
  size = passwordContainer_.get().frame.size;
  x = (window_width - size.width) / 2;
  y -= size.height;
  [passwordContainer_ setFrame:NSMakeRect(x, y, size.width, size.height)];

  y -= 10;

  // prompt
  size = [prompt_ fittingSize];
  y -= size.height;
  [prompt_ setFrame:NSMakeRect(x, y, size.width, size.height)];

  //[super layout];
}

- (void)OnSubmitButtonPressed {
  LOG(INFO) << "OnSubmitButtonPressed";
  [self submit:[password_ stringValue]];
}

- (void)show {
  [self windowImageShot];
  //[prompt_ setHidden:YES];
  [self layout];
  [lockView_ setNeedsDisplay:YES];
  [lockView_ setHidden:NO];
  [[lockView_ window] makeFirstResponder:password_.get()];
}

- (void)hide {
  [password_ setStringValue:@""];
  [lockView_ setHidden:YES];
}

- (void)setTitle:(NSString *)title {
  [title_ setStringValue:title];
}

- (void)setPromptMessage:(NSString *)message {
  if (message) {
    [prompt_ setStringValue:message];
    [prompt_ setHidden:NO];
  }
  else {
    [prompt_ setStringValue:@""];
    [prompt_ setHidden:YES];
  }
}

- (void)controlTextDidChange:(NSNotification *)obj {
  [self setPromptMessage:nil];
}

- (void)submit:(NSString *)password {
  LOG(INFO) << " >>[YSP] submit";
  std::string text = std::string([password UTF8String]);
  if (g_browser_process->local_state()->GetInteger(prefs::kYSPLockScreen) == Browser::TOKEN_EXPIRED_LOCKED) {
    YSPLoginManager::GetInstance()->StartLogin(YSPLoginManager::GetInstance()->GetLastCID(),
                                               YSPLoginManager::GetInstance()->GetLastUID(),
                                               text);
    return;
  }
  
  if (YSPLoginManager::GetInstance()->isValidPassword(text)) {
    g_browser_process->local_state()->SetInteger(prefs::kYSPLockScreen, Browser::UNLOCKED);
  }
  else {
    [self setPromptMessage:base::SysUTF16ToNSString(l10n_util::GetStringUTF16(IDS_YSP_LOCK_BROWSER_REENTER_PASSWORD))];
    [self layout];
  }
}

@end

