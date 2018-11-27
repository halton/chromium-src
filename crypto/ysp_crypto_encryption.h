// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// ysp+ { AES DES and SMS4 crypt }

#ifndef CRYPTO_YSP_CRYPTO_ENCRYPTION_H_
#define CRYPTO_YSP_CRYPTO_ENCRYPTION_H_

#include <string>
#include <vector>
#include "crypto/crypto_export.h"

class CRYPTO_EXPORT YspCryptoSingleton {
 private:
  YspCryptoSingleton():should_encrypt_(false){};
  bool should_encrypt_ ;
  static YspCryptoSingleton* instance_;

 public:
  static YspCryptoSingleton* GetInstance();
  void SetShouldEncrypt();
  bool GetShouldEncrypt();
  void Init(const std::string& crypto_key);
  std::string EncryptString(const std::string& plain_text);
  std::string DecryptString(const std::string& cipher_text);
  void SetUserId(const std::string& id);
  std::string GetUserId();

  void SetPinKeys(const std::vector<std::string>& pin_keys);
  const std::vector<std::string>& GetPinKeys();
  std::string GetPinKey(int index);
  std::string GetCurrentPinKey();
  void UpdateCurrentPinKey(const std::string& pin_key);
  int GetCurrentPinKeyIndex();

 private:
  std::vector<std::string> pin_keys_;
};

#endif  // CRYPTO_YSP_CRYPTO_ENCRYPTION_H_
