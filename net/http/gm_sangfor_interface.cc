// YSP +
#include "net/http/gm_sangfor_interface.h"

#include <windows.h>
#include "base/strings/utf_string_conversions.h"

static std::string sangfor_filepath = "";
std::string GetSangforPath()
{
	wchar_t path[MAX_PATH + 1] = { 0 };
	std::wstring file_path = L"";
	HMODULE child_dll_mod = LoadLibrary(L"chrome_child.dll");
	if (child_dll_mod == NULL)
	{
		child_dll_mod = LoadLibrary(L"chrome.dll");
		if (child_dll_mod == NULL)
			return nullptr;
	}
	GetModuleFileName(child_dll_mod, path, MAX_PATH);
	FreeLibrary(child_dll_mod);
	// GetModuleFileName(NULL, path, MAX_PATH);
	wchar_t driver[_MAX_DRIVE + 1] = { 0 };
	wchar_t dir[_MAX_DIR + 1] = { 0 };
	wchar_t fname[_MAX_FNAME + 1] = { 0 };
	wchar_t ext[_MAX_EXT + 1] = { 0 };
	::_wsplitpath_s(path, driver, dir, fname, ext);
	std::wstring sangforPath = std::wstring(driver) + dir + L"gm\\";
	return base::UTF16ToUTF8(sangforPath);
}

GM_SESSION SanforGMStream::s_gm_session_ = NULL;

SanforGMStream::SanforGMStream()
{
	sangfor_filepath = GetSangforPath();
}

SanforGMStream::~SanforGMStream()
{
	Destroy();
}

int cpu_sleep = 0;
void SanforGMStream::Destroy()
{
	if (gm_stream_)
	{
		(void)GMStreamShutdown(gm_stream_);
		GMStreamDestroy(gm_stream_);
		gm_stream_ = NULL;
	}
	if (gm_context_)
	{
		GMContextDestroy(gm_context_, gm_engine_);
		gm_context_ = NULL;
		gm_engine_ = NULL;
	}

	if (s_gm_session_)
	{
		GMSessionDestroy(s_gm_session_);
		s_gm_session_ = NULL;
	}
	cpu_sleep = 0;
}
std::string ca_cert_path ;
std::string signature_cert_path ;
std::string signature_key_path ;
std::string encryption_cert_path ;
std::string encryption_key_path ;
std::string fm_encryption_key_path ;

int SanforGMStream::Init()
{
	ca_cert_path.clear();
	signature_cert_path.clear();
	signature_key_path.clear();
	encryption_cert_path.clear();
	encryption_key_path.clear();
	fm_encryption_key_path.clear();
	cpu_sleep = 1;

	if (SocketLibStartup() < 0)
	{
		DLOG(INFO) << "WSA library init failure !";
		return -1;
	}
	std::string sangforDllFilePath = sangfor_filepath + "gmcrypto.dll";
	if (LoadDLL(sangforDllFilePath.c_str()) < 0)
	{
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
int SanforGMStream::SocketLibStartup()
{
	WORD sock_version = MAKEWORD(2, 2);
	WSADATA wsa_data;
	if (0 != WSAStartup(sock_version, &wsa_data))
	{
		return -1;
	}
	return 0;
}

/**
* 加载国密库 DLL 的接口函数
* @param c_szDllFileName: DLL 库文件路径
* @return 成功: 0 失败: -1
*/
int SanforGMStream::LoadDLL(const char * c_szDllFileName)
{
	HMODULE  h;

	if (NULL == c_szDllFileName)
	{
		ERROR_MESSAGE();
		return -1;
	}

	h = LoadLibraryA(c_szDllFileName);
	if (h == NULL)
	{
		ERROR_MESSAGE();
		return -1;
	}

	GM_init = (int(*)(
		)) GetProcAddress(h, "GM_init");

	GMContextCreate = (int(*)(
		_Out GM_CONTEXT *ctx,
		_Out GM_ENGINE *skf_engine
		)) GetProcAddress(h, "GMContextCreate");

	GMContextSetSignatureCertificate = (int(*)(
		_In GM_CONTEXT ctx,
		_In const char *c_sz_certificate,
		_In const char *c_sz_key,
		_In GM_ENGINE skf_engine
		)) GetProcAddress(h, "GMContextSetSignatureCertificate");

	GMContextSetEncryptionCertificate = (int(*)(
		_In GM_CONTEXT ctx,
		_In const char *c_sz_certificate,
		_In const char *c_sz_key,
		_In GM_ENGINE skf_engine
		)) GetProcAddress(h, "GMContextSetEncryptionCertificate");
	GMContextSetVerify = (int(*)(
		_In GM_CONTEXT ctx,
		_In UINT u_verify_mode,
		_In_opt_ GM_CERT_VERIFY_CALLBACK fnCallback,
		_In_opt_ const char *c_sz_cacertificate,
		_In_opt_ UINT u_verify_depth
		)) GetProcAddress(h, "GMContextSetVerify");

	GMContextSetCipherList = (int(*)(
		_In GM_CONTEXT ctx,
		_In const char *c_szcipher_list
		)) GetProcAddress(h, "GMContextSetCipherList");
	GMContextDestroy = (int(*)(
		_In GM_CONTEXT ctx,
		_In GM_ENGINE skf_engine
		)) GetProcAddress(h, "GMContextDestroy");
	GMStreamCreate = (int(*)(
		_Out GM_STREAM *ssl,
		_In GM_CONTEXT ctx
		)) GetProcAddress(h, "GMStreamCreate");

	GMStreamSetUserData = (int(*)(
		_In 	GM_STREAM 						gm_stream,
		_In 	void* 							pv_userdata
		)) GetProcAddress(h, "GMStreamSetUserData");

	GMStreamGetUserData = (int(*)(
		_In 	GM_STREAM 						gm_stream,
		_In 	void** 							ppv_userdata
		)) GetProcAddress(h, "GMStreamGetUserData");

	GMStreamSetHostName = (int(*)(
		_In 	GM_STREAM 						gm_stream,
		_In 	const char* 					c_szhostname
		)) GetProcAddress(h, "GMStreamSetHostName");

	GMStreamSetSocket = (int(*)(
		_In GM_STREAM ssl,
		_In int sockfd
		)) GetProcAddress(h, "GMStreamSetSocket");

	GMStreamGetPeerCertificate = (int(*)(
		_In 	GM_STREAM 						gm_stream,
		_Out 	GM_CERT* 						pgm_cert
		)) GetProcAddress(h, "GMStreamGetPeerCertificate");

	GMStreamInHandshake = (int(*)(
		_In 	GM_STREAM 						gm_stream,
		_Out 	BOOL* 							pb_state
		)) GetProcAddress(h, "GMStreamInHandshake");

	GMStreamGetError = (int(*)(
		_In 		GM_STREAM 					gm_stream,
		_Out 		int* 						pn_error_reason,
		_In_opt_ 	GM_STREAM_ERROR_CALLBACK 	fnCallback,
		_In_opt_ 	void* 						pv_userdata
		)) GetProcAddress(h, "GMStreamGetError");

	GMStreamConnect = (int(*)(
		_In GM_STREAM ssl
		)) GetProcAddress(h, "GMStreamConnect");

	GMStreamWrite = (int(*)(
		_In GM_STREAM ssl,
		_In const void* pcv_buffer,
		_In UINT u_buffer_size
		)) GetProcAddress(h, "GMStreamWrite");

	GMStreamRead = (int(*)(
		_In GM_STREAM ssl,
		_Out void* pcv_buffer,
		_In UINT u_buffer_size
		)) GetProcAddress(h, "GMStreamRead");

	GMStreamShutdown = (int(*)(
		_In GM_STREAM ssl
		)) GetProcAddress(h, "GMStreamShutdown");

	GMStreamDestroy = (int(*)(
		_In GM_STREAM ssl
		)) GetProcAddress(h, "GMStreamDestroy");

	GMStreamGetSession = (int(*)(
		_In GM_STREAM ssl,
		_Out GM_SESSION *session
		)) GetProcAddress(h, "GMStreamGetSession");

	GMStreamSetSession = (int(*)(
		_In GM_STREAM ssl,
		_In GM_SESSION session
		)) GetProcAddress(h, "GMStreamSetSession");

	GMSessionDestroy = (int(*)(
		_In GM_SESSION session
		)) GetProcAddress(h, "GMSessionDestroy");

	GMCertStoreContextGetStream = (int(*)(
		_Out 	GM_STREAM* 						pgm_stream,
		_In 	GM_CERT_STROE_CONTEXT 			gm_certStore_context
		)) GetProcAddress(h, "GMCertStoreContextGetStream");

	GMCertStoreContextGetError = (int(*)(
		_In 	GM_CERT_STROE_CONTEXT 			gm_certStore_context,
		_Out 	int* 							pn_error_number
		)) GetProcAddress(h, "GMCertStoreContextGetError");

	GMCertGetErrorString = (char* (*)(
		_In 	int 							nError
		)) GetProcAddress(h, "GMCertGetErrorString");

	GMCertStoreContextGetErrorDepth = (int(*)(
		_In 	GM_CERT_STROE_CONTEXT 			gm_certStore_context,
		_Out 	int* 							pn_error_depth
		)) GetProcAddress(h, "GMCertStoreContextGetErrorDepth");

	GMCertStoreContextGetCurrentCert = (int(*)(
		_In 	GM_CERT_STROE_CONTEXT 			gm_certStore_context,
		_Out 	GM_CERT* 						pgm_cert
		)) GetProcAddress(h, "GMCertDestroy");

	GMCertDestroy = (void(*)(
		_In 	GM_CERT 						gm_cert
		)) GetProcAddress(h, "GMCertDestroy");

	if (!GM_init ||
		!GMContextCreate ||
		!GMContextSetSignatureCertificate ||
		!GMContextSetEncryptionCertificate ||
		!GMContextSetVerify ||
		!GMContextSetCipherList ||
		!GMStreamCreate ||
		!GMStreamSetSocket ||
		!GMStreamConnect ||
		!GMStreamWrite ||
		!GMStreamRead ||
		!GMStreamGetSession ||
		!GMStreamSetSession ||
		!GMStreamShutdown ||
		!GMSessionDestroy ||
		!GMContextDestroy ||
		!GMStreamDestroy ||
		!GMStreamSetUserData ||
		!GMStreamGetUserData ||
		!GMStreamSetHostName ||
		!GMStreamGetPeerCertificate ||
		!GMStreamInHandshake ||
		!GMStreamGetError ||
		!GMCertStoreContextGetStream ||
		!GMCertStoreContextGetError ||
		!GMCertGetErrorString ||
		!GMCertStoreContextGetErrorDepth ||
		!GMCertStoreContextGetCurrentCert ||
		!GMCertDestroy
		) {
		ERROR_MESSAGE();
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
GM_CONTEXT SanforGMStream::GMContextInit()
{
	ca_cert_path = sangfor_filepath + "gmcert\\ca_cert.pem";
	signature_cert_path = sangfor_filepath + "gmcert\\signature_cert.pem";
	signature_key_path = sangfor_filepath + "gmcert\\signature_key.pem";
	encryption_cert_path = sangfor_filepath + "gmcert\\encryption_cert.pem";
	encryption_key_path = sangfor_filepath + "gmcert\\encryption_key.pem";
	fm_encryption_key_path = sangfor_filepath + "gmcert-hd\\encryption_key.pem";

	if (GMERR_OK != GMContextCreate(&gm_context_, &gm_engine_))
	{
		goto err;
	}

	// 设置服务器认证方式
	if (GMERR_OK != GMContextSetVerify(gm_context_,
		0// GM_VERIFY_NONE,// uVerifyMode, 不认证服务器身份 -- GM_VERIFY_PEER 
		NULL, 								// fnCallback, 使用国密库内部实现的认证方式
		ca_cert_path.c_str(), 							// c_szCACertificate, CA 证书文件路径
		10)) 								// uVerifyDepth, 最大证书认证深度
	{
		goto err;
	}

	// 设置支持的算法列表
	if (GMERR_OK != GMContextSetCipherList(gm_context_,
		"ECC-SM2-SM4-SM3:ECDHE-SM2-SM4-SM3"))
	{
		goto err;
	}

	// 根据 GM_CONTEXT 配置模板对象配置，创建 GM_STREAM 国密数据流对象
	if (GMERR_OK != GMStreamCreate(&gm_stream_, gm_context_))
	{
		// ERROR_MESSAGE();
		goto err;
	}

	if (GMERR_OK != GMContextSetSignatureCertificate(gm_stream_,
		signature_cert_path.c_str(),
		signature_key_path.c_str(),
		gm_engine_))
	{
		goto err;
	}

	// FM UKey 证书 (暂时注释，以后打开)
	/*if (GMERR_OK != GMContextSetEncryptionCertificate(gm_stream_,
		encryption_cert_path.c_str(),
		fm_encryption_key_path.c_str(),
		gm_engine_))
	{
		goto err;*/

		// 软算法功能先关闭，以后TODO*
		// 软算法证书
		if (GMERR_OK != GMContextSetEncryptionCertificate(gm_stream_,
			encryption_cert_path.c_str(),
			encryption_key_path.c_str(),
			gm_engine_))
		{
			goto err;
		}
	//	
	//}

	return gm_context_;
err:
	if (gm_context_) {
		(void)GMContextDestroy(gm_context_, gm_engine_);
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
int SanforGMStream::GMReadResponseData(net::IOBuffer* buf, int buf_len)
{
	int n_return = 0, offset = 0;
	char c_buffer[1024] = { 0 };

	if (gm_stream_ == NULL) {
		DLOG(INFO) << "gm_stream_ is NULL !";
		return -1;
	}
	// 接收业务数据应答报文 
	if (cpu_sleep) {
		Sleep(200); // 为防止第一次拿不到响应数据导致浏览器直接返回临时睡眠200毫秒
		cpu_sleep = 0;
	}
	
	while ((n_return = GMStreamRead(gm_stream_, c_buffer, 1024 - 1)) > 0)
	{
		c_buffer[n_return] = '\0';
		memcpy(buf->data() + offset, c_buffer, n_return);
		offset += n_return;
		// DLOG(INFO) << "ResponseData: " << ResponseData->c_str();
		if (n_return < 1024 - 1)
			break;
		if ((offset + 1024 - 1) > buf_len)
			break;
		DLOG(INFO) << "n_return: " << n_return;
	}
	// if (NULL == s_gm_session_)
	//	(void)GMStreamGetSession(gm_stream_, &s_gm_session_);
	// DLOG(INFO) << "n_return: " << n_return ;

	return offset;
}

int SanforGMStream::GMReadResponseData(net::IOBuffer* buf, int buf_len, 
  const net::CompletionCallback& callback)
{
	int n_return = 0, offset = 0;
	char c_buffer[GMBUFFERLEN] = { 0 };

	if (gm_stream_ == NULL) {
		DLOG(INFO) << "gm_stream_ is NULL !";
		return -1;
	}
	// 接收业务数据应答报文
	while ((n_return = GMStreamRead(gm_stream_, c_buffer, GMBUFFERLEN - 1)) > 0)
	{
		c_buffer[n_return] = '\0';
		memcpy(buf->data() + offset, c_buffer, n_return);
		offset += n_return;
		// DLOG(INFO) << "ResponseData: " << ResponseData->c_str();
		if (n_return < GMBUFFERLEN - 1)
			break;
		if ((offset + GMBUFFERLEN - 1) > buf_len)
			break;
	}
	// DLOG(INFO) << "n_return: " << n_return ;

	return offset;
}

/**
* 发送业务数据
* @param   c_szRequest: 要发送的请求数据
* @param   callback:   发送完回调函数
* @return  成功返回写入的字节数，失败返回-1
*/
int SanforGMStream::GMWriteRequestData(const char* c_sz_request, 
  int request_len, const net::CompletionCallback& callback)
{
	// 发送业务数据请求
	while (GMERR_OK != GMStreamWrite(gm_stream_, c_sz_request, request_len))
	{
		DLOG(INFO) << "GMStreamWrite failure !";
		return -1;
	}
	return request_len;
}

/**
* 国密握手
* @return  成功返回0，失败返回-1
*/
int SanforGMStream::GMHandShake()
{
	BOOL pbState = FALSE;

	if (GMERR_OK == (GMStreamInHandshake(gm_stream_, &pb_state))) {
		if (pb_state) {
			return 0;
		}
	}
	// 进行国密协议握手协商
	if (GMERR_OK != GMStreamConnect(gm_stream_))
	{
		DLOG(INFO) << "GMStreamConnect failure !";
		return -1;
	}
	return 0;
}

/**
* 对于给定的 Socket 对象，
* 创建国密数据流，进行收发业务数据
* @param   n_client:   Socket 对象
* @return  成功返回0，失败返回-1
*/
int SanforGMStream::GMCreateDataStream(int n_client)
{
	if (Init() < 0)
		return -1;
	// 创建 GM_CONTEXT 配置模板对象
	GMContextInit();
	if (NULL == gm_context_)
	{
		DLOG(INFO) << "GMContextInit failure !";
		return -1;
	}
	if (NULL == gm_context_) {
		DLOG(INFO) << "gm_context_ is NULL !";
		return -1;
	}

	if (NULL != s_gm_session_)
		(void) GMStreamSetSession(gm_stream_, s_gm_session_);

	// 为 GM_STREAM 国密数据流绑定 Socket 对象
	if (GMERR_OK != GMStreamSetSocket(gm_stream_, n_client))
	{
		DLOG(INFO) << "GMStreamSetSocket failure !";
		return -1;
	}
	return 0;
}

int(*GM_init)();
int(*GMContextCreate)(
	_Out GM_CONTEXT *ctx,
	_Out GM_ENGINE *skf_engine
	);
int(*GMContextSetSignatureCertificate)(
	_In GM_STREAM ssl,
	_In const char *c_sz_certificate,
	_In const char *c_sz_key,
	_In GM_ENGINE skf_engine
	);
int(*GMContextSetEncryptionCertificate)(
	_In GM_STREAM ssl,
	_In const char *c_sz_certificate,
	_In const char *c_sz_key,
	_In GM_ENGINE skf_engine
	);
int(*GMContextSetVerify)(
	_In GM_CONTEXT ctx,
	_In UINT u_verify_mode,
	_In_opt_ GM_CERT_VERIFY_CALLBACK fnCallback,
	_In_opt_ const char *c_sz_cacertificate,
	_In_opt_ UINT u_verify_depth);
int(*GMContextSetCipherList)(
	_In GM_CONTEXT ctx,
	_In const char *c_szcipher_list
	);
int(*GMContextDestroy)(
	_In GM_CONTEXT ctx,
	_In GM_ENGINE skf_engine
	);
int(*GMStreamCreate)(
	_Out GM_STREAM *ssl,
	_In GM_CONTEXT ctx
	);
int(*GMStreamSetUserData)(
	_In 	GM_STREAM 						gm_stream,
	_In 	void* 							pv_userdata
	);
int(*GMStreamGetUserData)(
	_In 	GM_STREAM 						gm_stream,
	_Out 	void** 							ppv_userdata
	);
int(*GMStreamSetHostName)(
	_In 	GM_STREAM 						gm_stream,
	_In 	const char* 					c_szhostname
	);
int(*GMStreamSetSocket)(
	_In GM_STREAM ssl,
	_In int sockfd
	);
int(*GMStreamGetPeerCertificate)(
	_In 	GM_STREAM 						gm_stream,
	_Out 	GM_CERT* 						pgm_cert
	);
int(*GMStreamInHandshake)(
	_In 	GM_STREAM 						gm_stream,
	_Out 	BOOL* 							pb_state
	);
int(*GMStreamGetError)(
	_In 		GM_STREAM 					gm_stream,
	_Out 		int* 						pn_error_reason,
	_In_opt_ 	GM_STREAM_ERROR_CALLBACK 	fnCallback,
	_In_opt_ 	void* 						pv_userdata
	);
int(*GMStreamConnect)(
	_In GM_STREAM ssl
	);
int(*GMStreamWrite)(
	_In GM_STREAM ssl,
	_In const void* pcv_buffer,
	_In UINT u_buffer_size
	);
int(*GMStreamRead)(
	_In GM_STREAM ssl,
	_Out void* pcv_buffer,
	_In UINT u_buffer_size
	);
int(*GMStreamShutdown)(
	_In GM_STREAM ssl
	);
int(*GMStreamDestroy)(
	_In GM_STREAM ssl
	);
int(*GMStreamGetSession)(
	_In GM_STREAM ssl,
	_Out GM_SESSION *session
	);
int(*GMStreamSetSession)(
	_In GM_STREAM ssl,
	_Out GM_SESSION session
	);
int(*GMSessionDestroy)(
	_In GM_STREAM ssl
	);

int(*GMCertStoreContextGetStream)(
	_Out 	GM_STREAM* 						pgm_stream,
	_In 	GM_CERT_STROE_CONTEXT 			gm_certStore_context
	);
int(*GMCertStoreContextGetError)(
	_In 	GM_CERT_STROE_CONTEXT 			gm_certStore_context,
	_Out 	int* 							pn_error_number
	);
char* (*GMCertGetErrorString)(
	_In 	int 							nError
	);
int(*GMCertStoreContextGetErrorDepth)(
	_In 	GM_CERT_STROE_CONTEXT 			gm_certStore_context,
	_Out 	int* 							pn_error_depth
	);
int(*GMCertStoreContextGetCurrentCert)(
	_In 	GM_CERT_STROE_CONTEXT 			gm_certStore_context,
	_Out 	GM_CERT* 						pgm_cert
	);
void(*GMCertDestroy)(
	_In 	GM_CERT 						gm_cert
	);