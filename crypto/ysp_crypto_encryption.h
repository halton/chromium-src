// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//ysp+ { AES DES and SMS4 crypt }

#ifndef CRYPTO_YSP_CRYPTO_ENCRYPTION_H_
#define CRYPTO_YSP_CRYPTO_ENCRYPTION_H_

#include <string>
#include "crypto/crypto_export.h"

class CRYPTO_EXPORT YSPCryptoCSingleton
{
    private:
        YSPCryptoCSingleton(){};
    bool should_encrypt = false;
        static YSPCryptoCSingleton *m_pInstance;
    public:
        static YSPCryptoCSingleton * GetInstance();
    void SetShouldEncrypt();
    bool GetShouldEncrypt();
        void Init(std::string crypto_key);
        std::string EncryptString(const std::string plaintext);
        std::string DecryptString(const std::string ciphertext);
        void SetUserId(const std::string id);
        std::string GetUserId();
};

#endif //CRYPTO_YSP_CRYPTO_ENCRYPTION_H_
