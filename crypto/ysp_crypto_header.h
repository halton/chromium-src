// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//ysp+ { crypto http header }

#ifndef CRYPTO_YSP_CRYPTO_HEADER_H_
#define CRYPTO_YSP_CRYPTO_HEADER_H_

#include <string>
#include "crypto/crypto_export.h"

class CRYPTO_EXPORT YspCryptoHeader {
 public:
  static YspCryptoHeader* GetInstance();
  void SetTimeDiff(const int time_diff) { time_diff_ = time_diff; }
  void Init(const std::string& crypto_key);
  std::string EncryptString(const std::string& plain_text);
  std::string DecryptString(const std::string& cipher_text);
  bool isAddHeaders();
  std::string GetEncString();
  std::string GetHmacEncString(const std::string& message_type,
                               const std::string& uri,
                               const std::string& version = "1");

 private:
  YspCryptoHeader():time_diff_(0){};
  static YspCryptoHeader* instance_;
  int time_diff_;
};

#endif //CRYPTO_YSP_CRYPTO_HEADER_H_
