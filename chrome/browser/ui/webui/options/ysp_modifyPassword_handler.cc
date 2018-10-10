#ifdef REDCORE

#include "chrome/browser/ui/webui/options/ysp_modifyPassword_handler.h"
#include "content/public/browser/web_ui.h"

namespace options 
{
	YSPModifyPasswordHandler::YSPModifyPasswordHandler() 
	{
		YSPLoginManager::GetInstance()->AddObserver(this);
	}
	
	YSPModifyPasswordHandler::~YSPModifyPasswordHandler() 
	{
		YSPLoginManager::GetInstance()->RemoveObserver(this);
	}

	void YSPModifyPasswordHandler::RegisterMessages()
	{
		web_ui()->RegisterMessageCallback("checkIfCanModifyPassword", 
										  base::Bind(&YSPModifyPasswordHandler::BindCheckIfCanModifyPassword,
										  base::Unretained(this)));

		web_ui()->RegisterMessageCallback("modifyPassword",
										  base::Bind(&YSPModifyPasswordHandler::BindModifyPassword,
										  base::Unretained(this)));
	}

	void YSPModifyPasswordHandler::BindCheckIfCanModifyPassword(const base::ListValue* args)
	{
		int source = YSPLoginManager::GetInstance()->GetAccountSourceType();
		base::StringValue param(source == 4 ? "false" : "true");
		web_ui()->CallJavascriptFunction("PersonalData.checkIfCanModifyPasswordCallback", param);
	}

	void YSPModifyPasswordHandler::BindModifyPassword(const base::ListValue* args)
	{
		std::string oldPassword;
		std::string newPassword;
		args->GetString(0, &oldPassword);
		args->GetString(1, &newPassword);
		if (oldPassword.empty() || newPassword.empty()) {
			return;
		}

		YSPLoginManager::GetInstance()->ModifyPassword(oldPassword, newPassword);
	}

	// YSPLoginManagerObserver:
	void YSPModifyPasswordHandler::OnConfigDataUpdated(const std::string& type, const std::string& data)
	{
		if (type == "modifyPassword")
		{
			base::StringValue command_value(data);
			web_ui()->CallJavascriptFunction("ResetPasswordOverlay.modifyPasswordCallback", command_value);
		}
	}
}

#endif