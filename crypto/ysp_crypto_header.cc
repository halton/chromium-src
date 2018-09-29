// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//ysp+ { crypto http header }

#include "base/json/json_reader.h"
#include "base/values.h"
#include "base/md5.h"
#include "base/base64.h"
#include "base/time/time.h"
#include "base/strings/string_number_conversions.h"

#include <openssl/aes.h>
#include "base/strings/string_util.h"
#include "crypto/ysp_crypto_header.h"

#include <map>
#include <set>

namespace cryptoHeaderkey{
    char flags = '0';
    char pwd[9] = "";
    char sal[9] = "";
    char iv[17] = "";
} //namespace cryptoHeaderkey
std::string version_ = "";
namespace {
class HeaderCryptor{
 public:
   HeaderCryptor();
  ~HeaderCryptor();
  bool EncryptString(const std::string& plaintext,
                     std::string* ciphertext);
  bool DecryptString(const std::string& ciphertext,
                     std::string* plaintext);
};

HeaderCryptor::HeaderCryptor() {
}

HeaderCryptor::~HeaderCryptor() {}

bool HeaderCryptor::EncryptString(const std::string& plaintext,
  std::string* ciphertext) {
  AES_KEY aes_key;
  char key_data[17] = {0};
  memcpy(key_data, cryptoHeaderkey::pwd, 8);
  memcpy(key_data + 8, cryptoHeaderkey::sal, 8);
  if (AES_set_encrypt_key(reinterpret_cast<const uint8_t*>(key_data),
    AES_BLOCK_SIZE * 8, &aes_key) != 0) {
    return false;
  }
  const size_t out_size = plaintext.size();
  std::string pla_padding = plaintext;
  int offset = AES_BLOCK_SIZE - out_size % AES_BLOCK_SIZE;
  size_t out_size_new = out_size + offset;
  char padding[AES_BLOCK_SIZE];
  memset(padding, offset, AES_BLOCK_SIZE);
  pla_padding.append(padding, offset);
  std::string result;
  uint8_t* out_ptr =
    reinterpret_cast<uint8_t*>(base::WriteInto(&result, out_size_new + 1));

  uint8_t ivec[AES_BLOCK_SIZE] = { 0 };
  memcpy(ivec, cryptoHeaderkey::iv, AES_BLOCK_SIZE);

  AES_cbc_encrypt(reinterpret_cast<const uint8_t*>(pla_padding.data()), out_ptr,
    pla_padding.size(), &aes_key, ivec, AES_ENCRYPT);
  ciphertext->swap(result);
  return true;
}

bool HeaderCryptor::DecryptString(const std::string& ciphertext,
  std::string* plaintext) {
  AES_KEY aes_key;
  char key_data[17] = { 0 };
  memcpy(key_data, cryptoHeaderkey::pwd, 8);
  memcpy(key_data + 8, cryptoHeaderkey::sal, 8);
  if (AES_set_decrypt_key(reinterpret_cast<const uint8_t*>(key_data),
    AES_BLOCK_SIZE * 8, &aes_key) != 0) {
    return false;
  }
  const size_t out_size = ciphertext.size();
  std::string result;
  uint8_t* out_ptr =
    reinterpret_cast<uint8_t*>(base::WriteInto(&result, out_size + 1));

  uint8_t ivec[AES_BLOCK_SIZE] = { 0 };
  memcpy(ivec, cryptoHeaderkey::iv, AES_BLOCK_SIZE);

  AES_cbc_encrypt(reinterpret_cast<const uint8_t*>(ciphertext.data()), out_ptr,
    ciphertext.size(), &aes_key, ivec, AES_DECRYPT);
  size_t length = result.length();
  int offset = result.c_str()[length - 1];
  plaintext->clear();
  plaintext->assign(result, 0, length - offset);
  return true;
}
} //namespace

YSPCryptoHeader* YSPCryptoHeader::m_pInstance = nullptr;
YSPCryptoHeader * YSPCryptoHeader::GetInstance() {
    if(m_pInstance == nullptr)  //判断是否第一次调用
        m_pInstance = new YSPCryptoHeader();
    return m_pInstance;
}

void YSPCryptoHeader::SetBrowserVersion(std::string version)
{
  version_ = version;
}

void YSPCryptoHeader::Init(std::string crypto_key) {
  if (crypto_key.empty())
    return;
  cryptoHeaderkey::flags = crypto_key.data()[0];
  if (crypto_key.length() >= 9) {
    memcpy(cryptoHeaderkey::pwd, crypto_key.data() + 1, 8);
    if (crypto_key.length() >= 17) {
      memcpy(cryptoHeaderkey::sal, crypto_key.data() + 9, 8);
      if (crypto_key.length() >= 33) {
        memcpy(cryptoHeaderkey::iv, crypto_key.data() + 17, 16);
      } else {
        memcpy(cryptoHeaderkey::iv, "the iv: 16 bytes", 16);
      }
    } else {
      memcpy(cryptoHeaderkey::sal, "ize.com.", 8);
      memcpy(cryptoHeaderkey::iv, "the iv: 16 bytes", 16);
    }
  } else {
    memcpy(cryptoHeaderkey::pwd, "allmobil", 8);
    memcpy(cryptoHeaderkey::sal, "ize.com.", 8);
    memcpy(cryptoHeaderkey::iv, "the iv: 16 bytes", 16);
  }
  //DLOG(INFO) << "cryptokey::pwd:" << cryptokey::pwd << " cryptokey::sal:" << cryptokey::sal << " cryptokey::iv" << cryptokey::iv;
}

std::string YSPCryptoHeader::EncryptString(const std::string plaintext) {
    std::string ciphertext;
  if (!plaintext.empty()) {
    HeaderCryptor Aes_Crypted_;
    Aes_Crypted_.EncryptString(plaintext, &ciphertext);
  }
    return ciphertext;
}
std::string YSPCryptoHeader::DecryptString(const std::string ciphertext) {
    std::string plaintext;
  if (!ciphertext.empty()) {
    HeaderCryptor Aes_Crypted_;
    Aes_Crypted_.DecryptString(ciphertext, &plaintext);
  }
    return plaintext;
}

bool YSPCryptoHeader::isAddHeaders()
{
  if (cryptoHeaderkey::flags == '3')
    return true;
  return false;
}

std::string YSPCryptoHeader::GetEncString()
{
  std::string base64_enc_timeStamp;
  std::string timeStamp = base::Int64ToString(base::Time::Now().ToTimeT());
  std::string enc_timeStamp = EncryptString(timeStamp);

  //DLOG(INFO) << "enc_timeStamp: " << base::HexEncode(enc_timeStamp.c_str(), enc_timeStamp.length());
  base::Base64Encode(enc_timeStamp, &base64_enc_timeStamp);
  DLOG(INFO) << "timeStamp:[" << timeStamp << "] base64_enc_timeStamp: [" << base64_enc_timeStamp << "]";

  //std::string base64_dec_tmp;
  //base::Base64Decode(base64_enc_timeStamp, &base64_dec_tmp);
  //std::string tmp_dec = DecryptString(base64_dec_tmp);
  //DLOG(INFO) << "dec_tmp: [" << tmp_dec << "]";
  return "RedCore/" + version_ + " (" + base64_enc_timeStamp + ")";
}

