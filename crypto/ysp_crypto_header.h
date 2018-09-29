// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//ysp+ { crypto http header }

#ifndef CRYPTO_YSP_CRYPTO_HEADER_H_
#define CRYPTO_YSP_CRYPTO_HEADER_H_

#include <string>
#include "crypto/crypto_export.h"

class CRYPTO_EXPORT YSPCryptoHeader {
 public:
  static YSPCryptoHeader * GetInstance();
  void SetBrowserVersion(std::string version);
  void Init(std::string crypto_key);
  std::string EncryptString(const std::string plaintext);
  std::string DecryptString(const std::string ciphertext);
  bool isAddHeaders();
  std::string GetEncString();

 private:
  YSPCryptoHeader(){};
  static YSPCryptoHeader *m_pInstance;
};

#endif //CRYPTO_YSP_CRYPTO_HEADER_H_
