// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_YSP_LOGIN_YSP_LOGIN_MANAGER_H_
#define CHROME_BROWSER_YSP_LOGIN_YSP_LOGIN_MANAGER_H_

#include <memory>
#include <string>
#include <vector>
#include <map>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "components/prefs/pref_service.h"
#include "chrome/browser/ysp_login/ysp_auto_config_fetcher.h" //ysp+ { auto get config }
#include "chrome/browser/ysp_login/ysp_fetcher_resource.h" //YSP+ { Fetcher resource }
#include "chrome/browser/ysp_login/ysp_login_fetcher.h"
#include "chrome/browser/ysp_login/ysp_replace_fetcher.h" //YSP+ { Resource Replace }
#include "chrome/browser/ysp_login/ysp_singlesignon_fetcher.h" //YSP+ { SingleSignOn config }
#include "chrome/browser/ysp_login/ysp_us_report_fetcher.h"
#include "ui/gfx/geometry/size.h"

struct ActivexDownloadInfo;

struct UrlTrusted {
  UrlTrusted();
  ~UrlTrusted();
  std::vector<std::wstring> trust_url;
  std::map<std::wstring, std::wstring> policy;
  UrlTrusted(const UrlTrusted& other);
};

enum {
  SATUS_VERIFY_CONFIG_DATA_FAIL = -9,
  SATUS_AUTO_FETCH_RESOURCE_RESPONSE_FAIL = -8,
  SATUS_AUTO_FETCH_RESOURCE_REQUEST_FAIL = -7,
  SATUS_LOGIN_FETCH_RESOURCE_RESPONSE_FAIL = -6,
  SATUS_LOGIN_FETCH_RESOURCE_REQUEST_FAIL = -5,
  SATUS_VERIFY_ACCOUNT_FAIL = -4,
  SATUS_LOGIN_RESPONSE_FAIL = -3,
  SATUS_LOGIN_REQUEST_FAIL = -2,
  SATUS_LOGOUT = -1,

  SATUS_LOCAL_LOGIN_SUCCESS = 1,
  SATUS_LOGIN_FETCH_START,
  SATUS_LOGIN_FETCH_RECEIVED,
  SATUS_VERIFY_ACCOUNT_SUCCESS,
  SATUS_LOGIN_FETCH_RESOURCE_START,
  SATUS_LOGIN_FETCH_RESOURCE_RECEIVED,
  SATUS_AUTO_FETCH_RESOURCE_START,
  SATUS_AUTO_FETCH_RESOURCE_RECEIVED,
  SATUS_VERIFY_CONFIG_SUCCESS = 100,
};

// YSPLoginManagerObserver
class YSPLoginManagerObserver {
 public:
  virtual void OnConfigDataUpdated(const std::string& type,
                                   const std::string& data) { }
  virtual void OnTokenStatusChanged(const std::string& type) { }
  virtual void OnLoginRequestFailure(const std::string& error) = 0;
  virtual void OnLoginResponseParseFailure(const std::string& error) = 0;
  virtual void OnLoginFailure(base::string16 message) = 0;
  virtual void OnLoginSuccess(const base::string16& name,
                              const std::string& head_image_url) = 0;
  virtual void OnLogout() = 0;

 protected:
  virtual ~YSPLoginManagerObserver() {}
};

// Singleton class
class YSPLoginManager : public base::SupportsWeakPtr<YSPLoginManager>,
                        public YSPLoginFetcherDelegate,
                        public YSPFetcherResourceDelegate,
                        public YSPReplaceFetcherDelegate,
                        public YSPSingleSignOnFetcherDelegate,
                        public ManagedSettingsProvider {
 public:
  YSPLoginManager();
  ~YSPLoginManager() override;

  static YSPLoginManager* GetInstance();

  void set_auto_login(bool auto_login);
  std::string GetLastCID();
  std::string GetLastUID();

  void Init();
  void SetPushData(const std::string& user_info);
  bool Restore();
  void StartLogin(const std::string& cid,
                  const std::string& account,
                  const std::string& password);
  void ClearCache();
  void Logout();

  bool GetLoginStatus();
  bool GetAutoLoginStatus();
  std::string GetLoginInfo();
  base::string16 BuildYSPBrowserVersionString(); //YSP+ { app version }
  std::string GetCompanyId();
  std::string GetManageServer();
  std::string GetValueForKey(const std::string& key);
  std::string GetUserInfoForKey(int key); //YSP+ { passwords AD manager }
  std::string GetDeviceInfo();
  bool HasManagedBookmarks();
  base::DictionaryValue* GetManagedBookmarks();
  std::string GetUserAgent();
  std::string GetAutoConfigUrl(); //ysp+ { auto get config }
  int GetAutoConfigDelta(); //ysp+ { auto get config }
  base::DictionaryValue * GetAutoUpgrade();
  base::DictionaryValue * GetWebsiteListEnabled(); //ysp+ { URL Blacklist And Whitelist }
  base::DictionaryValue* GetManagedResourceReplace(); //ysp+ { Resource Replace }
  base::DictionaryValue* GetManagedKernels(); //ysp+ { Kernel switching }
  std::string GetDefaultCore(); //ysp+ { Kernel switching }
  base::DictionaryValue* GetManagedGMStream(); //YSP+ { sangfor GM ssl }
  void GetSingleSignOnConfig(); //YSP+ { SingleSignOn config }
  base::DictionaryValue * GetManagedSingleSignOnConfig(); //YSP+ { SingleSignOn config }
  base::ListValue * GetManagedADProxyAuth(); //YSP+ { passwords AD manager }
  std::string GetEncryptionAndKey(); //ysp+ { AES DES and SMS4 crypt }
  bool GetCacheEncryption(); //YSP+ { cache encryption }
  std::string GetCryptoHeaderKey(); //ysp+ { crypto http header }
  std::string GetUserAgentTypes(); //ysp+ { crypto http header }
  int GetLockScreenTime(); //YSP+ { lock screen time }
  base::DictionaryValue * GetPopupSetting(); //YSP+ { window popup }
  //TODO (matianzhi): YSP+ { startup and home pages
  bool isStartupPages();
  base::ListValue* GetStartupPages();
  //YSP+ }
  base::DictionaryValue* GetPreviewDocOnline(); //YSP+ { doc online preview }
  bool GetPreviewDocOnlineEnable(); //YSP+ { doc online preview }
  base::DictionaryValue * GetPrivateDNS(); //ysp+ { private DNS }
  std::string GetYSPSingleSignOnString(); //ysp+ { ysp single sign on }
  void AddHeaders();
  bool GetAddressBarEnabled();
  bool GetStatusBarEnabled();
  bool GetSaveFileEnabled();
  bool GetPrintEnabled();
  bool GetMouseRightButtonEnabled();
  bool GetCutCopyEnabled();
  int GetWindowFrameColor(bool active) {
    return active ? window_frame_color_ : window_inactive_color_;
  }
  bool GetWatermarkEnabled();
  std::vector<base::string16> GetWatermarkString();
  uint32_t GetWatermarkColor();
  int GetWatermarkFontSize();
  bool GetScreenCaptureEnabled();
  bool GetDevToolsEnabled();
  base::string16 GetManagedBookmarksFolderName();
  bool GetWebsiteListStatus();
  std::string GetWebsiteListType();
  bool GetLoadURLAllowed(const GURL& url);
  bool GetDownloadFileAllowed(const base::FilePath& file_path);
  void ReportURLLoading(const GURL& url);
  base::string16 GetPACFileURL();
  void UpdatePACSettings(base::FilePath& pac_file);
  std::vector<ActivexDownloadInfo> GetActivexDownloadInfo();
  UrlTrusted GetUrlTrusted();
  bool GetUserAgent(std::string* ua_types, std::string* ua_string);

  void AddObserver(YSPLoginManagerObserver* observer);
  void RemoveObserver(YSPLoginManagerObserver* observer);

  // YSPLoginFetcherDelegate:
  void OnLoginRequestFailure(const std::string& error) override;
  void OnLoginResponseParseSuccess(
    std::unique_ptr<base::DictionaryValue> response_data) override;
  void OnLoginResponseParseFailure(const std::string& error) override;

  // YSPFetcherResourceDelegate:
  void OnFetcherResourceRequestFailure(const GURL& url,
                                       bool auto_fecth,
                                       const std::string& error) override;
  void OnFetcherResourceResponseParseSuccess(
      const GURL& url,
      std::unique_ptr<base::DictionaryValue> response_data,
      bool auto_fecth) override;
  void OnFetcherResourceResponseParseFailure(const GURL& url,
                                             bool auto_fecth,
                                             const std::string& error) override;
  void OnReplaceRequestFailure(const std::string& error) override;
  void OnReplaceResponseParseSuccess(const std::string& response_data,
                                     const std::string& path_url) override;
  void OnReplaceResponseParseFailure(const std::string& error) override;
  //YSP+ }
  //YSP+ { SingleSignOn config
  // YSPSingleSignOnInfoFetcherDelegate:
  void OnSingleSignOnRequestFailure() override;
  void OnSingleSignOnResponseParseSuccess(
    std::unique_ptr<base::DictionaryValue> response_data) override;
  void OnSingleSignOnResponseParseFailure(const std::string& error) override;
  //YSP+ } /* SingleSignOn config */
  // ManagedSettingsProvider:
  base::Value* GetManagedValue(const std::string& path) override;
  bool SetManageServer(const std::string& manage_server);
  void set_web_initialized(bool init) { web_initialized_ = init; }
  bool get_web_initialized() { return web_initialized_; }
  bool isValidPassword(const std::string& field) { return password_ == field; }
  std::string generateUserInfoForSettings();
  std::string generateLoginDeviceForSettings();
  //YSP+ { Fetcher resource
  int GetTimeDifference(); //YSP+ { time synchronization }
  std::string GetUserId();
  int GetStrategyVersion();
  std::string GetActivelId();
  int GetAccountSourceType();
  std::string GetAccessToken();
  std::string GetAccessTokenForLogin();
  std::string GetRefreshToken();
  std::string GetRefreshTokenForLogin();
  void GetApplicationfetcher(bool auto_fetch = true);
  void GetStrategyfetcher(bool auto_fetch = true);
  void GetTokenfetcher(bool auto_fetch = true);
  void GetSwafetcher(bool auto_fetch = true);
  void GetPcfetcher(bool auto_fetch = true);
  void GetUserfetcher(bool auto_fetch = true);
  void GetAuthTokenfetcher(std::string source_url, bool auto_fetch = true);
  void GetAutoConfigfetcher(bool auto_fetch = true);
  void GetSdpDevicefetcher(bool auto_fetch = true);
  void PutSdpDevicefetcher(const std::string& device_id,
                           bool auto_fetch = true);
  void PutApplictionStatusFetch(const std::string& application_id,
                                bool application_status);
  void GetGatewayApplictionFetch(bool auto_fetch = true);

  void OnAutoConfigParseSuccess(std::unique_ptr<base::DictionaryValue> response_data, bool auto_fetch);
  void ModifyPassword(const std::string& old_password,
                      const std::string& new_password);

  base::string16 GetYSPUserName();
  std::string GetHeadImageUrl();
  void OnApplicationFetcherResponseParse(std::unique_ptr<base::DictionaryValue>& response_data,
                                         bool from_local,
                                         bool auto_fetch);
  void OnStrategyFetcherResponseParse(std::unique_ptr<base::DictionaryValue>& response_data,
                                      bool from_local,
                                      bool auto_fetch);
  void OnTokenFetcherResponseParse(std::unique_ptr<base::DictionaryValue>& response_data,
                                   bool from_local,
                                   bool auto_fetch);
  void OnSwaFetcherResponseParse(std::unique_ptr<base::DictionaryValue>& response_data,
                                 bool from_local,
                                 bool auto_fetch);
  void OnPcFetcherResponseParse(std::unique_ptr<base::DictionaryValue>& response_data,
                                bool from_local,
                                bool auto_fetch);
  void OnUserFetcherResponseParse(std::unique_ptr<base::DictionaryValue>& response_data,
                                  bool from_local,
                                  bool auto_fetch);
  void OnAutoTokenFetcherResponseParse(std::unique_ptr<base::DictionaryValue>& response_data,
                                       bool from_local,
                                       bool auto_fetch);
  void OnSdpDeviceFetcherResponseParse(std::unique_ptr<base::DictionaryValue>& response_data,
                                       bool from_local,
                                       bool auto_fetch);
  void OnModifyPasswordResponseParse(std::string response);
  //YSP+ } /*Fetcher resource*/
 private:
  void OnLoginResponseParseSuccessInternal(
      std::unique_ptr<base::DictionaryValue>& response_data,
      bool from_local);
  void OnFetcherResourceResponseParseSuccessInternal(const GURL& url,
      std::unique_ptr<base::DictionaryValue>& response_data,
      bool from_local,
      bool auto_fetch); //YSP+ { Fetcher resource }
  void OnReplaceResponseParseSuccessInternal(
      const std::string& response_data,
      const std::string& path_url,
      bool from_local); //YSP+ { Resource Replace }
  void OnSingleSignOnResponseParseSuccessInternal(
      std::unique_ptr<base::DictionaryValue>& response_data,
      bool from_local); //YSP+ { SingleSignOn config }
  void SetIEUrlTrusted(const UrlTrusted& trust);
  std::wstring Utf8ToUTF16(const std::string& str);

  std::string GetCryptKey();
  bool GetCacheCrypt(); //YSP+ { cache encryption }
  std::string GetResponseStatusCode(
      std::unique_ptr<base::DictionaryValue>& response_data); //YSP+ { Fetcher resource }
  base::string16 GetResponseMessage(
      std::unique_ptr<base::DictionaryValue>& response_data); //YSP+ { Fetcher resource }
  base::string16 GetResponseErrorMessage(
      const std::string error_code);  // YSP+ { Fetcher resource }
  std::string GetUserInfoAsJSONString();
  std::string GetDataJSAsJSONString();
  std::string GetShowConfigAsJSONString();
  base::DictionaryValue* GetShowConfig();
  bool GetFunctionControlBool(const std::string& key, bool default_value);
  int GetFunctionControlInt(const std::string& key);
  int GetFunctionControlIntWithDefault(const std::string& key,
                                       int default_value);
  base::string16 GetFunctionControlString(const std::string& key);
  std::vector<base::string16> GetFunctionControlListString(
      const std::string& key);
  double GetFunctionControlDoubleWithDefault(const std::string& key,
                                             double default_value);

  std::string GetLoginStatusCode();
  base::string16 GetLoginErrorMessage();
  void LoadWindowFrameColors();

  void SaveLastId(const std::string& key, const std::string& value);
  std::string GetLastId(const std::string& key);

  bool URLInBacklist(const GURL& url);
  bool URLInWhitelist(const GURL& url);
  bool URLInPolicylist(const std::string& policy_path, const GURL& url);

  bool FileTypeInBlacklist(const base::FilePath& file_path);
  bool FileTypeInWhitelist(const std::string& file_ext);
  bool FileTypeInPolicylist(const std::string& policy_path,
                            const base::FilePath& file_path);

  bool EmptyPolicyList(const std::string& policy_path);

  void UpdateProxySettings();
  bool GetUSReportEnabled();
  void SetLoginStatus(int);
  void NotifyFailure();
  void NotifyConfigureUpdate(const std::string& type, const std::string& data);

  void UpdateLoginManagerSettings();

  YSPLoginFetcher* login_fetcher_;
  YSPReplaceFetcher* replace_fetcher_;
  YSPSingleSignOnFetcher* single_signon_fetcher_;
  YSPUSReportFetcher* report_fetcher_;
  //YSP+ { Fetcher resource
  YSPFetcherResource* get_user_fetcher_;
  YSPFetcherResource* get_application_fetcher_;
  YSPFetcherResource* get_configuration_strategy_fetcher_;
  YSPFetcherResource* get_sso_token_fetcher_;
  YSPFetcherResource* get_swa_fetcher_;
  YSPFetcherResource* get_configuration_pc_fetcher_;
  YSPFetcherResource* get_auth_token_fetcher_;
  YSPFetcherResource* get_sdp_device_fetcher_;
  YSPFetcherResource* put_sdp_device_fetcher_;
  YSPFetcherResource* put_appliction_status_fetcher_;
  YSPFetcherResource* get_gateway_appliction_fetcher_;
  YSPFetcherResource* put_modify_password_fetcher_;
  //YSP+ } /*Fetcher resource*/
  YSPAutoConfigFetcher* autoConfig_fetcher_;
  bool auto_login_;
  std::string account_;
  std::string password_;
  std::string cid_;
  std::unique_ptr<base::DictionaryValue> login_info_;
  //YSP+ { Fetcher resource
  std::unique_ptr<base::DictionaryValue> application_info_;
  std::unique_ptr<base::DictionaryValue> strategy_info_;
  std::unique_ptr<base::DictionaryValue> token_info_;
  std::unique_ptr<base::DictionaryValue> swa_info_;
  std::unique_ptr<base::DictionaryValue> pc_info_;
  std::unique_ptr<base::DictionaryValue> auto_config_info_;
  bool login_status_code_;
  bool application_status_code_;
  bool strategy_status_code_;
  bool token_status_code_;
  bool swa_status_code_;
  bool pc_status_code_;
  //YSP+ } /*Fetcher resource*/
  std::vector<std::string> send_request_list_;
  std::unique_ptr<base::DictionaryValue> single_signon_info_;
  int window_frame_color_;
  int window_inactive_color_;
  bool should_parse_response_;
  bool download_ = false;
  int login_status_ = SATUS_LOGOUT; //ysp+ { auto get config }
  bool ntp_login_status_ = false; //false is logout
  bool single_signon_status_ = false;  // YSP+ { SingleSignOn config }
  bool web_initialized_;
  bool offline_status_;

  std::vector<YSPLoginManagerObserver*> observers_;

  DISALLOW_COPY_AND_ASSIGN(YSPLoginManager);
};

#endif  // CHROME_BROWSER_YSP_LOGIN_YSP_LOGIN_MANAGER_H_
