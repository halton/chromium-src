// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CRYPTO_YSP_SGX_CRYPTO_H_
#define CRYPTO_YSP_SGX_CRYPTO_H_

#include <string>

#include "crypto/crypto_export.h"

class CRYPTO_EXPORT YspSgxCrypto {
 public:
  YspSgxCrypto();
  ~YspSgxCrypto();
  bool IsSuportSgx();

  bool InitSgxEnv();
  bool DestroySgxEnv();

  bool EncryptData(const std::string& plaintext,
                   char*& buffer,
                   int& buffer_size);
  bool DecryptData(const char* buffer, int buffer_size, std::string& plaintext);

 private:
  bool is_sgx_support_;
  uint64_t sgx_env_id_;
};
#endif
