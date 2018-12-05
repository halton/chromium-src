// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_EXTENSIONS_API_YSPMANAGER_YSP_MANAGER_API_H_
#define CHROME_BROWSER_EXTENSIONS_API_YSPMANAGER_YSP_MANAGER_API_H_

#include "chrome/browser/extensions/chrome_extension_function.h"

namespace extensions {

class YspmanagerSetPushDataFunction : public ChromeAsyncExtensionFunction {
  ~YspmanagerSetPushDataFunction() override{};
  bool RunAsync() override;
  DECLARE_EXTENSION_FUNCTION("yspmanager.setPushData", YSPMANAGER_SETPUSHDATA)
};

class YspmanagerGetEnterplorerInfoFunction
    : public ChromeAsyncExtensionFunction {
  ~YspmanagerGetEnterplorerInfoFunction() override{};
  bool RunAsync() override;
  DECLARE_EXTENSION_FUNCTION("yspmanager.getEnterplorerInfo",
                             YSPMANAGER_GETENTERPLORERINFO)
};

}  // namespace extensions

#endif  // CHROME_BROWSER_EXTENSIONS_API_YSPMANAGER_YSP_MANAGER_API_H_
