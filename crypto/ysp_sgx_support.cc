// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "crypto/ysp_sgx_support.h"

#include <tchar.h>

YspSgxSupport::YspSgxSupport() : support_type_(SGX_SUPPORT_UNKNOWN) {}

YspSgxSupport::~YspSgxSupport() {}

int YspSgxSupport::IsPswInstalled() {
  if ((dll_handle_ = LoadLibrary(_T("sgx_uae_service.dll"))) == NULL) {
    return 0;
  }
  LoadFunctions();
  return 1;
}

void YspSgxSupport::CheckSgxSupport() {
  sgx_device_status_t sgx_device_status;
  if (support_type_ != SGX_SUPPORT_UNKNOWN)
    return;

  support_type_ = SGX_SUPPORT_NO;
  if (!IsPswInstalled())
    return;

  support_type_ = SGX_SUPPORT_YES;
  if (this->EnableDevice(&sgx_device_status) != SGX_SUCCESS)
    return;

  if (sgx_device_status != SGX_ENABLED) {
    switch (sgx_device_status) {
      case SGX_DISABLED_REBOOT_REQUIRED:
        support_type_ = SGX_SUPPORT_REBOOT_REQUIRED;
        break;
      case SGX_DISABLED_LEGACY_OS:
        support_type_ = SGX_SUPPORT_ENABLE_REQUIRED;
        break;
      default:
        support_type_ = SGX_SUPPORT_NO;
    }
    return;
  }
  support_type_ = SGX_SUPPORT_ENABLED;
}

void YspSgxSupport::LoadFunctions() {
  enable_device_handle_ = (SgxEnableDeviceFunctionHandle)GetProcAddress(
      dll_handle_, "sgx_enable_device");
}

sgx_status_t YspSgxSupport::EnableDevice(sgx_device_status_t* device_status) {
  if (enable_device_handle_ == NULL) {
    return SGX_ERROR_UNEXPECTED;
  }
  return enable_device_handle_(device_status);
}

SGX_SUPPORT_TYPE YspSgxSupport::GetSgxSupport() {
  CheckSgxSupport();
  return support_type_;
}
