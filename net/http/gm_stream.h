// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_HTTP_GM_STREAM_H_
#define NET_HTTP_GM_STREAM_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#include "base/callback_helpers.h"
#include "base/compiler_specific.h"
#include "base/logging.h"
#include "net/base/completion_callback.h"
#include "net/base/io_buffer.h"

typedef void* GM_CONTEXT;
typedef void* GM_STREAM;
typedef void* GM_SESSION;
typedef void* GM_CERT;
typedef void* GM_CERT_STROE_CONTEXT;

#define GM_VERIFY_NONE 0
#define GM_VERIFY_PEER 0x01 | 0x02

#define GM_VERIFY_FAIL 0
#define GM_VERIFY_OK 1

#define GMERR_OK (0)

#define GMERR_NOMEM (-1)
#define GMERR_INVALPARAM (-2)

#define GMERR_INVALCERT (-3)
#define GMERR_INVALKEY (-4)

#define GMERR_STREAM_BROKEN (-5)
#define GMERR_STREAM_WOULDBLOCK (-6)

#ifndef _Out
#define _Out
#endif

#ifndef _In
#define _In
#endif

#ifndef _In_opt_
#define _In_opt_
#endif

#ifndef UINT
typedef unsigned int UINT;
#endif

#ifdef WIN32
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef BOOL
typedef int BOOL;
#endif
#else
#ifndef BOOL
typedef enum { FALSE = 0, TRUE = 1 } BOOL;
#endif
#endif
#define GMBUFFERLEN 64
#define CERTIFICATES_PATH ".\\"
#define SANGFORDLL "sangfor.dll"

typedef int (*GM_CERT_VERIFY_CALLBACK)(_In int nVerifyOk,
                                       _In GM_CERT_STROE_CONTEXT
                                           gmCertStroeContext);

typedef int (*GM_STREAM_ERROR_CALLBACK)(_In const char* c_szErrorString,
                                        _In size_t nErrorStringLength,
                                        _In void* pvUserData);

/******************************************************************************/

#ifdef WIN32

#ifdef __cplusplus
extern "C" {
#endif

extern int (*GM_init)();

extern int (*GMContextCreate)(_Out GM_CONTEXT* pgmContext);

extern int (*GMContextSetSignatureCertificate)(_In GM_CONTEXT context_,
                                               _In const char* c_szCertificate,
                                               _In const char* c_szKey);

extern int (*GMContextSetEncryptionCertificate)(_In GM_CONTEXT context_,
                                                _In const char* c_szCertificate,
                                                _In const char* c_szKey);

extern int (*GMContextSetVerify)(_In GM_CONTEXT context_,
                                 _In UINT uVerifyMode,
                                 _In_opt_ GM_CERT_VERIFY_CALLBACK fnCallback,
                                 _In_opt_ const char* c_szCACertificate,
                                 _In_opt_ UINT uVerifyDepth);

extern int (*GMContextSetCipherList)(_In GM_CONTEXT context_,
                                     _In const char* c_szCipherList);

extern void (*GMContextDestroy)(_In GM_CONTEXT context_);

/******************************************************************************/

extern int (*GMStreamCreate)(_Out GM_STREAM* pgmStream,
                             _In GM_CONTEXT context_);

extern int (*GMStreamSetUserData)(_In GM_STREAM stream_, _In void* pvUserData);

extern int (*GMStreamGetUserData)(_In GM_STREAM stream_,
                                  _Out void** ppvUserData);

extern int (*GMStreamSetHostName)(_In GM_STREAM stream_,
                                  _In const char* c_szHostName);

extern int (*GMStreamSetSocket)(_In GM_STREAM stream_, _In int nSocket);

extern int (*GMStreamGetPeerCertificate)(_In GM_STREAM stream_,
                                         _Out GM_CERT* pgmCert);

extern int (*GMStreamInHandshake)(_In GM_STREAM stream_, _Out BOOL* pbState);

extern int (*GMStreamGetError)(_In GM_STREAM stream_,
                               _Out int* pnErrorReason,
                               _In_opt_ GM_STREAM_ERROR_CALLBACK fnCallback,
                               _In_opt_ void* pvUserData);

extern int (*GMStreamConnect)(_In GM_STREAM stream_);

extern int (*GMStreamWrite)(_In GM_STREAM stream_,
                            _In const void* pcvBuffer,
                            _In UINT uBufferSize);

extern int (*GMStreamRead)(_In GM_STREAM stream_,
                           _Out void* pvBuffer,
                           _In UINT uBufferSize);

extern int (*GMStreamShutdown)(_In GM_STREAM stream_);

extern void (*GMStreamDestroy)(_In GM_STREAM stream_);

/******************************************************************************/

extern int (*GMStreamGetSession)(_In GM_STREAM stream_,
                                 _Out GM_SESSION* pgmSession);

extern int (*GMStreamSetSession)(_In GM_STREAM stream_,
                                 _In GM_SESSION gmSession);

extern void (*GMSessionDestroy)(_In GM_SESSION gmSession);

/******************************************************************************/

extern int (*GMCertStoreContextGetStream)(_Out GM_STREAM* pgmStream,
                                          _In GM_CERT_STROE_CONTEXT
                                              gmCertStoreContext);

extern int (*GMCertStoreContextGetError)(_In GM_CERT_STROE_CONTEXT
                                             gmCertStoreContext,
                                         _Out int* pnErrorNumber);

extern char* (*GMCertGetErrorString)(_In int nError);

extern int (*GMCertStoreContextGetErrorDepth)(_In GM_CERT_STROE_CONTEXT
                                                  gmCertStoreContext,
                                              _Out int* pnErrorDepth);

extern int (*GMCertStoreContextGetCurrentCert)(_In GM_CERT_STROE_CONTEXT
                                                   gmCertStoreContext,
                                               _Out GM_CERT* pgmCert);

extern void (*GMCertDestroy)(_In GM_CERT gmCert);

#ifdef __cplusplus
}
#endif

#endif  // WIN32

class GmStream {
 public:
  GmStream();
  ~GmStream();
  int Init();
  int ReadResponseData(net::IOBuffer* buffer, int length);
  int ReadResponseData(net::IOBuffer* buffer,
                       int length,
                       const net::CompletionCallback& callback);
  int WriteRequestData(const char* request,
                       int length,
                       const net::CompletionCallback& callback);
  int Handshake();
  int CreateDataStream(int client);
  GM_CONTEXT GetContext() { return context_; }
  GM_STREAM GetStream() { return stream_; }
  void Destroy();
  HMODULE module = NULL;

 private:
  int SocketLibStartup();
  int LoadDll(const char* filename);
  GM_CONTEXT GMContextInit();

  GM_CONTEXT context_;
  GM_STREAM stream_;

  static GM_SESSION session_;
};

#endif  // NET_HTTP_GM_STREAM_H_
