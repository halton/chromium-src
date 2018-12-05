// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/extensions/api/yspmanager/yspmanager_api.h"
#include "chrome/browser/ysp_login/ysp_login_manager.h"
#include "chrome/common/extensions/api/yspmanager.h"

namespace extensions {

namespace yspmanager = extensions::api::yspmanager;

bool YspmanagerSetPushDataFunction::RunAsync() {
  std::unique_ptr<yspmanager::SetPushData::Params> params(
      yspmanager::SetPushData::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params);
  YSPLoginManager::GetInstance()->SetPushData(params->push_data);
  DLOG(INFO) << "yspmanager userInfo: " << params->push_data;
  return true;
}

bool YspmanagerGetEnterplorerInfoFunction::RunAsync() {
  std::string info_data = YSPLoginManager::GetInstance()->GetLoginInfo();
  results_ = yspmanager::GetEnterplorerInfo::Results::Create(info_data);
  SendResponse(true);
  return true;
}

}  // namespace extensions
