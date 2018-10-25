// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/http/gm_stream.h"

#include <windows.h>
#include "base/strings/utf_string_conversions.h"

static std::string s_gm_dir = "";

// TODO (halton): Use chrome::path instead
std::string GetGmDir() {
  wchar_t path[MAX_PATH + 1] = {0};
  HMODULE chrome_dll_handle = LoadLibrary(L"chrome_child.dll");
  if (chrome_dll_handle == NULL) {
    chrome_dll_handle = LoadLibrary(L"chrome.dll");

    if (chrome_dll_handle == NULL)
      return nullptr;
  }
  GetModuleFileName(chrome_dll_handle, path, MAX_PATH);
  FreeLibrary(chrome_dll_handle);

  wchar_t driver[_MAX_DRIVE + 1] = {0};
  wchar_t dir[_MAX_DIR + 1] = {0};
  wchar_t fname[_MAX_FNAME + 1] = {0};
  wchar_t ext[_MAX_EXT + 1] = {0};

  ::_wsplitpath_s(path, driver, dir, fname, ext);
  std::wstring gm_path = std::wstring(driver) + dir + L"gm\\";

  return base::UTF16ToUTF8(gm_path);
}

GM_SESSION GmStream::session_ = NULL;
GmStream::GmStream() : context_(NULL), stream_(NULL) {
  s_gm_dir = GetGmDir();
}

GmStream::~GmStream() {
  Destroy();
}

void GmStream::Destroy() {
  if (stream_) {
    GMStreamShutdown(stream_);
    GMStreamDestroy(stream_);
    stream_ = NULL;
  }

  if (context_) {
    GMContextDestroy(context_);
    context_ = NULL;
  }
}

int GmStream::Init() {
  if (SocketLibStartup() < 0) {
    DLOG(INFO) << "WSA library init failure !";
    return -1;
  }

  std::string gm_dll_path = s_gm_dir + "gmcrypto.dll";
  if (LoadDll(gm_dll_path.c_str()) < 0) {
    DLOG(INFO) << "Load GMlibrary failure !";
    return -1;
  }

  // 初始化国密库函数
  (void)GM_init();
  return 0;
}

/**
* 初始化 windows平台的 WSA 库
* @param void
* @return 成功: 0
失败: -1
*/
int GmStream::SocketLibStartup() {
  WORD socket_version = MAKEWORD(2, 2);
  WSADATA data;

  if (0 != WSAStartup(socket_version, &data))
    return -1;

  return 0;
}

/**
* 加载国密库 DLL 的接口函数
* @param filename: DLL 库文件路径
* @return 成功: 0
失败: -1
*/
int GmStream::LoadDll(const char* filename) {
  if (NULL == filename) {
    DLOG(INFO) << "GMLibrary file name is empty !";
    return -1;
  }

  module = LoadLibraryA(filename);
  if (module == NULL) {
    DLOG(INFO) << "Load GMlibrary failure ! Lost " << filename
               << " file. Please try reinstalling the app.";
    return -1;
  }

  GM_init = (int (*)())GetProcAddress(module, "GM_init");

  // TODO(halton): Why use function type conversion?
  GMContextCreate = (int (*)(_Out GM_CONTEXT * pgmContext))
      GetProcAddress(module, "GMContextCreate");

  GMContextSetSignatureCertificate =
      (int (*)(_In GM_CONTEXT context_, _In const char* c_szCertificate,
               _In const char* c_szKey))
          GetProcAddress(module, "GMContextSetSignatureCertificate");

  GMContextSetEncryptionCertificate =
      (int (*)(_In GM_CONTEXT context_, _In const char* c_szCertificate,
               _In const char* c_szKey))
          GetProcAddress(module, "GMContextSetEncryptionCertificate");

  GMContextSetVerify = (int (*)(
      _In GM_CONTEXT context_, _In UINT uVerifyMode,
      _In_opt_ GM_CERT_VERIFY_CALLBACK fnCallback,
      _In_opt_ const char* c_szCACertificate,
      _In_opt_ UINT uVerifyDepth))GetProcAddress(module, "GMContextSetVerify");

  GMContextSetCipherList =
      (int (*)(_In GM_CONTEXT context_, _In const char* c_szCipherList))
          GetProcAddress(module, "GMContextSetCipherList");

  GMContextDestroy = (void (*)(_In GM_CONTEXT context_))GetProcAddress(
      module, "GMContextDestroy");

  /******************************************************************************/

  GMStreamCreate =
      (int (*)(_Out GM_STREAM * pgmStream, _In GM_CONTEXT context_))
          GetProcAddress(module, "GMStreamCreate");

  GMStreamSetUserData =
      (int (*)(_In GM_STREAM stream_, _In void* pvUserData))GetProcAddress(
          module, "GMStreamSetUserData");

  GMStreamGetUserData =
      (int (*)(_In GM_STREAM stream_, _Out void** ppvUserData))GetProcAddress(
          module, "GMStreamGetUserData");

  GMStreamSetHostName =
      (int (*)(_In GM_STREAM stream_, _In const char* c_szHostName))
          GetProcAddress(module, "GMStreamSetHostName");

  GMStreamSetSocket =
      (int (*)(_In GM_STREAM stream_, _In int nSocket))GetProcAddress(
          module, "GMStreamSetSocket");

  GMStreamGetPeerCertificate =
      (int (*)(_In GM_STREAM stream_, _Out GM_CERT * pgmCert))
          GetProcAddress(module, "GMStreamGetPeerCertificate");

  GMStreamInHandshake = (int (*)(_In GM_STREAM stream_, _Out BOOL * pbState))
      GetProcAddress(module, "GMStreamInHandshake");

  GMStreamGetError = (int (*)(
      _In GM_STREAM stream_, _Out int* pnErrorReason,
      _In_opt_ GM_STREAM_ERROR_CALLBACK fnCallback,
      _In_opt_ void* pvUserData))GetProcAddress(module, "GMStreamGetError");

  GMStreamConnect =
      (int (*)(_In GM_STREAM stream_))GetProcAddress(module, "GMStreamConnect");

  GMStreamWrite =
      (int (*)(_In GM_STREAM stream_, _In const void* pcvBuffer,
               _In UINT uBufferSize))GetProcAddress(module, "GMStreamWrite");

  GMStreamRead =
      (int (*)(_In GM_STREAM stream_, _Out void* pvBuffer,
               _In UINT uBufferSize))GetProcAddress(module, "GMStreamRead");

  GMStreamShutdown = (int (*)(_In GM_STREAM stream_))GetProcAddress(
      module, "GMStreamShutdown");

  GMStreamDestroy = (void (*)(_In GM_STREAM stream_))GetProcAddress(
      module, "GMStreamDestroy");

  /******************************************************************************/

  GMStreamGetSession =
      (int (*)(_In GM_STREAM stream_, _Out GM_SESSION * pgmSession))
          GetProcAddress(module, "GMStreamGetSession");

  GMStreamSetSession =
      (int (*)(_In GM_STREAM stream_, _In GM_SESSION gmSession))GetProcAddress(
          module, "GMStreamSetSession");

  GMSessionDestroy = (void (*)(_In GM_SESSION gmSession))GetProcAddress(
      module, "GMSessionDestroy");

  /******************************************************************************/

  GMCertStoreContextGetStream = (int (*)(
      _Out GM_STREAM * pgmStream, _In GM_CERT_STROE_CONTEXT gmCertStoreContext))
      GetProcAddress(module, "GMCertStoreContextGetStream");

  GMCertStoreContextGetError = (int (*)(
      _In GM_CERT_STROE_CONTEXT gmCertStoreContext,
      _Out int* pnErrorNumber))GetProcAddress(module,
                                              "GMCertStoreContextGetError");

  GMCertGetErrorString =
      (char* (*)(_In int nError))GetProcAddress(module, "GMCertGetErrorString");

  GMCertStoreContextGetErrorDepth = (int (*)(
      _In GM_CERT_STROE_CONTEXT gmCertStoreContext,
      _Out int* pnErrorDepth))GetProcAddress(module,
                                             "GMCertStoreContextGetErrorDepth");

  GMCertStoreContextGetCurrentCert = (int (*)(
      _In GM_CERT_STROE_CONTEXT gmCertStoreContext, _Out GM_CERT * pgmCert))
      GetProcAddress(module, "GMCertStoreContextGetCurrentCert");

  GMCertDestroy =
      (void (*)(_In GM_CERT gmCert))GetProcAddress(module, "GMCertDestroy");

  if (!GM_init || !GMContextCreate || !GMContextSetSignatureCertificate ||
      !GMContextSetEncryptionCertificate || !GMContextSetVerify ||
      !GMContextSetCipherList || !GMStreamCreate || !GMStreamSetSocket ||
      !GMStreamConnect || !GMStreamWrite || !GMStreamRead ||
      !GMStreamGetSession || !GMStreamSetSession || !GMStreamShutdown ||
      !GMSessionDestroy || !GMContextDestroy || !GMStreamDestroy ||
      !GMStreamSetUserData || !GMStreamGetUserData || !GMStreamSetHostName ||
      !GMStreamGetPeerCertificate || !GMStreamInHandshake ||
      !GMStreamGetError || !GMCertStoreContextGetStream ||
      !GMCertStoreContextGetError || !GMCertGetErrorString ||
      !GMCertStoreContextGetErrorDepth || !GMCertStoreContextGetCurrentCert ||
      !GMCertDestroy) {
    return -1;
  }
  return 0;
}

/**
* GM_CONTEXT 配置模板对象的初始化，包括
* 1. 设置签名证书
* 2. 设置加密证书
* 3. 设置服务器认证方式
* 4. 设置支持的算法列表
* @param void
* @return 成功: GM_CONTEXT 配置模板对象
失败: NULL
*/
GM_CONTEXT GmStream::GMContextInit() {
  GM_CONTEXT context_ = NULL;
  std::string ca_cert_path = s_gm_dir + "gmcert\\ca_cert.pem";
  std::string signature_cert_path = s_gm_dir + "gmcert\\signature_cert.pem";
  std::string signature_key_path = s_gm_dir + "gmcert\\signature_key.pem";
  std::string encryption_cert_path = s_gm_dir + "gmcert\\encryption_cert.pem";
  std::string encryption_key_path = s_gm_dir + "gmcert\\encryption_key.pem";

  // 创建 GM_CONTEXT 配置模板对象
  if (GMERR_OK != GMContextCreate(&context_)) {
    DLOG(INFO) << "GMContextCreate failure !";
    goto err;
  }

  // 设置签名证书
  if (GMERR_OK !=
      GMContextSetSignatureCertificate(context_, signature_cert_path.c_str(),
                                       signature_key_path.c_str())) {
    DLOG(INFO) << "GMContextSetSignatureCertificate failure !";
    goto err;
  }

  // 设置加密证书
  if (GMERR_OK !=
      GMContextSetEncryptionCertificate(context_, encryption_cert_path.c_str(),
                                        encryption_key_path.c_str())) {
    DLOG(INFO) << "GMContextSetEncryptionCertificate failure !";
    goto err;
  }

  // 设置服务器认证方式
  if (GMERR_OK !=
      GMContextSetVerify(
          context_,
          GM_VERIFY_NONE,  // uVerifyMode, 不认证服务器身份 -- GM_VERIFY_PEER
          NULL,  // fnCallback, 使用国密库内部实现的认证方式
          ca_cert_path.c_str(),  // c_szCACertificate, CA 证书文件路径
          10                     // uVerifyDepth, 最大证书认证深度
          )) {
    DLOG(INFO) << "GMContextSetVerify failure !";
    goto err;
  }

  // 设置支持的算法列表
  if (GMERR_OK !=
      GMContextSetCipherList(context_, "ECC-SM2-SM4-SM3:ECDHE-SM2-SM4-SM3")) {
    DLOG(INFO) << "GMContextSetCipherList failure !";
    goto err;
  }

  return context_;
err:
  if (context_) {
    (void)GMContextDestroy(context_);
    context_ = NULL;
  }
  return NULL;
}

/**
 * 从国密数据流，收取响应数据
 * @param   buffer: 存储响应数据的buf
 * @param   length:   要接收的数据长度
 * @param   callback:   接收完回调函数
 * @return  成功返回读取的字节数，失败返回-1
 */
int GmStream::ReadResponseData(net::IOBuffer* buffer, int length) {
  if (stream_ == NULL) {
    DLOG(INFO) << "stream_ is NULL !";
    return -1;
  }

  //为防止第一次拿不到响应数据导致浏览器直接返回，临时睡眠200毫秒
  Sleep(200);

  int nReturn = 0, offset = 0;
  char cBuffer[GMBUFFERLEN] = {0};
  while ((nReturn = GMStreamRead(stream_, cBuffer, GMBUFFERLEN - 1)) > 0) {
    cBuffer[nReturn] = '\0';
    memcpy(buffer->data() + offset, cBuffer, nReturn);
    offset += nReturn;
    // DLOG(INFO) << "ResponseData: " << ResponseData->c_str();
    if (nReturn < GMBUFFERLEN - 1)
      break;
    if ((offset + GMBUFFERLEN - 1) > length)
      break;
    DLOG(INFO) << "nReturn: " << nReturn;
  }
  // if (NULL == session_)
  //  (void)GMStreamGetSession(stream_, &session_);
  // DLOG(INFO) << "nReturn: " << nReturn ;

  return offset;
}

int GmStream::ReadResponseData(net::IOBuffer* buffer,
                               int length,
                               const net::CompletionCallback& callback) {
  if (stream_ == NULL) {
    DLOG(INFO) << "stream_ is NULL !";
    return -1;
  }

  int nReturn = 0, offset = 0;
  char cBuffer[GMBUFFERLEN] = {0};
  while ((nReturn = GMStreamRead(stream_, cBuffer, GMBUFFERLEN - 1)) > 0) {
    cBuffer[nReturn] = '\0';
    memcpy(buffer->data() + offset, cBuffer, nReturn);
    offset += nReturn;
    // DLOG(INFO) << "ResponseData: " << ResponseData->c_str();
    if (nReturn < GMBUFFERLEN - 1)
      break;
    if ((offset + GMBUFFERLEN - 1) > length)
      break;
  }

  return offset;
}

/**
 * 发送业务数据
 * @param   request: 要发送的请求数据
 * @param   callback:   发送完回调函数
 * @return  成功返回写入的字节数，失败返回-1
 */
int GmStream::WriteRequestData(const char* request,
                               int length,
                               const net::CompletionCallback& callback) {
  // 发送业务数据请求
  while (GMERR_OK != GMStreamWrite(stream_, request, length)) {
    DLOG(INFO) << "GMStreamWrite failure !";
    return -1;
  }
  return length;
}

/**
 * 国密握手
 * @return  成功返回0，失败返回-1
 */
int GmStream::Handshake() {
  BOOL pbState = FALSE;

  if (GMERR_OK == (GMStreamInHandshake(stream_, &pbState))) {
    if (pbState)
      return 0;
  }

  // 进行国密协议握手协商
  if (GMERR_OK != GMStreamConnect(stream_)) {
    DLOG(INFO) << "GMStreamConnect failure !";
    return -1;
  }
  return 0;
}

/**
 * 对于给定的 Socket 对象，
 * 创建国密数据流，进行收发业务数据
 * @param   client:   Socket 对象
 * @return  成功返回0，失败返回-1
 */
int GmStream::CreateDataStream(int client) {
  if (Init() < 0)
    return -1;

  // 创建 GM_CONTEXT 配置模板对象
  context_ = GMContextInit();
  if (NULL == context_) {
    DLOG(INFO) << "GMContextInit failure !";
    return -1;
  }
  if (NULL == context_) {
    DLOG(INFO) << "context_ is NULL !";
    return -1;
  }

  // 根据 GM_CONTEXT 配置模板对象配置，创建 GM_STREAM 国密数据流对象
  if (GMERR_OK != GMStreamCreate(&stream_, context_)) {
    DLOG(INFO) << "GMStreamCreate failure !";
    return -1;
  }

  if (NULL != session_)
    (void)GMStreamSetSession(stream_, session_);

  // 为 GM_STREAM 国密数据流绑定 Socket 对象
  if (GMERR_OK != GMStreamSetSocket(stream_, client)) {
    DLOG(INFO) << "GMStreamSetSocket failure !";
    return -1;
  }
  return 0;
}

int (*GM_init)();

int (*GMContextCreate)(_Out GM_CONTEXT* pgmContext);

int (*GMContextSetSignatureCertificate)(_In GM_CONTEXT context_,
                                        _In const char* c_szCertificate,
                                        _In const char* c_szKey);

int (*GMContextSetEncryptionCertificate)(_In GM_CONTEXT context_,
                                         _In const char* c_szCertificate,
                                         _In const char* c_szKey);

int (*GMContextSetVerify)(_In GM_CONTEXT context_,
                          _In UINT uVerifyMode,
                          _In_opt_ GM_CERT_VERIFY_CALLBACK fnCallback,
                          _In_opt_ const char* c_szCACertificate,
                          _In_opt_ UINT uVerifyDepth);

int (*GMContextSetCipherList)(_In GM_CONTEXT context_,
                              _In const char* c_szCipherList);

void (*GMContextDestroy)(_In GM_CONTEXT context_);

/******************************************************************************/

int (*GMStreamCreate)(_Out GM_STREAM* pgmStream, _In GM_CONTEXT context_);

int (*GMStreamSetUserData)(_In GM_STREAM stream_, _In void* pvUserData);

int (*GMStreamGetUserData)(_In GM_STREAM stream_, _Out void** ppvUserData);

int (*GMStreamSetHostName)(_In GM_STREAM stream_, _In const char* c_szHostName);

int (*GMStreamSetSocket)(_In GM_STREAM stream_, _In int nSocket);

int (*GMStreamGetPeerCertificate)(_In GM_STREAM stream_, _Out GM_CERT* pgmCert);

int (*GMStreamInHandshake)(_In GM_STREAM stream_, _Out BOOL* pbState);

int (*GMStreamGetError)(_In GM_STREAM stream_,
                        _Out int* pnErrorReason,
                        _In_opt_ GM_STREAM_ERROR_CALLBACK fnCallback,
                        _In_opt_ void* pvUserData);

int (*GMStreamConnect)(_In GM_STREAM stream_);

int (*GMStreamWrite)(_In GM_STREAM stream_,
                     _In const void* pcvBuffer,
                     _In UINT uBufferSize);

int (*GMStreamRead)(_In GM_STREAM stream_,
                    _Out void* pvBuffer,
                    _In UINT uBufferSize);

int (*GMStreamShutdown)(_In GM_STREAM stream_);

void (*GMStreamDestroy)(_In GM_STREAM stream_);

/******************************************************************************/

int (*GMStreamGetSession)(_In GM_STREAM stream_, _Out GM_SESSION* pgmSession);

int (*GMStreamSetSession)(_In GM_STREAM stream_, _In GM_SESSION gmSession);

void (*GMSessionDestroy)(_In GM_SESSION gmSession);

/******************************************************************************/

int (*GMCertStoreContextGetStream)(_Out GM_STREAM* pgmStream,
                                   _In GM_CERT_STROE_CONTEXT
                                       gmCertStoreContext);

int (*GMCertStoreContextGetError)(_In GM_CERT_STROE_CONTEXT gmCertStoreContext,
                                  _Out int* pnErrorNumber);

char* (*GMCertGetErrorString)(_In int nError);

int (*GMCertStoreContextGetErrorDepth)(_In GM_CERT_STROE_CONTEXT
                                           gmCertStoreContext,
                                       _Out int* pnErrorDepth);

int (*GMCertStoreContextGetCurrentCert)(_In GM_CERT_STROE_CONTEXT
                                            gmCertStoreContext,
                                        _Out GM_CERT* pgmCert);

void (*GMCertDestroy)(_In GM_CERT gmCert);
