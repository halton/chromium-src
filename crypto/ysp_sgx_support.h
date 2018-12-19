// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CRYPTO_YSP_SGX_SUPPORT_H_
#define CRYPTO_YSP_SGX_SUPPORT_H_

#include <Windows.h>

#include "third_party/sgxsdk/include/sgx_uae_service.h"

typedef sgx_status_t(SGXAPI* SgxEnableDeviceFunctionHandle)(sgx_device_status_t*);
enum SGX_SUPPORT_TYPE {
  SGX_SUPPORT_UNKNOWN,
  SGX_SUPPORT_NO,
  SGX_SUPPORT_YES,
  SGX_SUPPORT_REBOOT_REQUIRED,
  SGX_SUPPORT_ENABLE_REQUIRED,
  SGX_SUPPORT_ENABLED
};

class YspSgxSupport {
 public:
  YspSgxSupport();
  ~YspSgxSupport();
  SGX_SUPPORT_TYPE GetSgxSupport();
  sgx_status_t EnableDevice(sgx_device_status_t* device_status);

 private:
  bool IsPswInstalled();
  void CheckSgxSupport();
  void LoadFunctions();

  SGX_SUPPORT_TYPE support_type_;
  HINSTANCE dll_handle_;

  SgxEnableDeviceFunctionHandle enable_device_handle_;
};
#endif  // !CRYPTO_YSP_SGX_SUPPORT_H_
