// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/webui/ysp_me/me_ui.h"

#include "base/bind.h"
#include "base/json/json_writer.h"
#include "base/values.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/url_constants.h"
#include "chrome/grit/browser_resources.h"
#include "chrome/grit/generated_resources.h"
#include "components/strings/grit/components_strings.h"
#include "content/public/browser/web_ui.h"
#include "content/public/browser/web_ui_data_source.h"
#include "crypto/ysp_crypto_encryption.h"
#include "ui/base/l10n/l10n_util.h"

namespace {
content::WebUIDataSource* CreateWebUIDataSource() {
  content::WebUIDataSource* html_source =
      content::WebUIDataSource::Create(chrome::kChromeUIMeUIHost);
  // html
  html_source->AddResourcePath("index.html", IDR_YSP_ME_INDEX_HTML);
  html_source->AddResourcePath("device.js", IDR_YSP_ME_DEVICE_JS);
  html_source->AddResourcePath("index.css", IDR_YSP_ME_INDEX_CSS);
  html_source->AddResourcePath("index.js", IDR_YSP_ME_INDEX_JS);
  html_source->AddResourcePath("screenSet.js", IDR_YSP_ME_SCREENSET_JS);
  html_source->AddResourcePath("user.js", IDR_YSP_ME_USER_JS);
  html_source->AddResourcePath("jquery.js", IDR_YSP_ME_JQUERY_JS);
  html_source->AddResourcePath("rdc_util.js", IDR_YSP_ME_RDC_UTIL_JS);
  // imgs
  html_source->AddResourcePath("imgs/android.png", IDR_YSP_ME_IMGS_ANDROID_PNG);
  html_source->AddResourcePath("imgs/iPad.png", IDR_YSP_ME_IMGS_IPAD_PNG);
  html_source->AddResourcePath("imgs/iphone.png", IDR_YSP_ME_IMGS_IPHONE_PNG);
  html_source->AddResourcePath("imgs/linux.png", IDR_YSP_ME_IMGS_LINUX_PNG);
  html_source->AddResourcePath("imgs/lock_time_icon.png",
                               IDR_YSP_ME_IMGS_LOCK_TIME_ICON_PNG);
  html_source->AddResourcePath("imgs/mac.png", IDR_YSP_ME_IMGS_MAC_PNG);
  html_source->AddResourcePath("imgs/macbook.png", IDR_YSP_ME_IMGS_MACBOOK_PNG);
  html_source->AddResourcePath("imgs/not_login.png",
                               IDR_YSP_ME_IMGS_NOT_LOGIN_PNG);
  html_source->AddResourcePath("imgs/pad_android.png",
                               IDR_YSP_ME_IMGS_PAD_ANDROID_PNG);
  html_source->AddResourcePath("imgs/pin_modify.png",
                               IDR_YSP_ME_IMGS_PIN_MODIFY_PNG);
  html_source->AddResourcePath("imgs/upload_icon.png",
                               IDR_YSP_ME_IMGS_UPLOAD_ICON_PNG);
  html_source->AddResourcePath("imgs/windows_pc.png",
                               IDR_YSP_ME_IMGS_WINDOWS_PC_PNG);
  html_source->AddResourcePath("imgs/windowsbook.png",
                               IDR_YSP_ME_IMGS_WINDOWSBOOK_PNG);
  html_source->AddResourcePath("imgs/unknow.png",
                               IDR_YSP_ME_IMGS_UNKNOW_PNG);

  html_source->SetDefaultResource(IDR_YSP_ME_INDEX_HTML);
  html_source->SetJsonPath("strings.js");
  return html_source;
}
}  // namespace

MeUI::MeUI(content::WebUI* web_ui) : content::WebUIController(web_ui) {
  auto handler = std::make_unique<MeUIPageMessageHandler>(this);
  web_ui->AddMessageHandler(std::move(handler));

  web_ui->OverrideTitle(l10n_util::GetStringUTF16(IDS_USER_INFO));

  Profile* profile = Profile::FromWebUI(web_ui);
  content::WebUIDataSource::Add(profile, CreateWebUIDataSource());

  YSPLoginManager::GetInstance()->AddObserver(this);
}

MeUI::~MeUI() {
  YSPLoginManager::GetInstance()->RemoveObserver(this);
}

void MeUI::GetDeviceInfo(const base::ListValue* args) {
  YSPLoginManager::GetInstance()->GetSdpDevicefetcher(true);
}

void MeUI::Logout() {
  YSPLoginManager::GetInstance()->Logout();
}

void MeUI::GetUserInfo(const base::ListValue* args) {
  std::string str_user_info =
      YSPLoginManager::GetInstance()->generateUserInfoForSettings();

  base::Value user_info(str_user_info);
  std::string str_function_name = "User.getUserInfoFinish";
  web_ui()->CallJavascriptFunctionUnsafe(str_function_name, user_info);
}

void MeUI::ModifyPassword(const base::ListValue* args) {
  std::string old_password;
  std::string new_password;
  args->GetString(0, &old_password);
  args->GetString(1, &new_password);
  if (old_password.empty() || new_password.empty()) return;

  YSPLoginManager::GetInstance()->ModifyPassword(old_password, new_password);
}

void MeUI::CheckIfCanModifyPassword(const base::ListValue* args) {
  int source = YSPLoginManager::GetInstance()->GetAccountSourceType();
  base::Value param(source == 4 ? "false" : "true");
  web_ui()->CallJavascriptFunctionUnsafe("User.checkIfCanModifyPasswordFinish",
                                         param);
}

void MeUI::ModifyPINCode(const base::ListValue* args) {
  std::string str_param1 = "";
  std::string str_param2 = "";
  std::string str_param3 = "";
  if (!args || !args->GetString(0, &str_param1) ||
      !args->GetString(1, &str_param2) || !args->GetString(2, &str_param3)) {
    base::Value param(1);
    web_ui()->CallJavascriptFunctionUnsafe("User.modifyPINCodeFinish", param);
    return;
  }
  std::string input_pin =
      YSPLoginManager::GetInstance()->SHA256HashString(str_param1);
  std::string origin_pin_code = YSPLoginManager::GetInstance()->GetUserPinKey();
  if (origin_pin_code != input_pin) {
    base::Value param(2);
    web_ui()->CallJavascriptFunctionUnsafe("User.modifyPINCodeFinish", param);
    return;
  }
  const base::string16 pin_text = base::UTF8ToUTF16(str_param2);
  const base::string16 confirm_pin_text = base::UTF8ToUTF16(str_param3);
  if (!IsDigitString(pin_text) || !IsDigitString(confirm_pin_text) ||
      pin_text.length() != 6 || confirm_pin_text.length() != 6) {
    base::Value param(4);
    web_ui()->CallJavascriptFunctionUnsafe("User.modifyPINCodeFinish", param);
    return;
  }
  if (pin_text != confirm_pin_text) {
    base::Value param(3);
    web_ui()->CallJavascriptFunctionUnsafe("User.modifyPINCodeFinish", param);
    return;
  }
  std::string new_enc_pin_code =
      YSPLoginManager::GetInstance()->SHA256HashString(
          base::UTF16ToUTF8(pin_text));
  if (new_enc_pin_code == origin_pin_code) {
    base::Value param(5);
    web_ui()->CallJavascriptFunctionUnsafe("User.modifyPINCodeFinish", param);
    return;
  }
  YSPLoginManager::GetInstance()->UpdatePinKey(base::UTF16ToUTF8(pin_text));
  base::Value param(0);
  web_ui()->CallJavascriptFunctionUnsafe("User.modifyPINCodeFinish", param);
}

bool MeUI::IsDigitString(const base::string16& text) {
  for (size_t i = 0; i < text.length(); i++) {
    if (!(text[i] >= '0' && text[i] <= '9'))
      return false;
  }
  return true;
}

void MeUI::DeleteDevice(const base::ListValue* args) {
  std::string device_id = "";
  args->GetString(0, &device_id);
  YSPLoginManager::GetInstance()->PutSdpDevicefetcher(device_id, true);
}

// YSPLoginManagerObserver:
void MeUI::OnLoginRequestFailure(const std::string& error) {}

void MeUI::OnLoginResponseParseFailure(const std::string& error) {}

void MeUI::OnLoginFailure(const base::string16& message) {}

void MeUI::OnLoginSuccess(const base::string16& name,
                          const std::string& head_image_url) {
  base::Value user_info(
      YSPLoginManager::GetInstance()->generateUserInfoForSettings());
  std::string str_function_name = "User.getUserInfoFinish";
  web_ui()->CallJavascriptFunctionUnsafe(str_function_name, user_info);
  YSPLoginManager::GetInstance()->GetSdpDevicefetcher(true);
}

void MeUI::OnLogout() {
  base::Value value("");
  web_ui()->CallJavascriptFunctionUnsafe("User.logoutFinish", value);
}

void MeUI::OnTokenStatusChanged(const std::string& type) {}

void MeUI::OnConfigDataUpdated(const std::string& type,
                               const std::string& data) {
  base::Value value(data);
  if (type == "deleteLoginDevice") {
    web_ui()->CallJavascriptFunctionUnsafe("User.deleteDeviceFinish", value);
  }
  if (type == "getLoginDeviceInfo") {
    web_ui()->CallJavascriptFunctionUnsafe("User.getDeviceInfoFinish", value);
  }
  if (type == "modifyPassword") {
    web_ui()->CallJavascriptFunctionUnsafe("User.modifyPasswordFinish", value);
  }
  if (type == "pc") {
    GetLockScreenTime();
  }
}

void MeUI::GetLockScreenTime() {
  int lock_screen_time = YSPLoginManager::GetInstance()->GetLockScreenTime();
  if (lock_screen_time == 0) {
    lock_screen_time = 10;
  } else {
    lock_screen_time = lock_screen_time / 60;
  }
  base::Value command_value(lock_screen_time);
  web_ui()->CallJavascriptFunctionUnsafe("User.getLockScreenTimeFinish",
                                         command_value);
}

void MeUI::UploadAvatar(const base::ListValue* args) {
  std::string file_name;
  args->GetString(0, &file_name);
  std::string avatar_data;
  args->GetString(1, &avatar_data);
  YSPLoginManager::GetInstance()->UploadAvatar(file_name, avatar_data);
}

MeUIPageMessageHandler::MeUIPageMessageHandler(MeUI* me_ui) : me_ui_(me_ui) {}

MeUIPageMessageHandler::~MeUIPageMessageHandler() {}

void MeUIPageMessageHandler::BindGetDeviceInfoFun(const base::ListValue* args) {
  if (!me_ui_) return;

  me_ui_->GetDeviceInfo(args);
}

void MeUIPageMessageHandler::BindLogout(const base::ListValue* args) {
  if (!me_ui_) return;

  me_ui_->Logout();
}

void MeUIPageMessageHandler::BindGetUserInfo(const base::ListValue* args) {
  if (!me_ui_) return;

  me_ui_->GetUserInfo(args);
}

void MeUIPageMessageHandler::BindModifyPassword(const base::ListValue* args) {
  if (!me_ui_) return;

  me_ui_->ModifyPassword(args);
}

void MeUIPageMessageHandler::BindModifyPINCode(const base::ListValue* args) {
  if (!me_ui_) return;

  me_ui_->ModifyPINCode(args);
}

void MeUIPageMessageHandler::BindDeleteDevice(const base::ListValue* args) {
  if (!me_ui_) return;

  me_ui_->DeleteDevice(args);
}

void MeUIPageMessageHandler::BindCheckIfCanModifyPassword(
    const base::ListValue* args) {
  if (!me_ui_) return;

  me_ui_->CheckIfCanModifyPassword(args);
}

void MeUIPageMessageHandler::BindGetLockScreenTime(
    const base::ListValue* args) {
  if (!me_ui_) return;

  me_ui_->GetLockScreenTime();
}

void MeUIPageMessageHandler::BindUploadAvatar(const base::ListValue* args) {
  if (!me_ui_) return;

  me_ui_->UploadAvatar(args);
}

void MeUIPageMessageHandler::RegisterMessages() {
  web_ui()->RegisterMessageCallback(
      "getDeviceInfo", base::Bind(&MeUIPageMessageHandler::BindGetDeviceInfoFun,
                                  base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "logout",
      base::Bind(&MeUIPageMessageHandler::BindLogout, base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "getUserInfo", base::Bind(&MeUIPageMessageHandler::BindGetUserInfo,
                                base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "modifyPassword", base::Bind(&MeUIPageMessageHandler::BindModifyPassword,
                                   base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "modifyPINCode", base::Bind(&MeUIPageMessageHandler::BindModifyPINCode,
                                  base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "deleteDevice", base::Bind(&MeUIPageMessageHandler::BindDeleteDevice,
                                 base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "checkIfCanModifyPassword",
      base::Bind(&MeUIPageMessageHandler::BindCheckIfCanModifyPassword,
                 base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "getLockScreenTime",
      base::Bind(&MeUIPageMessageHandler::BindGetLockScreenTime,
                 base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "uploadAvatar", base::Bind(&MeUIPageMessageHandler::BindUploadAvatar,
                                 base::Unretained(this)));
}
