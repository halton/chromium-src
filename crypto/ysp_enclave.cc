// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "crypto/ysp_enclave.h"

#include <errno.h>

struct GenerateSealedDataInfo {
  uint32_t retval;
  uint8_t* sealed_data;
  int sealed_data_size;
  uint8_t* public_data;
  int public_data_size;
};

struct UnSealingDataInfo {
  uint32_t retval;
  uint8_t* public_data;
  int public_data_size;
  uint8_t* sealed_data;
  int sealed_data_size;
};

struct CalculateSealedSizeInfo {
  uint32_t retval;
  uint32_t* size;
  int buffer_size;
};

struct CalculateUnSealedSizeInfo {
  uint32_t retval;
  uint32_t* size;
  uint8_t* sealed_data;
  int sealed_data_size;
};

static const struct {
  size_t nr_ocall;
  void* func_addr[1];
} ocall_table_Enclave = {
    0,
    {NULL},
};

sgx_status_t GenerateSealedData(sgx_enclave_id_t eid,
                                uint32_t* retval,
                                uint8_t* sealed_data,
                                int sealed_data_size,
                                uint8_t* public_data,
                                int public_data_size) {
  sgx_status_t status;
  struct GenerateSealedDataInfo ms;
  ms.sealed_data = sealed_data;
  ms.sealed_data_size = sealed_data_size;
  ms.public_data = public_data;
  ms.public_data_size = public_data_size;
  status = sgx_ecall(eid, 0, &ocall_table_Enclave, &ms);
  if (status == SGX_SUCCESS && retval)
    *retval = ms.retval;
  return status;
}

sgx_status_t UnSealingData(sgx_enclave_id_t eid,
                           uint32_t* retval,
                           uint8_t* public_data,
                           int public_data_size,
                           uint8_t* sealed_data,
                           int sealed_data_size) {
  sgx_status_t status;
  struct UnSealingDataInfo ms;
  ms.public_data = public_data;
  ms.public_data_size = public_data_size;
  ms.sealed_data = sealed_data;
  ms.sealed_data_size = sealed_data_size;
  status = sgx_ecall(eid, 1, &ocall_table_Enclave, &ms);
  if (status == SGX_SUCCESS && retval)
    *retval = ms.retval;
  return status;
}

sgx_status_t CalculateSealedSize(sgx_enclave_id_t eid,
                                 uint32_t* retval,
                                 uint32_t* size,
                                 int bufferSize) {
  sgx_status_t status;
  struct CalculateSealedSizeInfo ms;
  ms.size = size;
  ms.buffer_size = bufferSize;
  status = sgx_ecall(eid, 2, &ocall_table_Enclave, &ms);
  if (status == SGX_SUCCESS && retval)
    *retval = ms.retval;
  return status;
}

sgx_status_t CalculateUnSealedSize(sgx_enclave_id_t eid,
                                   uint32_t* retval,
                                   uint32_t* size,
                                   uint8_t* sealed_data,
                                   int sealed_data_size) {
  sgx_status_t status;
  struct CalculateUnSealedSizeInfo ms;
  ms.size = size;
  ms.sealed_data = sealed_data;
  ms.sealed_data_size = sealed_data_size;
  status = sgx_ecall(eid, 3, &ocall_table_Enclave, &ms);
  if (status == SGX_SUCCESS && retval)
    *retval = ms.retval;
  return status;
}
