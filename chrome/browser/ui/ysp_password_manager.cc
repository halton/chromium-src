#ifdef REDCORE
//ysp+ { login }

#include "chrome/browser/ui/ysp_password_manager.h"

#include "chrome/browser/password_manager/chrome_password_manager_client.h"
#include "chrome/browser/password_manager/password_store_factory.h"

namespace ysp_password_manager {

autofill::PasswordForm BuildPasswordForm(
  const std::string& url,
  const base::string16& username_element,
  const base::string16& password_element,
  const base::string16& submit_element,
  const base::string16& username,
  const base::string16& password) {

  autofill::PasswordForm form;
  form.type = autofill::PasswordForm::TYPE_MANUAL;
  form.action = GURL(url);
  form.origin = GURL(url);
  form.signon_realm = url;
  form.scheme = autofill::PasswordForm::SCHEME_HTML;
  form.new_password_element = base::string16();
  form.times_used = 0;
  form.is_public_suffix_match = false;
  
  form.username_element = username_element;
  form.password_element = password_element;
  if(!submit_element.empty())
    form.submit_element = submit_element;

  form.username_value = username;
  form.password_value = password;
  
  return form;
}

void AddLogin(
  Profile* profile,
  const std::string& url,
  const base::string16& username_element,
  const base::string16& password_element,
  const base::string16& username,
  const base::string16& password) {

  if(!profile || 
     url.empty() || 
     username_element.empty() || 
     password_element.empty()) {
    return;
  }
  
  password_manager::PasswordStore* password_store =
    PasswordStoreFactory::GetForProfile(
             profile,
             ServiceAccessType::EXPLICIT_ACCESS).get();
  
  autofill::PasswordForm form =
    BuildPasswordForm(url,
      username_element,
      password_element,
      base::string16(),
      username,
      password);

  password_store->AddLogin(form);
}

void UpdateLogin(
  Profile* profile,
  const std::string& url,
  const base::string16& username_element,
  const base::string16& password_element,
  const base::string16& username,
  const base::string16& password) {

  if(!profile || 
     url.empty() || 
     username_element.empty() || 
     password_element.empty()) {
    return;
  }
  
  password_manager::PasswordStore* password_store =
    PasswordStoreFactory::GetForProfile(
             profile,
             ServiceAccessType::EXPLICIT_ACCESS).get();
  
  autofill::PasswordForm form =
    BuildPasswordForm(url,
      username_element,
      password_element,
      base::string16(),
      username,
      password);

  password_store->UpdateLogin(form);
}

void RemoveLogin(
  Profile* profile,
  const std::string& url,
  const base::string16& username_element,
  const base::string16& password_element,
  const base::string16& username) {

  if(!profile || 
     url.empty() || 
     username_element.empty() || 
     password_element.empty()) {
    return;
  }
  
  password_manager::PasswordStore* password_store =
    PasswordStoreFactory::GetForProfile(
             profile,
             ServiceAccessType::EXPLICIT_ACCESS).get();
  
  autofill::PasswordForm form =
    BuildPasswordForm(url,
      username_element,
      password_element,
      base::string16(),
      username,
      base::string16());

  password_store->RemoveLogin(form);
}

} // namespace ysp_password_manager

#endif
