// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/password_manager/core/browser/login_database.h"

#include "components/os_crypt/os_crypt.h"

namespace password_manager {

LoginDatabase::EncryptionResult LoginDatabase::EncryptedString(
    const base::string16& plain_text,
    std::string* cipher_text) const {
  return OSCrypt::EncryptString16(plain_text, cipher_text)
             ? ENCRYPTION_RESULT_SUCCESS
             : ENCRYPTION_RESULT_SERVICE_FAILURE;
}

LoginDatabase::EncryptionResult LoginDatabase::DecryptedString(
    const std::string& cipher_text,
    base::string16* plain_text) const {
  return OSCrypt::DecryptString16(cipher_text, plain_text)
             ? ENCRYPTION_RESULT_SUCCESS
             : ENCRYPTION_RESULT_SERVICE_FAILURE;
}

#ifdef REDCORE
LoginDatabase::EncryptionResult LoginDatabase::YspEncryptedString(
    const base::string16& plain_text,
    int& key_index,
    std::string* cipher_text) const {
  // TODO(sunlm04)
  return EncryptionResult::ENCRYPTION_RESULT_HARDWARE_SUCCESS;
}

LoginDatabase::EncryptionResult LoginDatabase::YspDecryptedString(
    const std::string& cipher_text,                                      
    int key_index,
    base::string16* plain_text,
    bool hardware_crypto) const {
  // TODO(sunlm04)
  return EncryptionResult::ENCRYPTION_RESULT_HARDWARE_SUCCESS;  
}
#endif  // REDCORE

}  // namespace password_manager
