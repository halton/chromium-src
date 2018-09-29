// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/extensions/api/web_navigation/web_navigation_api_constants.h"

namespace extensions {

namespace web_navigation_api_constants {

const char kErrorKey[] = "error";
const char kFrameIdKey[] = "frameId";
const char kParentFrameIdKey[] = "parentFrameId";
const char kProcessIdKey[] = "processId";
const char kReplacedTabIdKey[] = "replacedTabId";
const char kSourceFrameIdKey[] = "sourceFrameId";
const char kSourceProcessIdKey[] = "sourceProcessId";
const char kSourceTabIdKey[] = "sourceTabId";
const char kTabIdKey[] = "tabId";
const char kTimeStampKey[] = "timeStamp";
const char kTransitionTypeKey[] = "transitionType";
const char kTransitionQualifiersKey[] = "transitionQualifiers";
const char kUrlKey[] = "url";
#ifdef REDCORE
#ifdef IE_REDCORE
const char kUserName[] = "userName";
const char kUserPassword[] = "userPassword";
#endif /*IE_REDCORE*/
//TODO(matianzhi) ysp+{push server api}
const char kLoginStatusKey[] = "loginStatus";
const char kLoginManagerUrlKey[] = "managerServer";
const char kLoginDeviceIdKey[] = "deviceId";
const char kLoginUserIdKey[] = "userId";
const char kLoginCompanyIdKey[] = "companyId";
//YSP+ }
#endif /*REDCORE*/

}  // namespace web_navigation_api_constants

}  // namespace extensions
