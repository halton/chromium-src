// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CRYPTO_YSP_ENCLAVE_H_
#define CRYPTO_YSP_ENCLAVE_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>

#include "third_party/sgxsdk/include/sgx_edger8r.h" /* for sgx_status_t etc. */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

sgx_status_t GenerateSealedData(sgx_enclave_id_t eid,
                                uint32_t* retval,
                                uint8_t* sealed_data,
                                int sealed_data_size,
                                uint8_t* public_data,
                                int public_data_size);

sgx_status_t UnSealingData(sgx_enclave_id_t eid,
                           uint32_t* retval,
                           uint8_t* public_data,
                           int public_data_size,
                           uint8_t* sealed_data,
                           int sealed_data_size);

sgx_status_t CalculateSealedSize(sgx_enclave_id_t eid,
                                 uint32_t* retval,
                                 uint32_t* size,
                                 int bufferSize);

sgx_status_t CalculateUnSealedSize(sgx_enclave_id_t eid,
                                   uint32_t* retval,
                                   uint32_t* size,
                                   uint8_t* sealed_data,
                                   int sealed_data_size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
