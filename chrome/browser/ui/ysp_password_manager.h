// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef REDCORE
// ysp+ { login }

#ifndef CHROME_BROWSER_UI_YSP_PASSWORD_MANAGER_H_
#define CHROME_BROWSER_UI_YSP_PASSWORD_MANAGER_H_

#include <string>
#include "base/strings/string16.h"

class Profile;

namespace ysp_password_manager {

void AddLogin(
  Profile* profile,
  const std::string& url,
  const base::string16& username_element,
  const base::string16& password_element,
  const base::string16& username,
  const base::string16& password);
 
void UpdateLogin(
  Profile* profile,
  const std::string& url,
  const base::string16& username_element,
  const base::string16& password_element,
  const base::string16& username,
  const base::string16& password);

void RemoveLogin(
  Profile* profile,
  const std::string& url,
  const base::string16& username_element,
  const base::string16& password_element,
  const base::string16& username);

} // namespace ysp_password_manager

#endif  // CHROME_BROWSER_UI_YSP_PASSWORD_MANAGER_H_

#endif