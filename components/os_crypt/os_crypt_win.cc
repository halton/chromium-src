// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/os_crypt/os_crypt.h"

#include <windows.h>

#include "base/lazy_instance.h"
#include "base/strings/utf_string_conversions.h"
#include "crypto/wincrypt_shim.h"
#ifdef REDCORE
#include "crypto/ysp_sgx_crypto.h"
#endif

bool OSCrypt::EncryptString16(const base::string16& plaintext,
                              std::string* ciphertext) {
  return EncryptString(base::UTF16ToUTF8(plaintext), ciphertext);
}

bool OSCrypt::DecryptString16(const std::string& ciphertext,
                              base::string16* plaintext) {
  std::string utf8;
  if (!DecryptString(ciphertext, &utf8))
    return false;

  *plaintext = base::UTF8ToUTF16(utf8);
  return true;
}

bool OSCrypt::EncryptString(const std::string& plaintext,
                            std::string* ciphertext) {
  DATA_BLOB input;
  input.pbData = const_cast<BYTE*>(
      reinterpret_cast<const BYTE*>(plaintext.data()));
  input.cbData = static_cast<DWORD>(plaintext.length());

  DATA_BLOB output;
  BOOL result =
      CryptProtectData(&input, L"", nullptr, nullptr, nullptr, 0, &output);
  if (!result) {
    PLOG(ERROR) << "Failed to encrypt";
    return false;
  }

  // this does a copy
  ciphertext->assign(reinterpret_cast<std::string::value_type*>(output.pbData),
                     output.cbData);

  LocalFree(output.pbData);
  return true;
}

bool OSCrypt::DecryptString(const std::string& ciphertext,
                            std::string* plaintext) {
  DATA_BLOB input;
  input.pbData = const_cast<BYTE*>(
      reinterpret_cast<const BYTE*>(ciphertext.data()));
  input.cbData = static_cast<DWORD>(ciphertext.length());

  DATA_BLOB output;
  BOOL result = CryptUnprotectData(&input, nullptr, nullptr, nullptr, nullptr,
                                   0, &output);
  if (!result) {
    PLOG(ERROR) << "Failed to decrypt";
    return false;
  }

  plaintext->assign(reinterpret_cast<char*>(output.pbData), output.cbData);
  LocalFree(output.pbData);
  return true;
}

#ifdef REDCORE
bool OSCrypt::EncryptString16(const base::string16& plaintext,
                              const std::string& key_text,
                              std::string* ciphertext) {
  return EncryptString(base::UTF16ToUTF8(plaintext), key_text, ciphertext);
}

bool OSCrypt::DecryptString16(const std::string& ciphertext,
                              const std::string& key_text,
                              base::string16* plaintext) {
  std::string utf8;
  if (!DecryptString(ciphertext, key_text, &utf8))
    return false;

  *plaintext = base::UTF8ToUTF16(utf8);
  return true;
}

bool OSCrypt::EncryptString(const std::string& plaintext,
                            const std::string& key_text,
                            std::string* ciphertext) {
  DATA_BLOB input;
  input.pbData =
      const_cast<BYTE*>(reinterpret_cast<const BYTE*>(plaintext.data()));
  input.cbData = static_cast<DWORD>(plaintext.length());

  DATA_BLOB key_input;
  key_input.pbData =
      const_cast<BYTE*>(reinterpret_cast<const BYTE*>(key_text.data()));
  key_input.cbData = static_cast<DWORD>(key_text.length());

  DATA_BLOB output;
  BOOL result =
      CryptProtectData(&input, L"", &key_input, nullptr, nullptr, 0, &output);
  if (!result) {
    PLOG(ERROR) << "Failed to encrypt";
    return false;
  }

  // this does a copy
  ciphertext->assign(reinterpret_cast<std::string::value_type*>(output.pbData),
                     output.cbData);

  LocalFree(output.pbData);
  return true;
}

bool OSCrypt::DecryptString(const std::string& ciphertext,
                            const std::string& key_text,
                            std::string* plaintext) {
  DATA_BLOB input;
  input.pbData =
      const_cast<BYTE*>(reinterpret_cast<const BYTE*>(ciphertext.data()));
  input.cbData = static_cast<DWORD>(ciphertext.length());

  DATA_BLOB key_input;
  key_input.pbData =
      const_cast<BYTE*>(reinterpret_cast<const BYTE*>(key_text.data()));
  key_input.cbData = static_cast<DWORD>(key_text.length());

  DATA_BLOB output;
  BOOL result = CryptUnprotectData(&input, nullptr, &key_input, nullptr,
                                   nullptr, 0, &output);
  if (!result) {
    PLOG(ERROR) << "Failed to decrypt";
    return false;
  }

  plaintext->assign(reinterpret_cast<char*>(output.pbData), output.cbData);
  LocalFree(output.pbData);
  return true;
}

static base::LazyInstance<YspSgxCrypto>::DestructorAtExit g_sgx_crypto =
    LAZY_INSTANCE_INITIALIZER;

bool OSCrypt::IsSupportHardwareCrypto() {
  return g_sgx_crypto.Get().IsSuportSgx();
}

bool OSCrypt::HardwareEncryptString16(const base::string16& plaintext,
                                      std::string* ciphertext) {
  return HardwareEncryptString(base::UTF16ToUTF8(plaintext), ciphertext);
}

bool OSCrypt::HardwareDecryptString16(const std::string& ciphertext,
                                      base::string16* plaintext) {
  std::string utf8;
  if (!HardwareDecryptString(ciphertext, &utf8))
    return false;

  *plaintext = base::UTF8ToUTF16(utf8);
  return true;
}

bool OSCrypt::HardwareEncryptString(const std::string& plaintext,
                                    std::string* ciphertext) {
  if (!IsSupportHardwareCrypto())
    return false;

  char* cipher_buffer = NULL;
  int cipher_buffer_size = 0;
  if (g_sgx_crypto.Get().EncryptData(plaintext, cipher_buffer,
                                     cipher_buffer_size)) {
    ciphertext->assign(cipher_buffer, cipher_buffer_size);
    free(cipher_buffer);
    cipher_buffer = NULL;
    return true;
  }

  return false;
}

bool OSCrypt::HardwareDecryptString(const std::string& ciphertext,
                                    std::string* plaintext) {
  if (IsSupportHardwareCrypto())
    return g_sgx_crypto.Get().DecryptData(ciphertext.data(), ciphertext.length(),
                                          *plaintext);

  PLOG(ERROR) << "Failed to sgx decrypt";
  return false;
}
#endif
