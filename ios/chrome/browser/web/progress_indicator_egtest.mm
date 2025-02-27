// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import <EarlGrey/EarlGrey.h>

#include "base/mac/foundation_util.h"
#include "base/memory/ptr_util.h"
#include "base/run_loop.h"
#include "base/strings/stringprintf.h"
#include "base/synchronization/condition_variable.h"
#include "base/task_scheduler/post_task.h"
#import "base/test/ios/wait_util.h"
#include "base/threading/thread_restrictions.h"
#include "base/time/time.h"
#include "ios/chrome/browser/ui/ui_util.h"
#include "ios/chrome/test/app/navigation_test_util.h"
#include "ios/chrome/test/app/web_view_interaction_test_util.h"
#import "ios/chrome/test/earl_grey/chrome_earl_grey.h"
#import "ios/chrome/test/earl_grey/chrome_earl_grey_ui.h"
#import "ios/chrome/test/earl_grey/chrome_test_case.h"
#import "ios/third_party/material_components_ios/src/components/ProgressView/src/MaterialProgressView.h"
#include "ios/web/public/test/http_server/html_response_provider.h"
#import "ios/web/public/test/http_server/http_server.h"
#import "ios/web/public/test/http_server/http_server_util.h"
#include "url/gurl.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

namespace {

// Text to display in form page.
const char kFormPageText[] = "Form testing page";

// Text to display in infinite loading page.
const char kPageText[] = "Navigation testing page";

// Identifier of form to submit on form page.
const char kFormID[] = "testform";

// URL string for a form page.
const char kFormURL[] = "http://form";

// URL string for an infinite pending page.
const char kInfinitePendingPageURL[] = "http://infinite";

// URL string for a simple page containing |kPageText|.
const char kSimplePageURL[] = "http://simplepage";

// Matcher for progress view.
id<GREYMatcher> ProgressView() {
  return grey_kindOfClass([MDCProgressView class]);
}

// Matcher for the progress view with |progress|.
id<GREYMatcher> ProgressViewWithProgress(CGFloat progress) {
  MatchesBlock matches = ^BOOL(UIView* view) {
    MDCProgressView* progressView = base::mac::ObjCCast<MDCProgressView>(view);
    return progressView && progressView.progress == progress;
  };

  DescribeToBlock describe = ^(id<GREYDescription> description) {
    [description
        appendText:[NSString stringWithFormat:@"progress view with progress:%f",
                                              progress]];
  };

  return [[GREYElementMatcherBlock alloc] initWithMatchesBlock:matches
                                              descriptionBlock:describe];
}

// Response provider that serves the page which never finishes loading.
// TODO(crbug.com/708307): Convert this to Embedded Test Server.
class InfinitePendingResponseProvider : public HtmlResponseProvider {
 public:
  explicit InfinitePendingResponseProvider(const GURL& url)
      : url_(url),
        aborted_(false),
        terminated_(false),
        condition_variable_(&lock_) {}
  ~InfinitePendingResponseProvider() override {
    GREYAssert(terminated_, @"Request was not aborted.");
  }

  // Interrupt the current infinite request.
  // Must be called before the object is destroyed.
  void Abort() {
    {
      base::AutoLock auto_lock(lock_);
      aborted_.store(true, std::memory_order_release);
      condition_variable_.Signal();
    }

    base::test::ios::WaitUntilCondition(
        ^{
          base::AutoLock auto_lock(lock_);
          return terminated_.load(std::memory_order_release);
        },
        false, base::TimeDelta::FromSeconds(10));
  }

  // HtmlResponseProvider overrides:
  bool CanHandleRequest(const Request& request) override {
    return request.url == url_ ||
           request.url == GetInfinitePendingResponseUrl();
  }
  void GetResponseHeadersAndBody(
      const Request& request,
      scoped_refptr<net::HttpResponseHeaders>* headers,
      std::string* response_body) override {
    *headers = GetDefaultResponseHeaders();
    if (request.url == url_) {
      *response_body =
          base::StringPrintf("<p>%s</p><img src='%s'/>", kPageText,
                             GetInfinitePendingResponseUrl().spec().c_str());
    } else {
      *response_body = base::StringPrintf("<p>%s</p>", kPageText);
      {
        base::AutoLock auto_lock(lock_);
        while (!aborted_.load(std::memory_order_release))
          condition_variable_.Wait();
        terminated_.store(true, std::memory_order_release);
      }
    }
  }

 private:
  // Returns a url for which this response provider will never reply.
  GURL GetInfinitePendingResponseUrl() const {
    GURL::Replacements replacements;
    replacements.SetPathStr("resource");
    return url_.GetOrigin().ReplaceComponents(replacements);
  }

  // Main page URL that never finish loading.
  GURL url_;

  // Everything below is protected by lock_.
  mutable base::Lock lock_;
  std::atomic_bool aborted_;
  std::atomic_bool terminated_;
  base::ConditionVariable condition_variable_;
};

}  // namespace

// Tests webpage loading progress indicator.
@interface ProgressIndicatorTestCase : ChromeTestCase
@end

@implementation ProgressIndicatorTestCase

// Returns an HTML string for a form with the submission action set to
// |submitURL|.
- (std::string)formPageHTMLWithFormSubmitURL:(GURL)submitURL {
  return base::StringPrintf(
      "<p>%s</p><form id='%s' method='post' action='%s'>"
      "<input type='submit'></form>",
      kFormPageText, kFormID, submitURL.spec().c_str());
}

// Returns an HTML string for a form with a submit event that returns false.
- (std::string)formPageHTMLWithSuppressedSubmitEvent {
  return base::StringPrintf(
      "<p>%s</p><form id='%s' method='post' onsubmit='return false'>"
      "<input type='submit'></form>",
      kFormPageText, kFormID);
}

// Tests that the progress indicator is shown and has expected progress value
// for a simple two item page, and the toolbar is visible.
- (void)testProgressIndicatorShown {
  if (IsIPadIdiom()) {
    EARL_GREY_TEST_SKIPPED(@"Skipped for iPad (no progress view in tablet)");
  }

  // Load a page which never finishes loading.
  const GURL infinitePendingURL =
      web::test::HttpServer::MakeUrl(kInfinitePendingPageURL);
  auto uniqueInfinitePendingProvider =
      base::MakeUnique<InfinitePendingResponseProvider>(infinitePendingURL);
  InfinitePendingResponseProvider* infinitePendingProvider =
      uniqueInfinitePendingProvider.get();
  web::test::SetUpHttpServer(std::move(uniqueInfinitePendingProvider));
  // The page being loaded never completes, so call the LoadUrl helper that
  // does not wait for the page to complete loading.
  chrome_test_util::LoadUrl(infinitePendingURL);

  // Wait until the page is half loaded.
  [ChromeEarlGrey waitForWebViewContainingText:kPageText];

  // Verify progress view visible and halfway progress.
  [[EarlGrey selectElementWithMatcher:ProgressViewWithProgress(0.5)]
      assertWithMatcher:grey_sufficientlyVisible()];

  [ChromeEarlGreyUI waitForToolbarVisible:YES];
  infinitePendingProvider->Abort();
}

// Tests that the progress indicator is shown and has expected progress value
// after a form is submitted, and the toolbar is visible.
- (void)testProgressIndicatorShownOnFormSubmit {
  if (IsIPadIdiom()) {
    EARL_GREY_TEST_SKIPPED(@"Skipped for iPad (no progress view in tablet)");
  }

  const GURL formURL = web::test::HttpServer::MakeUrl(kFormURL);
  const GURL infinitePendingURL =
      web::test::HttpServer::MakeUrl(kInfinitePendingPageURL);

  // Create a page with a form to test.
  std::map<GURL, std::string> responses;
  responses[formURL] = [self formPageHTMLWithFormSubmitURL:infinitePendingURL];
  web::test::SetUpSimpleHttpServer(responses);

  // Add responseProvider for page that never finishes loading.
  auto uniqueInfinitePendingProvider =
      base::MakeUnique<InfinitePendingResponseProvider>(infinitePendingURL);
  InfinitePendingResponseProvider* infinitePendingProvider =
      uniqueInfinitePendingProvider.get();
  web::test::AddResponseProvider(std::move(uniqueInfinitePendingProvider));

  // Load form first.
  [ChromeEarlGrey loadURL:formURL];
  [ChromeEarlGrey waitForWebViewContainingText:kFormPageText];

  chrome_test_util::SubmitWebViewFormWithId(kFormID);

  // Wait until the page is half loaded.
  [ChromeEarlGrey waitForWebViewContainingText:kPageText];

  // Verify progress view visible and halfway progress.
  [[EarlGrey selectElementWithMatcher:ProgressViewWithProgress(0.5)]
      assertWithMatcher:grey_sufficientlyVisible()];

  [ChromeEarlGreyUI waitForToolbarVisible:YES];
  infinitePendingProvider->Abort();
}

// Tests that the progress indicator disappears after form has been submitted.
- (void)testProgressIndicatorDisappearsAfterFormSubmit {
  if (IsIPadIdiom()) {
    EARL_GREY_TEST_SKIPPED(@"Skipped for iPad (no progress view in tablet)");
  }

  const GURL formURL = web::test::HttpServer::MakeUrl(kFormURL);
  const GURL simplePageURL = web::test::HttpServer::MakeUrl(kSimplePageURL);

  // Create a page with a form to test.
  std::map<GURL, std::string> responses;
  responses[formURL] = [self formPageHTMLWithFormSubmitURL:simplePageURL];
  responses[simplePageURL] = kPageText;
  web::test::SetUpSimpleHttpServer(responses);

  [ChromeEarlGrey loadURL:formURL];

  [ChromeEarlGrey waitForWebViewContainingText:kFormPageText];

  chrome_test_util::SubmitWebViewFormWithId(kFormID);

  // Verify the new page has been loaded.
  [ChromeEarlGrey waitForWebViewContainingText:kPageText];

  // Verify progress view is not visible.
  [[EarlGrey selectElementWithMatcher:ProgressView()]
      assertWithMatcher:grey_notVisible()];
}

// Tests that the progress indicator disappears after form post attempt with a
// submit event that returns false.
- (void)testProgressIndicatorDisappearsAfterSuppressedFormPost {
  if (IsIPadIdiom()) {
    EARL_GREY_TEST_SKIPPED(@"Skipped for iPad (no progress view in tablet)");
  }

  // Create a page with a form to test.
  const GURL formURL = web::test::HttpServer::MakeUrl(kFormURL);
  std::map<GURL, std::string> responses;
  responses[formURL] = [self formPageHTMLWithSuppressedSubmitEvent];
  web::test::SetUpSimpleHttpServer(responses);

  [ChromeEarlGrey loadURL:formURL];

  // Verify the form page has been loaded.
  [ChromeEarlGrey waitForWebViewContainingText:kFormPageText];

  chrome_test_util::SubmitWebViewFormWithId(kFormID);

  // Verify progress view is not visible.
  [[EarlGrey selectElementWithMatcher:grey_kindOfClass([MDCProgressView class])]
      assertWithMatcher:grey_notVisible()];
}

@end
