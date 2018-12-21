// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef REDCORE
#ifndef CHROME_BROWSER_UI_WEBUI_EP_NEWTAB_UI_H
#define CHROME_BROWSER_UI_WEBUI_EP_NEWTAB_UI_H
#pragma once

#include "base/macros.h"
#include "content/public/browser/web_ui_controller.h"
#include "content/public/browser/web_ui_message_handler.h"
#include "chrome/browser/ysp_login/ysp_login_manager.h"
#include "chrome/browser/ysp_login/ysp_iplookup_fetcher.h"

namespace base {
class ListValue;
class Value;
}

class EPNewTabUI : public content::WebUIController,
                   public YSPLoginManagerObserver,
                   public YSPIPLookupFetcherDelegate {
public:
  explicit EPNewTabUI(content::WebUI* web_ui);
  ~EPNewTabUI() override;

public:
  void GetParamString(const base::ListValue* args);
  //void OnLoginEventFired(EP_LOGIN_STATUS emStatus, std::string& strLoginInfo);
  void SetParam(const base::ListValue* args);
  void GetDeviceInfo(const base::ListValue* args);
  void GetCity(const base::ListValue* args);
  void Login(const base::ListValue* args);
  void Logout();

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

  // YSPIPLookupFetcherDelegate
  void OnIPLookupRequestFailure() override;
  void OnIPLookupResponseParseSuccess(
  std::unique_ptr<base::DictionaryValue> response_data) override;
  void OnIPLookupResponseParseFailure(const std::string& error) override;

private:
  void CallJsFunc(std::string& strFuncName, base::Value* pResult);
  void CallJsFunc2(std::string& strFuncName, base::Value* pResult1, base::Value* pResult2);

  YSPIPLookupFetcher* ip_fetcher_;
  bool is_login_;

  DISALLOW_COPY_AND_ASSIGN(EPNewTabUI);
};

class EPHomePageMessageHandler : public content::WebUIMessageHandler {
public:
  explicit EPHomePageMessageHandler(EPNewTabUI* pEpTabUI);
  ~EPHomePageMessageHandler() override;

  void RegisterMessages() override;
  //virtual void OnEpLoginEventFired(EP_LOGIN_STATUS emStatus, std::string& strLoginInfo);

private:
  void BindGetParamFun(const base::ListValue* args);
  void BindSetParamFun(const base::ListValue* args);
  void BindGetDeviceInfoFun(const base::ListValue* args);
  void BindGetCity(const base::ListValue* args);
  void BindLogin(const base::ListValue* args);
  void BindLogout(const base::ListValue* args);

private:
 EPNewTabUI* p_ep_tab_ui_;
 DISALLOW_COPY_AND_ASSIGN(EPHomePageMessageHandler);
};

#endif // CHROME_BROWSER_UI_WEBUI_EP_NEWTAB_UI_H
#endif
