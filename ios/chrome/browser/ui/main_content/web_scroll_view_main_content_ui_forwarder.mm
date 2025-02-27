// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "ios/chrome/browser/ui/main_content/web_scroll_view_main_content_ui_forwarder.h"

#include "base/logging.h"
#include "base/memory/ptr_util.h"
#import "ios/chrome/browser/ui/main_content/main_content_ui_state.h"
#import "ios/chrome/browser/web_state_list/web_state_list.h"
#import "ios/chrome/browser/web_state_list/web_state_list_observer_bridge.h"
#import "ios/web/public/web_state/ui/crw_web_view_proxy.h"
#import "ios/web/public/web_state/ui/crw_web_view_scroll_view_proxy.h"
#import "ios/web/public/web_state/web_state.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

@interface WebScrollViewMainContentUIForwarder ()<
    CRWWebViewScrollViewProxyObserver,
    WebStateListObserving> {
  // The observer bridge.
  std::unique_ptr<WebStateListObserver> _bridge;
}

// The updater being driven by this object.
@property(nonatomic, readonly, strong) MainContentUIStateUpdater* updater;
// The WebStateList whose active WebState's scroll state is being forwaded.
@property(nonatomic, readonly) WebStateList* webStateList;
// The WebStateList's active WebState.
@property(nonatomic, assign) web::WebState* webState;
// The scroll view proxy whose scroll events are forwarded to |updater|.
@property(nonatomic, readonly, strong) CRWWebViewScrollViewProxy* proxy;
@end

@implementation WebScrollViewMainContentUIForwarder
@synthesize updater = _updater;
@synthesize webStateList = _webStateList;
@synthesize webState = _webState;
@synthesize proxy = _proxy;

- (instancetype)initWithUpdater:(MainContentUIStateUpdater*)updater
                   webStateList:(WebStateList*)webStateList {
  if (self = [super init]) {
    _updater = updater;
    DCHECK(_updater);
    _webStateList = webStateList;
    DCHECK(_webStateList);
    _bridge = base::MakeUnique<WebStateListObserverBridge>(self);
    _webStateList->AddObserver(_bridge.get());
    web::WebState* activeWebState = webStateList->GetActiveWebState();
    if (activeWebState) {
      _webState = activeWebState;
      _proxy = activeWebState->GetWebViewProxy().scrollViewProxy;
      [_proxy addObserver:self];
    }
  }
  return self;
}

- (void)dealloc {
  // |-disconnect| must be called before deallocation.
  DCHECK(!_bridge);
  DCHECK(!_webState);
  DCHECK(!_proxy);
}

#pragma mark Accessors

- (void)setWebState:(web::WebState*)webState {
  if (_webState == webState)
    return;
  _webState = webState;
  self.proxy =
      _webState ? _webState->GetWebViewProxy().scrollViewProxy : nullptr;
}

- (void)setProxy:(CRWWebViewScrollViewProxy*)proxy {
  if (_proxy == proxy)
    return;
  [_proxy removeObserver:self];
  _proxy = proxy;
  [_proxy addObserver:self];
}

#pragma mark Public

- (void)disconnect {
  self.webStateList->RemoveObserver(_bridge.get());
  _bridge = nullptr;
  self.webState = nullptr;
}

#pragma mark CRWWebViewScrollViewObserver

- (void)webViewScrollViewDidScroll:
    (CRWWebViewScrollViewProxy*)webViewScrollViewProxy {
  [self.updater scrollViewDidScrollToOffset:self.proxy.contentOffset];
}

- (void)webViewScrollViewWillBeginDragging:
    (CRWWebViewScrollViewProxy*)webViewScrollViewProxy {
  [self.updater
      scrollViewWillBeginDraggingWithGesture:self.proxy.panGestureRecognizer];
}

- (void)webViewScrollViewWillEndDragging:
            (CRWWebViewScrollViewProxy*)webViewScrollViewProxy
                            withVelocity:(CGPoint)velocity
                     targetContentOffset:(inout CGPoint*)targetContentOffset {
  [self.updater
      scrollViewDidEndDraggingWithGesture:self.proxy.panGestureRecognizer
                         residualVelocity:velocity];
}

- (void)webViewScrollViewDidEndDecelerating:
    (CRWWebViewScrollViewProxy*)webViewScrollViewProxy {
  [self.updater scrollViewDidEndDecelerating];
}

#pragma mark - WebStateListObserving

- (void)webStateList:(WebStateList*)webStateList
    didReplaceWebState:(web::WebState*)oldWebState
          withWebState:(web::WebState*)newWebState
               atIndex:(int)atIndex {
  if (newWebState == webStateList->GetActiveWebState())
    self.webState = newWebState;
}

- (void)webStateList:(WebStateList*)webStateList
    didChangeActiveWebState:(web::WebState*)newWebState
                oldWebState:(web::WebState*)oldWebState
                    atIndex:(int)atIndex
                     reason:(int)reason {
  self.webState = newWebState;
}

@end
