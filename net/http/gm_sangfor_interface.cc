//YSP +
#include "net/http/gm_sangfor_interface.h"

#include <windows.h>
#include "base/strings/utf_string_conversions.h"
static std::string sangforFilePath = "";
std::string GetSangforPath()
{
  wchar_t path[MAX_PATH + 1] = { 0 };
  GetModuleFileName(NULL, path, MAX_PATH);
  wchar_t driver[_MAX_DRIVE + 1] = { 0 };
  wchar_t dir[_MAX_DIR + 1] = { 0 };
  wchar_t fname[_MAX_FNAME + 1] = { 0 };
  wchar_t ext[_MAX_EXT + 1] = { 0 };
  ::_wsplitpath_s(path, driver, dir, fname, ext);
  std::wstring sangforPath = std::wstring(driver) + dir + L"sangfor\\";
  return base::UTF16ToUTF8(sangforPath);

}

GM_SESSION SanforGMStream::s_gmSession = NULL;
SanforGMStream::SanforGMStream()
{
  sangforFilePath = GetSangforPath();
  //Init();
}

SanforGMStream::~SanforGMStream()
{
  Destroy();
}

void SanforGMStream::Destroy()
{
  if (gmStream)
  {
    (void)GMStreamShutdown(gmStream);
    GMStreamDestroy(gmStream);
    gmStream = NULL;
  }
  if (gmContext)
  {
    GMContextDestroy(gmContext);
    gmContext = NULL;
  }

  //if (s_gmSession)
  //{
  //  GMSessionDestroy(s_gmSession);
  //}
}

int SanforGMStream::Init()
{
  if (SocketLibStartup() < 0)
  {
    DLOG(INFO) << "WSA library init failure !";
    return -1;
  }
  std::string sangforDllFilePath = sangforFilePath + "sangfor.dll";
  if (LoadSanforDLL(sangforDllFilePath.c_str()) < 0)
  {
    DLOG(INFO) << "Load GMlibrary failure !";
    return -1;
  }
  /* 初始化国密库函数 */
  (void)GM_init();

  return 0;
}

/**
* 初始化 windows平台的 WSA 库
* @param void
* @return 成功: 0
失败: -1
*/
int SanforGMStream::SocketLibStartup()
{
  WORD sockVersion = MAKEWORD(2, 2);
  WSADATA wsaData;
  if (0 != WSAStartup(sockVersion, &wsaData))
  {
    return -1;
  }
  return 0;
}

/**
* 加载国密库 DLL 的接口函数
* @param c_szDllFileName: DLL 库文件路径
* @return 成功: 0
失败: -1
*/
int SanforGMStream::LoadSanforDLL(const char * c_szDllFileName)
{
  //HMODULE  h;

  if (NULL == c_szDllFileName)
  {
    DLOG(INFO) << "GMLibrary file name is empty !" ;
    return -1;
  }

  module = LoadLibraryA(c_szDllFileName);
  if (module == NULL)
  {
    DLOG(INFO) << "Load GMlibrary failure ! Lost "  << c_szDllFileName << " file. Please try reinstalling the app.";
    return -1;
  }

  GM_init = (int(*)(
    )) GetProcAddress(module, "GM_init");

  GMContextCreate = (int(*)(
    _Out  GM_CONTEXT*           pgmContext
    )) GetProcAddress(module, "GMContextCreate");

  GMContextSetSignatureCertificate = (int(*)(
    _In   GM_CONTEXT            gmContext,
    _In   const char*           c_szCertificate,
    _In   const char*           c_szKey
    )) GetProcAddress(module, "GMContextSetSignatureCertificate");

  GMContextSetEncryptionCertificate = (int(*)(
    _In   GM_CONTEXT            gmContext,
    _In   const char*           c_szCertificate,
    _In   const char*           c_szKey
    )) GetProcAddress(module, "GMContextSetEncryptionCertificate");

  GMContextSetVerify = (int(*)(
    _In     GM_CONTEXT          gmContext,
    _In     UINT            uVerifyMode,
    _In_opt_  GM_CERT_VERIFY_CALLBACK   fnCallback,
    _In_opt_  const char*         c_szCACertificate,
    _In_opt_  UINT            uVerifyDepth
    )) GetProcAddress(module, "GMContextSetVerify");

  GMContextSetCipherList = (int(*)(
    _In   GM_CONTEXT            gmContext,
    _In   const char*           c_szCipherList
    )) GetProcAddress(module, "GMContextSetCipherList");

  GMContextDestroy = (void(*)(
    _In   GM_CONTEXT            gmContext
    )) GetProcAddress(module, "GMContextDestroy");


  /******************************************************************************/

  GMStreamCreate = (int(*)(
    _Out  GM_STREAM*            pgmStream,
    _In   GM_CONTEXT            gmContext
    )) GetProcAddress(module, "GMStreamCreate");

  GMStreamSetUserData = (int(*)(
    _In   GM_STREAM             gmStream,
    _In   void*               pvUserData
    )) GetProcAddress(module, "GMStreamSetUserData");

  GMStreamGetUserData = (int(*)(
    _In   GM_STREAM             gmStream,
    _Out  void**              ppvUserData
    )) GetProcAddress(module, "GMStreamGetUserData");

  GMStreamSetHostName = (int(*)(
    _In   GM_STREAM             gmStream,
    _In   const char*           c_szHostName
    )) GetProcAddress(module, "GMStreamSetHostName");

  GMStreamSetSocket = (int(*)(
    _In   GM_STREAM             gmStream,
    _In   int               nSocket
    )) GetProcAddress(module, "GMStreamSetSocket");

  GMStreamGetPeerCertificate = (int(*)(
    _In   GM_STREAM             gmStream,
    _Out  GM_CERT*            pgmCert
    )) GetProcAddress(module, "GMStreamGetPeerCertificate");

  GMStreamInHandshake = (int(*)(
    _In   GM_STREAM             gmStream,
    _Out  BOOL*               pbState
    )) GetProcAddress(module, "GMStreamInHandshake");

  GMStreamGetError = (int(*)(
    _In     GM_STREAM           gmStream,
    _Out    int*            pnErrorReason,
    _In_opt_  GM_STREAM_ERROR_CALLBACK  fnCallback,
    _In_opt_  void*             pvUserData
    )) GetProcAddress(module, "GMStreamGetError");

  GMStreamConnect = (int(*)(
    _In   GM_STREAM             gmStream
    )) GetProcAddress(module, "GMStreamConnect");

  GMStreamWrite = (int(*)(
    _In   GM_STREAM             gmStream,
    _In   const void*           pcvBuffer,
    _In   UINT              uBufferSize
    )) GetProcAddress(module, "GMStreamWrite");

  GMStreamRead = (int(*)(
    _In   GM_STREAM             gmStream,
    _Out  void*               pvBuffer,
    _In   UINT              uBufferSize
    )) GetProcAddress(module, "GMStreamRead");

  GMStreamShutdown = (int(*)(
    _In   GM_STREAM             gmStream
    )) GetProcAddress(module, "GMStreamShutdown");

  GMStreamDestroy = (void(*)(
    _In   GM_STREAM             gmStream
    )) GetProcAddress(module, "GMStreamDestroy");

  /******************************************************************************/

  GMStreamGetSession = (int(*)(
    _In   GM_STREAM             gmStream,
    _Out  GM_SESSION*           pgmSession
    )) GetProcAddress(module, "GMStreamGetSession");

  GMStreamSetSession = (int(*)(
    _In   GM_STREAM             gmStream,
    _In   GM_SESSION            gmSession
    )) GetProcAddress(module, "GMStreamSetSession");

  GMSessionDestroy = (void(*)(
    _In   GM_SESSION            gmSession
    )) GetProcAddress(module, "GMSessionDestroy");

  /******************************************************************************/

  GMCertStoreContextGetStream = (int(*)(
    _Out  GM_STREAM*            pgmStream,
    _In   GM_CERT_STROE_CONTEXT       gmCertStoreContext
    )) GetProcAddress(module, "GMCertStoreContextGetStream");

  GMCertStoreContextGetError = (int(*)(
    _In   GM_CERT_STROE_CONTEXT       gmCertStoreContext,
    _Out  int*              pnErrorNumber
    )) GetProcAddress(module, "GMCertStoreContextGetError");

  GMCertGetErrorString = (char* (*)(
    _In   int               nError
    )) GetProcAddress(module, "GMCertGetErrorString");

  GMCertStoreContextGetErrorDepth = (int(*)(
    _In   GM_CERT_STROE_CONTEXT       gmCertStoreContext,
    _Out  int*              pnErrorDepth
    )) GetProcAddress(module, "GMCertStoreContextGetErrorDepth");

  GMCertStoreContextGetCurrentCert = (int(*)(
    _In   GM_CERT_STROE_CONTEXT       gmCertStoreContext,
    _Out  GM_CERT*            pgmCert
    )) GetProcAddress(module, "GMCertStoreContextGetCurrentCert");

  GMCertDestroy = (void(*)(
    _In   GM_CERT             gmCert
    )) GetProcAddress(module, "GMCertDestroy");
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
GM_CONTEXT SanforGMStream::GMContextInit()
{
  GM_CONTEXT gmContext = NULL;
  std::string ca_cert_path = sangforFilePath + "ca_cert.pem";
  std::string signature_cert_path = sangforFilePath + "signature_cert.pem";
  std::string signature_key_path = sangforFilePath + "signature_key.pem";
  std::string encryption_cert_path = sangforFilePath + "encryption_cert.pem";
  std::string encryption_key_path = sangforFilePath + "encryption_key.pem";

  /* 创建 GM_CONTEXT 配置模板对象 */
  if (GMERR_OK != GMContextCreate(&gmContext))
  {
    DLOG(INFO) << "GMContextCreate failure !" ;
    goto err;
  }

  /* 设置签名证书 */
  if (GMERR_OK != GMContextSetSignatureCertificate(gmContext,
    signature_cert_path.c_str(),
    signature_key_path.c_str()))
  {
    DLOG(INFO) << "GMContextSetSignatureCertificate failure !" ;
    goto err;
  }

  /* 设置加密证书 */
  if (GMERR_OK != GMContextSetEncryptionCertificate(gmContext,
    encryption_cert_path.c_str(),
    encryption_key_path.c_str()))
  {
    DLOG(INFO) << "GMContextSetEncryptionCertificate failure !";
    goto err;
  }

  /* 设置服务器认证方式 */
  if (GMERR_OK != GMContextSetVerify(gmContext,
    GM_VERIFY_NONE,           /* uVerifyMode, 不认证服务器身份 -- GM_VERIFY_PEER */
    NULL,                 /* fnCallback, 使用国密库内部实现的认证方式*/
    ca_cert_path.c_str(),   /* c_szCACertificate, CA 证书文件路径 */
    10))                /* uVerifyDepth, 最大证书认证深度*/
  {
    DLOG(INFO) << "GMContextSetVerify failure !";
    goto err;
  }

  /* 设置支持的算法列表 */
  if (GMERR_OK != GMContextSetCipherList(gmContext,
    "ECC-SM2-SM4-SM3:ECDHE-SM2-SM4-SM3"))
  {
    DLOG(INFO) << "GMContextSetCipherList failure !";
    goto err;
  }

  return gmContext;
err:
  if (gmContext)
  {
    (void)GMContextDestroy(gmContext);
    gmContext = NULL;
  }
  return NULL;
}

/**
* 从国密数据流，收取响应数据
* @param   buf: 存储响应数据的buf
* @param   buf_len:   要接收的数据长度
* @param   callback:   接收完回调函数
* @return  成功返回读取的字节数，失败返回-1
*/
int SanforGMStream::GMReadResponseData(net::IOBuffer* buf, int buf_len/*, const net::CompletionCallback& callback*/)
{
  int nReturn = 0, offset = 0;
  char cBuffer[GMBUFFERLEN] = { 0 };
  bool responseFlags = false, wouldBlock = true;

  if (gmStream == NULL) {
    DLOG(INFO) << "gmStream is NULL !";
    return -1;
  }
  base::Time time = base::Time::Now();
  //int64_t millisecond = time.ToJavaTime();
  //LOG(INFO) << "milliseconds:[" << base::Time::Now().ToJavaTime() << "][" << millisecond << "]";
  //if (GMFlags) {
  //  GMFlags = false;
  //  while ((base::Time::Now().ToJavaTime() - time.ToJavaTime()) <= 500) {}
  //}
  /* 接收业务数据应答报文 */
  while ((nReturn = GMStreamRead(gmStream, cBuffer, GMBUFFERLEN - 1)) > 0 || nReturn == GMERR_STREAM_WOULDBLOCK)
  {
    if (nReturn == GMERR_STREAM_WOULDBLOCK && wouldBlock) {
      time = base::Time::Now();
      wouldBlock = false;
    }
    if (nReturn != GMERR_STREAM_WOULDBLOCK) {
      time = base::Time::Now();
      wouldBlock = true;
      cBuffer[nReturn] = '\0';
      //ResponseData->append(cBuffer);
      memcpy(buf->data() + offset, cBuffer, nReturn);
      offset += nReturn;
      //DLOG(INFO) << "ResponseData: " << ResponseData->c_str();
      if (nReturn < GMBUFFERLEN - 1)
        break;
      responseFlags = true;
    }
    if ((offset + GMBUFFERLEN - 1) > buf_len)
      break;
    if (nReturn == GMERR_STREAM_WOULDBLOCK) {
      if (responseFlags)
        break;
      if ((base::Time::Now().ToTimeT() - time.ToTimeT()) > 5) {
        break;
      }
    }
    DLOG(INFO) << "nReturn: " << nReturn;
  }
  if (NULL == s_gmSession)
    (void) GMStreamGetSession(gmStream, &s_gmSession);
  //DLOG(INFO) << "nReturn: " << nReturn ;

  return offset;
}
int SanforGMStream::GMReadResponseData(net::IOBuffer* buf, int buf_len, const net::CompletionCallback& callback)
{
  int nReturn = 0, offset = 0;
  char cBuffer[GMBUFFERLEN] = { 0 };

  if (gmStream == NULL) {
    DLOG(INFO) << "gmStream is NULL !";
    return -1;
  }
  /* 接收业务数据应答报文 */
  while ((nReturn = GMStreamRead(gmStream, cBuffer, GMBUFFERLEN - 1)) > 0)
  {
    if (nReturn != GMERR_STREAM_WOULDBLOCK) {
      cBuffer[nReturn] = '\0';
      memcpy(buf->data() + offset, cBuffer, nReturn);
      offset += nReturn;
      //DLOG(INFO) << "ResponseData: " << ResponseData->c_str();
      if (nReturn < GMBUFFERLEN - 1)
        break;
    }
    if ((offset + GMBUFFERLEN - 1) > buf_len)
      break;
  }
  //DLOG(INFO) << "nReturn: " << nReturn ;

  return offset;
}

/**
* 发送业务数据
* @param   c_szRequest: 要发送的请求数据
* @param   callback:   发送完回调函数
* @return  成功返回写入的字节数，失败返回-1
*/
int SanforGMStream::GMWriteRequestData(const char* c_szRequest, int request_len, const net::CompletionCallback& callback)
{
  int nErrorReason;
  base::Time time = base::Time::Now();
  //if (gmStream)
  //{
  //  DLOG(INFO) << "gmStream is NULL !";
  //  return -1;
  //}
  /* 发送业务数据请求 */
  while (GMERR_OK != GMStreamWrite(gmStream, c_szRequest, request_len))
  {
    (void)GMStreamGetError(gmStream, &nErrorReason, ErrorPrint, NULL);
    if (nErrorReason != GMERR_STREAM_WOULDBLOCK) {
      DLOG(INFO) << "GMStreamWrite failure !";
      return -1;
    }
    if ((base::Time::Now().ToJavaTime() - time.ToJavaTime()) > 2000)
      break;
  }
  return request_len;
}

/**
* 国密握手
* @return  成功返回0，失败返回-1
*/
int SanforGMStream::GMHandShake()
{
  int nErrorReason;
  BOOL pbState = FALSE;

  if (GMERR_OK == (GMStreamInHandshake(gmStream, &pbState))) {
    if (pbState) {
      return 0;
    }
  }
  base::Time time = base::Time::Now();
  //if (gmStream)
  //{
  //  DLOG(INFO) << "gmStream is NULL !";
  //  return -1;
  //}
  /* 进行国密协议握手协商 */
  while (GMERR_OK != GMStreamConnect(gmStream))
  {
    (void)GMStreamGetError(gmStream, &nErrorReason, ErrorPrint, NULL);
    if (nErrorReason != GMERR_STREAM_WOULDBLOCK) {
      DLOG(INFO) << "GMStreamConnect failure !";
      return -1;
    }
    //LOG(INFO) << "Time:[" << time.ToTimeT() << "] [" << base::Time::Now().ToTimeT() << "]";
    if ((base::Time::Now().ToJavaTime() - time.ToJavaTime()) > 2000)
      break;
  }
  return 0;
}

/**
* 对于给定的 Socket 对象，
* 创建国密数据流，进行收发业务数据
* @param   nClient:   Socket 对象
* @return  成功返回0，失败返回-1
*/
int SanforGMStream::GMCreateDataStream(int nClient)
{
  if (Init() < 0)
    return -1;
  /* 创建 GM_CONTEXT 配置模板对象 */
  gmContext = GMContextInit();
  if (NULL == gmContext)
  {
    DLOG(INFO) << "GMContextInit failure !";
    return -1;
  }
  //GM_STREAM gmStream = NULL;
  //int nErrorReason;
  //base::Time time = base::Time::Now();
  if (NULL == gmContext) {
    DLOG(INFO) << "gmContext is NULL !";
    return -1;
  }

  /* 根据 GM_CONTEXT 配置模板对象配置，创建 GM_STREAM 国密数据流对象 */
  if (GMERR_OK != GMStreamCreate(&gmStream, gmContext))
  {
    DLOG(INFO) << "GMStreamCreate failure !";
    return -1;
  }

  if (NULL != s_gmSession)
    (void) GMStreamSetSession(gmStream, s_gmSession);

  /* 为 GM_STREAM 国密数据流绑定 Socket 对象 */
  if (GMERR_OK != GMStreamSetSocket(gmStream, nClient))
  {
    DLOG(INFO) << "GMStreamSetSocket failure !";
    return -1;
  }
  ///* 进行国密协议握手协商 */
  //time = base::Time::Now();
  //while (GMERR_OK != GMStreamConnect(gmStream))
  //{
  //  (void)GMStreamGetError(gmStream, &nErrorReason, ErrorPrint, NULL);
  //  if (nErrorReason != GMERR_STREAM_WOULDBLOCK) {
  //    DLOG(INFO) << "GMStreamConnect failure !";
  //    goto err;
  //  }
  //  //LOG(INFO) << "Time:[" << time.ToTimeT() << "] [" << base::Time::Now().ToTimeT() << "]";
  //  if ((base::Time::Now().ToTimeT() - time.ToTimeT()) > 1)
  //    break;
  //}
  /* 发送业务数据请求 */
  //if (GMERR_OK != GMStreamWrite(gmStream, c_szRequest, strlen(c_szRequest)))
  //{
  //  (void)GMStreamGetError(gmStream, &nErrorReason, ErrorPrint, NULL);
  //  DLOG(INFO) << "GMStreamWrite failure !";
  //  goto err;
  //}
  ///* 接收业务数据应答报文 */
  //while ((nReturn = GMStreamRead(gmStream, cBuffer, sizeof(cBuffer) - 1)) > 0 || nReturn == GMERR_STREAM_WOULDBLOCK)
  //{
  //  if (nReturn != GMERR_STREAM_WOULDBLOCK) {
  //    cBuffer[nReturn] = '\0';
  //    requestBuffer->append(cBuffer);
  //    //DLOG(INFO) << "requestBuffer: " << requestBuffer->c_str();
  //    //return requestBuffer.c_str();
  //  }
  //}
  //DLOG(INFO) << "nReturn: " << nReturn ;

  //(void)GMStreamShutdown(gmStream);
  return 0;
}

int(*GM_init)(
  );

int(*GMContextCreate)(
  _Out  GM_CONTEXT*           pgmContext
  );

int(*GMContextSetSignatureCertificate)(
  _In   GM_CONTEXT            gmContext,
  _In   const char*           c_szCertificate,
  _In   const char*           c_szKey
  );

int(*GMContextSetEncryptionCertificate)(
  _In   GM_CONTEXT            gmContext,
  _In   const char*           c_szCertificate,
  _In   const char*           c_szKey
  );

int(*GMContextSetVerify)(
  _In     GM_CONTEXT          gmContext,
  _In     UINT            uVerifyMode,
  _In_opt_  GM_CERT_VERIFY_CALLBACK   fnCallback,
  _In_opt_  const char*         c_szCACertificate,
  _In_opt_  UINT            uVerifyDepth
  );

int(*GMContextSetCipherList)(
  _In   GM_CONTEXT            gmContext,
  _In   const char*           c_szCipherList
  );

void(*GMContextDestroy)(
  _In   GM_CONTEXT            gmContext
  );


/******************************************************************************/

int(*GMStreamCreate)(
  _Out  GM_STREAM*            pgmStream,
  _In   GM_CONTEXT            gmContext
  );

int(*GMStreamSetUserData)(
  _In   GM_STREAM             gmStream,
  _In   void*               pvUserData
  );

int(*GMStreamGetUserData)(
  _In   GM_STREAM             gmStream,
  _Out  void**              ppvUserData
  );

int(*GMStreamSetHostName)(
  _In   GM_STREAM             gmStream,
  _In   const char*           c_szHostName
  );

int(*GMStreamSetSocket)(
  _In   GM_STREAM             gmStream,
  _In   int               nSocket
  );

int(*GMStreamGetPeerCertificate)(
  _In   GM_STREAM             gmStream,
  _Out  GM_CERT*            pgmCert
  );

int(*GMStreamInHandshake)(
  _In   GM_STREAM             gmStream,
  _Out  BOOL*               pbState
  );

int(*GMStreamGetError)(
  _In     GM_STREAM           gmStream,
  _Out    int*            pnErrorReason,
  _In_opt_  GM_STREAM_ERROR_CALLBACK  fnCallback,
  _In_opt_  void*             pvUserData
  );

int(*GMStreamConnect)(
  _In   GM_STREAM             gmStream
  );

int(*GMStreamWrite)(
  _In   GM_STREAM             gmStream,
  _In   const void*           pcvBuffer,
  _In   UINT              uBufferSize
  );

int(*GMStreamRead)(
  _In   GM_STREAM             gmStream,
  _Out  void*               pvBuffer,
  _In   UINT              uBufferSize
  );

int(*GMStreamShutdown)(
  _In   GM_STREAM             gmStream
  );

void(*GMStreamDestroy)(
  _In   GM_STREAM             gmStream
  );

/******************************************************************************/

int(*GMStreamGetSession)(
  _In   GM_STREAM             gmStream,
  _Out  GM_SESSION*           pgmSession
  );

int(*GMStreamSetSession)(
  _In   GM_STREAM             gmStream,
  _In   GM_SESSION            gmSession
  );

void(*GMSessionDestroy)(
  _In   GM_SESSION            gmSession
  );

/******************************************************************************/

int(*GMCertStoreContextGetStream)(
  _Out  GM_STREAM*            pgmStream,
  _In   GM_CERT_STROE_CONTEXT       gmCertStoreContext
  );

int(*GMCertStoreContextGetError)(
  _In   GM_CERT_STROE_CONTEXT       gmCertStoreContext,
  _Out  int*              pnErrorNumber
  );

char* (*GMCertGetErrorString)(
  _In   int               nError
  );

int(*GMCertStoreContextGetErrorDepth)(
  _In   GM_CERT_STROE_CONTEXT       gmCertStoreContext,
  _Out  int*              pnErrorDepth
  );

int(*GMCertStoreContextGetCurrentCert)(
  _In   GM_CERT_STROE_CONTEXT       gmCertStoreContext,
  _Out  GM_CERT*            pgmCert
  );

void(*GMCertDestroy)(
  _In   GM_CERT             gmCert
  );
