// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_EXTENSIONS_API_YSPLOGINMANAGER_YSPLOGINMANAGER_API_H_
#define CHROME_BROWSER_EXTENSIONS_API_YSPLOGINMANAGER_YSPLOGINMANAGER_API_H_

#include "chrome/browser/extensions/chrome_extension_function.h"
#include "chrome/common/extensions/api/yspmanager.h"
#include "extensions/browser/extension_function.h"

namespace extensions {

class YspmanagerSetPushDataFunction : public UIThreadExtensionFunction {
 public:
  YspmanagerSetPushDataFunction();

 protected:
  ~YspmanagerSetPushDataFunction() override {};
  ExtensionFunction::ResponseAction Run() override;

 private:
  DECLARE_EXTENSION_FUNCTION("yspmanager.setPushData",
                             YSPMANAGER_SETPUSHDATA)
};

class YspmanagerGetEnterplorerInfoFunction : public ChromeAsyncExtensionFunction {
 public:
  YspmanagerGetEnterplorerInfoFunction();

  protected:
  ~YspmanagerGetEnterplorerInfoFunction() override {}
  bool RunAsync() override;

private:
  DECLARE_EXTENSION_FUNCTION("yspmanager.getEnterplorerInfo",
                             YSPMANAGER_GETENTERPLORERINFO)
};

}  // namespace extensions

#endif  // CHROME_BROWSER_EXTENSIONS_API_YSPLOGINMANAGER_YSPLOGINMANAGER_API_H_
