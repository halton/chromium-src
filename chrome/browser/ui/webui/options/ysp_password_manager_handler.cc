// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef REDCORE
// YSP+ { passwords AD manager }

#include "chrome/browser/ui/webui/options/ysp_password_manager_handler.h"

#include "base/bind.h"
#include "base/command_line.h"
#include "base/macros.h"
#include "base/prefs/pref_service.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "build/build_config.h"
#include "chrome/browser/chrome_notification_types.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/sync/profile_sync_service_factory.h"
#include "chrome/common/pref_names.h"
#include "chrome/common/url_constants.h"
#include "chrome/grit/generated_resources.h"
#include "components/autofill/core/common/password_form.h"
#include "components/browser_sync/browser/profile_sync_service.h"
#include "components/password_manager/core/browser/password_bubble_experiment.h"
#include "components/password_manager/core/browser/password_manager_constants.h"
#include "components/password_manager/core/browser/password_ui_utils.h"
#include "components/password_manager/core/common/experiments.h"
#include "components/url_formatter/url_formatter.h"
#include "content/public/browser/notification_details.h"
#include "content/public/browser/notification_source.h"
#include "content/public/browser/user_metrics.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_ui.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/origin_util.h"
#include "net/base/net_util.h"
#include "ui/base/l10n/l10n_util.h"
#include "base/json/json_file_value_serializer.h"
#include "base/path_service.h"
#include "chrome/common/chrome_paths.h"
#include "components/password_manager/core/browser/password_manager_constants.h"
#include "components/password_manager/core/browser/password_form_manager.h"
#include "components/password_manager/core/browser/password_store.h"
#include "content/public/browser/browser_thread.h"
#include "components/ysp_login/ysp_login_manager.h"
#include "base/json/json_writer.h"
#include "base/json/json_reader.h"
#include "chrome/browser/password_manager/password_store_factory.h"
#include "components/password_manager/core/browser/password_store_factory_util.h"

#if defined(OS_WIN) && defined(USE_ASH)
#include "chrome/browser/ui/ash/ash_util.h"
#endif

namespace options {

namespace {
// The following constants should be synchronized with the constants in
// chrome/browser/resources/options/password_manager_list.js.
const char kOriginField[] = "origin";
const char kAppNameField[] = "appName";
const char kShownUrlField[] = "shownUrl";
const char kIsAndroidUriField[] = "isAndroidUri";
const char kIsSecureField[] = "isSecure";
const char kUsernameField[] = "username";
const char kPasswordField[] = "password";
const char kFederationField[] = "federation";
const char kIconUrlField[] = "iconUrl";

// Copies from |form| to |entry| the origin, shown origin, whether the origin is
// Android URI, and whether the origin is secure.
void CopyOriginInfoOfPasswordForm(const autofill::PasswordForm& form,
                                  const std::string& languages,
                                  base::DictionaryValue* entry) {
  entry->SetString(
      kOriginField,
      url_formatter::FormatUrl(
          form.origin, languages, url_formatter::kFormatUrlOmitNothing,
          net::UnescapeRule::SPACES, nullptr, nullptr, nullptr));
  bool is_android_uri = false;
  entry->SetString(kShownUrlField, password_manager::GetShownOrigin(
                                       form, languages, &is_android_uri));
  entry->SetBoolean(kIsAndroidUriField, is_android_uri);
  entry->SetBoolean(kIsSecureField, content::IsOriginSecure(form.origin));
}

}  // namespace

YSPPasswordManagerHandler::YSPPasswordManagerHandler()
    : password_manager_presenter_(this) {}

YSPPasswordManagerHandler::~YSPPasswordManagerHandler() {}

Profile* YSPPasswordManagerHandler::GetProfile() {
  return Profile::FromWebUI(web_ui());
}

#if !defined(OS_ANDROID)
gfx::NativeWindow YSPPasswordManagerHandler::GetNativeWindow() const {
  return web_ui()->GetWebContents()->GetTopLevelNativeWindow();
}
#endif

void YSPPasswordManagerHandler::GetLocalizedValues(
    base::DictionaryValue* localized_strings) {
  DCHECK(localized_strings);

  static const OptionsStringResource resources[] = {
      {"autoSigninTitle", IDS_PASSWORDS_AUTO_SIGNIN_TITLE},
      {"autoSigninDescription", IDS_PASSWORDS_AUTO_SIGNIN_DESCRIPTION},
      {"savedPasswordsTitle", IDS_PASSWORDS_SHOW_PASSWORDS_TAB_TITLE},
      {"ADpasswordsmanageSearchApp", IDS_OPTIONS_AD_PASSWORD_SEARCH_APP},//YSP+ { passwords AD manager }
      {"passwordShowButton", IDS_PASSWORDS_PAGE_VIEW_SHOW_BUTTON},
      {"passwordHideButton", IDS_PASSWORDS_PAGE_VIEW_HIDE_BUTTON},
	  { "ADpasswordsmanageUsername", IDS_OPTIONS_AD_PASSWORD_USERNAME }, //YSP+ { passwords AD manager }
	  { "ADpasswordsmanagePassword", IDS_OPTIONS_AD_PASSWORD_PASSWORD }, //YSP+ { passwords AD manager }
      //{"passwordsNoPasswordsDescription",
      // IDS_PASSWORDS_PAGE_VIEW_NO_PASSWORDS_DESCRIPTION},
      //{"passwordsNoExceptionsDescription",
      // IDS_PASSWORDS_PAGE_VIEW_NO_EXCEPTIONS_DESCRIPTION},
  };

  RegisterStrings(localized_strings, resources, arraysize(resources));

  const ProfileSyncService* sync_service =
      ProfileSyncServiceFactory::GetForProfile(GetProfile());
  int title_id =
      password_bubble_experiment::IsSmartLockBrandingEnabled(sync_service)
          ? IDS_PASSWORD_MANAGER_SMART_LOCK_FOR_PASSWORDS
          : IDS_PASSWORDS_EXCEPTIONS_WINDOW_TITLE;
  RegisterTitle(localized_strings, "passwordsPage", title_id);

  localized_strings->SetString("passwordManagerLearnMoreURL",
                               chrome::kPasswordManagerLearnMoreURL);
  localized_strings->SetString(
      "passwordsManagePasswordsLink",
      password_manager::kPasswordManagerAccountDashboardURL);

  std::string management_hostname =
      GURL(password_manager::kPasswordManagerAccountDashboardURL).host();
  base::string16 link_text = base::UTF8ToUTF16(management_hostname);
  size_t offset;
  base::string16 full_text = l10n_util::GetStringFUTF16(
      IDS_MANAGE_PASSWORDS_REMOTE_TEXT, link_text, &offset);

  localized_strings->SetString("passwordsManagePasswordsBeforeLinkText",
                               full_text.substr(0, offset));
  localized_strings->SetString("passwordsManagePasswordsLinkText",
                               full_text.substr(offset, link_text.size()));
  localized_strings->SetString("passwordsManagePasswordsAfterLinkText",
                               full_text.substr(offset + link_text.size()));

  bool disable_show_passwords = false;

#if defined(OS_WIN) && defined(USE_ASH)
  // We disable the ability to show passwords when running in Windows Metro
  // interface.  This is because we cannot pop native Win32 dialogs from the
  // Metro process.
  // TODO(wfh): Revisit this if Metro usage grows.
  if (chrome::IsNativeWindowInAsh(GetNativeWindow()))
    disable_show_passwords = true;
#endif

  localized_strings->SetBoolean("disableShowPasswords", disable_show_passwords);
  localized_strings->SetBoolean(
      "enableCredentialManagerAPI",
      base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableCredentialManagerAPI));
}

void YSPPasswordManagerHandler::RegisterMessages() {
  web_ui()->RegisterMessageCallback(
      "updateADPasswordLists",
      base::Bind(&YSPPasswordManagerHandler::HandleUpdatePasswordLists,
                 base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "removeSavedADPassword",
      base::Bind(&YSPPasswordManagerHandler::HandleRemoveSavedPassword,
                 base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "removeADPasswordException",
      base::Bind(&YSPPasswordManagerHandler::HandleRemovePasswordException,
                 base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "requestShowADPassword",
      base::Bind(&YSPPasswordManagerHandler::HandleRequestShowPassword,
                 base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
	  "saveADPasswordLists",
	  base::Bind(&YSPPasswordManagerHandler::HandleSaveADPasswordLists,
		  base::Unretained(this)));
}

void YSPPasswordManagerHandler::InitializeHandler() {
  password_manager_presenter_.Initialize();
}

void YSPPasswordManagerHandler::HandleRemoveSavedPassword(
    const base::ListValue* args) {
  std::string string_value = base::UTF16ToUTF8(ExtractStringValue(args));
  int index;
  if (base::StringToInt(string_value, &index) && index >= 0) {
    password_manager_presenter_.RemoveSavedPassword(static_cast<size_t>(index));
  }
}

void YSPPasswordManagerHandler::HandleRemovePasswordException(
    const base::ListValue* args) {
  std::string string_value = base::UTF16ToUTF8(ExtractStringValue(args));
  int index;
  if (base::StringToInt(string_value, &index) && index >= 0) {
    password_manager_presenter_.RemovePasswordException(
        static_cast<size_t>(index));
  }
}

void YSPPasswordManagerHandler::HandleRequestShowPassword(
    const base::ListValue* args) {
  int index;
  if (!ExtractIntegerValue(args, &index))
    NOTREACHED();

  password_manager_presenter_.RequestShowPassword(static_cast<size_t>(index));
}

void YSPPasswordManagerHandler::ShowPassword(
    size_t index,
    const std::string& origin_url,
    const std::string& username,
    const base::string16& password_value) {
  // Call back the front end to reveal the password.
  web_ui()->CallJavascriptFunction(
      "PasswordADManager.showADPassword",
      base::FundamentalValue(static_cast<int>(index)),
      base::StringValue(password_value));
}

void YSPPasswordManagerHandler::HandleUpdatePasswordLists(
    const base::ListValue* args) {
  password_manager_presenter_.UpdatePasswordLists();
}

void YSPPasswordManagerHandler::SetPasswordList(
    const std::vector<scoped_ptr<autofill::PasswordForm>>& password_list,
    bool show_passwords) {
  base::ListValue entries;
  languages_ = GetProfile()->GetPrefs()->GetString(prefs::kAcceptLanguages);
  base::string16 placeholder(base::ASCIIToUTF16("        "));
  for (const auto& saved_password : password_list) {
    scoped_ptr<base::DictionaryValue> entry(new base::DictionaryValue);
    CopyOriginInfoOfPasswordForm(*saved_password, languages_, entry.get());

	entry->SetString(kAppNameField, saved_password->YSPAppName_value);
	//DLOG(INFO) << "username: " << saved_password->username_value;
    entry->SetString(kUsernameField, saved_password->username_value);
    if (show_passwords) {
      entry->SetString(kPasswordField, saved_password->password_value);
    } else {
      // Use a placeholder value with the same length as the password.
      entry->SetString(
          kPasswordField,
          base::string16(saved_password->password_value.length(), ' '));
    }
    const GURL& federation_url = saved_password->federation_url;
    if (!federation_url.is_empty()) {
      entry->SetString(
          kFederationField,
          l10n_util::GetStringFUTF16(IDS_PASSWORDS_VIA_FEDERATION,
                                     base::UTF8ToUTF16(federation_url.host())));
    }
	entry->SetString(kIconUrlField, saved_password->icon_url.spec());
	//DLOG(INFO) << "icon_url: " << saved_password->icon_url.spec();
	if (saved_password->YSPAppName_value.empty())
		continue;
	std::string uuidKey = "onlyid";
	std::string loggingstatus = "loggingStatus";
	std::string username = YSPLoginManager::GetInstance()->GetValueForKey(uuidKey);
	std::string loginstatus = YSPLoginManager::GetInstance()->GetValueForKey(loggingstatus);
	if (loginstatus != "100")
		username = "";
	if (saved_password->YSPUserName_value != base::UTF8ToUTF16(username))
		continue;

    entries.Append(entry.release());
  }

  web_ui()->CallJavascriptFunction("PasswordADManager.setSavedADPasswordsList",
                                   entries);
}

static bool passwordIsSpace(std::string& password)
{
	for (unsigned int i = 0; i < password.length(); i++)
	{
		if (password.c_str()[i] != ' ')
			return false;
	}
	return true;
}

void YSPPasswordManagerHandler::HandleSaveADPasswordLists(const base::ListValue* entries)
{
	std::string entries_str = "";
	base::JSONWriter::Write(*entries, &entries_str);
	if (!entries_str.empty()) {
		content::BrowserThread::PostTask(
			content::BrowserThread::DB, FROM_HERE,
			base::Bind(&YSPPasswordManagerHandler::SaveADPasswordListsInternal,
				base::Unretained(this), entries_str));
	}
}

bool YSPPasswordManagerHandler::RemoveLogin(const autofill::PasswordForm& form, autofill::PasswordForm* old_primary_key)
{
	ScopedVector<autofill::PasswordForm> matched_forms;
	scoped_refptr<password_manager::PasswordStore> password_store =
		PasswordStoreFactory::GetForProfile(GetProfile(), ServiceAccessType::EXPLICIT_ACCESS);

	if (password_store) {
		password_store->GetYSPLogins(form, password_manager::PasswordStore::AuthorizationPromptPolicy::ALLOW_PROMPT, &matched_forms);
		for (auto& login : matched_forms) {
			if (form.signon_realm == login->signon_realm && form.origin == login->origin && form.YSPUserName_value == login->YSPUserName_value) {
				old_primary_key->signon_realm = login->signon_realm;
				old_primary_key->origin = login->origin;
				old_primary_key->YSPAppName_value = login->YSPAppName_value;
				old_primary_key->YSPUserName_value = login->YSPUserName_value;
				old_primary_key->YSPLoginType_value = login->YSPLoginType_value;
				old_primary_key->action = login->action;
				old_primary_key->icon_url = login->icon_url;
				old_primary_key->username_value = login->username_value;
				old_primary_key->password_value = login->password_value;
				old_primary_key->username_element = login->username_element;
				old_primary_key->password_element = login->password_element;
				old_primary_key->form_data = login->form_data;
				return true;
			}
		}
	}
	matched_forms.clear();
	return false;
}

void YSPPasswordManagerHandler::SaveADPasswordListsInternal(const std::string& entries_str)
{
	if (entries_str.empty())
		return;
	scoped_ptr<base::Value> entries_tmp = base::JSONReader::Read(entries_str);
	scoped_ptr<base::ListValue> entries;
	entries.reset(static_cast<base::ListValue*>(entries_tmp.release()));
	scoped_refptr<password_manager::PasswordStore> password_store =
		PasswordStoreFactory::GetForProfile(GetProfile(), ServiceAccessType::EXPLICIT_ACCESS);
	if (entries && !entries->empty()) {
		for (size_t i = 0; i < entries->GetSize(); ++i) {
			autofill::PasswordForm forms;
			const base::DictionaryValue* entry = nullptr;
			if (entries->GetDictionary(i, &entry)) {
				std::string YSPappName, YSPuserName, userName, password, showUrl, origin, iconUrl;
				std::string uuidKey = "onlyid";
				YSPuserName = YSPLoginManager::GetInstance()->GetValueForKey(uuidKey);
				entry->GetString(kAppNameField, &YSPappName);
				entry->GetString(kShownUrlField, &showUrl);
				entry->GetString(kUsernameField, &userName);
				entry->GetString(kPasswordField, &password);
				entry->GetString(kOriginField, &origin);
				entry->GetString(kIconUrlField, &iconUrl);
				forms.YSPAppName_value = base::UTF8ToUTF16(YSPappName);
				forms.YSPUserName_value = base::UTF8ToUTF16(YSPuserName);
				forms.origin = GURL(origin);
				forms.action = GURL(origin);
				forms.icon_url = GURL(iconUrl);
				forms.signon_realm = GURL(origin).GetOrigin().spec();
				if (password_store) {
					if (!password.empty()) {
						if (!passwordIsSpace(password)) {
							autofill::PasswordForm old_primary_key, new_forms;
							if (RemoveLogin(forms, &old_primary_key)) {
								new_forms.username_element = old_primary_key.username_element;
								new_forms.password_element = old_primary_key.password_element;
								new_forms.form_data = old_primary_key.form_data;
							}
							new_forms.signon_realm = forms.signon_realm;
							new_forms.origin = forms.origin;
							new_forms.YSPAppName_value = forms.YSPAppName_value;
							new_forms.YSPUserName_value = forms.YSPUserName_value;
							new_forms.YSPLoginType_value = old_primary_key.YSPLoginType_value;
							new_forms.action = forms.action;
							new_forms.icon_url = forms.icon_url;
							new_forms.username_value = base::UTF8ToUTF16(userName);
							new_forms.password_value = base::UTF8ToUTF16(password);
							password_store->UpdateLoginWithPrimaryKey(new_forms, old_primary_key);
						}
					}
				}
				DLOG(INFO) << "YSPappname: " << YSPappName << " showUrl: " << showUrl << " username: " << userName << " password: " << password;
			}
		}
	}
}

void YSPPasswordManagerHandler::SetPasswordExceptionList(
    const std::vector<scoped_ptr<autofill::PasswordForm>>&
        password_exception_list) {
  //base::ListValue entries;
  //for (const auto& exception : password_exception_list) {
  //  scoped_ptr<base::DictionaryValue> entry(new base::DictionaryValue);
  //  CopyOriginInfoOfPasswordForm(*exception, languages_, entry.get());
  //  entries.Append(entry.release());
  //}

  //web_ui()->CallJavascriptFunction("PasswordADManager.setADPasswordExceptionsList",
  //                                 entries);
}

}  // namespace options
#endif
