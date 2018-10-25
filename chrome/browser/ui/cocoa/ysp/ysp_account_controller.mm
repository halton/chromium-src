// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/ysp_login/ysp_login_manager.h"
#include "base/mac/mac_util.h"
#include "base/strings/sys_string_conversions.h"
#include "base/prefs/pref_service.h"
#include "grit/theme_resources.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/theme_provider.h"
#include "chrome/grit/generated_resources.h"

#include "chrome/common/pref_names.h"
#include "chrome/browser/ui/views/ysp_login_View.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/themes/theme_properties.h"

#import "ui/base/cocoa/tracking_area.h"
#import "chrome/browser/ui/cocoa/image_button_cell.h"
#import "chrome/browser/ui/cocoa/ysp/ysp_account_controller.h"

const CGFloat kAvatarSize = 24;

@interface NSView(Partial)
@property (readonly) NSSize fittingSize;
@end

@interface NSColor(Partial)
@property (readonly) CGColorRef CGColor;
@end

// A simple view class that contains the traffic light buttons. This class
// ensures that the buttons display the icons when the mouse hovers over
// them by overriding the _mouseInGroup method.
@interface CustomWindowControlsView : NSView {
@private
  BOOL mouseInside_;
}

// Overrides the undocumented NSView method: _mouseInGroup. When the traffic
// light buttons are drawn, they call _mouseInGroup from the superview. If
// _mouseInGroup returns YES, the buttons will draw themselves with the icons
// inside.
- (BOOL)_mouseInGroup:(NSButton*)button;
- (void)setMouseInside:(BOOL)isInside;

@end

@interface yspAccountController(Private)
- (void)relayout;
- (void)OnMousePressed;
- (void)addCustomWindowControls;
- (void)setBackground;
- (void)OnLockButtonPressed;
- (void)lockBrowser:(int)status;
- (void)unlockBrowser;
- (void)showTokenError;
@end

class LoginObserver : public YSPLoginManagerObserver {
public:
  LoginObserver();
  void OnConfigDataUpdated(const std::string& type, const std::string& data) override;
  void OnLoginRequestFailure(const std::string& error) override;
  void OnLoginResponseParseFailure(const std::string& error) override;
  void OnLoginFailure(base::string16 message) override;
  void OnLoginSuccess(const base::string16& name, const std::string& head_image_url) override;
  void OnLogout() override;
  void OnTokenStatusChanged(const std::string& type) override;
  void OnLockStatusChanged();
  void SetController(yspAccountController *controller) { controller_ = controller; }
  
private:
  yspAccountController* controller_;
  PrefChangeRegistrar profile_pref_registrar_;
};

LoginObserver::LoginObserver() : controller_(NULL) {
  profile_pref_registrar_.Init(g_browser_process->local_state());
  profile_pref_registrar_.Add(prefs::kYSPLockScreen,
                              base::Bind(&LoginObserver::OnLockStatusChanged, base::Unretained(this)));
}

void LoginObserver::OnConfigDataUpdated(const std::string& type, const std::string& data) {
  
}

void LoginObserver::OnLoginRequestFailure(const std::string& error) {
  
}

void LoginObserver::OnLoginResponseParseFailure(const std::string& error) {
  
}

void LoginObserver::OnLoginFailure(base::string16 message) {
  
}

void LoginObserver::OnLoginSuccess(const base::string16& name, const std::string& head_image_url) {
  if (controller_) {
    LOG(INFO) << " >>>> OnLoginSuccess name: " << name;
    //g_browser_process->local_state()->SetInteger(prefs::kYSPLockScreen, Browser::UNLOCKED);
    NSString *userName = base::SysUTF16ToNSString(name);
    [controller_ login:userName];
  }
}

void LoginObserver::OnLogout() {
  if (controller_)
    [controller_ logout];
}

void LoginObserver::OnTokenStatusChanged(const std::string& type) {
  LOG(INFO) << "LoginObserver::OnTokenStatusChanged: " << type;
  if (type == "TokenExpired") {
    g_browser_process->local_state()->SetInteger(prefs::kYSPLockScreen, Browser::TOKEN_EXPIRED_LOCKED);
  }
  else if (type == "TokenAvailable") {
    g_browser_process->local_state()->SetInteger(prefs::kYSPLockScreen, Browser::UNLOCKED);
  }
  else if (type == "failure") {
    [controller_ showTokenError];
  }
}

void LoginObserver::OnLockStatusChanged() {
  Browser::YSPLockStatus lock = static_cast<Browser::YSPLockStatus>(g_browser_process->local_state()->GetInteger(prefs::kYSPLockScreen));
  if (lock)
    [controller_ lockBrowser:lock];
  else
    [controller_ unlockBrowser];
}

@implementation CustomWindowControlsView

- (void)setMouseInside:(BOOL)isInside {
  if (mouseInside_ != isInside) {
    mouseInside_ = isInside;
    for (NSButton* button : [self subviews])
      [button setNeedsDisplay];
  }
}

- (BOOL)_mouseInGroup:(NSButton*)button {
  return mouseInside_;
}

- (void)mouseEntered:(NSEvent *)theEvent {
  [self setMouseInside:YES];
}

- (void)mouseExited:(NSEvent *)theEvent {
  [self setMouseInside:NO];
}

@end

@implementation YSPTitleView

// ThemedWindowDrawing protocal

- (void)windowDidChangeTheme {
  [self setNeedsDisplay:YES];
}

- (void)windowDidChangeActive {
  const ui::ThemeProvider* themeProvider = [[self window] themeProvider];
  if (themeProvider) {
    bool active = [[self window] isMainWindow];
    NSColor* color = active ? themeProvider->GetNSImageColorNamed(IDR_YSP_TITLE_ACTIVE)
    : themeProvider->GetNSImageColorNamed(IDR_YSP_TITLE_INACTIVE);
    [self layer].backgroundColor = [color CGColor];
    
    [self setNeedsDisplay:YES];
  }
}

@end

@implementation yspAccountController

- (void)setBackground:(bool)active {
  const ui::ThemeProvider* themeProvider = [[titleView_ window] themeProvider];
  if (!themeProvider)
    return;
  
  NSColor* color = active ? themeProvider->GetNSImageColorNamed(IDR_YSP_TITLE_ACTIVE)
                          : themeProvider->GetNSImageColorNamed(IDR_YSP_TITLE_INACTIVE);
  [titleView_ layer].backgroundColor = [color CGColor];
}


- (id)initWithView:(NSView *)titleView
          lockView:(YSPLockView *)lockView
          browser:(Browser *)browser
          delegate:(id<YSPControllerDelegate>)delegate {
  if (self = [super init]) {
    titleView_.reset([titleView retain]);
    lockViewController_.reset([[YSPLockViewController alloc] initWithView:lockView]);

    browser_ = browser;
    delegate_ = delegate;

    //CGFloat offsetY = (38 - kAvatarSize) / 2;
    [titleView_ setWantsLayer:YES];
    [self setBackground:YES];
    // username, greetings
    greetings_.reset([[NSTextField alloc] init]);
    [greetings_ setEditable:NO];
    [greetings_ setBordered:NO];
    [greetings_ setBackgroundColor:[NSColor colorWithCalibratedWhite:1
                                                         alpha:0]];
    [greetings_ setAutoresizingMask:NSViewMinYMargin | NSViewMaxXMargin];

    [greetings_ setStringValue:@"未登录"];
    [greetings_ setFont:[NSFont titleBarFontOfSize:12]];
    NSSize greetingsSize = [greetings_ fittingSize];
    [greetings_ setFrame:NSMakeRect(82,
                                (38 - greetingsSize.height)/2,
                                greetingsSize.width,
                                greetingsSize.height)];


    // avatar button
    avatarButton_.reset([[NSButton alloc]
                            initWithFrame:NSMakeRect(NSWidth([titleView_ bounds]) - (kAvatarSize + 58),
                                                     (38 - kAvatarSize) / 2,
                                                     kAvatarSize,
                                                     kAvatarSize)]);
    [avatarButton_ setWantsLayer:YES];
    [avatarButton_ setAutoresizingMask:NSViewMinYMargin | NSViewMinXMargin];
    [avatarButton_ setBordered:NO];
    [avatarButton_ layer].cornerRadius = kAvatarSize / 2;
    [avatarButton_ layer].masksToBounds = YES;
    NSButtonCell *cell = [avatarButton_ cell];
    cell.imageScaling = NSImageScaleAxesIndependently;

    ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
    [avatarButton_ setImage:rb.GetNativeImageNamed(IDR_YSP_LOGIN_AVATAR).ToNSImage()];

    [avatarButton_ setTarget:self];
    [avatarButton_ setAction:@selector(OnMousePressed)];

    // title
    title_.reset([[NSTextField alloc] init]);
    
    [title_ setEditable:NO];
    [title_ setBordered:NO];
    [title_ setBackgroundColor:[NSColor colorWithCalibratedWhite:1
                                                               alpha:0]];
    [title_ setStringValue:@"红芯企业浏览器"];
    [title_ setFont:[NSFont titleBarFontOfSize:14]];

    NSSize titleSize = [title_ fittingSize];
    [title_ setFrame:NSMakeRect((NSMaxX([titleView_ bounds]) - titleSize.width)/2,
                               (38 - titleSize.height)/2,
                               titleSize.width,
                               titleSize.height)];
    [title_ setAutoresizingMask:NSViewMinYMargin | NSViewMinXMargin | NSViewMaxXMargin];
    
    lockButton_.reset([[NSButton alloc] initWithFrame:(NSMakeRect(NSWidth([titleView_ bounds]) - (kAvatarSize + 15),
                                                      (38 - kAvatarSize) / 2,
                                                      kAvatarSize,
                                                      kAvatarSize))]);
    [lockButton_ setHidden:YES];
    [lockButton_ setWantsLayer:YES];
    [lockButton_ setAutoresizingMask:NSViewMinYMargin | NSViewMinXMargin];
    [lockButton_ setBordered:NO];

    ImageButtonCell *lockButtonCell = [[ImageButtonCell alloc] init];
    [lockButton_ setCell:lockButtonCell];
    
    [[lockButton_ cell] setImageID:IDR_YSP_LOCK_SCREEN
                    forButtonState:image_button_cell::kDefaultState];
    [[lockButton_ cell] setImageID:IDR_YSP_LOCK_SCREEN_H
                    forButtonState:image_button_cell::kHoverState];
    [[lockButton_ cell] setImageID:IDR_YSP_LOCK_SCREEN_P
                    forButtonState:image_button_cell::kPressedState];

    [lockButton_ layer].cornerRadius = kAvatarSize / 2;
    [lockButton_ layer].masksToBounds = YES;
    [[lockButton_ cell] setImageScaling:NSImageScaleAxesIndependently];

    //ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
    //[lockButton_ setImage:rb.GetNativeImageNamed(IDR_YSP_LOGIN_AVATAR).ToNSImage()];
    [lockButton_ setTarget:self];
    [lockButton_ setAction:@selector(OnLockButtonPressed)];

    [titleView_ addSubview:greetings_];
    [titleView_ addSubview:avatarButton_];
    [titleView_ addSubview:title_];
    [titleView_ addSubview:lockButton_];

    [self addCustomWindowControls];
    // TODO: register ysp manager listener
    loginObserver_.reset(new LoginObserver());
    loginObserver_->SetController(self);
    YSPLoginManager::GetInstance()->AddObserver(loginObserver_.get());
  }
  return self;
}

- (void)browserWillBeDestroyed {
  browser_ = nullptr;
  YSPLoginManager::GetInstance()->RemoveObserver(loginObserver_.get());
  loginObserver_->SetController(nullptr);
}

- (void)dealloc {
  [self browserWillBeDestroyed];
  [super dealloc];
}

- (void)OnMousePressed {
  std::string url_str;
  YSPLoginManager* manager = YSPLoginManager::GetInstance();
  manager->GetAutoConfigfetcher();
  manager->GetUserfetcher();
  manager->GetApplicationfetcher();
  manager->GetStrategyfetcher();
  manager->GetTokenfetcher();
  manager->GetSwafetcher();
  manager->GetPcfetcher();

  if (manager->GetLoginStatus())
    url_str = "redcore://settings";
  else
    url_str = "redcore://newtab";

  const GURL url(url_str);
  const content::Referrer ref(url, blink::WebReferrerPolicyDefault);
  content::OpenURLParams param(url,
                               ref,
                               SINGLETON_TAB,
                               ui::PAGE_TRANSITION_AUTO_TOPLEVEL,
                               false);
  browser_->OpenURL(param);
  
  return;
}

- (void)OnLockButtonPressed {
  LOG(INFO) << "OnLockButtonPressed";
  g_browser_process->local_state()->SetInteger(prefs::kYSPLockScreen, Browser::SCREEN_LOCKED);
}

- (void)layout:(NSRect&)frame {
  //if (!lockView_.get().hidden) {
  //  [lockView_ layout];
  //}
#if 0
  // TODO:
  if (frame.size.height == 0) {
    [greetings_ setHidden:YES];
    [avatarButton_ setHidden:YES];
    return;
  }

  [greetings_ setHidden:NO];
  [avatarButton_ setHidden:NO];

  NSSize labelSize = [greetings_ fittingSize];
  NSRect labelFrame = NSMakeRect(frame.origin.x, frame.origin.y, labelSize.width, labelSize.height);

  [greetings_ setFrame:labelFrame];
  [avatarButton_ setFrame:avatarFrame];
#endif
  
}

- (void)relayout {
  NSSize labelSize = [greetings_ fittingSize];
  //NSLog(@" >>>> [LW] relayout size: w:%f, h:%f", labelSize.width, labelSize.height);
  NSRect frame = [greetings_ frame];
  frame.size = labelSize;
  [greetings_ setFrame:frame];
}

- (void)login:(NSString *)userName {
  NSString *text = [NSString stringWithFormat:@"%@, 你好", userName];
  [greetings_ setStringValue:text];
  [lockButton_ setHidden:NO];
  [self relayout];
}

- (void)logout {
  [greetings_ setStringValue:@"未登录"];
  //[avatarButton_ setTitle:@""];
  //ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
  //[avatarButton_ setImage:rb.GetNativeImageNamed(IDR_YSP_LOGIN_AVATAR).ToNSImage()];
  [lockButton_ setHidden:YES];

  [self relayout];
}

- (void)addCustomWindowControls {
  if (!customWindowControls_) {
    // Make the container view.
    CGFloat height = NSHeight([titleView_ frame]);
    NSRect frame = NSMakeRect(0, 0, 80, height);
    customWindowControls_.reset(
                                [[CustomWindowControlsView alloc] initWithFrame:frame]);
    [customWindowControls_
     setAutoresizingMask:NSViewMaxXMargin | NSViewHeightSizable];
    
    // Add the traffic light buttons. The horizontal layout was determined by
    // manual inspection on Yosemite.
    CGFloat closeButtonX = 11;
    CGFloat pinnedButtonX = 31;
    CGFloat zoomButtonX = 51;
    
    NSUInteger styleMask = [[titleView_ window] styleMask];
    NSButton* closeButton = [NSWindow standardWindowButton:NSWindowCloseButton
                                              forStyleMask:styleMask];
    
    // Vertically center the buttons in the tab strip.
    CGFloat buttonY = floor((height - NSHeight([closeButton bounds])) / 2);
    [closeButton setFrameOrigin:NSMakePoint(closeButtonX, buttonY)];
    [customWindowControls_ addSubview:closeButton];
    
    NSButton* miniaturizeButton =
    [NSWindow standardWindowButton:NSWindowMiniaturizeButton
                      forStyleMask:styleMask];
    [miniaturizeButton setFrameOrigin:NSMakePoint(pinnedButtonX, buttonY)];
    [miniaturizeButton setEnabled:NO];
    [customWindowControls_ addSubview:miniaturizeButton];
    
    NSButton* zoomButton =
    [NSWindow standardWindowButton:NSWindowZoomButton
                      forStyleMask:styleMask];
    [customWindowControls_ addSubview:zoomButton];
    [zoomButton setFrameOrigin:NSMakePoint(zoomButtonX, buttonY)];
    
    customWindowControlsTrackingArea_.reset([[CrTrackingArea alloc]
                                             initWithRect:[customWindowControls_ bounds]
                                             options:(NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways)
                                             owner:customWindowControls_ //self
                                             userInfo:nil]);
    [customWindowControls_
     addTrackingArea:customWindowControlsTrackingArea_.get()];

    [titleView_ addSubview:customWindowControls_];
  }
  
  [customWindowControls_ setHidden:NO];

#if 0
  if (![permanentSubviews_ containsObject:customWindowControls_]) {
    [self addSubviewToPermanentList:customWindowControls_];
    [self regenerateSubviewList];
  }
#endif
}

- (void)removeCustomWindowControls {
  if (customWindowControls_) {
    [customWindowControls_ setHidden:YES];
    [customWindowControls_ setMouseInside:NO];
  }
}

- (NSView*)titleView {
  return titleView_;
}

- (void)lockBrowser:(int)status {
  if (status == Browser::SCREEN_LOCKED)
    [lockViewController_ setTitle:base::SysUTF16ToNSString(l10n_util::GetStringUTF16(IDS_YSP_LOCK_BROWSER_IN_LOCK_MODE))];
  else if (status == Browser::TOKEN_EXPIRED_LOCKED)
    [lockViewController_ setTitle:base::SysUTF16ToNSString(l10n_util::GetStringUTF16(IDS_YSP_FLOAT_LOGIN_WINDOW_VIEW_TITLE))];
  else
    NOTREACHED();

  [delegate_ enterLockScreen];
  [lockViewController_ show];
  
}
- (void)unlockBrowser {
  [lockViewController_ hide];
  [delegate_ exitLockScreen];
}

- (void)showTokenError {
  NSString *errorMessage = base::SysUTF16ToNSString(l10n_util::GetStringUTF16(IDS_YSP_LOCK_BROWSER_REENTER_PASSWORD));
  [lockViewController_ setPromptMessage:errorMessage];
}
@end
