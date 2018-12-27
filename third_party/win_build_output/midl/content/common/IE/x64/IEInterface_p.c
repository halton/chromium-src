

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 8.xx.xxxx */
/* at a redacted point in time
 */
/* Compiler settings for ../../content/common/IE/IEInterface.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.xx.xxxx 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif /* __RPCPROXY_H_VERSION__ */


#include "IEInterface.h"

#define TYPE_FORMAT_STRING_SIZE   1051                              
#define PROC_FORMAT_STRING_SIZE   1661                              
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   2            

typedef struct _IEInterface_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } IEInterface_MIDL_TYPE_FORMAT_STRING;

typedef struct _IEInterface_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } IEInterface_MIDL_PROC_FORMAT_STRING;

typedef struct _IEInterface_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } IEInterface_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const IEInterface_MIDL_TYPE_FORMAT_STRING IEInterface__MIDL_TypeFormatString;
extern const IEInterface_MIDL_PROC_FORMAT_STRING IEInterface__MIDL_ProcFormatString;
extern const IEInterface_MIDL_EXPR_FORMAT_STRING IEInterface__MIDL_ExprFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IBrowserContainer_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IBrowserContainer_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IBrowserEventHandler_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IBrowserEventHandler_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IHttpMonitor_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IHttpMonitor_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const IEInterface_MIDL_PROC_FORMAT_STRING IEInterface__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure CreateBrowser */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
/*  8 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 10 */	NdrFcShort( 0x16 ),	/* 22 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 16 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hWnd */

/* 26 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 28 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 30 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter browserEmulation */

/* 32 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 34 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 36 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter isNewWindow */

/* 38 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 40 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 42 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 44 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 46 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_WebBrowser */

/* 50 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x8 ),	/* 8 */
/* 58 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x8 ),	/* 8 */
/* 64 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 66 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 68 */	NdrFcShort( 0x0 ),	/* 0 */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */
/* 72 */	NdrFcShort( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pVal */

/* 76 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 78 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 80 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 82 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 84 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 86 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CloseBrowser */

/* 88 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 90 */	NdrFcLong( 0x0 ),	/* 0 */
/* 94 */	NdrFcShort( 0x9 ),	/* 9 */
/* 96 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 98 */	NdrFcShort( 0x0 ),	/* 0 */
/* 100 */	NdrFcShort( 0x8 ),	/* 8 */
/* 102 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 104 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 106 */	NdrFcShort( 0x0 ),	/* 0 */
/* 108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 112 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 114 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 116 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 118 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetRect */

/* 120 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 122 */	NdrFcLong( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0xa ),	/* 10 */
/* 128 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 130 */	NdrFcShort( 0x20 ),	/* 32 */
/* 132 */	NdrFcShort( 0x8 ),	/* 8 */
/* 134 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 136 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 140 */	NdrFcShort( 0x0 ),	/* 0 */
/* 142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 144 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter left */

/* 146 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 148 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 150 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter top */

/* 152 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 154 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 156 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter right */

/* 158 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 160 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 162 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter bottom */

/* 164 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 166 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 170 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 172 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 174 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure putref_HostApp */

/* 176 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 178 */	NdrFcLong( 0x0 ),	/* 0 */
/* 182 */	NdrFcShort( 0xb ),	/* 11 */
/* 184 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 188 */	NdrFcShort( 0x8 ),	/* 8 */
/* 190 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 192 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 198 */	NdrFcShort( 0x0 ),	/* 0 */
/* 200 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter app */

/* 202 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 204 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 206 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 208 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 210 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ExcuteJS */

/* 214 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 216 */	NdrFcLong( 0x0 ),	/* 0 */
/* 220 */	NdrFcShort( 0xc ),	/* 12 */
/* 222 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 224 */	NdrFcShort( 0x0 ),	/* 0 */
/* 226 */	NdrFcShort( 0x8 ),	/* 8 */
/* 228 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 230 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 232 */	NdrFcShort( 0x1 ),	/* 1 */
/* 234 */	NdrFcShort( 0x1 ),	/* 1 */
/* 236 */	NdrFcShort( 0x0 ),	/* 0 */
/* 238 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter src */

/* 240 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 242 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 244 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter result */

/* 246 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 248 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 250 */	NdrFcShort( 0x3e4 ),	/* Type Offset=996 */

	/* Return value */

/* 252 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 254 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 256 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Show */

/* 258 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 260 */	NdrFcLong( 0x0 ),	/* 0 */
/* 264 */	NdrFcShort( 0xd ),	/* 13 */
/* 266 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 268 */	NdrFcShort( 0x6 ),	/* 6 */
/* 270 */	NdrFcShort( 0x8 ),	/* 8 */
/* 272 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 274 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 280 */	NdrFcShort( 0x0 ),	/* 0 */
/* 282 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bShow */

/* 284 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 286 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 288 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 290 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 292 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 294 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDocumentMode */

/* 296 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 298 */	NdrFcLong( 0x0 ),	/* 0 */
/* 302 */	NdrFcShort( 0xe ),	/* 14 */
/* 304 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 308 */	NdrFcShort( 0x24 ),	/* 36 */
/* 310 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 312 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 318 */	NdrFcShort( 0x0 ),	/* 0 */
/* 320 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter mode */

/* 322 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 324 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 326 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 328 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 330 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 332 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetDocumentMode */

/* 334 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 336 */	NdrFcLong( 0x0 ),	/* 0 */
/* 340 */	NdrFcShort( 0xf ),	/* 15 */
/* 342 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 344 */	NdrFcShort( 0x8 ),	/* 8 */
/* 346 */	NdrFcShort( 0x8 ),	/* 8 */
/* 348 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 350 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 356 */	NdrFcShort( 0x0 ),	/* 0 */
/* 358 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter mode */

/* 360 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 362 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 364 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 366 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 368 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 370 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetCookie */

/* 372 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 374 */	NdrFcLong( 0x0 ),	/* 0 */
/* 378 */	NdrFcShort( 0x10 ),	/* 16 */
/* 380 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 382 */	NdrFcShort( 0x8 ),	/* 8 */
/* 384 */	NdrFcShort( 0x8 ),	/* 8 */
/* 386 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 388 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 390 */	NdrFcShort( 0x0 ),	/* 0 */
/* 392 */	NdrFcShort( 0x1 ),	/* 1 */
/* 394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 396 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter json */

/* 398 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 400 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 402 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter flag */

/* 404 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 406 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 408 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 410 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 412 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 414 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetUserAgent */

/* 416 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 418 */	NdrFcLong( 0x0 ),	/* 0 */
/* 422 */	NdrFcShort( 0x11 ),	/* 17 */
/* 424 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 426 */	NdrFcShort( 0x0 ),	/* 0 */
/* 428 */	NdrFcShort( 0x8 ),	/* 8 */
/* 430 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 432 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 434 */	NdrFcShort( 0x0 ),	/* 0 */
/* 436 */	NdrFcShort( 0x1 ),	/* 1 */
/* 438 */	NdrFcShort( 0x0 ),	/* 0 */
/* 440 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter usStr */

/* 442 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 444 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 446 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 448 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 450 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 452 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure FinishDownload */

/* 454 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 456 */	NdrFcLong( 0x0 ),	/* 0 */
/* 460 */	NdrFcShort( 0x12 ),	/* 18 */
/* 462 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 464 */	NdrFcShort( 0x6 ),	/* 6 */
/* 466 */	NdrFcShort( 0x8 ),	/* 8 */
/* 468 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 470 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 472 */	NdrFcShort( 0x0 ),	/* 0 */
/* 474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 478 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter isCancel */

/* 480 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 482 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 484 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 486 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 488 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 490 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetFunctionControl */

/* 492 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 494 */	NdrFcLong( 0x0 ),	/* 0 */
/* 498 */	NdrFcShort( 0x13 ),	/* 19 */
/* 500 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 502 */	NdrFcShort( 0x0 ),	/* 0 */
/* 504 */	NdrFcShort( 0x8 ),	/* 8 */
/* 506 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 508 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 512 */	NdrFcShort( 0x1 ),	/* 1 */
/* 514 */	NdrFcShort( 0x0 ),	/* 0 */
/* 516 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter jsonStr */

/* 518 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 520 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 522 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 524 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 526 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 528 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AutoLogin */

/* 530 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 532 */	NdrFcLong( 0x0 ),	/* 0 */
/* 536 */	NdrFcShort( 0x14 ),	/* 20 */
/* 538 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 540 */	NdrFcShort( 0x0 ),	/* 0 */
/* 542 */	NdrFcShort( 0x8 ),	/* 8 */
/* 544 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 546 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 548 */	NdrFcShort( 0x0 ),	/* 0 */
/* 550 */	NdrFcShort( 0x1 ),	/* 1 */
/* 552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 554 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter frameXPath */

/* 556 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 558 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 560 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter userNameXPath */

/* 562 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 564 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 566 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter userNameValue */

/* 568 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 570 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 572 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter pwdXPath */

/* 574 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 576 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 578 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter pwdValue */

/* 580 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 582 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 584 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter loginBtnXPath */

/* 586 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 588 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 590 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 592 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 594 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 596 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetListenLoginXPath */

/* 598 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 600 */	NdrFcLong( 0x0 ),	/* 0 */
/* 604 */	NdrFcShort( 0x15 ),	/* 21 */
/* 606 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 610 */	NdrFcShort( 0x8 ),	/* 8 */
/* 612 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 614 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 618 */	NdrFcShort( 0x1 ),	/* 1 */
/* 620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 622 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter url */

/* 624 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 626 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 628 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter frameXPath */

/* 630 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 632 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 634 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter userNameXPath */

/* 636 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 638 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 640 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter pwdXPath */

/* 642 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 644 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 646 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter loginbtnXPath */

/* 648 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 650 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 652 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 654 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 656 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 658 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NotifyCommand */

/* 660 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 662 */	NdrFcLong( 0x0 ),	/* 0 */
/* 666 */	NdrFcShort( 0x16 ),	/* 22 */
/* 668 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 670 */	NdrFcShort( 0x0 ),	/* 0 */
/* 672 */	NdrFcShort( 0x8 ),	/* 8 */
/* 674 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 676 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 678 */	NdrFcShort( 0x0 ),	/* 0 */
/* 680 */	NdrFcShort( 0x1 ),	/* 1 */
/* 682 */	NdrFcShort( 0x0 ),	/* 0 */
/* 684 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter command */

/* 686 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 688 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 690 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 692 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 694 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 696 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDevToolsOpenStatus */

/* 698 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 700 */	NdrFcLong( 0x0 ),	/* 0 */
/* 704 */	NdrFcShort( 0x17 ),	/* 23 */
/* 706 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 708 */	NdrFcShort( 0x1a ),	/* 26 */
/* 710 */	NdrFcShort( 0x22 ),	/* 34 */
/* 712 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 714 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 716 */	NdrFcShort( 0x0 ),	/* 0 */
/* 718 */	NdrFcShort( 0x0 ),	/* 0 */
/* 720 */	NdrFcShort( 0x0 ),	/* 0 */
/* 722 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter isOpen */

/* 724 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 726 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 728 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 730 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 732 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 734 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ShowDevTools */

/* 736 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 738 */	NdrFcLong( 0x0 ),	/* 0 */
/* 742 */	NdrFcShort( 0x18 ),	/* 24 */
/* 744 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 746 */	NdrFcShort( 0x6 ),	/* 6 */
/* 748 */	NdrFcShort( 0x8 ),	/* 8 */
/* 750 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 752 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 754 */	NdrFcShort( 0x0 ),	/* 0 */
/* 756 */	NdrFcShort( 0x0 ),	/* 0 */
/* 758 */	NdrFcShort( 0x0 ),	/* 0 */
/* 760 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter show */

/* 762 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 764 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 766 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 768 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 770 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 772 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnBeforeNavigate */

/* 774 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 776 */	NdrFcLong( 0x0 ),	/* 0 */
/* 780 */	NdrFcShort( 0x7 ),	/* 7 */
/* 782 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 784 */	NdrFcShort( 0x20 ),	/* 32 */
/* 786 */	NdrFcShort( 0x22 ),	/* 34 */
/* 788 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 790 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 792 */	NdrFcShort( 0x0 ),	/* 0 */
/* 794 */	NdrFcShort( 0x1 ),	/* 1 */
/* 796 */	NdrFcShort( 0x0 ),	/* 0 */
/* 798 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter url */

/* 800 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 802 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 804 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter navNewUrl */

/* 806 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 808 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 810 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter cancel */

/* 812 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 814 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 816 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 818 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 820 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 822 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnFinishNavigate */

/* 824 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 826 */	NdrFcLong( 0x0 ),	/* 0 */
/* 830 */	NdrFcShort( 0x8 ),	/* 8 */
/* 832 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 834 */	NdrFcShort( 0x0 ),	/* 0 */
/* 836 */	NdrFcShort( 0x8 ),	/* 8 */
/* 838 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 840 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 842 */	NdrFcShort( 0x0 ),	/* 0 */
/* 844 */	NdrFcShort( 0x1 ),	/* 1 */
/* 846 */	NdrFcShort( 0x0 ),	/* 0 */
/* 848 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter url */

/* 850 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 852 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 854 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter faviconUrls */

/* 856 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 858 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 860 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 862 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 864 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 866 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnTitleChange */

/* 868 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 870 */	NdrFcLong( 0x0 ),	/* 0 */
/* 874 */	NdrFcShort( 0x9 ),	/* 9 */
/* 876 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 878 */	NdrFcShort( 0x0 ),	/* 0 */
/* 880 */	NdrFcShort( 0x8 ),	/* 8 */
/* 882 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 884 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 886 */	NdrFcShort( 0x0 ),	/* 0 */
/* 888 */	NdrFcShort( 0x1 ),	/* 1 */
/* 890 */	NdrFcShort( 0x0 ),	/* 0 */
/* 892 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter title */

/* 894 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 896 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 898 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 900 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 902 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 904 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnLoadUrlNewWnd */

/* 906 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 908 */	NdrFcLong( 0x0 ),	/* 0 */
/* 912 */	NdrFcShort( 0xa ),	/* 10 */
/* 914 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 916 */	NdrFcShort( 0x22 ),	/* 34 */
/* 918 */	NdrFcShort( 0x22 ),	/* 34 */
/* 920 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 922 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 924 */	NdrFcShort( 0x0 ),	/* 0 */
/* 926 */	NdrFcShort( 0x1 ),	/* 1 */
/* 928 */	NdrFcShort( 0x0 ),	/* 0 */
/* 930 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter url */

/* 932 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 934 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 936 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter flag */

/* 938 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 940 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 942 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter cancel */

/* 944 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 946 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 948 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter pDisp */

/* 950 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 952 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 954 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 956 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 958 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 960 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnBrowserClosing */

/* 962 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 964 */	NdrFcLong( 0x0 ),	/* 0 */
/* 968 */	NdrFcShort( 0xb ),	/* 11 */
/* 970 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 972 */	NdrFcShort( 0x0 ),	/* 0 */
/* 974 */	NdrFcShort( 0x8 ),	/* 8 */
/* 976 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 978 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 980 */	NdrFcShort( 0x0 ),	/* 0 */
/* 982 */	NdrFcShort( 0x0 ),	/* 0 */
/* 984 */	NdrFcShort( 0x0 ),	/* 0 */
/* 986 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 988 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 990 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 992 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnBeforeBrowserClose */

/* 994 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 996 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1000 */	NdrFcShort( 0xc ),	/* 12 */
/* 1002 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1004 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1006 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1008 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1010 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1012 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1014 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1016 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1018 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1020 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1022 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1024 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnShowContextMenu */

/* 1026 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1028 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1032 */	NdrFcShort( 0xd ),	/* 13 */
/* 1034 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1036 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1038 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1040 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 1042 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1044 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1046 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1048 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1050 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dwID */

/* 1052 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1054 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1056 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter x */

/* 1058 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1060 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1062 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter y */

/* 1064 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1066 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1068 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1070 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1072 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1074 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnIEServerWndCreated */

/* 1076 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1078 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1082 */	NdrFcShort( 0xe ),	/* 14 */
/* 1084 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1086 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1088 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1090 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1092 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1094 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1096 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1098 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1100 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hwnd */

/* 1102 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1104 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1106 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1108 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1110 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1112 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnDownLoadFile */

/* 1114 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1116 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1120 */	NdrFcShort( 0xf ),	/* 15 */
/* 1122 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1124 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1126 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1128 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1130 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1134 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1138 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter url */

/* 1140 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1142 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1144 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter status */

/* 1146 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1148 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1150 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter variant */

/* 1152 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1154 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1156 */	NdrFcShort( 0x3fe ),	/* Type Offset=1022 */

	/* Return value */

/* 1158 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1160 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1162 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnCanGoBackOrForward */

/* 1164 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1166 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1170 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1172 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1174 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1176 */	NdrFcShort( 0x3c ),	/* 60 */
/* 1178 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1180 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1182 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1188 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter back */

/* 1190 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1192 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1194 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter forward */

/* 1196 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1198 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1200 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 1202 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1204 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1206 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnGoBack */

/* 1208 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1210 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1214 */	NdrFcShort( 0x11 ),	/* 17 */
/* 1216 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1218 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1220 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1222 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1224 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1228 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1232 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1234 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1236 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1238 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnGoForward */

/* 1240 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1242 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1246 */	NdrFcShort( 0x12 ),	/* 18 */
/* 1248 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1252 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1254 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1256 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1264 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1266 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1268 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1270 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnRequestChangePos */

/* 1272 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1274 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1278 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1280 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 1282 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1284 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1286 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 1288 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1290 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1292 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1296 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter left */

/* 1298 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1300 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1302 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter top */

/* 1304 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1306 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1308 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter width */

/* 1310 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1312 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1314 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter height */

/* 1316 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1318 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1320 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1322 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1324 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1326 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnGetMainWndPos */

/* 1328 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1330 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1334 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1336 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 1338 */	NdrFcShort( 0x70 ),	/* 112 */
/* 1340 */	NdrFcShort( 0x78 ),	/* 120 */
/* 1342 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 1344 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1348 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1352 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter left */

/* 1354 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1356 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1358 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter top */

/* 1360 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1362 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1364 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter width */

/* 1366 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1368 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1370 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter height */

/* 1372 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1374 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1378 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1380 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1382 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnUpdateCookie */

/* 1384 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1386 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1390 */	NdrFcShort( 0x15 ),	/* 21 */
/* 1392 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1396 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1398 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 1400 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1404 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1408 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cookie */

/* 1410 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1412 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1414 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 1416 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1418 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1420 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnGetLoginInfo */

/* 1422 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1424 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1428 */	NdrFcShort( 0x16 ),	/* 22 */
/* 1430 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1434 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1436 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1438 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1442 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1446 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter url */

/* 1448 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1450 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1452 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter userName */

/* 1454 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1456 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1458 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter userPwd */

/* 1460 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1462 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1464 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 1466 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1468 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1470 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnRequestAcceleratorFromKeyboard */

/* 1472 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1474 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1478 */	NdrFcShort( 0x17 ),	/* 23 */
/* 1480 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1482 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1484 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1486 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1488 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1490 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1492 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1496 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter keyCode */

/* 1498 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1500 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1502 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter modifiers */

/* 1504 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1506 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1508 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1510 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1512 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1514 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnRequestAcceleratorFromMouseWheel */

/* 1516 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1518 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1522 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1524 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 1526 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1528 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1530 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 1532 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1536 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1538 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1540 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter delta */

/* 1542 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1544 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1546 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Pt_x */

/* 1548 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1550 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1552 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Pt_y */

/* 1554 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1556 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1558 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter modifiers */

/* 1560 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1562 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1564 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1566 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1568 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 1570 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnWinDomainAuthenticate */

/* 1572 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1574 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1578 */	NdrFcShort( 0x19 ),	/* 25 */
/* 1580 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1584 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1586 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1588 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 1590 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1592 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1594 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1596 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter userName */

/* 1598 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1600 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1602 */	NdrFcShort( 0x410 ),	/* Type Offset=1040 */

	/* Parameter userPwd */

/* 1604 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1606 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1608 */	NdrFcShort( 0x410 ),	/* Type Offset=1040 */

	/* Return value */

/* 1610 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1612 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1614 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnQueryPrivateDNS */

/* 1616 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1618 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1622 */	NdrFcShort( 0x1a ),	/* 26 */
/* 1624 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 1626 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1628 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1630 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1632 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 1634 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1636 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1638 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1640 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter host */

/* 1642 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1644 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1646 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter ipList */

/* 1648 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1650 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1652 */	NdrFcShort( 0x410 ),	/* Type Offset=1040 */

	/* Return value */

/* 1654 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1656 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1658 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const IEInterface_MIDL_TYPE_FORMAT_STRING IEInterface__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/*  4 */	NdrFcShort( 0x2 ),	/* Offset= 2 (6) */
/*  6 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/*  8 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 12 */	NdrFcShort( 0x0 ),	/* 0 */
/* 14 */	NdrFcShort( 0x0 ),	/* 0 */
/* 16 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 18 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 20 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 22 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 24 */	
			0x12, 0x0,	/* FC_UP */
/* 26 */	NdrFcShort( 0xe ),	/* Offset= 14 (40) */
/* 28 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 30 */	NdrFcShort( 0x2 ),	/* 2 */
/* 32 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 34 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 36 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 38 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 40 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 42 */	NdrFcShort( 0x8 ),	/* 8 */
/* 44 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (28) */
/* 46 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 48 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 50 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 52 */	NdrFcShort( 0x0 ),	/* 0 */
/* 54 */	NdrFcShort( 0x8 ),	/* 8 */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0xffde ),	/* Offset= -34 (24) */
/* 60 */	
			0x11, 0x0,	/* FC_RP */
/* 62 */	NdrFcShort( 0x3a6 ),	/* Offset= 934 (996) */
/* 64 */	
			0x13, 0x0,	/* FC_OP */
/* 66 */	NdrFcShort( 0x38e ),	/* Offset= 910 (976) */
/* 68 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 70 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 72 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 74 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 76 */	NdrFcShort( 0x2 ),	/* Offset= 2 (78) */
/* 78 */	NdrFcShort( 0x10 ),	/* 16 */
/* 80 */	NdrFcShort( 0x2f ),	/* 47 */
/* 82 */	NdrFcLong( 0x14 ),	/* 20 */
/* 86 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 88 */	NdrFcLong( 0x3 ),	/* 3 */
/* 92 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 94 */	NdrFcLong( 0x11 ),	/* 17 */
/* 98 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 100 */	NdrFcLong( 0x2 ),	/* 2 */
/* 104 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 106 */	NdrFcLong( 0x4 ),	/* 4 */
/* 110 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 112 */	NdrFcLong( 0x5 ),	/* 5 */
/* 116 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 118 */	NdrFcLong( 0xb ),	/* 11 */
/* 122 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 124 */	NdrFcLong( 0xa ),	/* 10 */
/* 128 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 130 */	NdrFcLong( 0x6 ),	/* 6 */
/* 134 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (366) */
/* 136 */	NdrFcLong( 0x7 ),	/* 7 */
/* 140 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 142 */	NdrFcLong( 0x8 ),	/* 8 */
/* 146 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (372) */
/* 148 */	NdrFcLong( 0xd ),	/* 13 */
/* 152 */	NdrFcShort( 0xe0 ),	/* Offset= 224 (376) */
/* 154 */	NdrFcLong( 0x9 ),	/* 9 */
/* 158 */	NdrFcShort( 0xff68 ),	/* Offset= -152 (6) */
/* 160 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 164 */	NdrFcShort( 0xe6 ),	/* Offset= 230 (394) */
/* 166 */	NdrFcLong( 0x24 ),	/* 36 */
/* 170 */	NdrFcShort( 0x2dc ),	/* Offset= 732 (902) */
/* 172 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 176 */	NdrFcShort( 0x2d6 ),	/* Offset= 726 (902) */
/* 178 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 182 */	NdrFcShort( 0x2d4 ),	/* Offset= 724 (906) */
/* 184 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 188 */	NdrFcShort( 0x2d2 ),	/* Offset= 722 (910) */
/* 190 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 194 */	NdrFcShort( 0x2d0 ),	/* Offset= 720 (914) */
/* 196 */	NdrFcLong( 0x4014 ),	/* 16404 */
/* 200 */	NdrFcShort( 0x2ce ),	/* Offset= 718 (918) */
/* 202 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 206 */	NdrFcShort( 0x2cc ),	/* Offset= 716 (922) */
/* 208 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 212 */	NdrFcShort( 0x2ca ),	/* Offset= 714 (926) */
/* 214 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 218 */	NdrFcShort( 0x2b4 ),	/* Offset= 692 (910) */
/* 220 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 224 */	NdrFcShort( 0x2b2 ),	/* Offset= 690 (914) */
/* 226 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 230 */	NdrFcShort( 0x2bc ),	/* Offset= 700 (930) */
/* 232 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 236 */	NdrFcShort( 0x2b2 ),	/* Offset= 690 (926) */
/* 238 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 242 */	NdrFcShort( 0x2b4 ),	/* Offset= 692 (934) */
/* 244 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 248 */	NdrFcShort( 0x2b2 ),	/* Offset= 690 (938) */
/* 250 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 254 */	NdrFcShort( 0x2b0 ),	/* Offset= 688 (942) */
/* 256 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 260 */	NdrFcShort( 0x2ae ),	/* Offset= 686 (946) */
/* 262 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 266 */	NdrFcShort( 0x2ac ),	/* Offset= 684 (950) */
/* 268 */	NdrFcLong( 0x10 ),	/* 16 */
/* 272 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 274 */	NdrFcLong( 0x12 ),	/* 18 */
/* 278 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 280 */	NdrFcLong( 0x13 ),	/* 19 */
/* 284 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 286 */	NdrFcLong( 0x15 ),	/* 21 */
/* 290 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 292 */	NdrFcLong( 0x16 ),	/* 22 */
/* 296 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 298 */	NdrFcLong( 0x17 ),	/* 23 */
/* 302 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 304 */	NdrFcLong( 0xe ),	/* 14 */
/* 308 */	NdrFcShort( 0x28a ),	/* Offset= 650 (958) */
/* 310 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 314 */	NdrFcShort( 0x28e ),	/* Offset= 654 (968) */
/* 316 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 320 */	NdrFcShort( 0x28c ),	/* Offset= 652 (972) */
/* 322 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 326 */	NdrFcShort( 0x248 ),	/* Offset= 584 (910) */
/* 328 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 332 */	NdrFcShort( 0x246 ),	/* Offset= 582 (914) */
/* 334 */	NdrFcLong( 0x4015 ),	/* 16405 */
/* 338 */	NdrFcShort( 0x244 ),	/* Offset= 580 (918) */
/* 340 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 344 */	NdrFcShort( 0x23a ),	/* Offset= 570 (914) */
/* 346 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 350 */	NdrFcShort( 0x234 ),	/* Offset= 564 (914) */
/* 352 */	NdrFcLong( 0x0 ),	/* 0 */
/* 356 */	NdrFcShort( 0x0 ),	/* Offset= 0 (356) */
/* 358 */	NdrFcLong( 0x1 ),	/* 1 */
/* 362 */	NdrFcShort( 0x0 ),	/* Offset= 0 (362) */
/* 364 */	NdrFcShort( 0xffff ),	/* Offset= -1 (363) */
/* 366 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 368 */	NdrFcShort( 0x8 ),	/* 8 */
/* 370 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 372 */	
			0x13, 0x0,	/* FC_OP */
/* 374 */	NdrFcShort( 0xfeb2 ),	/* Offset= -334 (40) */
/* 376 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 378 */	NdrFcLong( 0x0 ),	/* 0 */
/* 382 */	NdrFcShort( 0x0 ),	/* 0 */
/* 384 */	NdrFcShort( 0x0 ),	/* 0 */
/* 386 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 388 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 390 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 392 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 394 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 396 */	NdrFcShort( 0x2 ),	/* Offset= 2 (398) */
/* 398 */	
			0x13, 0x0,	/* FC_OP */
/* 400 */	NdrFcShort( 0x1e4 ),	/* Offset= 484 (884) */
/* 402 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x89,		/* 137 */
/* 404 */	NdrFcShort( 0x20 ),	/* 32 */
/* 406 */	NdrFcShort( 0xa ),	/* 10 */
/* 408 */	NdrFcLong( 0x8 ),	/* 8 */
/* 412 */	NdrFcShort( 0x50 ),	/* Offset= 80 (492) */
/* 414 */	NdrFcLong( 0xd ),	/* 13 */
/* 418 */	NdrFcShort( 0x70 ),	/* Offset= 112 (530) */
/* 420 */	NdrFcLong( 0x9 ),	/* 9 */
/* 424 */	NdrFcShort( 0x90 ),	/* Offset= 144 (568) */
/* 426 */	NdrFcLong( 0xc ),	/* 12 */
/* 430 */	NdrFcShort( 0xb0 ),	/* Offset= 176 (606) */
/* 432 */	NdrFcLong( 0x24 ),	/* 36 */
/* 436 */	NdrFcShort( 0x102 ),	/* Offset= 258 (694) */
/* 438 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 442 */	NdrFcShort( 0x11e ),	/* Offset= 286 (728) */
/* 444 */	NdrFcLong( 0x10 ),	/* 16 */
/* 448 */	NdrFcShort( 0x138 ),	/* Offset= 312 (760) */
/* 450 */	NdrFcLong( 0x2 ),	/* 2 */
/* 454 */	NdrFcShort( 0x14e ),	/* Offset= 334 (788) */
/* 456 */	NdrFcLong( 0x3 ),	/* 3 */
/* 460 */	NdrFcShort( 0x164 ),	/* Offset= 356 (816) */
/* 462 */	NdrFcLong( 0x14 ),	/* 20 */
/* 466 */	NdrFcShort( 0x17a ),	/* Offset= 378 (844) */
/* 468 */	NdrFcShort( 0xffff ),	/* Offset= -1 (467) */
/* 470 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 472 */	NdrFcShort( 0x0 ),	/* 0 */
/* 474 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 478 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 480 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 484 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 486 */	
			0x13, 0x0,	/* FC_OP */
/* 488 */	NdrFcShort( 0xfe40 ),	/* Offset= -448 (40) */
/* 490 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 492 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 494 */	NdrFcShort( 0x10 ),	/* 16 */
/* 496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 498 */	NdrFcShort( 0x6 ),	/* Offset= 6 (504) */
/* 500 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 502 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 504 */	
			0x11, 0x0,	/* FC_RP */
/* 506 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (470) */
/* 508 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 512 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 514 */	NdrFcShort( 0x0 ),	/* 0 */
/* 516 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 518 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 522 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 524 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 526 */	NdrFcShort( 0xff6a ),	/* Offset= -150 (376) */
/* 528 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 530 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 532 */	NdrFcShort( 0x10 ),	/* 16 */
/* 534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 536 */	NdrFcShort( 0x6 ),	/* Offset= 6 (542) */
/* 538 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 540 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 542 */	
			0x11, 0x0,	/* FC_RP */
/* 544 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (508) */
/* 546 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 548 */	NdrFcShort( 0x0 ),	/* 0 */
/* 550 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 554 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 556 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 560 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 562 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 564 */	NdrFcShort( 0xfdd2 ),	/* Offset= -558 (6) */
/* 566 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 568 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 570 */	NdrFcShort( 0x10 ),	/* 16 */
/* 572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 574 */	NdrFcShort( 0x6 ),	/* Offset= 6 (580) */
/* 576 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 578 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 580 */	
			0x11, 0x0,	/* FC_RP */
/* 582 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (546) */
/* 584 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 586 */	NdrFcShort( 0x0 ),	/* 0 */
/* 588 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 592 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 594 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 598 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 600 */	
			0x13, 0x0,	/* FC_OP */
/* 602 */	NdrFcShort( 0x176 ),	/* Offset= 374 (976) */
/* 604 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 606 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 608 */	NdrFcShort( 0x10 ),	/* 16 */
/* 610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 612 */	NdrFcShort( 0x6 ),	/* Offset= 6 (618) */
/* 614 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 616 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 618 */	
			0x11, 0x0,	/* FC_RP */
/* 620 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (584) */
/* 622 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 624 */	NdrFcLong( 0x2f ),	/* 47 */
/* 628 */	NdrFcShort( 0x0 ),	/* 0 */
/* 630 */	NdrFcShort( 0x0 ),	/* 0 */
/* 632 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 634 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 636 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 638 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 640 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 642 */	NdrFcShort( 0x1 ),	/* 1 */
/* 644 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 646 */	NdrFcShort( 0x4 ),	/* 4 */
/* 648 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 650 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 652 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 654 */	NdrFcShort( 0x18 ),	/* 24 */
/* 656 */	NdrFcShort( 0x0 ),	/* 0 */
/* 658 */	NdrFcShort( 0xa ),	/* Offset= 10 (668) */
/* 660 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 662 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 664 */	NdrFcShort( 0xffd6 ),	/* Offset= -42 (622) */
/* 666 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 668 */	
			0x13, 0x0,	/* FC_OP */
/* 670 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (640) */
/* 672 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 674 */	NdrFcShort( 0x0 ),	/* 0 */
/* 676 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 678 */	NdrFcShort( 0x0 ),	/* 0 */
/* 680 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 682 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 686 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 688 */	
			0x13, 0x0,	/* FC_OP */
/* 690 */	NdrFcShort( 0xffda ),	/* Offset= -38 (652) */
/* 692 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 694 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 696 */	NdrFcShort( 0x10 ),	/* 16 */
/* 698 */	NdrFcShort( 0x0 ),	/* 0 */
/* 700 */	NdrFcShort( 0x6 ),	/* Offset= 6 (706) */
/* 702 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 704 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 706 */	
			0x11, 0x0,	/* FC_RP */
/* 708 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (672) */
/* 710 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 712 */	NdrFcShort( 0x8 ),	/* 8 */
/* 714 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 716 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 718 */	NdrFcShort( 0x10 ),	/* 16 */
/* 720 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 722 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 724 */	0x0,		/* 0 */
			NdrFcShort( 0xfff1 ),	/* Offset= -15 (710) */
			0x5b,		/* FC_END */
/* 728 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 730 */	NdrFcShort( 0x20 ),	/* 32 */
/* 732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 734 */	NdrFcShort( 0xa ),	/* Offset= 10 (744) */
/* 736 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 738 */	0x36,		/* FC_POINTER */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 740 */	0x0,		/* 0 */
			NdrFcShort( 0xffe7 ),	/* Offset= -25 (716) */
			0x5b,		/* FC_END */
/* 744 */	
			0x11, 0x0,	/* FC_RP */
/* 746 */	NdrFcShort( 0xff12 ),	/* Offset= -238 (508) */
/* 748 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 750 */	NdrFcShort( 0x1 ),	/* 1 */
/* 752 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 754 */	NdrFcShort( 0x0 ),	/* 0 */
/* 756 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 758 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 760 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 762 */	NdrFcShort( 0x10 ),	/* 16 */
/* 764 */	NdrFcShort( 0x0 ),	/* 0 */
/* 766 */	NdrFcShort( 0x6 ),	/* Offset= 6 (772) */
/* 768 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 770 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 772 */	
			0x13, 0x0,	/* FC_OP */
/* 774 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (748) */
/* 776 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 778 */	NdrFcShort( 0x2 ),	/* 2 */
/* 780 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 782 */	NdrFcShort( 0x0 ),	/* 0 */
/* 784 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 786 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 788 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 790 */	NdrFcShort( 0x10 ),	/* 16 */
/* 792 */	NdrFcShort( 0x0 ),	/* 0 */
/* 794 */	NdrFcShort( 0x6 ),	/* Offset= 6 (800) */
/* 796 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 798 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 800 */	
			0x13, 0x0,	/* FC_OP */
/* 802 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (776) */
/* 804 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 806 */	NdrFcShort( 0x4 ),	/* 4 */
/* 808 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 810 */	NdrFcShort( 0x0 ),	/* 0 */
/* 812 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 814 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 816 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 818 */	NdrFcShort( 0x10 ),	/* 16 */
/* 820 */	NdrFcShort( 0x0 ),	/* 0 */
/* 822 */	NdrFcShort( 0x6 ),	/* Offset= 6 (828) */
/* 824 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 826 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 828 */	
			0x13, 0x0,	/* FC_OP */
/* 830 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (804) */
/* 832 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 834 */	NdrFcShort( 0x8 ),	/* 8 */
/* 836 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 838 */	NdrFcShort( 0x0 ),	/* 0 */
/* 840 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 842 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 844 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 846 */	NdrFcShort( 0x10 ),	/* 16 */
/* 848 */	NdrFcShort( 0x0 ),	/* 0 */
/* 850 */	NdrFcShort( 0x6 ),	/* Offset= 6 (856) */
/* 852 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 854 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 856 */	
			0x13, 0x0,	/* FC_OP */
/* 858 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (832) */
/* 860 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 862 */	NdrFcShort( 0x8 ),	/* 8 */
/* 864 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 866 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 868 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 870 */	NdrFcShort( 0x8 ),	/* 8 */
/* 872 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 874 */	NdrFcShort( 0xffc8 ),	/* -56 */
/* 876 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 878 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 880 */	NdrFcShort( 0xffec ),	/* Offset= -20 (860) */
/* 882 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 884 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 886 */	NdrFcShort( 0x38 ),	/* 56 */
/* 888 */	NdrFcShort( 0xffec ),	/* Offset= -20 (868) */
/* 890 */	NdrFcShort( 0x0 ),	/* Offset= 0 (890) */
/* 892 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 894 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 896 */	0x40,		/* FC_STRUCTPAD4 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 898 */	0x0,		/* 0 */
			NdrFcShort( 0xfe0f ),	/* Offset= -497 (402) */
			0x5b,		/* FC_END */
/* 902 */	
			0x13, 0x0,	/* FC_OP */
/* 904 */	NdrFcShort( 0xff04 ),	/* Offset= -252 (652) */
/* 906 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 908 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 910 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 912 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 914 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 916 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 918 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 920 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 922 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 924 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 926 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 928 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 930 */	
			0x13, 0x0,	/* FC_OP */
/* 932 */	NdrFcShort( 0xfdca ),	/* Offset= -566 (366) */
/* 934 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 936 */	NdrFcShort( 0xfdcc ),	/* Offset= -564 (372) */
/* 938 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 940 */	NdrFcShort( 0xfdcc ),	/* Offset= -564 (376) */
/* 942 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 944 */	NdrFcShort( 0xfc56 ),	/* Offset= -938 (6) */
/* 946 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 948 */	NdrFcShort( 0xfdd6 ),	/* Offset= -554 (394) */
/* 950 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 952 */	NdrFcShort( 0x2 ),	/* Offset= 2 (954) */
/* 954 */	
			0x13, 0x0,	/* FC_OP */
/* 956 */	NdrFcShort( 0x14 ),	/* Offset= 20 (976) */
/* 958 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 960 */	NdrFcShort( 0x10 ),	/* 16 */
/* 962 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 964 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 966 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 968 */	
			0x13, 0x0,	/* FC_OP */
/* 970 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (958) */
/* 972 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 974 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 976 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 978 */	NdrFcShort( 0x20 ),	/* 32 */
/* 980 */	NdrFcShort( 0x0 ),	/* 0 */
/* 982 */	NdrFcShort( 0x0 ),	/* Offset= 0 (982) */
/* 984 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 986 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 988 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 990 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 992 */	NdrFcShort( 0xfc64 ),	/* Offset= -924 (68) */
/* 994 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 996 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 998 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1000 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1002 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1004 */	NdrFcShort( 0xfc54 ),	/* Offset= -940 (64) */
/* 1006 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1008 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1010 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 1012 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1014 */	
			0x11, 0x0,	/* FC_RP */
/* 1016 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1022) */
/* 1018 */	
			0x12, 0x0,	/* FC_UP */
/* 1020 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (976) */
/* 1022 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1024 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1026 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1028 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1030 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1018) */
/* 1032 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 1034 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1036 */	
			0x11, 0x0,	/* FC_RP */
/* 1038 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1040) */
/* 1040 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1042 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1044 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1046 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1048 */	NdrFcShort( 0xfd5c ),	/* Offset= -676 (372) */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            },
            {
            VARIANT_UserSize
            ,VARIANT_UserMarshal
            ,VARIANT_UserUnmarshal
            ,VARIANT_UserFree
            }

        };



/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IBrowserContainer, ver. 0.0,
   GUID={0xDB1BC009,0x56ED,0x4A59,{0xB4,0x10,0x1D,0x69,0xAF,0xE4,0x09,0x50}} */

#pragma code_seg(".orpc")
static const unsigned short IBrowserContainer_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    50,
    88,
    120,
    176,
    214,
    258,
    296,
    334,
    372,
    416,
    454,
    492,
    530,
    598,
    660,
    698,
    736
    };

static const MIDL_STUBLESS_PROXY_INFO IBrowserContainer_ProxyInfo =
    {
    &Object_StubDesc,
    IEInterface__MIDL_ProcFormatString.Format,
    &IBrowserContainer_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IBrowserContainer_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    IEInterface__MIDL_ProcFormatString.Format,
    &IBrowserContainer_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(25) _IBrowserContainerProxyVtbl = 
{
    &IBrowserContainer_ProxyInfo,
    &IID_IBrowserContainer,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::CreateBrowser */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::get_WebBrowser */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::CloseBrowser */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::SetRect */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::putref_HostApp */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::ExcuteJS */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::Show */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::GetDocumentMode */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::SetDocumentMode */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::SetCookie */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::SetUserAgent */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::FinishDownload */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::SetFunctionControl */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::AutoLogin */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::SetListenLoginXPath */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::NotifyCommand */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::GetDevToolsOpenStatus */ ,
    (void *) (INT_PTR) -1 /* IBrowserContainer::ShowDevTools */
};


static const PRPC_STUB_FUNCTION IBrowserContainer_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IBrowserContainerStubVtbl =
{
    &IID_IBrowserContainer,
    &IBrowserContainer_ServerInfo,
    25,
    &IBrowserContainer_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IBrowserEventHandler, ver. 0.0,
   GUID={0xE74F8824,0xD752,0x48A2,{0x82,0xE0,0x7D,0xF0,0xFD,0xAA,0xB7,0x86}} */

#pragma code_seg(".orpc")
static const unsigned short IBrowserEventHandler_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    774,
    824,
    868,
    906,
    962,
    994,
    1026,
    1076,
    1114,
    1164,
    1208,
    1240,
    1272,
    1328,
    1384,
    1422,
    1472,
    1516,
    1572,
    1616
    };

static const MIDL_STUBLESS_PROXY_INFO IBrowserEventHandler_ProxyInfo =
    {
    &Object_StubDesc,
    IEInterface__MIDL_ProcFormatString.Format,
    &IBrowserEventHandler_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IBrowserEventHandler_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    IEInterface__MIDL_ProcFormatString.Format,
    &IBrowserEventHandler_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(27) _IBrowserEventHandlerProxyVtbl = 
{
    &IBrowserEventHandler_ProxyInfo,
    &IID_IBrowserEventHandler,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnBeforeNavigate */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnFinishNavigate */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnTitleChange */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnLoadUrlNewWnd */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnBrowserClosing */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnBeforeBrowserClose */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnShowContextMenu */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnIEServerWndCreated */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnDownLoadFile */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnCanGoBackOrForward */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnGoBack */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnGoForward */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnRequestChangePos */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnGetMainWndPos */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnUpdateCookie */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnGetLoginInfo */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnRequestAcceleratorFromKeyboard */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnRequestAcceleratorFromMouseWheel */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnWinDomainAuthenticate */ ,
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnQueryPrivateDNS */
};


static const PRPC_STUB_FUNCTION IBrowserEventHandler_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IBrowserEventHandlerStubVtbl =
{
    &IID_IBrowserEventHandler,
    &IBrowserEventHandler_ServerInfo,
    27,
    &IBrowserEventHandler_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IHttpMonitor, ver. 0.0,
   GUID={0x8C11DD57,0x767B,0x480E,{0xA7,0x2B,0x92,0x0A,0x78,0xD0,0x6D,0x73}} */

#pragma code_seg(".orpc")
static const unsigned short IHttpMonitor_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IHttpMonitor_ProxyInfo =
    {
    &Object_StubDesc,
    IEInterface__MIDL_ProcFormatString.Format,
    &IHttpMonitor_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IHttpMonitor_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    IEInterface__MIDL_ProcFormatString.Format,
    &IHttpMonitor_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IHttpMonitorProxyVtbl = 
{
    0,
    &IID_IHttpMonitor,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */
};


static const PRPC_STUB_FUNCTION IHttpMonitor_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION
};

CInterfaceStubVtbl _IHttpMonitorStubVtbl =
{
    &IID_IHttpMonitor,
    &IHttpMonitor_ServerInfo,
    7,
    &IHttpMonitor_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    IEInterface__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x801026e, /* MIDL Version 8.1.622 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * const _IEInterface_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IBrowserContainerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IBrowserEventHandlerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IHttpMonitorProxyVtbl,
    0
};

const CInterfaceStubVtbl * const _IEInterface_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IBrowserContainerStubVtbl,
    ( CInterfaceStubVtbl *) &_IBrowserEventHandlerStubVtbl,
    ( CInterfaceStubVtbl *) &_IHttpMonitorStubVtbl,
    0
};

PCInterfaceName const _IEInterface_InterfaceNamesList[] = 
{
    "IBrowserContainer",
    "IBrowserEventHandler",
    "IHttpMonitor",
    0
};

const IID *  const _IEInterface_BaseIIDList[] = 
{
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    0
};


#define _IEInterface_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _IEInterface, pIID, n)

int __stdcall _IEInterface_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _IEInterface, 3, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _IEInterface, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _IEInterface, 3, *pIndex )
    
}

const ExtendedProxyFileInfo IEInterface_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _IEInterface_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _IEInterface_StubVtblList,
    (const PCInterfaceName * ) & _IEInterface_InterfaceNamesList,
    (const IID ** ) & _IEInterface_BaseIIDList,
    & _IEInterface_IID_Lookup, 
    3,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* defined(_M_AMD64)*/

