// YSP +

#ifndef GM_SANGFOR_INTERFACE_H
#define GM_SANGFOR_INTERFACE_H

#include <stdlib.h> // size_t
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <winsock2.h>
#include "base/callback_helpers.h"
#include "net/base/io_buffer.h"
#include "base/compiler_specific.h"
#include "net/base/completion_callback.h"
#include "base/logging.h"

#if 0

typedef struct ssl_ctx_st *GM_CONTEXT;
typedef struct ssl_st *GM_STREAM;
typedef struct ssl_session_st *GM_SESSION;
typedef struct x509_st *GM_CERT;
typedef struct x509_store_ctx_st *GM_CERT_STROE_CONTEXT;

#else

typedef void *GM_CONTEXT;
typedef void *GM_STREAM;
typedef void *GM_SESSION;
typedef void *GM_CERT;
typedef void *GM_CERT_STROE_CONTEXT;
typedef void *GM_ENGINE;

#endif


#define GM_VERIFY_NONE 					0
#define GM_VERIFY_PEER 					0x01 | 0x02

#define GM_VERIFY_FAIL 					0
#define GM_VERIFY_OK 					1

#define  GMERR_OK 						( 0)

#define  GMERR_NOMEM 					(-1)
#define  GMERR_INVALPARAM 				(-2)

#define  GMERR_INVALCERT 				(-3)
#define  GMERR_INVALKEY 				(-4)

#define  GMERR_STREAM_BROKEN 			(-5)
#define  GMERR_STREAM_WOULDBLOCK 		(-6)

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
typedef enum {FALSE = 0, TRUE = 1} BOOL;
#endif
#endif

#define GMBUFFERLEN 64
#define CERTIFICATES_PATH ".\\"
#define SANGFORDLL "sangfor.dll"
#define ERROR_MESSAGE() printf("[%s:%d]\n", __FUNCTION__, __LINE__)

typedef
int (*GM_CERT_VERIFY_CALLBACK)(
	_In 	int 							n_verifyok,
	_In 	GM_CERT_STROE_CONTEXT 			gm_cert_stroe_context
);

typedef
int (*GM_STREAM_ERROR_CALLBACK)(
	_In 	const char* 					c_sz_error_string,
	_In 	size_t 							n_error_string_length,
	_In 	void* 							pv_userdata
);

#ifdef WIN32
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
	extern int(*GM_init)();
	extern int(*GMContextCreate)(
		_Out GM_CONTEXT *ctx,
		_Out GM_ENGINE *skf_engine
		);
	extern int(*GMContextSetSignatureCertificate)(
		_In GM_STREAM ssl,
		_In const char *c_sz_certificate,
		_In const char *c_sz_key,
		_In GM_ENGINE skf_engine
		);
	extern int(*GMContextSetEncryptionCertificate)(
		_In GM_STREAM ssl,
		_In const char *c_sz_certificate,
		_In const char *c_sz_key,
		_In GM_ENGINE skf_engine
		);
	extern int(*GMContextSetVerify)(
		_In GM_CONTEXT ctx,
		_In UINT u_verify_mode,
		_In_opt_ GM_CERT_VERIFY_CALLBACK fnCallback,
		_In_opt_ const char *c_sz_cacertificate,
		_In_opt_ UINT u_verify_depth);
	extern int(*GMContextSetCipherList)(
		_In GM_CONTEXT ctx,
		_In const char *c_szcipher_list
		);
	extern int(*GMContextDestroy)(
		_In GM_CONTEXT ctx,
		_In GM_ENGINE skf_engine
		);
	extern int(*GMStreamCreate)(
		_Out GM_STREAM *ssl,
		_In GM_CONTEXT ctx
		);
	extern int(*GMStreamSetUserData)(
		_In 	GM_STREAM 						gm_stream,
		_In 	void* 							pv_userdata
		);
	extern int(*GMStreamGetUserData)(
		_In 	GM_STREAM 						gm_stream,
		_Out 	void** 							ppv_userdata
		);
	extern int(*GMStreamSetHostName)(
		_In 	GM_STREAM 						gm_stream,
		_In 	const char* 					c_szhostname
		);
	extern int(*GMStreamSetSocket)(
		_In GM_STREAM ssl,
		_In int sockfd
		);
	extern int(*GMStreamGetPeerCertificate)(
		_In 	GM_STREAM 						gm_stream,
		_Out 	GM_CERT* 						pgm_cert
		);
	extern int(*GMStreamInHandshake)(
		_In 	GM_STREAM 						gm_stream,
		_Out 	BOOL* 							pb_state
		);
	extern int(*GMStreamGetError)(
		_In 		GM_STREAM 					gm_stream,
		_Out 		int* 						pn_error_reason,
		_In_opt_ 	GM_STREAM_ERROR_CALLBACK 	fnCallback,
		_In_opt_ 	void* 						pv_userdata
		);
	extern int(*GMStreamConnect)(
		_In GM_STREAM ssl
		);
	extern int(*GMStreamWrite)(
		_In GM_STREAM ssl,
		_In const void* pcv_buffer,
		_In UINT u_buffer_size
		);
	extern int(*GMStreamRead)(
		_In GM_STREAM ssl,
		_Out void* pcv_buffer,
		_In UINT u_buffer_size
		);
	extern int(*GMStreamShutdown)(
		_In GM_STREAM ssl
		);
	extern int(*GMStreamDestroy)(
		_In GM_STREAM ssl
		);
	extern int(*GMStreamGetSession)(
		_In GM_STREAM ssl,
		_Out GM_SESSION *session
		);
	extern int(*GMStreamSetSession)(
		_In GM_STREAM ssl,
		_Out GM_SESSION session
		);
	extern int(*GMSessionDestroy)(
		_In GM_STREAM ssl
		);

	extern int(*GMCertStoreContextGetStream)(
		_Out 	GM_STREAM* 						pgm_stream,
		_In 	GM_CERT_STROE_CONTEXT 			gm_certStore_context
		);
	extern int(*GMCertStoreContextGetError)(
		_In 	GM_CERT_STROE_CONTEXT 			gm_certStore_context,
		_Out 	int* 							pn_error_number
		);
	extern char* (*GMCertGetErrorString)(
		_In 	int 							nError
		);
	extern int(*GMCertStoreContextGetErrorDepth)(
		_In 	GM_CERT_STROE_CONTEXT 			gm_certStore_context,
		_Out 	int* 							pn_error_depth
		);
	extern int(*GMCertStoreContextGetCurrentCert)(
		_In 	GM_CERT_STROE_CONTEXT 			gm_certStore_context,
		_Out 	GM_CERT* 						pgm_cert
		);
	extern void(*GMCertDestroy)(
		_In 	GM_CERT 						gm_cert
		);
#ifdef __cplusplus
}
#endif //__cplusplus
#endif //WIN32

static __inline
int ErrorPrint(
	const char* 			c_sz_error_string,
	size_t 					n_error_string_length,
	void* 					pv_userdata
)
{
	LOG(INFO) << c_sz_error_string;
	return 1;
}

class SanforGMStream {
public:
	SanforGMStream();
	~SanforGMStream();
	int Init();
	int GMReadResponseData(net::IOBuffer* buf, int buf_len
  /*, const net::CompletionCallback& callback*/);
	int GMReadResponseData(net::IOBuffer* buf, int buf_len,
    const net::CompletionCallback& callback);
	int GMWriteRequestData(const char * c_sz_request, int request_len,
    const net::CompletionCallback & callback);
	int GMHandShake();
	int GMCreateDataStream(int n_client);
	GM_CONTEXT GetGMContext() { return gm_context_; }
	GM_STREAM GetGMStream() { return gm_stream_;  }
	void Destroy();
	HMODULE module = NULL;

private:
	int SocketLibStartup();
	int LoadDLL(const char* c_szDllFileName);
	GM_CONTEXT GMContextInit();

	GM_CONTEXT gm_context_ = NULL;
	GM_STREAM gm_stream_ = NULL;
	GM_ENGINE gm_engine_ = NULL;
	static GM_SESSION s_gm_session_;
	bool gm_flags_ = true;
};

#endif // GM_SANGFOR_INTERFACE_H
