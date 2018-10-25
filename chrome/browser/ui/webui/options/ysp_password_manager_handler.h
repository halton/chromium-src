// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef REDCORE
// YSP+ { passwords AD manager }

#ifndef CHROME_BROWSER_UI_WEBUI_OPTIONS_YSP_PASSWORD_MANAGER_HANDLER_H_
#define CHROME_BROWSER_UI_WEBUI_OPTIONS_YSP_PASSWORD_MANAGER_HANDLER_H_

#include <stddef.h>

#include <string>
#include <vector>

#include "base/macros.h"
#include "base/prefs/pref_member.h"
#include "build/build_config.h"
#include "chrome/browser/ui/passwords/password_manager_presenter.h"
#include "chrome/browser/ui/passwords/password_ui_view.h"
#include "chrome/browser/ui/webui/options/options_ui.h"

namespace options {

// The WebUI based PasswordUIView. Displays passwords in the web ui.
class YSPPasswordManagerHandler : public OptionsPageUIHandler,
                               public PasswordUIView {
 public:
  YSPPasswordManagerHandler();
  ~YSPPasswordManagerHandler() override;

  // OptionsPageUIHandler implementation.
  void GetLocalizedValues(base::DictionaryValue* localized_strings) override;
  void InitializeHandler() override;
  void RegisterMessages() override;

  // PasswordUIView implementation.
  Profile* GetProfile() override;
  void ShowPassword(
      size_t index,
      const std::string& origin_url,
      const std::string& username,
      const base::string16& password_value) override;
  void SetPasswordList(
      const std::vector<scoped_ptr<autofill::PasswordForm>>& password_list,
      bool show_passwords) override;
  void HandleSaveADPasswordLists(const base::ListValue* entries);
  //bool RemoveLogin(std::string & origin, autofill::PasswordForm * old_primary_key);
  bool RemoveLogin(const autofill::PasswordForm & form, autofill::PasswordForm * old_primary_key);
  void SaveADPasswordListsInternal(const std::string & forms_str);
  void SetPasswordExceptionList(
      const std::vector<scoped_ptr<autofill::PasswordForm>>&
          password_exception_list) override;
#if !defined(OS_ANDROID)
  gfx::NativeWindow GetNativeWindow() const override;
#endif
 private:
  // Clears and then populates the list of passwords and password exceptions.
  // Called when the JS PasswordManager object is initialized.
  void HandleUpdatePasswordLists(const base::ListValue* args);

  // Removes a saved password entry.
  // |value| the entry index to be removed.
  void HandleRemoveSavedPassword(const base::ListValue* args);

  // Removes a saved password exception.
  // |value| the entry index to be removed.
  void HandleRemovePasswordException(const base::ListValue* args);

  // Requests the plain text password for an entry to be revealed.
  // |index| The index of the entry.
  void HandleRequestShowPassword(const base::ListValue* args);

  // User pref for storing accept languages.
  std::string languages_;

  // The PasswordManagerPresenter object owned by the this view.
  PasswordManagerPresenter password_manager_presenter_;

  DISALLOW_COPY_AND_ASSIGN(YSPPasswordManagerHandler);
};

}  // namespace options

#endif  // CHROME_BROWSER_UI_WEBUI_OPTIONS_YSP_PASSWORD_MANAGER_HANDLER_H_
#endif