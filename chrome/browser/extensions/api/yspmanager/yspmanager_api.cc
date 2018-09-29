// YSP+

#include "chrome/browser/extensions/api/yspmanager/yspmanager_api.h"
#include "chrome/browser/ysp_login/ysp_login_manager.h"

namespace extensions {

namespace yspmanager = api::yspmanager;

YspmanagerSetPushDataFunction::YspmanagerSetPushDataFunction() {

}

ExtensionFunction::ResponseAction YspmanagerSetPushDataFunction::Run() {
  std::unique_ptr<yspmanager::SetPushData::Params>
      params(yspmanager::SetPushData::Params::Create(*args_));

  EXTENSION_FUNCTION_VALIDATE(params);
  YSPLoginManager::GetInstance()->SetPushData(params->push_data);
  DLOG(INFO) << "yspmanager userInfo: " << params->push_data;

  return ExtensionFunction::RespondNow(ExtensionFunction::NoArguments());
}

YspmanagerGetEnterplorerInfoFunction::YspmanagerGetEnterplorerInfoFunction() {

}

bool YspmanagerGetEnterplorerInfoFunction::RunAsync() {
  std::string info_data = YSPLoginManager::GetInstance()->GetLoginInfo();
  results_ = yspmanager::GetEnterplorerInfo::Results::Create(info_data);

  SendResponse(true);
  return true;
}

}  // namespace extensions
