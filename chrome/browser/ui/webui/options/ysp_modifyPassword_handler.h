#ifdef REDCORE

#ifndef CHROME_BROWSER_UI_WEBUI_OPTIONS_YSP_MODIFY_PASSWORD_HANDLER_H_
#define CHROME_BROWSER_UI_WEBUI_OPTIONS_YSP_MODIFY_PASSWORD_HANDLER_H_

#pragma once

#include <stddef.h>

#include <string>
#include <vector>

#include "base/macros.h"
#include "base/prefs/pref_member.h"
#include "build/build_config.h"
#include "chrome/browser/ui/webui/options/options_ui.h"
#include "components/ysp_login/ysp_login_manager.h"

namespace options 
{
	class YSPModifyPasswordHandler : public options::OptionsPageUIHandler, public YSPLoginManagerObserver
	{
	public:
		YSPModifyPasswordHandler();
		~YSPModifyPasswordHandler() override;

		void GetLocalizedValues(base::DictionaryValue* localized_strings) override  {}
		void InitializeHandler() override {}
		void RegisterMessages() override;

		// YSPLoginManagerObserver:
		void OnConfigDataUpdated(const std::string& type, const std::string& data) override;
		void OnTokenStatusChanged(const std::string& type) override  {}
		void OnLoginRequestFailure(const std::string& error) override {}
		void OnLoginResponseParseFailure(const std::string& error) override {}
		void OnLoginFailure(base::string16 message) override {}
		void OnLoginSuccess(const base::string16& name, const std::string& head_image_url) override {}
		void OnLogout() override {}

	private:
		void BindCheckIfCanModifyPassword(const base::ListValue* args);
		void BindModifyPassword(const base::ListValue* args);

		DISALLOW_COPY_AND_ASSIGN(YSPModifyPasswordHandler);
	};
}

#endif // CHROME_BROWSER_UI_WEBUI_OPTIONS_YSP_MODIFY_PASSWORD_HANDLER_H_
#endif // REDCORE