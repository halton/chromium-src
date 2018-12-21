// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef REDCORE
// ysp+ { show config }
#ifndef CHROME_BROWSER_UI_WEBUI_EP_SHOW_CONFIG_UI_H
#define CHROME_BROWSER_UI_WEBUI_EP_SHOW_CONFIG_UI_H
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

class EPShowConfigUI : public content::WebUIController,
                   public YSPLoginManagerObserver,
                   public YSPIPLookupFetcherDelegate {
public:
  explicit EPShowConfigUI(content::WebUI* web_ui);
  ~EPShowConfigUI() override;

public:
  void GetParamString(const base::ListValue* args);
  //void OnLoginEventFired(EP_LOGIN_STATUS emStatus, std::string& strLoginInfo);
  void SetParam(const base::ListValue* args);
  //void GetDeviceInfo(const base::ListValue* args);
 // void GetCity(const base::ListValue* args);

  // YSPLoginManagerObserver:
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

  YSPIPLookupFetcher* ip_fetcher_;
  bool is_login_;

  DISALLOW_COPY_AND_ASSIGN(EPShowConfigUI);
};

class EPShowConfigMessageHandler : public content::WebUIMessageHandler {
public:
  explicit EPShowConfigMessageHandler(EPShowConfigUI* pEpTabUI);
  ~EPShowConfigMessageHandler() override;

  void RegisterMessages() override;
  //virtual void OnEpLoginEventFired(EP_LOGIN_STATUS emStatus, std::string& strLoginInfo);

private:
  void BindFunction(const base::ListValue* args);
  void BindSetParamFun(const base::ListValue* args);
  //void BindGetDeviceInfoFun(const base::ListValue* args);
 // void BindGetCity(const base::ListValue* args);

private:
 EPShowConfigUI* p_ep_tab_ui_;
 DISALLOW_COPY_AND_ASSIGN(EPShowConfigMessageHandler);
};

#endif // CHROME_BROWSER_UI_WEBUI_EP_SHOW_CONFIG_UI_H
#endif
