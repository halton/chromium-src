// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_BROWSER_UI_BOOKMARKS_HOME_VIEW_CONTROLLER_H_
#define IOS_CHROME_BROWSER_UI_BOOKMARKS_HOME_VIEW_CONTROLLER_H_

#import <UIKit/UIKit.h>

#include <set>
#include <vector>

@protocol ApplicationCommands;
@protocol UrlLoader;
class GURL;

namespace ios {
class ChromeBrowserState;
}  // namespace ios

namespace bookmarks {
class BookmarkModelBridge;
class BookmarkNode;
}  // namespace bookmarks

@class BookmarkHomeViewController;

@protocol BookmarkHomeViewControllerDelegate
// The view controller wants to be dismissed. If |urls| is not empty, then
// the user has selected to navigate to those URLs in the current tab mode.
- (void)bookmarkHomeViewControllerWantsDismissal:
            (BookmarkHomeViewController*)controller
                                navigationToUrls:(const std::vector<GURL>&)urls;

// The view controller wants to be dismissed. If |urls| is not empty, then
// the user has selected to navigate to those URLs with specified tab mode.
- (void)bookmarkHomeViewControllerWantsDismissal:
            (BookmarkHomeViewController*)controller
                                navigationToUrls:(const std::vector<GURL>&)urls
                                     inIncognito:(BOOL)inIncognito
                                          newTab:(BOOL)newTab;

@end

// Class to navigate the bookmark hierarchy.
@interface BookmarkHomeViewController : UIViewController {
  // TODO(crbug.com/753599) : Move ivars declaration to the implement file when
  // removing bookmark_home_handset_view_controller.
 @protected
  // Bridge to register for bookmark changes.
  std::unique_ptr<bookmarks::BookmarkModelBridge> _bridge;

  // The root node, whose child nodes are shown in the bookmark table view.
  const bookmarks::BookmarkNode* _rootNode;
}

- (instancetype)initWithNibName:(NSString*)nibNameOrNil
                         bundle:(NSBundle*)nibBundleOrNil NS_UNAVAILABLE;
- (instancetype)initWithCoder:(NSCoder*)aDecoder NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithLoader:(id<UrlLoader>)loader
                  browserState:(ios::ChromeBrowserState*)browserState
                    dispatcher:(id<ApplicationCommands>)dispatcher
    NS_DESIGNATED_INITIALIZER;

// Set to YES, only when this view controller instance is being created
// from cached path. Once the view controller is shown, this is set to NO.
// This is so that the cache code is called only once in viewWillAppear, and
// not every time the view appears.
@property(nonatomic, assign) BOOL isReconstructingFromCache;

// Setter to set _rootNode value.
- (void)setRootNode:(const bookmarks::BookmarkNode*)rootNode;

// Delegate for presenters. Note that this delegate is currently being set only
// in case of handset, and not tablet. In the future it will be used by both
// cases.
@property(nonatomic, weak) id<BookmarkHomeViewControllerDelegate> homeDelegate;

// Dispatcher for sending commands.
@property(nonatomic, readonly, weak) id<ApplicationCommands> dispatcher;
@end

#endif  // IOS_CHROME_BROWSER_UI_BOOKMARKS_HOME_VIEW_CONTROLLER_H_
