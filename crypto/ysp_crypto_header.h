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
  void SetTimeDiff(const int timeDiff) { timeDiff_ = timeDiff; }
  void Init(std::string crypto_key);
  std::string EncryptString(const std::string plaintext);
  std::string DecryptString(const std::string ciphertext);
  bool isAddHeaders();
  std::string GetEncString();
  std::string GetHMACEncString(const std::string messageType, const std::string uri);

 private:
  YSPCryptoHeader(){};
  static YSPCryptoHeader *m_pInstance;
  int timeDiff_ = 0;
};

#endif //CRYPTO_YSP_CRYPTO_HEADER_H_
