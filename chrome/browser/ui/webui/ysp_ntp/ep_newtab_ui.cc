// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef REDCORE

#include "chrome/browser/ui/webui/ysp_ntp/ep_newtab_ui.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/url_constants.h"
#include "content/public/browser/web_ui_data_source.h"
#include "chrome/grit/browser_resources.h"
#include "chrome/grit/generated_resources.h"
#include "ui/base/l10n/l10n_util.h"
#include "content/public/browser/web_ui.h"
#include "base/values.h"
#include "base/bind.h"
#include "components/strings/grit/components_strings.h"
#include "chrome/browser/browser_process.h"
#include "base/json/json_writer.h"

namespace {
  content::WebUIDataSource* CreateWebUIDataSource() {
    content::WebUIDataSource* html_source =
        content::WebUIDataSource::Create(chrome::kChromeUINewTabHost);

    html_source->AddResourcePath("Home.html", IDR_REDCORE_NEWTAB_HOME_HTML);
    html_source->AddResourcePath("Home.js", IDR_REDCORE_NEWTAB_HOME_JS);
    html_source->AddResourcePath("HomeBrowser.js", IDR_REDCORE_NEWTAB_HOMEBROWSER_JS);
    html_source->AddResourcePath("HomeProxy.js", IDR_REDCORE_NEWTAB_HOMEPROXY_JS);
    html_source->AddResourcePath("login.js", IDR_REDCORE_NEWTAB_LOGIN_JS);
    html_source->AddResourcePath("search.js", IDR_REDCORE_NEWTAB_SEARCH_JS);
    html_source->AddResourcePath("qrcode.min.js", IDR_REDCORE_NEWTAB_QRCODE_MIN_JS);
    html_source->AddResourcePath("generateHeader.js", IDR_REDCORE_NEWTAB_GENERATEHEADER_JS);
    html_source->AddResourcePath("generateTab.js", IDR_REDCORE_NEWTAB_GENERATETAB_JS);
    html_source->AddResourcePath("amazeui.min.js", IDR_REDCORE_NEWTAB_AMAZEUI_MIN_JS);
    html_source->AddResourcePath("jquery.js", IDR_REDCORE_NEWTAB_JQUERY_JS);
    html_source->AddResourcePath("amazeui.min.css", IDR_REDCORE_NEWTAB_AMAZEUI_MIN_CSS);
    html_source->AddResourcePath("styles.css", IDR_REDCORE_NEWTAB_STYLES_CSS);
    /*/imgs { */
		html_source->AddResourcePath("imgs/add_icon.svg", IDR_REDCORE_NEWTAB_ADD_ICON_SVG);
		html_source->AddResourcePath("imgs/app_back_icon.svg", IDR_REDCORE_NEWTAB_APP_BACK_ICON_SVG);
		html_source->AddResourcePath("imgs/appearance_hover_icon.png", IDR_REDCORE_NEWTAB_APPEARANCE_HOVER_ICON_PNG);
		html_source->AddResourcePath("imgs/appearancel_icon.png", IDR_REDCORE_NEWTAB_APPEARANCE_ICON_PNG);
		html_source->AddResourcePath("imgs/back_hover_icon.png", IDR_REDCORE_NEWTAB_BACK_HOVER_ICON_PNG);
		html_source->AddResourcePath("imgs/back_normal_icon.png", IDR_REDCORE_NEWTAB_BACK_NORMAL_ICON_PNG);
		html_source->AddResourcePath("imgs/close_icon.png", IDR_REDCORE_NEWTAB_CLOSE_ICON_PNG);
		html_source->AddResourcePath("imgs/defaultIcon.png", IDR_REDCORE_NEWTAB_DEFAULTICON_PNG);
		html_source->AddResourcePath("imgs/defaultLogo.png", IDR_REDCORE_NEWTAB_DEFAULTLOGO_PNG);
		html_source->AddResourcePath("imgs/error_icon.png", IDR_REDCORE_NEWTAB_ERROR_ICON_PNG);
		html_source->AddResourcePath("imgs/invalid.png", IDR_REDCORE_NEWTAB_INVALID_PNG);
		html_source->AddResourcePath("imgs/load_failure_icon.png", IDR_REDCORE_NEWTAB_LOAD_FAILURE_ICON_PNG);
		html_source->AddResourcePath("imgs/loading.png", IDR_REDCORE_NEWTAB_LOADING_PNG);
		html_source->AddResourcePath("imgs/search_icon.png", IDR_REDCORE_NEWTAB_SEARCH_ICON_PNG);
		html_source->AddResourcePath("imgs/selected_icon.png", IDR_REDCORE_NEWTAB_SELECTED_ICON_PNG);
		html_source->AddResourcePath("imgs/selectIcon.png", IDR_REDCORE_NEWTAB_SELECTICON_PNG);
		/*/imgs } */
  /*/imgs/background { */
 		html_source->AddResourcePath("imgs/background/bg-1.png", IDR_REDCORE_NEWTAB_IMGS_BACKGROUND_BG_1_PNG);
 		html_source->AddResourcePath("imgs/background/bg-2.png", IDR_REDCORE_NEWTAB_IMGS_BACKGROUND_BG_2_PNG);
 		html_source->AddResourcePath("imgs/background/bg-3.png", IDR_REDCORE_NEWTAB_IMGS_BACKGROUND_BG_3_PNG);
 		html_source->AddResourcePath("imgs/background/bg-4.png", IDR_REDCORE_NEWTAB_IMGS_BACKGROUND_BG_4_PNG);
 		html_source->AddResourcePath("imgs/background/bg-5.png", IDR_REDCORE_NEWTAB_IMGS_BACKGROUND_BG_5_PNG);
 		html_source->AddResourcePath("imgs/background/bg-6.png", IDR_REDCORE_NEWTAB_IMGS_BACKGROUND_BG_6_PNG);
 		html_source->AddResourcePath("imgs/background/bg-7.png", IDR_REDCORE_NEWTAB_IMGS_BACKGROUND_BG_7_PNG);
 		html_source->AddResourcePath("imgs/background/bg-8.png", IDR_REDCORE_NEWTAB_IMGS_BACKGROUND_BG_8_PNG);
 		html_source->AddResourcePath("imgs/background/abb-1.png", IDR_REDCORE_NEWTAB_IMGS_BACKGROUND_ABB_1_PNG);
 		html_source->AddResourcePath("imgs/background/abb-2.png", IDR_REDCORE_NEWTAB_IMGS_BACKGROUND_ABB_2_PNG);
 		html_source->AddResourcePath("imgs/background/abb-3.png", IDR_REDCORE_NEWTAB_IMGS_BACKGROUND_ABB_3_PNG);
 		html_source->AddResourcePath("imgs/background/abb-4.png", IDR_REDCORE_NEWTAB_IMGS_BACKGROUND_ABB_4_PNG);
 		html_source->AddResourcePath("imgs/background/abb-5.png", IDR_REDCORE_NEWTAB_IMGS_BACKGROUND_ABB_5_PNG);
 		html_source->AddResourcePath("imgs/background/abb-6.png", IDR_REDCORE_NEWTAB_IMGS_BACKGROUND_ABB_6_PNG);
 		html_source->AddResourcePath("imgs/background/abb-7.png", IDR_REDCORE_NEWTAB_IMGS_BACKGROUND_ABB_7_PNG);
 		html_source->AddResourcePath("imgs/background/abb-8.png", IDR_REDCORE_NEWTAB_IMGS_BACKGROUND_ABB_8_PNG);
 		/*/imgs/background } */
    /*/imgs/login { */
    html_source->AddResourcePath("imgs/login/backIcon.png", IDR_REDCORE_NEWTAB_BACKICON_PNG);
    html_source->AddResourcePath("imgs/login/back-user.png", IDR_REDCORE_NEWTAB_BACKUSER_PNG);
    		html_source->AddResourcePath("imgs/login/home_index.png", IDR_REDCORE_NEWTAB_HOME_INDEX_PNG);
		html_source->AddResourcePath("imgs/login/password_icon.png", IDR_REDCORE_NEWTAB_PASSWORD_ICON_PNG);
    html_source->AddResourcePath("imgs/login/Group.png", IDR_REDCORE_NEWTAB_GROUP_PNG);
    html_source->AddResourcePath("imgs/login/qrcode.png", IDR_REDCORE_NEWTAB_QRCODE_PNG);
    html_source->AddResourcePath("imgs/login/qrcodeSuccess.png", IDR_REDCORE_NEWTAB_QRCODESUCCESS_PNG);
    html_source->AddResourcePath("imgs/login/redcoreLogo.png", IDR_REDCORE_NEWTAB_REDCORELOGO_PNG);
    html_source->AddResourcePath("imgs/login/scan.png", IDR_REDCORE_NEWTAB_SCAN_PNG);
    html_source->AddResourcePath("imgs/login/user_icon.png", IDR_REDCORE_NEWTAB_USER_ICON_PNG);

    /*/imgs/login { */
    html_source->AddResourcePath("hello/motion.css", IDR_REDCORE_NEWTAB_HELLO_MOTION_CSS);
    html_source->AddResourcePath("hello/index.css", IDR_REDCORE_NEWTAB_HELLO_INDEX_CSS);
    html_source->AddResourcePath("hello/swiper.min.css", IDR_REDCORE_NEWTAB_HELLO_SWIPER_MIN_CSS);
    html_source->AddResourcePath("hello/swiper.min.js", IDR_REDCORE_NEWTAB_HELLO_SWIPER_MIN_JS);
    html_source->AddResourcePath("hello/bg.png", IDR_REDCORE_NEWTAB_HELLO_IMG_BG_PNG);
    html_source->AddResourcePath("hello/logo.png", IDR_REDCORE_NEWTAB_HELLO_IMG_LOGO_PNG);
    html_source->AddResourcePath("hello/round.png", IDR_REDCORE_NEWTAB_HELLO_IMG_ROUND_PNG);
    html_source->AddResourcePath("hello/up.png", IDR_REDCORE_NEWTAB_HELLO_IMG_UP_PNG);
    html_source->AddResourcePath("hello/up-red.png", IDR_REDCORE_NEWTAB_HELLO_IMG_UP_RED_PNG);
    html_source->AddResourcePath("hello/img/swiper1/001.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER1_001_PNG);
    html_source->AddResourcePath("hello/img/swiper1/1.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER1_1_PNG);
    html_source->AddResourcePath("hello/img/swiper1/2.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER1_2_PNG);
    html_source->AddResourcePath("hello/img/swiper1/3.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER1_3_PNG);
    html_source->AddResourcePath("hello/img/swiper1/4.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER1_4_PNG);
    html_source->AddResourcePath("hello/img/swiper1/5.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER1_5_PNG);
    html_source->AddResourcePath("hello/img/swiper1/6.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER1_6_PNG);
    html_source->AddResourcePath("hello/img/swiper1/7.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER1_7_PNG);
    html_source->AddResourcePath("hello/img/swiper1/8.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER1_8_PNG);
    html_source->AddResourcePath("hello/img/swiper1/19.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER1_19_PNG);
    html_source->AddResourcePath("hello/img/swiper1/888.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER1_888_PNG);
    html_source->AddResourcePath("hello/img/swiper2/1.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER2_1_PNG);
    html_source->AddResourcePath("hello/img/swiper2/2.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER2_2_PNG);
    html_source->AddResourcePath("hello/img/swiper2/2-2.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER2_2_2_PNG);
    html_source->AddResourcePath("hello/img/swiper2/3.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER2_3_PNG);
    html_source->AddResourcePath("hello/img/swiper2/3-3.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER2_3_3_PNG);
    html_source->AddResourcePath("hello/img/swiper2/4.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER2_4_PNG);
    html_source->AddResourcePath("hello/img/swiper2/4-4.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER2_4_4_PNG);
    html_source->AddResourcePath("hello/img/swiper2/5.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER2_5_PNG);
    html_source->AddResourcePath("hello/img/swiper2/6.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER2_6_PNG);
    html_source->AddResourcePath("hello/img/swiper3/7.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER3_7_PNG);
    html_source->AddResourcePath("hello/img/swiper3/8.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER3_8_PNG);
    html_source->AddResourcePath("hello/img/swiper3/9.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER3_9_PNG);
    html_source->AddResourcePath("hello/img/swiper3/10.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER3_10_PNG);
    html_source->AddResourcePath("hello/img/swiper3/11.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER3_11_PNG);
    html_source->AddResourcePath("hello/img/swiper3/12.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER3_12_PNG);
    html_source->AddResourcePath("hello/img/swiper3/13.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER3_13_PNG);
    html_source->AddResourcePath("hello/img/swiper3/21.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER3_21_PNG);
    html_source->AddResourcePath("hello/img/swiper4/5.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER4_5_PNG);
    html_source->AddResourcePath("hello/img/swiper4/6.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER4_6_PNG);
    html_source->AddResourcePath("hello/img/swiper4/15.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER4_15_PNG);
    html_source->AddResourcePath("hello/img/swiper4/16.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER4_16_PNG);
    html_source->AddResourcePath("hello/img/swiper4/17.png", IDR_REDCORE_NEWTAB_HELLO_IMG_SWIPER4_17_PNG);
    html_source->SetDefaultResource(IDR_REDCORE_NEWTAB_HOME_HTML);
    html_source->SetJsonPath("strings.js");
    return html_source;
  }
}

EPNewTabUI::EPNewTabUI(content::WebUI* web_ui)
  : content::WebUIController(web_ui),
    ip_fetcher_(nullptr),
    is_login_(false) {
  auto handler =
      std::make_unique<EPHomePageMessageHandler>(this);
  web_ui->AddMessageHandler(std::move(handler));

  web_ui->OverrideTitle(l10n_util::GetStringUTF16(IDS_NEW_TAB_TITLE));

  Profile* profile = Profile::FromWebUI(web_ui);
  content::WebUIDataSource::Add(profile, CreateWebUIDataSource());

  YSPLoginManager::GetInstance()->AddObserver(this);
}

EPNewTabUI::~EPNewTabUI() {
  YSPLoginManager::GetInstance()->RemoveObserver(this);
  if(ip_fetcher_) {
    delete ip_fetcher_;
    ip_fetcher_ = nullptr;
  }
}

void EPNewTabUI::GetParamString(const base::ListValue* args) {
  DLOG(INFO) << "EPNewTabUI::GetParamString";
  std::string str_param = "";
  if (!args || !args->GetString(0, &str_param)) {
    return;
  }

  if (str_param.compare("loggingStatus") == 0 && is_login_) {
    YSPLoginManager::GetInstance()->GetUserfetcher();
  }

  std::string str_result =
      YSPLoginManager::GetInstance()->GetValueForKey(str_param);
  DLOG(INFO) << "EPNewTabUI::GetParamString: " << str_param
             << ", return: " << str_result;

  base::Value key(str_param);
  base::Value value(str_result);
  std::string str_function_name = "HOMEBROWSER.getParamCallback";
  web_ui()->CallJavascriptFunctionUnsafe(str_function_name, key, value);
}

void EPNewTabUI::SetParam(const base::ListValue* args) {
  std::string str_param1 = "";
  if (!args || !args->GetString(0, &str_param1)) {
    return;
  }

  if (str_param1.compare("initialized") == 0) {
    DLOG(INFO) << "EPNewTabUI::SetParam: " << str_param1;
    YSPLoginManager::GetInstance()->set_web_initialized(true);
    return;
  } else if (str_param1.compare("autoLogin") == 0) {
    bool b_param2 = false;
    if (!args->GetBoolean(1, &b_param2))
      return;
    DLOG(INFO) << "EPNewTabUI::SetParam: " << str_param1 << " = " << b_param2;
    YSPLoginManager::GetInstance()->set_auto_login(b_param2);
  } else if (str_param1.compare("managerAddress") == 0) {
    std::string b_param2 = "";
    if (!args->GetString(1, &b_param2))
      return;
    DLOG(INFO) << "EPNewTabUI::SetParam: " << str_param1 << " = " << b_param2;
    YSPLoginManager::GetInstance()->SetManageServer(b_param2);
  } else if (str_param1.compare("setApplicationStatus") == 0) {
    std::string application_id = "";
    bool appliction_status = false;
    if (!args->GetString(1, &application_id) ||
        !args->GetBoolean(2, &appliction_status))
      return;
    DLOG(INFO) << "EPNewTabUI::SetParam: " << str_param1 << " = "
               << application_id << " appliction_status" << appliction_status;
    YSPLoginManager::GetInstance()->PutApplictionStatusFetch(application_id,
                                                             appliction_status);
        }

        base::Value key(str_param1);
        std::string str_function_name = "HOMEBROWSER.setParamCallback";
        web_ui()->CallJavascriptFunctionUnsafe(str_function_name, key);
}

void EPNewTabUI::GetDeviceInfo(const base::ListValue* args) {
  DLOG(INFO) << "EPNewTabUI::GetDeviceInfo";

  std::string str_device_info = YSPLoginManager::GetInstance()->GetDeviceInfo();

  base::Value device_info(str_device_info);
  std::string str_function_name = "HOMEBROWSER.getDeviceInfoCallback";
  web_ui()->CallJavascriptFunctionUnsafe(str_function_name, device_info);
}

void EPNewTabUI::GetCity(const base::ListValue* args) {
  DLOG(INFO) << "EPNewTabUI::GetCity";
  if (!ip_fetcher_) {
    ip_fetcher_ =
      new YSPIPLookupFetcher(this,
        g_browser_process->system_request_context());
  }
  if (ip_fetcher_)
    ip_fetcher_->Start();
}

void EPNewTabUI::Login(const base::ListValue* args) {
  DLOG(INFO) << "EPNewTabUI::Login";
  std::string str_domain = "";
  std::string str_email = "";
  std::string str_password = "";
  if (!args || !args->GetString(0, &str_domain) ||
      !args->GetString(1, &str_email) || !args->GetString(2, &str_password)) {
    return;
  }
  YSPLoginManager::GetInstance()->StartLogin(str_domain, str_email,
                                             str_password);
}

void EPNewTabUI::Logout() {
  DLOG(INFO) << "EPNewTabUI::Logout";
  YSPLoginManager::GetInstance()->Logout();
}

void EPNewTabUI::CallJsFunc(std::string& strFuncName, base::Value* pResult) {
  pResult ? (web_ui()->CallJavascriptFunctionUnsafe(strFuncName, *pResult)) : (web_ui()->CallJavascriptFunctionUnsafe(strFuncName));
}
void EPNewTabUI::CallJsFunc2(std::string& strFuncName, base::Value* pResult1, base::Value* pResult2) {
  web_ui()->CallJavascriptFunctionUnsafe(strFuncName, *pResult1, *pResult2);
}

// YSPLoginManagerObserver:
void EPNewTabUI::OnLoginRequestFailure(const std::string& error) {
  if (!YSPLoginManager::GetInstance()->get_web_initialized())
    return;
  DLOG(INFO) << "EPNewTabUI::OnLoginRequestFailure";

  std::string str_param = "loggingStatus";
  base::Value str(error);
  base::Value status(YSPLoginManager::GetInstance()->GetValueForKey(str_param));

  std::string str_function_name = "HOMEBROWSER.configDataChanged";
  CallJsFunc2(str_function_name, &status, &str);
}

void EPNewTabUI::OnLoginResponseParseFailure(const std::string& error) {
  if (!YSPLoginManager::GetInstance()->get_web_initialized())
    return;
  DLOG(INFO) << "EPNewTabUI::OnLoginResponseParseFailure";

  std::string str_param = "loggingStatus";
  base::Value str(error);
  base::Value status(YSPLoginManager::GetInstance()->GetValueForKey(str_param));

  std::string str_function_name = "HOMEBROWSER.configDataChanged";
  CallJsFunc2(str_function_name, &status, &str);
}

void EPNewTabUI::OnLoginFailure(base::string16 message) {
  if (!YSPLoginManager::GetInstance()->get_web_initialized())
    return;
  DLOG(INFO) << "EPNewTabUI::OnLoginFailure";
  is_login_ = false;

  std::string str_param = "loggingStatus";
  base::Value str(message);
  base::Value status(YSPLoginManager::GetInstance()->GetValueForKey(str_param));

  std::string str_function_name = "HOMEBROWSER.configDataChanged";
  CallJsFunc2(str_function_name, &status, &str);
}

void EPNewTabUI::OnLoginSuccess(const base::string16& name,
  const std::string& head_image_url) {
  if (!YSPLoginManager::GetInstance()->get_web_initialized())
    return;
  //if(is_login_)
  //  return;
  is_login_ = true;
  DLOG(INFO) << "EPNewTabUI::OnLoginSuccess";

  std::string str_param = "loggingStatus";
  base::Value str("login");
  base::Value status(YSPLoginManager::GetInstance()->GetValueForKey(str_param));

  std::string str_function_name = "HOMEBROWSER.configDataChanged";
  CallJsFunc2(str_function_name, &status, &str);
}

void EPNewTabUI::OnLogout() {
  DLOG(INFO) << "EPNewTabUI::OnLogout";
  is_login_ = false;
  if (!YSPLoginManager::GetInstance()->get_web_initialized())
    return;

  std::string str_param = "loggingStatus";
  base::Value str("logout");
  base::Value status(YSPLoginManager::GetInstance()->GetValueForKey(str_param));

  std::string str_function_name = "HOMEBROWSER.configDataChanged";
  CallJsFunc2(str_function_name, &status, &str);
}

void EPNewTabUI::OnConfigDataUpdated(const std::string& type,
  const std::string& data) {
  if (!YSPLoginManager::GetInstance()->get_web_initialized())
    return;

  DLOG(INFO) << "EPNewTabUI::OnConfigDataUpdated";
  base::Value msg(type);
  base::Value value(data);
  std::string str_function_name = "HOMEBROWSER.configDataChanged";
  CallJsFunc2(str_function_name, &value, &msg);
}

void EPNewTabUI::OnTokenStatusChanged(const std::string & type)
{
  if (type == "TokenAvailable")
  {
    is_login_ = true;
    std::string str_param = "loggingStatus";
    base::Value str("login");
    base::Value status(
        YSPLoginManager::GetInstance()->GetValueForKey(str_param));

    std::string str_function_name = "HOMEBROWSER.configDataChanged";
    CallJsFunc2(str_function_name, &status, &str);
  }
}

// YSPIPLookupFetcherDelegate
void EPNewTabUI::OnIPLookupRequestFailure() {
  DLOG(INFO) << "EPNewTabUI::OnIPLookupRequestFailure";

}

void EPNewTabUI::OnIPLookupResponseParseSuccess(
  std::unique_ptr<base::DictionaryValue> response_data) {
  if (!YSPLoginManager::GetInstance()->get_web_initialized())
    return;

  DLOG(INFO) << "EPNewTabUI::OnIPLookupResponseParseSuccess";

  std::string strCityInfo;
  base::JSONWriter::Write(*response_data, &strCityInfo);

  base::Value city_info(strCityInfo);
  std::string str_function_name = "HOMEBROWSER.getCityCallback";
  web_ui()->CallJavascriptFunctionUnsafe(str_function_name, city_info);
}

void EPNewTabUI::OnIPLookupResponseParseFailure(const std::string& error) {
  DLOG(INFO) << "EPNewTabUI::OnIPLookupResponseParseFailure";
}

/******************** EPHomePageMessageHandler *************************/
EPHomePageMessageHandler::EPHomePageMessageHandler(EPNewTabUI* pEpTabUI)
    : p_ep_tab_ui_(pEpTabUI) {}

EPHomePageMessageHandler::~EPHomePageMessageHandler() {
  //EpLoginManager::LoginMgrInstance()->RemoveLoginEventHandle(this);
}

void EPHomePageMessageHandler::BindGetParamFun(const base::ListValue* args) {
  if (!p_ep_tab_ui_ || !args) {
    return;
  }

  p_ep_tab_ui_->GetParamString(args);
}

void EPHomePageMessageHandler::BindGetDeviceInfoFun(const base::ListValue* args) {
  if (!p_ep_tab_ui_) {
    return;
  }
  p_ep_tab_ui_->GetDeviceInfo(args);
}

void EPHomePageMessageHandler::BindSetParamFun(const base::ListValue* args) {
  if (!p_ep_tab_ui_ || !args) {
    return;
  }
  p_ep_tab_ui_->SetParam(args);
}

void EPHomePageMessageHandler::BindGetCity(const base::ListValue* args) {
  if (!p_ep_tab_ui_ || !args) {
    return;
  }
  p_ep_tab_ui_->GetCity(args);
}

void EPHomePageMessageHandler::BindLogin(const base::ListValue* args) {
  if (!p_ep_tab_ui_ || !args) {
    return;
  }
  p_ep_tab_ui_->Login(args);
}

void EPHomePageMessageHandler::BindLogout(const base::ListValue* args) {
  if (!p_ep_tab_ui_) {
    return;
  }
  p_ep_tab_ui_->Logout();
}

void EPHomePageMessageHandler::RegisterMessages() {
  web_ui()->RegisterMessageCallback(
    "getParam",
    base::Bind(&EPHomePageMessageHandler::BindGetParamFun,
      base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
    "setParam",
    base::Bind(&EPHomePageMessageHandler::BindSetParamFun,
      base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
    "getDeviceInfo",
    base::Bind(&EPHomePageMessageHandler::BindGetDeviceInfoFun,
      base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
    "getCity",
    base::Bind(&EPHomePageMessageHandler::BindGetCity,
      base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
    "login",
    base::Bind(&EPHomePageMessageHandler::BindLogin,
      base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
    "logout",
    base::Bind(&EPHomePageMessageHandler::BindLogout,
      base::Unretained(this)));
}

/*
void EPHomePageMessageHandler::OnEpLoginEventFired(EP_LOGIN_STATUS emStatus,
std::string& strLoginInfo) { if (!p_ep_tab_ui_) { return;
  }

  p_ep_tab_ui_->OnLoginEventFired(emStatus, strLoginInfo);
}
*/
#endif
