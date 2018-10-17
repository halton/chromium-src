#ifdef REDCORE
//ysp+ { show config }
#include "chrome/browser/ui/webui/ysp_show_config/ep_show_config_ui.h"
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
        content::WebUIDataSource::Create(chrome::kChromeUIShowConfigHost);

    html_source->AddResourcePath("base.css", IDR_YSP_CONFIG_BASE_CSS);
    html_source->AddResourcePath("showConfig.html", IDR_YSP_CONFIG_SHOW_CONFIG_HTML);
    html_source->AddResourcePath("ShowBrowser.js", IDR_YSP_CONFIG_SHOW_BROWSER_JS);
    html_source->AddResourcePath("ShowData.js", IDR_YSP_CONFIG_SHOW_DATA_JS);
    html_source->AddResourcePath("jquery.json-2.4.js", IDR_YSP_CONFIG_JQUERY_JSON_JS);
    html_source->AddResourcePath("jquery.min.js", IDR_YSP_CONFIG_JQUERY_MIN_JS);
    html_source->AddResourcePath("show.js", IDR_YSP_CONFIG_SHOW_JS);
    html_source->SetDefaultResource(IDR_YSP_CONFIG_SHOW_CONFIG_HTML);

    html_source->SetJsonPath("strings.js");

    return html_source;
  }
}

EPShowConfigUI::EPShowConfigUI(content::WebUI* web_ui)
  : content::WebUIController(web_ui),
    ip_fetcher_(nullptr),
    is_login_(false) {

  auto handler =
      std::make_unique<EPShowConfigMessageHandler>(this);
  web_ui->AddMessageHandler(std::move(handler));

  web_ui->OverrideTitle(l10n_util::GetStringUTF16(IDS_SHOW_CONFIG_TITLE));

  Profile* profile = Profile::FromWebUI(web_ui);
  content::WebUIDataSource::Add(profile, CreateWebUIDataSource());

  YSPLoginManager::GetInstance()->AddObserver(this);
}

EPShowConfigUI::~EPShowConfigUI() {
  YSPLoginManager::GetInstance()->RemoveObserver(this);
  if(ip_fetcher_) {
    delete ip_fetcher_;
    ip_fetcher_ = nullptr;
  }
}

void EPShowConfigUI::GetParamString(const base::ListValue* args) {
  DLOG(INFO) << "EPNewTabUI::GetParamString";
  std::string strParam = "";
  if (!args || !args->GetString(0, &strParam)) {
    return;
  }

  std::string strResult =
    YSPLoginManager::GetInstance()->GetValueForKey(strParam);
  DLOG(INFO) << "EPNewTabUI::GetParamString: " << strParam << ", return: " << strResult;

  base::Value key(strParam);
  base::Value value(strResult);
  std::string strFunctionName = "SHOWBROWSER.getParamFinish";
  web_ui()->CallJavascriptFunctionUnsafe(strFunctionName, key, value);
}

void EPShowConfigUI::SetParam(const base::ListValue* args) {
  std::string strParam1 = "";
  std::string strParam2 = "";
  if (!args || !args->GetString(0, &strParam1) || !args->GetString(1, &strParam2)) {
    return;
  }

  //EpLoginManager::LoginMgrInstance()->SetLoginInfoToStorage(strParam1, strParam2);
  DLOG(INFO) << "EPShowConfigUI::SetParam: " << strParam1 << " = " << strParam2;

  base::Value key(strParam1);
  std::string strFunctionName = "SHOWBROWSER.setParamFinish";
  web_ui()->CallJavascriptFunctionUnsafe(strFunctionName, key);
}

//void EPShowConfigUI::GetDeviceInfo(const base::ListValue* args) {
//  DLOG(INFO) << "EPShowConfigUI::GetDeviceInfo";
//
//  std::string strDeviceInfo = YSPLoginManager::GetInstance()->GetDeviceInfo();
//
//  base::Value deviceInfo(strDeviceInfo);
//  std::string strFunctionName = "SHOWBROWSER.getDeviceInfoFinish";
//  web_ui()->CallJavascriptFunctionUnsafe(strFunctionName, deviceInfo);
//}
//
//void EPShowConfigUI::GetCity(const base::ListValue* args) {
//  DLOG(INFO) << "EPShowConfigUI::GetCity";
//  if (!ip_fetcher_) {
//    ip_fetcher_ =
//      new YSPIPLookupFetcher(this,
//        g_browser_process->system_request_context());
//  }
//  if (ip_fetcher_)
//    ip_fetcher_->Start();
//}

void EPShowConfigUI::CallJsFunc(std::string& strFuncName, base::Value* pResult) {
  pResult ? (web_ui()->CallJavascriptFunctionUnsafe(strFuncName, *pResult)) : (web_ui()->CallJavascriptFunctionUnsafe(strFuncName));
}

// YSPLoginManagerObserver:
void EPShowConfigUI::OnLoginRequestFailure(const std::string& error) {
  DLOG(INFO) << "EPShowConfigUI::OnLoginRequestFailure";
}

void EPShowConfigUI::OnLoginResponseParseFailure(const std::string& error) {
  DLOG(INFO) << "EPShowConfigUI::OnLoginResponseParseFailure";
}

void EPShowConfigUI::OnLoginFailure(base::string16 message) {
  DLOG(INFO) << "EPShowConfigUI::OnLoginFailure";
  is_login_ = false;
  base::Value str(message);
  //std::string strFunctionName = "SHOWBROWSER.refreshDataJS";
  //CallJsFunc(strFunctionName, &str);

  std::string loginChangedFunc = "SHOWBROWSER.loginChanged";
  CallJsFunc(loginChangedFunc, &str);
}

void EPShowConfigUI::OnLoginSuccess(const base::string16& name,
  const std::string& head_image_url) {
  //if(is_login_)
  //  return;
  is_login_ = true;
  DLOG(INFO) << "EPShowConfigUI::OnLoginSuccess";
  base::Value str("login");
  //std::string strFunctionName = "SHOWBROWSER.refreshDataJS";
  //CallJsFunc(strFunctionName, &str);

  std::string loginChangedFunc = "SHOWBROWSER.loginChanged";
  CallJsFunc(loginChangedFunc, &str);
}

void EPShowConfigUI::OnLogout() {
  DLOG(INFO) << "EPShowConfigUI::OnLogout";
  is_login_ = false;


  base::Value str("logout");

  //std::string strFunctionName = "SHOWBROWSER.refreshDataJS";
  //CallJsFunc(strFunctionName, &str);

  std::string loginChangedFunc = "SHOWBROWSER.loginChanged";
  CallJsFunc(loginChangedFunc, &str);
}

// YSPIPLookupFetcherDelegate
void EPShowConfigUI::OnIPLookupRequestFailure() {
  DLOG(INFO) << "EPShowConfigUI::OnIPLookupRequestFailure";

}

void EPShowConfigUI::OnIPLookupResponseParseSuccess(
  std::unique_ptr<base::DictionaryValue> response_data) {
  DLOG(INFO) << "EPShowConfigUI::OnIPLookupResponseParseSuccess";

 // std::string strCityInfo;
 // base::JSONWriter::Write(*response_data, &strCityInfo);

  //base::Value cityInfo(strCityInfo);
  //std::string strFunctionName = "SHOWBROWSER.getCityFinish";
  //web_ui()->CallJavascriptFunctionUnsafe(strFunctionName, cityInfo);
}

void EPShowConfigUI::OnIPLookupResponseParseFailure(const std::string& error) {
  DLOG(INFO) << "EPShowConfigUI::OnIPLookupResponseParseFailure";
}

/******************** EPShowConfigMessageHandler *************************/
EPShowConfigMessageHandler::EPShowConfigMessageHandler(EPShowConfigUI* pEpTabUI)
  :m_pEpTabUI(pEpTabUI) {
}

EPShowConfigMessageHandler::~EPShowConfigMessageHandler() {
  //EpLoginManager::LoginMgrInstance()->RemoveLoginEventHandle(this);
}

void EPShowConfigMessageHandler::BindFunction(const base::ListValue* args) {
  if (!m_pEpTabUI || !args) {
    return;
  }

  m_pEpTabUI->GetParamString(args);
}

//void EPShowConfigMessageHandler::BindGetDeviceInfoFun(const base::ListValue* args) {
//  if (!m_pEpTabUI) {
//    return;
//  }
//  m_pEpTabUI->GetDeviceInfo(args);
//}

void EPShowConfigMessageHandler::BindSetParamFun(const base::ListValue* args) {
  if (!m_pEpTabUI || !args) {
    return;
  }
  m_pEpTabUI->SetParam(args);
}

//void EPShowConfigMessageHandler::BindGetCity(const base::ListValue* args) {
//  if (!m_pEpTabUI || !args) {
//    return;
//  }
//  m_pEpTabUI->GetCity(args);
//}

void EPShowConfigMessageHandler::RegisterMessages() {
  web_ui()->RegisterMessageCallback(
    "getParam",
    base::Bind(&EPShowConfigMessageHandler::BindFunction,
      base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
    "setParam",
    base::Bind(&EPShowConfigMessageHandler::BindSetParamFun,
      base::Unretained(this)));
  //web_ui()->RegisterMessageCallback(
  //  "getDeviceInfo",
  //  base::Bind(&EPShowConfigMessageHandler::BindGetDeviceInfoFun,
  //    base::Unretained(this)));
  //web_ui()->RegisterMessageCallback(
  //  "getCity",
  //  base::Bind(&EPShowConfigMessageHandler::BindGetCity,
  //    base::Unretained(this)));
}

/*
void EPShowConfigMessageHandler::OnEpLoginEventFired(EP_LOGIN_STATUS emStatus, std::string& strLoginInfo) {
  if (!m_pEpTabUI) {
    return;
  }

  m_pEpTabUI->OnLoginEventFired(emStatus, strLoginInfo);
}
*/
#endif
