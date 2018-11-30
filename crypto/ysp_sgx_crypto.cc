// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ysp_sgx_crypto.h"

#include "base/logging.h"
#include "crypto/ysp_sgx_support.h"
#include "enclave_u.h"
#include "third_party/sgxsdk/include/sgx_urts.h"

#define ENCLAVE_PATH L"Enclave.signed.dll"

YspSgxCrypto::YspSgxCrypto() : is_sgx_support_(false), sgx_env_id_(0) {
  YspSgxSupport support;
  if (support.GetSgxSupport() == SGX_SUPPORT_ENABLED) {
    is_sgx_support_ = true;
    if (!InitSgxEnv())
      is_sgx_support_ = false;
  }
}

YspSgxCrypto::~YspSgxCrypto() {
  if (IsSuportSgx()) {
    DestroySgxEnv();
  }
}

bool YspSgxCrypto::IsSuportSgx() {
  return is_sgx_support_ && sgx_env_id_ != 0;
}

bool YspSgxCrypto::InitSgxEnv() {
  sgx_launch_token_t token = {0};
  sgx_status_t status;
  int updated = 0;
  status = sgx_create_enclave(ENCLAVE_PATH, SGX_DEBUG_FLAG, &token, &updated,
                              &sgx_env_id_, 0);
  if (status != SGX_SUCCESS)
    return false;
  return true;
}

bool YspSgxCrypto::DestroySgxEnv() {
  sgx_status_t status;
  status = sgx_destroy_enclave(sgx_env_id_);
  if (status != SGX_SUCCESS)
    return false;
  return true;
}

bool YspSgxCrypto::EncryptData(const std::string& plaintext,
                               char*& buffer,
                               int& buffer_size) {
  int content_length = plaintext.length() + 1;
  uint32_t sealed_key_size;
  uint32_t retval = 0;
  sgx_status_t status;
  status = CalculateSealedSize(sgx_env_id_, &retval, &sealed_key_size,
                               content_length * sizeof(char));
  if (sealed_key_size == 0) {
    DLOG(INFO) << "CalculateSealedSize failed";
    return false;
  }
  uint8_t* sealed_key = (uint8_t*)calloc(sealed_key_size, sizeof(uint8_t));
  if (!sealed_key)
    return false;
  status = GenerateSealedData(sgx_env_id_, &retval, (uint8_t*)sealed_key,
                              sealed_key_size, (uint8_t*)plaintext.c_str(),
                              content_length * sizeof(char));
  buffer = (char*)sealed_key;
  buffer_size = sealed_key_size;
  if (status != SGX_SUCCESS) {
    DLOG(INFO) << "GenerateSealedData failed";
    return false;
  }
  return true;
}

bool YspSgxCrypto::DecryptData(const char* buffer,
                               int buffer_size,
                               std::string& plaintext) {
  sgx_status_t status;
  uint32_t retval = 0;
  uint32_t decode_buffer_size;
  status = CalculateUnSealedSize(sgx_env_id_, &retval, &decode_buffer_size,
                                 (uint8_t*)buffer, buffer_size);
  if (decode_buffer_size == UINT32_MAX) {
    DLOG(INFO) << "CalculateUnSealedSize failed";
    return false;
  }
  uint8_t* decode_buffer =
      (uint8_t*)calloc(decode_buffer_size, sizeof(uint8_t));
  status = UnSealingData(sgx_env_id_, &retval, (uint8_t*)decode_buffer,
                         decode_buffer_size, (uint8_t*)buffer, buffer_size);
  if (status != SGX_SUCCESS) {
    DLOG(INFO) << "UnSealingData failed";
    return false;
  }
  plaintext = (char*)decode_buffer;
  free(decode_buffer);
  decode_buffer = NULL;
  return true;
}
