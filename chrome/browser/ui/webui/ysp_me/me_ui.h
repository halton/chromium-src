// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_WEBUI_ME_UI_H
#define CHROME_BROWSER_UI_WEBUI_ME_UI_H

#include "base/macros.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/ysp_login/ysp_iplookup_fetcher.h"
#include "chrome/browser/ysp_login/ysp_login_manager.h"
#include "content/public/browser/web_ui_controller.h"
#include "content/public/browser/web_ui_message_handler.h"

namespace base {
class ListValue;
class Value;
}  // namespace base

class MeUI : public content::WebUIController, public YSPLoginManagerObserver {
 public:
  explicit MeUI(content::WebUI* web_ui);
  ~MeUI() override;

  void GetDeviceInfo(const base::ListValue* args);
  void Logout();
  void GetUserInfo(const base::ListValue* args);
  void ModifyPassword(const base::ListValue* args);
  void ModifyPINCode(const base::ListValue* args);
  void DeleteDevice(const base::ListValue* args);
  void CheckIfCanModifyPassword(const base::ListValue* args);
  void GetLockScreenTime(const base::ListValue* args);
  void UploadAvatar(const base::ListValue* args);

  // YSPLoginManagerObserver:
  void OnConfigDataUpdated(const std::string& type,
                           const std::string& data) override;
  void OnTokenStatusChanged(const std::string& type) override;
  void OnLoginRequestFailure(const std::string& error) override;
  void OnLoginResponseParseFailure(const std::string& error) override;
  void OnLoginFailure(const base::string16& message) override;
  void OnLoginSuccess(const base::string16& name,
                      const std::string& head_image_url) override;
  void OnLogout() override;

 private:
  bool IsDigitString(const base::string16& text);

  DISALLOW_COPY_AND_ASSIGN(MeUI);
};

class MeUIPageMessageHandler : public content::WebUIMessageHandler {
 public:
  explicit MeUIPageMessageHandler(MeUI* me);
  ~MeUIPageMessageHandler() override;

  void RegisterMessages() override;

 private:
  void BindGetDeviceInfoFun(const base::ListValue* args);
  void BindLogout(const base::ListValue* args);
  void BindGetUserInfo(const base::ListValue* args);
  void BindModifyPassword(const base::ListValue* args);
  void BindModifyPINCode(const base::ListValue* args);
  void BindDeleteDevice(const base::ListValue* args);
  void BindCheckIfCanModifyPassword(const base::ListValue* args);
  void BindGetLockScreenTime(const base::ListValue* args);
  void BindUploadAvatar(const base::ListValue* args);

 private:
  MeUI* me_ui_;
  DISALLOW_COPY_AND_ASSIGN(MeUIPageMessageHandler);
};

#endif  // CHROME_BROWSER_UI_WEBUI_ME_UI_H
