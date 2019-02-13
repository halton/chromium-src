// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/password_manager/core/browser/login_database.h"

#include "base/strings/string_util.h"
#include "components/os_crypt/os_crypt.h"
#ifdef REDCORE
#include "crypto/ysp_crypto_encryption.h"
#endif  // REDCORE

namespace password_manager {
#ifdef REDCORE
LoginDatabase::EncryptionResult LoginDatabase::YspEncryptedString(
    const base::string16& plain_text,
    int& key_index,
    std::string* cipher_text) const {
  if (OSCrypt::IsSupportHardwareCrypto()) {
    if (OSCrypt::HardwareEncryptString16(plain_text, cipher_text))
      return ENCRYPTION_RESULT_HARDWARE_SUCCESS;
  }

  key_index = YspCryptoSingleton::GetInstance()->GetCurrentPinKeyIndex();
  if (OSCrypt::EncryptString16(
          plain_text, YspCryptoSingleton::GetInstance()->GetCurrentPinKey(),
          cipher_text))
    return ENCRYPTION_RESULT_SUCCESS;

  return ENCRYPTION_RESULT_ITEM_FAILURE;
}

LoginDatabase::EncryptionResult LoginDatabase::YspDecryptedString(
    const std::string& cipher_text,
    int key_index,
    base::string16* plain_text,
    bool hardware_crypto) const {

  if (hardware_crypto) {
    if (OSCrypt::HardwareDecryptString16(cipher_text, plain_text))
      return ENCRYPTION_RESULT_SUCCESS;
  } else {
    if ((key_index == -1) &&
        (OSCrypt::DecryptString16(cipher_text, plain_text)))
      return ENCRYPTION_RESULT_SUCCESS;

    if (OSCrypt::DecryptString16(
            cipher_text,
            YspCryptoSingleton::GetInstance()->GetPinKey(key_index),
            plain_text))
      return ENCRYPTION_RESULT_SUCCESS;
  }

  return ENCRYPTION_RESULT_ITEM_FAILURE;
}
#endif  // REDCORE
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

}  // namespace password_manager
