

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 8.00.0613 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for IEInterface.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0613 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if defined(_M_AMD64)


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
#include "ndr64types.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif /* __RPCPROXY_H_VERSION__ */


#include "IEInterface_h.h"

#define TYPE_FORMAT_STRING_SIZE   1231                              
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

static const RPC_SYNTAX_IDENTIFIER  _NDR64_RpcTransferSyntax = 
{{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}};



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


extern const USER_MARSHAL_ROUTINE_QUADRUPLE NDR64_UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x47,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, has range on conformance */
/* 232 */	NdrFcShort( 0x1 ),	/* 1 */
/* 234 */	NdrFcShort( 0x1 ),	/* 1 */
/* 236 */	NdrFcShort( 0x0 ),	/* 0 */
/* 238 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter src */

/* 240 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 242 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 244 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Parameter result */

/* 246 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 248 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 250 */	NdrFcShort( 0x498 ),	/* Type Offset=1176 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 390 */	NdrFcShort( 0x0 ),	/* 0 */
/* 392 */	NdrFcShort( 0x1 ),	/* 1 */
/* 394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 396 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter json */

/* 398 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 400 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 402 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 434 */	NdrFcShort( 0x0 ),	/* 0 */
/* 436 */	NdrFcShort( 0x1 ),	/* 1 */
/* 438 */	NdrFcShort( 0x0 ),	/* 0 */
/* 440 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter usStr */

/* 442 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 444 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 446 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 512 */	NdrFcShort( 0x1 ),	/* 1 */
/* 514 */	NdrFcShort( 0x0 ),	/* 0 */
/* 516 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter jsonStr */

/* 518 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 520 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 522 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 548 */	NdrFcShort( 0x0 ),	/* 0 */
/* 550 */	NdrFcShort( 0x1 ),	/* 1 */
/* 552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 554 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter frameXPath */

/* 556 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 558 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 560 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Parameter userNameXPath */

/* 562 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 564 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 566 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Parameter userNameValue */

/* 568 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 570 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 572 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Parameter pwdXPath */

/* 574 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 576 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 578 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Parameter pwdValue */

/* 580 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 582 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 584 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Parameter loginBtnXPath */

/* 586 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 588 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 590 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 616 */	NdrFcShort( 0x0 ),	/* 0 */
/* 618 */	NdrFcShort( 0x1 ),	/* 1 */
/* 620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 622 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter url */

/* 624 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 626 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 628 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Parameter frameXPath */

/* 630 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 632 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 634 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Parameter userNameXPath */

/* 636 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 638 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 640 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Parameter pwdXPath */

/* 642 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 644 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 646 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Parameter loginbtnXPath */

/* 648 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 650 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 652 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 678 */	NdrFcShort( 0x0 ),	/* 0 */
/* 680 */	NdrFcShort( 0x1 ),	/* 1 */
/* 682 */	NdrFcShort( 0x0 ),	/* 0 */
/* 684 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter command */

/* 686 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 688 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 690 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 792 */	NdrFcShort( 0x0 ),	/* 0 */
/* 794 */	NdrFcShort( 0x1 ),	/* 1 */
/* 796 */	NdrFcShort( 0x0 ),	/* 0 */
/* 798 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter url */

/* 800 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 802 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 804 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 842 */	NdrFcShort( 0x0 ),	/* 0 */
/* 844 */	NdrFcShort( 0x1 ),	/* 1 */
/* 846 */	NdrFcShort( 0x0 ),	/* 0 */
/* 848 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter url */

/* 850 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 852 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 854 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Parameter faviconUrls */

/* 856 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 858 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 860 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 886 */	NdrFcShort( 0x0 ),	/* 0 */
/* 888 */	NdrFcShort( 0x1 ),	/* 1 */
/* 890 */	NdrFcShort( 0x0 ),	/* 0 */
/* 892 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter title */

/* 894 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 896 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 898 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 924 */	NdrFcShort( 0x0 ),	/* 0 */
/* 926 */	NdrFcShort( 0x1 ),	/* 1 */
/* 928 */	NdrFcShort( 0x0 ),	/* 0 */
/* 930 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter url */

/* 932 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 934 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 936 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 1132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1134 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1138 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter url */

/* 1140 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1142 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1144 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Parameter status */

/* 1146 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1148 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1150 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter variant */

/* 1152 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1154 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1156 */	NdrFcShort( 0x4b2 ),	/* Type Offset=1202 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 1402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1404 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1408 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cookie */

/* 1410 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1412 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1414 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

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
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 1440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1442 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1446 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter url */

/* 1448 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1450 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1452 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Parameter userName */

/* 1454 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1456 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1458 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Parameter userPwd */

/* 1460 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1462 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 1464 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
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
			0x47,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, has range on conformance */
/* 1590 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1592 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1594 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1596 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter userName */

/* 1598 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1600 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1602 */	NdrFcShort( 0x4c4 ),	/* Type Offset=1220 */

	/* Parameter userPwd */

/* 1604 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1606 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1608 */	NdrFcShort( 0x4c4 ),	/* Type Offset=1220 */

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
			0x47,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, has range on conformance */
/* 1634 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1636 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1638 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1640 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter host */

/* 1642 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1644 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 1646 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Parameter ipList */

/* 1648 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1650 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 1652 */	NdrFcShort( 0x4c4 ),	/* Type Offset=1220 */

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
/* 26 */	NdrFcShort( 0x18 ),	/* Offset= 24 (50) */
/* 28 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 30 */	NdrFcShort( 0x2 ),	/* 2 */
/* 32 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 34 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 36 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 38 */	0x0 , 
			0x0,		/* 0 */
/* 40 */	NdrFcLong( 0x0 ),	/* 0 */
/* 44 */	NdrFcLong( 0x0 ),	/* 0 */
/* 48 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 50 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 52 */	NdrFcShort( 0x8 ),	/* 8 */
/* 54 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (28) */
/* 56 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 58 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 60 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */
/* 64 */	NdrFcShort( 0x8 ),	/* 8 */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (24) */
/* 70 */	
			0x11, 0x0,	/* FC_RP */
/* 72 */	NdrFcShort( 0x450 ),	/* Offset= 1104 (1176) */
/* 74 */	
			0x13, 0x0,	/* FC_OP */
/* 76 */	NdrFcShort( 0x438 ),	/* Offset= 1080 (1156) */
/* 78 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 80 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 82 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 84 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 86 */	0x0 , 
			0x0,		/* 0 */
/* 88 */	NdrFcLong( 0x0 ),	/* 0 */
/* 92 */	NdrFcLong( 0x0 ),	/* 0 */
/* 96 */	NdrFcShort( 0x2 ),	/* Offset= 2 (98) */
/* 98 */	NdrFcShort( 0x10 ),	/* 16 */
/* 100 */	NdrFcShort( 0x2f ),	/* 47 */
/* 102 */	NdrFcLong( 0x14 ),	/* 20 */
/* 106 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 108 */	NdrFcLong( 0x3 ),	/* 3 */
/* 112 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 114 */	NdrFcLong( 0x11 ),	/* 17 */
/* 118 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 120 */	NdrFcLong( 0x2 ),	/* 2 */
/* 124 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 126 */	NdrFcLong( 0x4 ),	/* 4 */
/* 130 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 132 */	NdrFcLong( 0x5 ),	/* 5 */
/* 136 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 138 */	NdrFcLong( 0xb ),	/* 11 */
/* 142 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 144 */	NdrFcLong( 0xa ),	/* 10 */
/* 148 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 150 */	NdrFcLong( 0x6 ),	/* 6 */
/* 154 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (386) */
/* 156 */	NdrFcLong( 0x7 ),	/* 7 */
/* 160 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 162 */	NdrFcLong( 0x8 ),	/* 8 */
/* 166 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (392) */
/* 168 */	NdrFcLong( 0xd ),	/* 13 */
/* 172 */	NdrFcShort( 0xe0 ),	/* Offset= 224 (396) */
/* 174 */	NdrFcLong( 0x9 ),	/* 9 */
/* 178 */	NdrFcShort( 0xff54 ),	/* Offset= -172 (6) */
/* 180 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 184 */	NdrFcShort( 0xe6 ),	/* Offset= 230 (414) */
/* 186 */	NdrFcLong( 0x24 ),	/* 36 */
/* 190 */	NdrFcShort( 0x37c ),	/* Offset= 892 (1082) */
/* 192 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 196 */	NdrFcShort( 0x376 ),	/* Offset= 886 (1082) */
/* 198 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 202 */	NdrFcShort( 0x374 ),	/* Offset= 884 (1086) */
/* 204 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 208 */	NdrFcShort( 0x372 ),	/* Offset= 882 (1090) */
/* 210 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 214 */	NdrFcShort( 0x370 ),	/* Offset= 880 (1094) */
/* 216 */	NdrFcLong( 0x4014 ),	/* 16404 */
/* 220 */	NdrFcShort( 0x36e ),	/* Offset= 878 (1098) */
/* 222 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 226 */	NdrFcShort( 0x36c ),	/* Offset= 876 (1102) */
/* 228 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 232 */	NdrFcShort( 0x36a ),	/* Offset= 874 (1106) */
/* 234 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 238 */	NdrFcShort( 0x354 ),	/* Offset= 852 (1090) */
/* 240 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 244 */	NdrFcShort( 0x352 ),	/* Offset= 850 (1094) */
/* 246 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 250 */	NdrFcShort( 0x35c ),	/* Offset= 860 (1110) */
/* 252 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 256 */	NdrFcShort( 0x352 ),	/* Offset= 850 (1106) */
/* 258 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 262 */	NdrFcShort( 0x354 ),	/* Offset= 852 (1114) */
/* 264 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 268 */	NdrFcShort( 0x352 ),	/* Offset= 850 (1118) */
/* 270 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 274 */	NdrFcShort( 0x350 ),	/* Offset= 848 (1122) */
/* 276 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 280 */	NdrFcShort( 0x34e ),	/* Offset= 846 (1126) */
/* 282 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 286 */	NdrFcShort( 0x34c ),	/* Offset= 844 (1130) */
/* 288 */	NdrFcLong( 0x10 ),	/* 16 */
/* 292 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 294 */	NdrFcLong( 0x12 ),	/* 18 */
/* 298 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 300 */	NdrFcLong( 0x13 ),	/* 19 */
/* 304 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 306 */	NdrFcLong( 0x15 ),	/* 21 */
/* 310 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 312 */	NdrFcLong( 0x16 ),	/* 22 */
/* 316 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 318 */	NdrFcLong( 0x17 ),	/* 23 */
/* 322 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 324 */	NdrFcLong( 0xe ),	/* 14 */
/* 328 */	NdrFcShort( 0x32a ),	/* Offset= 810 (1138) */
/* 330 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 334 */	NdrFcShort( 0x32e ),	/* Offset= 814 (1148) */
/* 336 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 340 */	NdrFcShort( 0x32c ),	/* Offset= 812 (1152) */
/* 342 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 346 */	NdrFcShort( 0x2e8 ),	/* Offset= 744 (1090) */
/* 348 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 352 */	NdrFcShort( 0x2e6 ),	/* Offset= 742 (1094) */
/* 354 */	NdrFcLong( 0x4015 ),	/* 16405 */
/* 358 */	NdrFcShort( 0x2e4 ),	/* Offset= 740 (1098) */
/* 360 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 364 */	NdrFcShort( 0x2da ),	/* Offset= 730 (1094) */
/* 366 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 370 */	NdrFcShort( 0x2d4 ),	/* Offset= 724 (1094) */
/* 372 */	NdrFcLong( 0x0 ),	/* 0 */
/* 376 */	NdrFcShort( 0x0 ),	/* Offset= 0 (376) */
/* 378 */	NdrFcLong( 0x1 ),	/* 1 */
/* 382 */	NdrFcShort( 0x0 ),	/* Offset= 0 (382) */
/* 384 */	NdrFcShort( 0xffff ),	/* Offset= -1 (383) */
/* 386 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 388 */	NdrFcShort( 0x8 ),	/* 8 */
/* 390 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 392 */	
			0x13, 0x0,	/* FC_OP */
/* 394 */	NdrFcShort( 0xfea8 ),	/* Offset= -344 (50) */
/* 396 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 398 */	NdrFcLong( 0x0 ),	/* 0 */
/* 402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 406 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 408 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 410 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 412 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 414 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 416 */	NdrFcShort( 0x2 ),	/* Offset= 2 (418) */
/* 418 */	
			0x13, 0x0,	/* FC_OP */
/* 420 */	NdrFcShort( 0x284 ),	/* Offset= 644 (1064) */
/* 422 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x89,		/* 137 */
/* 424 */	NdrFcShort( 0x20 ),	/* 32 */
/* 426 */	NdrFcShort( 0xa ),	/* 10 */
/* 428 */	NdrFcLong( 0x8 ),	/* 8 */
/* 432 */	NdrFcShort( 0x64 ),	/* Offset= 100 (532) */
/* 434 */	NdrFcLong( 0xd ),	/* 13 */
/* 438 */	NdrFcShort( 0x98 ),	/* Offset= 152 (590) */
/* 440 */	NdrFcLong( 0x9 ),	/* 9 */
/* 444 */	NdrFcShort( 0xcc ),	/* Offset= 204 (648) */
/* 446 */	NdrFcLong( 0xc ),	/* 12 */
/* 450 */	NdrFcShort( 0x100 ),	/* Offset= 256 (706) */
/* 452 */	NdrFcLong( 0x24 ),	/* 36 */
/* 456 */	NdrFcShort( 0x170 ),	/* Offset= 368 (824) */
/* 458 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 462 */	NdrFcShort( 0x18c ),	/* Offset= 396 (858) */
/* 464 */	NdrFcLong( 0x10 ),	/* 16 */
/* 468 */	NdrFcShort( 0x1b0 ),	/* Offset= 432 (900) */
/* 470 */	NdrFcLong( 0x2 ),	/* 2 */
/* 474 */	NdrFcShort( 0x1d0 ),	/* Offset= 464 (938) */
/* 476 */	NdrFcLong( 0x3 ),	/* 3 */
/* 480 */	NdrFcShort( 0x1f0 ),	/* Offset= 496 (976) */
/* 482 */	NdrFcLong( 0x14 ),	/* 20 */
/* 486 */	NdrFcShort( 0x210 ),	/* Offset= 528 (1014) */
/* 488 */	NdrFcShort( 0xffff ),	/* Offset= -1 (487) */
/* 490 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 492 */	NdrFcShort( 0x0 ),	/* 0 */
/* 494 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 498 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 500 */	0x0 , 
			0x0,		/* 0 */
/* 502 */	NdrFcLong( 0x0 ),	/* 0 */
/* 506 */	NdrFcLong( 0x0 ),	/* 0 */
/* 510 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 514 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 516 */	0x0 , 
			0x0,		/* 0 */
/* 518 */	NdrFcLong( 0x0 ),	/* 0 */
/* 522 */	NdrFcLong( 0x0 ),	/* 0 */
/* 526 */	
			0x13, 0x0,	/* FC_OP */
/* 528 */	NdrFcShort( 0xfe22 ),	/* Offset= -478 (50) */
/* 530 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 532 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 534 */	NdrFcShort( 0x10 ),	/* 16 */
/* 536 */	NdrFcShort( 0x0 ),	/* 0 */
/* 538 */	NdrFcShort( 0x6 ),	/* Offset= 6 (544) */
/* 540 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 542 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 544 */	
			0x11, 0x0,	/* FC_RP */
/* 546 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (490) */
/* 548 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 550 */	NdrFcShort( 0x0 ),	/* 0 */
/* 552 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 556 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 558 */	0x0 , 
			0x0,		/* 0 */
/* 560 */	NdrFcLong( 0x0 ),	/* 0 */
/* 564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 568 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 572 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 574 */	0x0 , 
			0x0,		/* 0 */
/* 576 */	NdrFcLong( 0x0 ),	/* 0 */
/* 580 */	NdrFcLong( 0x0 ),	/* 0 */
/* 584 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 586 */	NdrFcShort( 0xff42 ),	/* Offset= -190 (396) */
/* 588 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 590 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 592 */	NdrFcShort( 0x10 ),	/* 16 */
/* 594 */	NdrFcShort( 0x0 ),	/* 0 */
/* 596 */	NdrFcShort( 0x6 ),	/* Offset= 6 (602) */
/* 598 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 600 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 602 */	
			0x11, 0x0,	/* FC_RP */
/* 604 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (548) */
/* 606 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 610 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 612 */	NdrFcShort( 0x0 ),	/* 0 */
/* 614 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 616 */	0x0 , 
			0x0,		/* 0 */
/* 618 */	NdrFcLong( 0x0 ),	/* 0 */
/* 622 */	NdrFcLong( 0x0 ),	/* 0 */
/* 626 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 630 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 632 */	0x0 , 
			0x0,		/* 0 */
/* 634 */	NdrFcLong( 0x0 ),	/* 0 */
/* 638 */	NdrFcLong( 0x0 ),	/* 0 */
/* 642 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 644 */	NdrFcShort( 0xfd82 ),	/* Offset= -638 (6) */
/* 646 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 648 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 650 */	NdrFcShort( 0x10 ),	/* 16 */
/* 652 */	NdrFcShort( 0x0 ),	/* 0 */
/* 654 */	NdrFcShort( 0x6 ),	/* Offset= 6 (660) */
/* 656 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 658 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 660 */	
			0x11, 0x0,	/* FC_RP */
/* 662 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (606) */
/* 664 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 666 */	NdrFcShort( 0x0 ),	/* 0 */
/* 668 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 670 */	NdrFcShort( 0x0 ),	/* 0 */
/* 672 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 674 */	0x0 , 
			0x0,		/* 0 */
/* 676 */	NdrFcLong( 0x0 ),	/* 0 */
/* 680 */	NdrFcLong( 0x0 ),	/* 0 */
/* 684 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 688 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 690 */	0x0 , 
			0x0,		/* 0 */
/* 692 */	NdrFcLong( 0x0 ),	/* 0 */
/* 696 */	NdrFcLong( 0x0 ),	/* 0 */
/* 700 */	
			0x13, 0x0,	/* FC_OP */
/* 702 */	NdrFcShort( 0x1c6 ),	/* Offset= 454 (1156) */
/* 704 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 706 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 708 */	NdrFcShort( 0x10 ),	/* 16 */
/* 710 */	NdrFcShort( 0x0 ),	/* 0 */
/* 712 */	NdrFcShort( 0x6 ),	/* Offset= 6 (718) */
/* 714 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 716 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 718 */	
			0x11, 0x0,	/* FC_RP */
/* 720 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (664) */
/* 722 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 724 */	NdrFcLong( 0x2f ),	/* 47 */
/* 728 */	NdrFcShort( 0x0 ),	/* 0 */
/* 730 */	NdrFcShort( 0x0 ),	/* 0 */
/* 732 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 734 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 736 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 738 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 740 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 742 */	NdrFcShort( 0x1 ),	/* 1 */
/* 744 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 746 */	NdrFcShort( 0x4 ),	/* 4 */
/* 748 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 750 */	0x0 , 
			0x0,		/* 0 */
/* 752 */	NdrFcLong( 0x0 ),	/* 0 */
/* 756 */	NdrFcLong( 0x0 ),	/* 0 */
/* 760 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 762 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 764 */	NdrFcShort( 0x18 ),	/* 24 */
/* 766 */	NdrFcShort( 0x0 ),	/* 0 */
/* 768 */	NdrFcShort( 0xa ),	/* Offset= 10 (778) */
/* 770 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 772 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 774 */	NdrFcShort( 0xffcc ),	/* Offset= -52 (722) */
/* 776 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 778 */	
			0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 780 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (740) */
/* 782 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 784 */	NdrFcShort( 0x0 ),	/* 0 */
/* 786 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 788 */	NdrFcShort( 0x0 ),	/* 0 */
/* 790 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 792 */	0x0 , 
			0x0,		/* 0 */
/* 794 */	NdrFcLong( 0x0 ),	/* 0 */
/* 798 */	NdrFcLong( 0x0 ),	/* 0 */
/* 802 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 806 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 808 */	0x0 , 
			0x0,		/* 0 */
/* 810 */	NdrFcLong( 0x0 ),	/* 0 */
/* 814 */	NdrFcLong( 0x0 ),	/* 0 */
/* 818 */	
			0x13, 0x0,	/* FC_OP */
/* 820 */	NdrFcShort( 0xffc6 ),	/* Offset= -58 (762) */
/* 822 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 824 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 826 */	NdrFcShort( 0x10 ),	/* 16 */
/* 828 */	NdrFcShort( 0x0 ),	/* 0 */
/* 830 */	NdrFcShort( 0x6 ),	/* Offset= 6 (836) */
/* 832 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 834 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 836 */	
			0x11, 0x0,	/* FC_RP */
/* 838 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (782) */
/* 840 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 842 */	NdrFcShort( 0x8 ),	/* 8 */
/* 844 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 846 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 848 */	NdrFcShort( 0x10 ),	/* 16 */
/* 850 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 852 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 854 */	0x0,		/* 0 */
			NdrFcShort( 0xfff1 ),	/* Offset= -15 (840) */
			0x5b,		/* FC_END */
/* 858 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 860 */	NdrFcShort( 0x20 ),	/* 32 */
/* 862 */	NdrFcShort( 0x0 ),	/* 0 */
/* 864 */	NdrFcShort( 0xa ),	/* Offset= 10 (874) */
/* 866 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 868 */	0x36,		/* FC_POINTER */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 870 */	0x0,		/* 0 */
			NdrFcShort( 0xffe7 ),	/* Offset= -25 (846) */
			0x5b,		/* FC_END */
/* 874 */	
			0x11, 0x0,	/* FC_RP */
/* 876 */	NdrFcShort( 0xfeb8 ),	/* Offset= -328 (548) */
/* 878 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 880 */	NdrFcShort( 0x1 ),	/* 1 */
/* 882 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 884 */	NdrFcShort( 0x0 ),	/* 0 */
/* 886 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 888 */	0x0 , 
			0x0,		/* 0 */
/* 890 */	NdrFcLong( 0x0 ),	/* 0 */
/* 894 */	NdrFcLong( 0x0 ),	/* 0 */
/* 898 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 900 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 902 */	NdrFcShort( 0x10 ),	/* 16 */
/* 904 */	NdrFcShort( 0x0 ),	/* 0 */
/* 906 */	NdrFcShort( 0x6 ),	/* Offset= 6 (912) */
/* 908 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 910 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 912 */	
			0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 914 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (878) */
/* 916 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 918 */	NdrFcShort( 0x2 ),	/* 2 */
/* 920 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 922 */	NdrFcShort( 0x0 ),	/* 0 */
/* 924 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 926 */	0x0 , 
			0x0,		/* 0 */
/* 928 */	NdrFcLong( 0x0 ),	/* 0 */
/* 932 */	NdrFcLong( 0x0 ),	/* 0 */
/* 936 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 938 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 940 */	NdrFcShort( 0x10 ),	/* 16 */
/* 942 */	NdrFcShort( 0x0 ),	/* 0 */
/* 944 */	NdrFcShort( 0x6 ),	/* Offset= 6 (950) */
/* 946 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 948 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 950 */	
			0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 952 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (916) */
/* 954 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 956 */	NdrFcShort( 0x4 ),	/* 4 */
/* 958 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 960 */	NdrFcShort( 0x0 ),	/* 0 */
/* 962 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 964 */	0x0 , 
			0x0,		/* 0 */
/* 966 */	NdrFcLong( 0x0 ),	/* 0 */
/* 970 */	NdrFcLong( 0x0 ),	/* 0 */
/* 974 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 976 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 978 */	NdrFcShort( 0x10 ),	/* 16 */
/* 980 */	NdrFcShort( 0x0 ),	/* 0 */
/* 982 */	NdrFcShort( 0x6 ),	/* Offset= 6 (988) */
/* 984 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 986 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 988 */	
			0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 990 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (954) */
/* 992 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 994 */	NdrFcShort( 0x8 ),	/* 8 */
/* 996 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 998 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1000 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1002 */	0x0 , 
			0x0,		/* 0 */
/* 1004 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1008 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1012 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1014 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1016 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1018 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1020 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1026) */
/* 1022 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1024 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1026 */	
			0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 1028 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (992) */
/* 1030 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1032 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1034 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1036 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1038 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1040 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1042 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1044 */	NdrFcShort( 0xffc8 ),	/* -56 */
/* 1046 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1048 */	0x0 , 
			0x0,		/* 0 */
/* 1050 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1054 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1058 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1060 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (1030) */
/* 1062 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1064 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1066 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1068 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (1038) */
/* 1070 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1070) */
/* 1072 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1074 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1076 */	0x40,		/* FC_STRUCTPAD4 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1078 */	0x0,		/* 0 */
			NdrFcShort( 0xfd6f ),	/* Offset= -657 (422) */
			0x5b,		/* FC_END */
/* 1082 */	
			0x13, 0x0,	/* FC_OP */
/* 1084 */	NdrFcShort( 0xfebe ),	/* Offset= -322 (762) */
/* 1086 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1088 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 1090 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1092 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1094 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1096 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1098 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1100 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 1102 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1104 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1106 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1108 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 1110 */	
			0x13, 0x0,	/* FC_OP */
/* 1112 */	NdrFcShort( 0xfd2a ),	/* Offset= -726 (386) */
/* 1114 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1116 */	NdrFcShort( 0xfd2c ),	/* Offset= -724 (392) */
/* 1118 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1120 */	NdrFcShort( 0xfd2c ),	/* Offset= -724 (396) */
/* 1122 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1124 */	NdrFcShort( 0xfba2 ),	/* Offset= -1118 (6) */
/* 1126 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1128 */	NdrFcShort( 0xfd36 ),	/* Offset= -714 (414) */
/* 1130 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1132 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1134) */
/* 1134 */	
			0x13, 0x0,	/* FC_OP */
/* 1136 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1156) */
/* 1138 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 1140 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1142 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 1144 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 1146 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1148 */	
			0x13, 0x0,	/* FC_OP */
/* 1150 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1138) */
/* 1152 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1154 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1156 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1158 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1160 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1162 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1162) */
/* 1164 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1166 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1168 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1170 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1172 */	NdrFcShort( 0xfbba ),	/* Offset= -1094 (78) */
/* 1174 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1176 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1178 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1180 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1182 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1184 */	NdrFcShort( 0xfbaa ),	/* Offset= -1110 (74) */
/* 1186 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1188 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1190 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 1192 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1194 */	
			0x11, 0x0,	/* FC_RP */
/* 1196 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1202) */
/* 1198 */	
			0x12, 0x0,	/* FC_UP */
/* 1200 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (1156) */
/* 1202 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1204 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1206 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1208 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1210 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1198) */
/* 1212 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 1214 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1216 */	
			0x11, 0x0,	/* FC_RP */
/* 1218 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1220) */
/* 1220 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1224 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1228 */	NdrFcShort( 0xfcbc ),	/* Offset= -836 (392) */

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



#endif /* defined(_M_AMD64)*/



/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 8.00.0613 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for IEInterface.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0613 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if defined(_M_AMD64)



extern const USER_MARSHAL_ROUTINE_QUADRUPLE NDR64_UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];
extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif


#include "ndr64types.h"
#include "pshpack8.h"


typedef 
NDR64_FORMAT_CHAR
__midl_frag347_t;
extern const __midl_frag347_t __midl_frag347;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag346_t;
extern const __midl_frag346_t __midl_frag346;

typedef 
struct _NDR64_USER_MARSHAL_FORMAT
__midl_frag345_t;
extern const __midl_frag345_t __midl_frag345;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag344_t;
extern const __midl_frag344_t __midl_frag344;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag343_t;
extern const __midl_frag343_t __midl_frag343;

typedef 
struct _NDR64_USER_MARSHAL_FORMAT
__midl_frag342_t;
extern const __midl_frag342_t __midl_frag342;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag341_t;
extern const __midl_frag341_t __midl_frag341;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag333_t;
extern const __midl_frag333_t __midl_frag333;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag327_t;
extern const __midl_frag327_t __midl_frag327;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag323_t;
extern const __midl_frag323_t __midl_frag323;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag315_t;
extern const __midl_frag315_t __midl_frag315;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag311_t;
extern const __midl_frag311_t __midl_frag311;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag308_t;
extern const __midl_frag308_t __midl_frag308;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag301_t;
extern const __midl_frag301_t __midl_frag301;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
}
__midl_frag293_t;
extern const __midl_frag293_t __midl_frag293;

typedef 
NDR64_FORMAT_CHAR
__midl_frag289_t;
extern const __midl_frag289_t __midl_frag289;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag288_t;
extern const __midl_frag288_t __midl_frag288;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag285_t;
extern const __midl_frag285_t __midl_frag285;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag283_t;
extern const __midl_frag283_t __midl_frag283;

typedef 
struct _NDR64_USER_MARSHAL_FORMAT
__midl_frag282_t;
extern const __midl_frag282_t __midl_frag282;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag281_t;
extern const __midl_frag281_t __midl_frag281;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag277_t;
extern const __midl_frag277_t __midl_frag277;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag274_t;
extern const __midl_frag274_t __midl_frag274;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag269_t;
extern const __midl_frag269_t __midl_frag269;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag263_t;
extern const __midl_frag263_t __midl_frag263;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag262_t;
extern const __midl_frag262_t __midl_frag262;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag261_t;
extern const __midl_frag261_t __midl_frag261;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
}
__midl_frag255_t;
extern const __midl_frag255_t __midl_frag255;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag245_t;
extern const __midl_frag245_t __midl_frag245;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag238_t;
extern const __midl_frag238_t __midl_frag238;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag235_t;
extern const __midl_frag235_t __midl_frag235;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag231_t;
extern const __midl_frag231_t __midl_frag231;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
    struct _NDR64_PARAM_FORMAT frag7;
}
__midl_frag215_t;
extern const __midl_frag215_t __midl_frag215;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
    struct _NDR64_PARAM_FORMAT frag6;
    struct _NDR64_PARAM_FORMAT frag7;
    struct _NDR64_PARAM_FORMAT frag8;
}
__midl_frag201_t;
extern const __midl_frag201_t __midl_frag201;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag185_t;
extern const __midl_frag185_t __midl_frag185;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag179_t;
extern const __midl_frag179_t __midl_frag179;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag178_t;
extern const __midl_frag178_t __midl_frag178;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag172_t;
extern const __midl_frag172_t __midl_frag172;

typedef 
NDR64_FORMAT_CHAR
__midl_frag169_t;
extern const __midl_frag169_t __midl_frag169;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag168_t;
extern const __midl_frag168_t __midl_frag168;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag164_t;
extern const __midl_frag164_t __midl_frag164;

typedef 
NDR64_FORMAT_CHAR
__midl_frag163_t;
extern const __midl_frag163_t __midl_frag163;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag162_t;
extern const __midl_frag162_t __midl_frag162;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag161_t;
extern const __midl_frag161_t __midl_frag161;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag160_t;
extern const __midl_frag160_t __midl_frag160;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag153_t;
extern const __midl_frag153_t __midl_frag153;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag152_t;
extern const __midl_frag152_t __midl_frag152;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag151_t;
extern const __midl_frag151_t __midl_frag151;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag150_t;
extern const __midl_frag150_t __midl_frag150;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag149_t;
extern const __midl_frag149_t __midl_frag149;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag146_t;
extern const __midl_frag146_t __midl_frag146;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag145_t;
extern const __midl_frag145_t __midl_frag145;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag144_t;
extern const __midl_frag144_t __midl_frag144;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag143_t;
extern const __midl_frag143_t __midl_frag143;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag141_t;
extern const __midl_frag141_t __midl_frag141;

typedef 
NDR64_FORMAT_CHAR
__midl_frag140_t;
extern const __midl_frag140_t __midl_frag140;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag139_t;
extern const __midl_frag139_t __midl_frag139;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag138_t;
extern const __midl_frag138_t __midl_frag138;

typedef 
NDR64_FORMAT_CHAR
__midl_frag131_t;
extern const __midl_frag131_t __midl_frag131;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag130_t;
extern const __midl_frag130_t __midl_frag130;

typedef 
NDR64_FORMAT_CHAR
__midl_frag123_t;
extern const __midl_frag123_t __midl_frag123;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag122_t;
extern const __midl_frag122_t __midl_frag122;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag121_t;
extern const __midl_frag121_t __midl_frag121;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag120_t;
extern const __midl_frag120_t __midl_frag120;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag118_t;
extern const __midl_frag118_t __midl_frag118;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag117_t;
extern const __midl_frag117_t __midl_frag117;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag116_t;
extern const __midl_frag116_t __midl_frag116;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag115_t;
extern const __midl_frag115_t __midl_frag115;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag112_t;
extern const __midl_frag112_t __midl_frag112;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag111_t;
extern const __midl_frag111_t __midl_frag111;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag110_t;
extern const __midl_frag110_t __midl_frag110;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag107_t;
extern const __midl_frag107_t __midl_frag107;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag106_t;
extern const __midl_frag106_t __midl_frag106;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag105_t;
extern const __midl_frag105_t __midl_frag105;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag102_t;
extern const __midl_frag102_t __midl_frag102;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag101_t;
extern const __midl_frag101_t __midl_frag101;

typedef 
struct 
{
    struct _NDR64_FIX_ARRAY_HEADER_FORMAT frag1;
}
__midl_frag100_t;
extern const __midl_frag100_t __midl_frag100;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag99_t;
extern const __midl_frag99_t __midl_frag99;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_REPEAT_FORMAT frag1;
        struct 
        {
            struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag1;
            struct _NDR64_POINTER_FORMAT frag2;
        } frag2;
        NDR64_FORMAT_CHAR frag3;
    } frag2;
    struct _NDR64_ARRAY_ELEMENT_INFO frag3;
}
__midl_frag94_t;
extern const __midl_frag94_t __midl_frag94;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag5;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag6;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag7;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag8;
    } frag2;
}
__midl_frag93_t;
extern const __midl_frag93_t __midl_frag93;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag92_t;
extern const __midl_frag92_t __midl_frag92;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag89_t;
extern const __midl_frag89_t __midl_frag89;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag88_t;
extern const __midl_frag88_t __midl_frag88;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag87_t;
extern const __midl_frag87_t __midl_frag87;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_NO_REPEAT_FORMAT frag1;
        struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag2;
        struct _NDR64_POINTER_FORMAT frag3;
        struct _NDR64_NO_REPEAT_FORMAT frag4;
        struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag5;
        struct _NDR64_POINTER_FORMAT frag6;
        NDR64_FORMAT_CHAR frag7;
    } frag2;
}
__midl_frag86_t;
extern const __midl_frag86_t __midl_frag86;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_REPEAT_FORMAT frag1;
        struct 
        {
            struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag1;
            struct _NDR64_POINTER_FORMAT frag2;
        } frag2;
        NDR64_FORMAT_CHAR frag3;
    } frag2;
    struct _NDR64_ARRAY_ELEMENT_INFO frag3;
}
__midl_frag84_t;
extern const __midl_frag84_t __midl_frag84;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag83_t;
extern const __midl_frag83_t __midl_frag83;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag82_t;
extern const __midl_frag82_t __midl_frag82;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_REPEAT_FORMAT frag1;
        struct 
        {
            struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag1;
            struct _NDR64_POINTER_FORMAT frag2;
        } frag2;
        NDR64_FORMAT_CHAR frag3;
    } frag2;
    struct _NDR64_ARRAY_ELEMENT_INFO frag3;
}
__midl_frag79_t;
extern const __midl_frag79_t __midl_frag79;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag78_t;
extern const __midl_frag78_t __midl_frag78;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag77_t;
extern const __midl_frag77_t __midl_frag77;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_REPEAT_FORMAT frag1;
        struct 
        {
            struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag1;
            struct _NDR64_POINTER_FORMAT frag2;
        } frag2;
        NDR64_FORMAT_CHAR frag3;
    } frag2;
    struct _NDR64_ARRAY_ELEMENT_INFO frag3;
}
__midl_frag72_t;
extern const __midl_frag72_t __midl_frag72;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag71_t;
extern const __midl_frag71_t __midl_frag71;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag64_t;
extern const __midl_frag64_t __midl_frag64;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag63_t;
extern const __midl_frag63_t __midl_frag63;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_REPEAT_FORMAT frag1;
        struct 
        {
            struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag1;
            struct _NDR64_POINTER_FORMAT frag2;
        } frag2;
        NDR64_FORMAT_CHAR frag3;
    } frag2;
    struct _NDR64_ARRAY_ELEMENT_INFO frag3;
}
__midl_frag60_t;
extern const __midl_frag60_t __midl_frag60;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag59_t;
extern const __midl_frag59_t __midl_frag59;

typedef 
struct 
{
    struct _NDR64_ENCAPSULATED_UNION frag1;
    struct _NDR64_UNION_ARM_SELECTOR frag2;
    struct _NDR64_UNION_ARM frag3;
    struct _NDR64_UNION_ARM frag4;
    struct _NDR64_UNION_ARM frag5;
    struct _NDR64_UNION_ARM frag6;
    struct _NDR64_UNION_ARM frag7;
    struct _NDR64_UNION_ARM frag8;
    struct _NDR64_UNION_ARM frag9;
    struct _NDR64_UNION_ARM frag10;
    struct _NDR64_UNION_ARM frag11;
    struct _NDR64_UNION_ARM frag12;
    NDR64_UINT32 frag13;
}
__midl_frag58_t;
extern const __midl_frag58_t __midl_frag58;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag57_t;
extern const __midl_frag57_t __midl_frag57;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag56_t;
extern const __midl_frag56_t __midl_frag56;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag55_t;
extern const __midl_frag55_t __midl_frag55;

typedef 
struct 
{
    struct _NDR64_CONF_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_REGION_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag54_t;
extern const __midl_frag54_t __midl_frag54;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag45_t;
extern const __midl_frag45_t __midl_frag45;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag36_t;
extern const __midl_frag36_t __midl_frag36;

typedef 
struct 
{
    struct _NDR64_NON_ENCAPSULATED_UNION frag1;
    struct _NDR64_UNION_ARM_SELECTOR frag2;
    struct _NDR64_UNION_ARM frag3;
    struct _NDR64_UNION_ARM frag4;
    struct _NDR64_UNION_ARM frag5;
    struct _NDR64_UNION_ARM frag6;
    struct _NDR64_UNION_ARM frag7;
    struct _NDR64_UNION_ARM frag8;
    struct _NDR64_UNION_ARM frag9;
    struct _NDR64_UNION_ARM frag10;
    struct _NDR64_UNION_ARM frag11;
    struct _NDR64_UNION_ARM frag12;
    struct _NDR64_UNION_ARM frag13;
    struct _NDR64_UNION_ARM frag14;
    struct _NDR64_UNION_ARM frag15;
    struct _NDR64_UNION_ARM frag16;
    struct _NDR64_UNION_ARM frag17;
    struct _NDR64_UNION_ARM frag18;
    struct _NDR64_UNION_ARM frag19;
    struct _NDR64_UNION_ARM frag20;
    struct _NDR64_UNION_ARM frag21;
    struct _NDR64_UNION_ARM frag22;
    struct _NDR64_UNION_ARM frag23;
    struct _NDR64_UNION_ARM frag24;
    struct _NDR64_UNION_ARM frag25;
    struct _NDR64_UNION_ARM frag26;
    struct _NDR64_UNION_ARM frag27;
    struct _NDR64_UNION_ARM frag28;
    struct _NDR64_UNION_ARM frag29;
    struct _NDR64_UNION_ARM frag30;
    struct _NDR64_UNION_ARM frag31;
    struct _NDR64_UNION_ARM frag32;
    struct _NDR64_UNION_ARM frag33;
    struct _NDR64_UNION_ARM frag34;
    struct _NDR64_UNION_ARM frag35;
    struct _NDR64_UNION_ARM frag36;
    struct _NDR64_UNION_ARM frag37;
    struct _NDR64_UNION_ARM frag38;
    struct _NDR64_UNION_ARM frag39;
    struct _NDR64_UNION_ARM frag40;
    struct _NDR64_UNION_ARM frag41;
    struct _NDR64_UNION_ARM frag42;
    struct _NDR64_UNION_ARM frag43;
    struct _NDR64_UNION_ARM frag44;
    struct _NDR64_UNION_ARM frag45;
    struct _NDR64_UNION_ARM frag46;
    struct _NDR64_UNION_ARM frag47;
    struct _NDR64_UNION_ARM frag48;
    struct _NDR64_UNION_ARM frag49;
    NDR64_UINT32 frag50;
}
__midl_frag35_t;
extern const __midl_frag35_t __midl_frag35;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_REGION_FORMAT frag1;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
    } frag2;
}
__midl_frag34_t;
extern const __midl_frag34_t __midl_frag34;

typedef 
struct _NDR64_USER_MARSHAL_FORMAT
__midl_frag32_t;
extern const __midl_frag32_t __midl_frag32;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag31_t;
extern const __midl_frag31_t __midl_frag31;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag28_t;
extern const __midl_frag28_t __midl_frag28;

typedef 
struct 
{
    struct _NDR64_CONF_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag27_t;
extern const __midl_frag27_t __midl_frag27;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag24_t;
extern const __midl_frag24_t __midl_frag24;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag20_t;
extern const __midl_frag20_t __midl_frag20;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag7_t;
extern const __midl_frag7_t __midl_frag7;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
    struct _NDR64_PARAM_FORMAT frag5;
}
__midl_frag2_t;
extern const __midl_frag2_t __midl_frag2;

typedef 
NDR64_FORMAT_UINT32
__midl_frag1_t;
extern const __midl_frag1_t __midl_frag1;

static const __midl_frag347_t __midl_frag347 =
0x5    /* FC64_INT32 */;

static const __midl_frag346_t __midl_frag346 =
{ 
/* *FLAGGED_WORD_BLOB */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag27
};

static const __midl_frag345_t __midl_frag345 =
{ 
/* wireBSTR */
    0xa2,    /* FC64_USER_MARSHAL */
    (NDR64_UINT8) 128 /* 0x80 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    (NDR64_UINT16) 7 /* 0x7 */,
    (NDR64_UINT16) 8 /* 0x8 */,
    (NDR64_UINT32) 8 /* 0x8 */,
    (NDR64_UINT32) 0 /* 0x0 */,
    &__midl_frag346
};

static const __midl_frag344_t __midl_frag344 =
{ 
/* *wireBSTR */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag345
};

static const __midl_frag343_t __midl_frag343 =
{ 
/* *FLAGGED_WORD_BLOB */
    0x21,    /* FC64_UP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag27
};

static const __midl_frag342_t __midl_frag342 =
{ 
/* wireBSTR */
    0xa2,    /* FC64_USER_MARSHAL */
    (NDR64_UINT8) 128 /* 0x80 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    (NDR64_UINT16) 7 /* 0x7 */,
    (NDR64_UINT16) 8 /* 0x8 */,
    (NDR64_UINT32) 8 /* 0x8 */,
    (NDR64_UINT32) 0 /* 0x0 */,
    &__midl_frag343
};

static const __midl_frag341_t __midl_frag341 =
{ 
/* OnQueryPrivateDNS */
    { 
    /* OnQueryPrivateDNS */      /* procedure OnQueryPrivateDNS */
        (NDR64_UINT32) 7209283 /* 0x6e0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, ClientMustSize, HasReturn, ServerCorrelation, ClientCorrelation */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* host */      /* parameter host */
        &__midl_frag342,
        { 
        /* host */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* ipList */      /* parameter ipList */
        &__midl_frag345,
        { 
        /* ipList */
            1,
            1,
            0,
            1,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], [out], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag333_t __midl_frag333 =
{ 
/* OnWinDomainAuthenticate */
    { 
    /* OnWinDomainAuthenticate */      /* procedure OnWinDomainAuthenticate */
        (NDR64_UINT32) 7209283 /* 0x6e0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, ClientMustSize, HasReturn, ServerCorrelation, ClientCorrelation */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* userName */      /* parameter userName */
        &__midl_frag345,
        { 
        /* userName */
            1,
            1,
            0,
            1,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], [out], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* userPwd */      /* parameter userPwd */
        &__midl_frag345,
        { 
        /* userPwd */
            1,
            1,
            0,
            1,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], [out], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag327_t __midl_frag327 =
{ 
/* OnRequestAcceleratorFromMouseWheel */
    { 
    /* OnRequestAcceleratorFromMouseWheel */      /* procedure OnRequestAcceleratorFromMouseWheel */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 32 /* 0x20 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* delta */      /* parameter delta */
        &__midl_frag347,
        { 
        /* delta */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* Pt_x */      /* parameter Pt_x */
        &__midl_frag347,
        { 
        /* Pt_x */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* Pt_y */      /* parameter Pt_y */
        &__midl_frag347,
        { 
        /* Pt_y */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* modifiers */      /* parameter modifiers */
        &__midl_frag347,
        { 
        /* modifiers */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag323_t __midl_frag323 =
{ 
/* OnRequestAcceleratorFromKeyboard */
    { 
    /* OnRequestAcceleratorFromKeyboard */      /* procedure OnRequestAcceleratorFromKeyboard */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 16 /* 0x10 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* keyCode */      /* parameter keyCode */
        &__midl_frag347,
        { 
        /* keyCode */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* modifiers */      /* parameter modifiers */
        &__midl_frag347,
        { 
        /* modifiers */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag315_t __midl_frag315 =
{ 
/* OnGetLoginInfo */
    { 
    /* OnGetLoginInfo */      /* procedure OnGetLoginInfo */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* url */      /* parameter url */
        &__midl_frag342,
        { 
        /* url */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* userName */      /* parameter userName */
        &__midl_frag342,
        { 
        /* userName */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* userPwd */      /* parameter userPwd */
        &__midl_frag342,
        { 
        /* userPwd */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag311_t __midl_frag311 =
{ 
/* OnUpdateCookie */
    { 
    /* OnUpdateCookie */      /* procedure OnUpdateCookie */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* cookie */      /* parameter cookie */
        &__midl_frag342,
        { 
        /* cookie */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag308_t __midl_frag308 =
{ 
/* *int */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag347
};

static const __midl_frag301_t __midl_frag301 =
{ 
/* OnGetMainWndPos */
    { 
    /* OnGetMainWndPos */      /* procedure OnGetMainWndPos */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 128 /* 0x80 */,
        (NDR64_UINT32) 136 /* 0x88 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* left */      /* parameter left */
        &__midl_frag347,
        { 
        /* left */
            0,
            0,
            0,
            1,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], [out], Basetype, SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* top */      /* parameter top */
        &__midl_frag347,
        { 
        /* top */
            0,
            0,
            0,
            1,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], [out], Basetype, SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* width */      /* parameter width */
        &__midl_frag347,
        { 
        /* width */
            0,
            0,
            0,
            1,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], [out], Basetype, SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* height */      /* parameter height */
        &__midl_frag347,
        { 
        /* height */
            0,
            0,
            0,
            1,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], [out], Basetype, SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag293_t __midl_frag293 =
{ 
/* OnGoForward */
    { 
    /* OnGoForward */      /* procedure OnGoForward */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 16 /* 0x10 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 1 /* 0x1 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    }
};

static const __midl_frag289_t __midl_frag289 =
0x4    /* FC64_INT16 */;

static const __midl_frag288_t __midl_frag288 =
{ 
/* *VARIANT_BOOL */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag289
};

static const __midl_frag285_t __midl_frag285 =
{ 
/* OnCanGoBackOrForward */
    { 
    /* OnCanGoBackOrForward */      /* procedure OnCanGoBackOrForward */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 60 /* 0x3c */,
        (NDR64_UINT32) 68 /* 0x44 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* back */      /* parameter back */
        &__midl_frag289,
        { 
        /* back */
            0,
            0,
            0,
            1,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], [out], Basetype, SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* forward */      /* parameter forward */
        &__midl_frag289,
        { 
        /* forward */
            0,
            0,
            0,
            1,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], [out], Basetype, SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag283_t __midl_frag283 =
{ 
/* *_wireVARIANT */
    0x21,    /* FC64_UP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag34
};

static const __midl_frag282_t __midl_frag282 =
{ 
/* wireVARIANT */
    0xa2,    /* FC64_USER_MARSHAL */
    (NDR64_UINT8) 128 /* 0x80 */,
    (NDR64_UINT16) 1 /* 0x1 */,
    (NDR64_UINT16) 7 /* 0x7 */,
    (NDR64_UINT16) 8 /* 0x8 */,
    (NDR64_UINT32) 24 /* 0x18 */,
    (NDR64_UINT32) 0 /* 0x0 */,
    &__midl_frag283
};

static const __midl_frag281_t __midl_frag281 =
{ 
/* *wireVARIANT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag282
};

static const __midl_frag277_t __midl_frag277 =
{ 
/* OnDownLoadFile */
    { 
    /* OnDownLoadFile */      /* procedure OnDownLoadFile */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* url */      /* parameter url */
        &__midl_frag342,
        { 
        /* url */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* status */      /* parameter status */
        &__midl_frag347,
        { 
        /* status */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* variant */      /* parameter variant */
        &__midl_frag282,
        { 
        /* variant */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag274_t __midl_frag274 =
{ 
/* OnIEServerWndCreated */
    { 
    /* OnIEServerWndCreated */      /* procedure OnIEServerWndCreated */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* hwnd */      /* parameter hwnd */
        &__midl_frag347,
        { 
        /* hwnd */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag269_t __midl_frag269 =
{ 
/* OnShowContextMenu */
    { 
    /* OnShowContextMenu */      /* procedure OnShowContextMenu */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 24 /* 0x18 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* dwID */      /* parameter dwID */
        &__midl_frag347,
        { 
        /* dwID */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* x */      /* parameter x */
        &__midl_frag347,
        { 
        /* x */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* y */      /* parameter y */
        &__midl_frag347,
        { 
        /* y */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag263_t __midl_frag263 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0x00020400,
        0x0000,
        0x0000,
        {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
    }
};

static const __midl_frag262_t __midl_frag262 =
{ 
/* *struct _NDR64_POINTER_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag263
};

static const __midl_frag261_t __midl_frag261 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag262
};

static const __midl_frag255_t __midl_frag255 =
{ 
/* OnLoadUrlNewWnd */
    { 
    /* OnLoadUrlNewWnd */      /* procedure OnLoadUrlNewWnd */
        (NDR64_UINT32) 3014979 /* 0x2e0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 48 /* 0x30 */ ,  /* Stack size */
        (NDR64_UINT32) 38 /* 0x26 */,
        (NDR64_UINT32) 38 /* 0x26 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 5 /* 0x5 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* url */      /* parameter url */
        &__midl_frag342,
        { 
        /* url */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* flag */      /* parameter flag */
        &__midl_frag347,
        { 
        /* flag */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* cancel */      /* parameter cancel */
        &__midl_frag289,
        { 
        /* cancel */
            0,
            0,
            0,
            1,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], [out], Basetype, SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* pDisp */      /* parameter pDisp */
        &__midl_frag261,
        { 
        /* pDisp */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    }
};

static const __midl_frag245_t __midl_frag245 =
{ 
/* OnFinishNavigate */
    { 
    /* OnFinishNavigate */      /* procedure OnFinishNavigate */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* url */      /* parameter url */
        &__midl_frag342,
        { 
        /* url */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* faviconUrls */      /* parameter faviconUrls */
        &__midl_frag342,
        { 
        /* faviconUrls */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag238_t __midl_frag238 =
{ 
/* OnBeforeNavigate */
    { 
    /* OnBeforeNavigate */      /* procedure OnBeforeNavigate */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 36 /* 0x24 */,
        (NDR64_UINT32) 38 /* 0x26 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* url */      /* parameter url */
        &__midl_frag342,
        { 
        /* url */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* navNewUrl */      /* parameter navNewUrl */
        &__midl_frag289,
        { 
        /* navNewUrl */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* cancel */      /* parameter cancel */
        &__midl_frag289,
        { 
        /* cancel */
            0,
            0,
            0,
            1,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], [out], Basetype, SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag235_t __midl_frag235 =
{ 
/* ShowDevTools */
    { 
    /* ShowDevTools */      /* procedure ShowDevTools */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 6 /* 0x6 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* show */      /* parameter show */
        &__midl_frag289,
        { 
        /* show */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag231_t __midl_frag231 =
{ 
/* GetDevToolsOpenStatus */
    { 
    /* GetDevToolsOpenStatus */      /* procedure GetDevToolsOpenStatus */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 30 /* 0x1e */,
        (NDR64_UINT32) 38 /* 0x26 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* isOpen */      /* parameter isOpen */
        &__midl_frag289,
        { 
        /* isOpen */
            0,
            0,
            0,
            1,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], [out], Basetype, SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag215_t __midl_frag215 =
{ 
/* SetListenLoginXPath */
    { 
    /* SetListenLoginXPath */      /* procedure SetListenLoginXPath */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 56 /* 0x38 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 6 /* 0x6 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* url */      /* parameter url */
        &__midl_frag342,
        { 
        /* url */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* frameXPath */      /* parameter frameXPath */
        &__midl_frag342,
        { 
        /* frameXPath */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* userNameXPath */      /* parameter userNameXPath */
        &__midl_frag342,
        { 
        /* userNameXPath */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* pwdXPath */      /* parameter pwdXPath */
        &__midl_frag342,
        { 
        /* pwdXPath */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* loginbtnXPath */      /* parameter loginbtnXPath */
        &__midl_frag342,
        { 
        /* loginbtnXPath */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        48 /* 0x30 */,   /* Stack offset */
    }
};

static const __midl_frag201_t __midl_frag201 =
{ 
/* AutoLogin */
    { 
    /* AutoLogin */      /* procedure AutoLogin */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 64 /* 0x40 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 7 /* 0x7 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* frameXPath */      /* parameter frameXPath */
        &__midl_frag342,
        { 
        /* frameXPath */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* userNameXPath */      /* parameter userNameXPath */
        &__midl_frag342,
        { 
        /* userNameXPath */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* userNameValue */      /* parameter userNameValue */
        &__midl_frag342,
        { 
        /* userNameValue */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* pwdXPath */      /* parameter pwdXPath */
        &__midl_frag342,
        { 
        /* pwdXPath */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    },
    { 
    /* pwdValue */      /* parameter pwdValue */
        &__midl_frag342,
        { 
        /* pwdValue */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        40 /* 0x28 */,   /* Stack offset */
    },
    { 
    /* loginBtnXPath */      /* parameter loginBtnXPath */
        &__midl_frag342,
        { 
        /* loginBtnXPath */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        48 /* 0x30 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        56 /* 0x38 */,   /* Stack offset */
    }
};

static const __midl_frag185_t __midl_frag185 =
{ 
/* SetCookie */
    { 
    /* SetCookie */      /* procedure SetCookie */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* json */      /* parameter json */
        &__midl_frag342,
        { 
        /* json */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* flag */      /* parameter flag */
        &__midl_frag347,
        { 
        /* flag */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag179_t __midl_frag179 =
{ 
/* *int */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 12 /* 0xc */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag347
};

static const __midl_frag178_t __midl_frag178 =
{ 
/* GetDocumentMode */
    { 
    /* GetDocumentMode */      /* procedure GetDocumentMode */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* mode */      /* parameter mode */
        &__midl_frag347,
        { 
        /* mode */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag172_t __midl_frag172 =
{ 
/* *UINT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag347
};

static const __midl_frag169_t __midl_frag169 =
0x7    /* FC64_INT64 */;

static const __midl_frag168_t __midl_frag168 =
{ 
/* *ULONGLONG */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag169
};

static const __midl_frag164_t __midl_frag164 =
{ 
/* *USHORT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag289
};

static const __midl_frag163_t __midl_frag163 =
0x10    /* FC64_CHAR */;

static const __midl_frag162_t __midl_frag162 =
{ 
/* *CHAR */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag163
};

static const __midl_frag161_t __midl_frag161 =
{ 
/* *DECIMAL */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag160
};

static const __midl_frag160_t __midl_frag160 =
{ 
/* DECIMAL */
    { 
    /* DECIMAL */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* DECIMAL */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */
    }
};

static const __midl_frag153_t __midl_frag153 =
{ 
/* *_wireVARIANT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag34
};

static const __midl_frag152_t __midl_frag152 =
{ 
/* **_wireVARIANT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag153
};

static const __midl_frag151_t __midl_frag151 =
{ 
/* *_wireSAFEARRAY */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag54
};

static const __midl_frag150_t __midl_frag150 =
{ 
/* **_wireSAFEARRAY */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag151
};

static const __midl_frag149_t __midl_frag149 =
{ 
/* ***_wireSAFEARRAY */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag150
};

static const __midl_frag146_t __midl_frag146 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag262
};

static const __midl_frag145_t __midl_frag145 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0x00000000,
        0x0000,
        0x0000,
        {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
    }
};

static const __midl_frag144_t __midl_frag144 =
{ 
/* *struct _NDR64_POINTER_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag145
};

static const __midl_frag143_t __midl_frag143 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag144
};

static const __midl_frag141_t __midl_frag141 =
{ 
/* **FLAGGED_WORD_BLOB */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag346
};

static const __midl_frag140_t __midl_frag140 =
0xc    /* FC64_FLOAT64 */;

static const __midl_frag139_t __midl_frag139 =
{ 
/* *DATE */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag140
};

static const __midl_frag138_t __midl_frag138 =
{ 
/* *CY */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag45
};

static const __midl_frag131_t __midl_frag131 =
0xb    /* FC64_FLOAT32 */;

static const __midl_frag130_t __midl_frag130 =
{ 
/* *FLOAT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag131
};

static const __midl_frag123_t __midl_frag123 =
0x2    /* FC64_INT8 */;

static const __midl_frag122_t __midl_frag122 =
{ 
/* *BYTE */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag123
};

static const __midl_frag121_t __midl_frag121 =
{ 
/* *_wireBRECORD */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag86
};

static const __midl_frag120_t __midl_frag120 =
{ 
/*  */
    { 
    /* *hyper */
        0x22,    /* FC64_OP */
        (NDR64_UINT8) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag117
    }
};

static const __midl_frag118_t __midl_frag118 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x6,    /* FC64_UINT32 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 0 /* 0x0 */
    }
};

static const __midl_frag117_t __midl_frag117 =
{ 
/* *hyper */
    { 
    /* *hyper */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* *hyper */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag118
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag169
    }
};

static const __midl_frag116_t __midl_frag116 =
{ 
/* HYPER_SIZEDARR */
    { 
    /* HYPER_SIZEDARR */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* HYPER_SIZEDARR */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag120,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag115_t __midl_frag115 =
{ 
/*  */
    { 
    /* *ULONG */
        0x22,    /* FC64_OP */
        (NDR64_UINT8) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag112
    }
};

static const __midl_frag112_t __midl_frag112 =
{ 
/* *ULONG */
    { 
    /* *ULONG */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* *ULONG */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 4 /* 0x4 */,
        &__midl_frag118
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 4 /* 0x4 */,
        &__midl_frag347
    }
};

static const __midl_frag111_t __midl_frag111 =
{ 
/* DWORD_SIZEDARR */
    { 
    /* DWORD_SIZEDARR */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* DWORD_SIZEDARR */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag115,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag110_t __midl_frag110 =
{ 
/*  */
    { 
    /* *short */
        0x22,    /* FC64_OP */
        (NDR64_UINT8) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag107
    }
};

static const __midl_frag107_t __midl_frag107 =
{ 
/* *short */
    { 
    /* *short */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 1 /* 0x1 */,
        { 
        /* *short */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 2 /* 0x2 */,
        &__midl_frag118
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 2 /* 0x2 */,
        &__midl_frag289
    }
};

static const __midl_frag106_t __midl_frag106 =
{ 
/* WORD_SIZEDARR */
    { 
    /* WORD_SIZEDARR */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* WORD_SIZEDARR */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag110,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag105_t __midl_frag105 =
{ 
/*  */
    { 
    /* *byte */
        0x22,    /* FC64_OP */
        (NDR64_UINT8) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag102
    }
};

static const __midl_frag102_t __midl_frag102 =
{ 
/* *byte */
    { 
    /* *byte */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 0 /* 0x0 */,
        { 
        /* *byte */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag118
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag123
    }
};

static const __midl_frag101_t __midl_frag101 =
{ 
/* BYTE_SIZEDARR */
    { 
    /* BYTE_SIZEDARR */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* BYTE_SIZEDARR */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag105,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag100_t __midl_frag100 =
{ 
/*  */
    { 
    /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
        0x40,    /* FC64_FIX_ARRAY */
        (NDR64_UINT8) 0 /* 0x0 */,
        { 
        /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */
    }
};

static const __midl_frag99_t __midl_frag99 =
{ 
/*  */
    { 
    /* **struct _NDR64_POINTER_FORMAT */
        0x20,    /* FC64_RP */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag94
    }
};

static const __midl_frag94_t __midl_frag94 =
{ 
/* ** */
    { 
    /* **struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* **struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag118
    },
    { 
    /*  */
        { 
        /* struct _NDR64_REPEAT_FORMAT */
            0x82,    /* FC64_VARIABLE_REPEAT */
            { 
            /* struct _NDR64_REPEAT_FORMAT */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT8) 0 /* 0x0 */
            },
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 1 /* 0x1 */
        },
        { 
        /*  */
            { 
            /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
                (NDR64_UINT32) 0 /* 0x0 */,
                (NDR64_UINT32) 0 /* 0x0 */
            },
            { 
            /* *struct _NDR64_POINTER_FORMAT */
                0x24,    /* FC64_IP */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT16) 0 /* 0x0 */,
                &__midl_frag145
            }
        },
        0x93    /* FC64_END */
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag144
    }
};

static const __midl_frag93_t __midl_frag93 =
{ 
/* SAFEARR_HAVEIID */
    { 
    /* SAFEARR_HAVEIID */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_HAVEIID */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        0,
        0,
        &__midl_frag99,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x4,    /* FC64_INT16 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x4,    /* FC64_INT16 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag100
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag92_t __midl_frag92 =
{ 
/*  */
    { 
    /* **_wireBRECORD */
        0x20,    /* FC64_RP */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag84
    }
};

static const __midl_frag89_t __midl_frag89 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x6,    /* FC64_UINT32 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 4 /* 0x4 */
    }
};

static const __midl_frag88_t __midl_frag88 =
{ 
/* *byte */
    { 
    /* *byte */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 0 /* 0x0 */,
        { 
        /* *byte */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag89
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag123
    }
};

static const __midl_frag87_t __midl_frag87 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0x0000002f,
        0x0000,
        0x0000,
        {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
    }
};

static const __midl_frag86_t __midl_frag86 =
{ 
/* _wireBRECORD */
    { 
    /* _wireBRECORD */
        0x31,    /* FC64_PSTRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* _wireBRECORD */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 24 /* 0x18 */
    },
    { 
    /*  */
        { 
        /* struct _NDR64_NO_REPEAT_FORMAT */
            0x80,    /* FC64_NO_REPEAT */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* *struct _NDR64_POINTER_FORMAT */
            0x24,    /* FC64_IP */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag87
        },
        { 
        /* struct _NDR64_NO_REPEAT_FORMAT */
            0x80,    /* FC64_NO_REPEAT */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
            (NDR64_UINT32) 16 /* 0x10 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* *byte */
            0x22,    /* FC64_OP */
            (NDR64_UINT8) 32 /* 0x20 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag88
        },
        0x93    /* FC64_END */
    }
};

static const __midl_frag84_t __midl_frag84 =
{ 
/* **_wireBRECORD */
    { 
    /* **_wireBRECORD */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* **_wireBRECORD */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag118
    },
    { 
    /*  */
        { 
        /* struct _NDR64_REPEAT_FORMAT */
            0x82,    /* FC64_VARIABLE_REPEAT */
            { 
            /* struct _NDR64_REPEAT_FORMAT */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT8) 0 /* 0x0 */
            },
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 1 /* 0x1 */
        },
        { 
        /*  */
            { 
            /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
                (NDR64_UINT32) 0 /* 0x0 */,
                (NDR64_UINT32) 0 /* 0x0 */
            },
            { 
            /* *_wireBRECORD */
                0x22,    /* FC64_OP */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT16) 0 /* 0x0 */,
                &__midl_frag86
            }
        },
        0x93    /* FC64_END */
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag121
    }
};

static const __midl_frag83_t __midl_frag83 =
{ 
/* SAFEARR_BRECORD */
    { 
    /* SAFEARR_BRECORD */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_BRECORD */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag92,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag82_t __midl_frag82 =
{ 
/*  */
    { 
    /* **_wireVARIANT */
        0x20,    /* FC64_RP */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag79
    }
};

static const __midl_frag79_t __midl_frag79 =
{ 
/* **_wireVARIANT */
    { 
    /* **_wireVARIANT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* **_wireVARIANT */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag118
    },
    { 
    /*  */
        { 
        /* struct _NDR64_REPEAT_FORMAT */
            0x82,    /* FC64_VARIABLE_REPEAT */
            { 
            /* struct _NDR64_REPEAT_FORMAT */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT8) 0 /* 0x0 */
            },
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 1 /* 0x1 */
        },
        { 
        /*  */
            { 
            /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
                (NDR64_UINT32) 0 /* 0x0 */,
                (NDR64_UINT32) 0 /* 0x0 */
            },
            { 
            /* *_wireVARIANT */
                0x22,    /* FC64_OP */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT16) 0 /* 0x0 */,
                &__midl_frag34
            }
        },
        0x93    /* FC64_END */
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag153
    }
};

static const __midl_frag78_t __midl_frag78 =
{ 
/* SAFEARR_VARIANT */
    { 
    /* SAFEARR_VARIANT */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_VARIANT */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag82,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag77_t __midl_frag77 =
{ 
/*  */
    { 
    /* **struct _NDR64_POINTER_FORMAT */
        0x20,    /* FC64_RP */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag72
    }
};

static const __midl_frag72_t __midl_frag72 =
{ 
/* ** */
    { 
    /* **struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* **struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag118
    },
    { 
    /*  */
        { 
        /* struct _NDR64_REPEAT_FORMAT */
            0x82,    /* FC64_VARIABLE_REPEAT */
            { 
            /* struct _NDR64_REPEAT_FORMAT */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT8) 0 /* 0x0 */
            },
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 1 /* 0x1 */
        },
        { 
        /*  */
            { 
            /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
                (NDR64_UINT32) 0 /* 0x0 */,
                (NDR64_UINT32) 0 /* 0x0 */
            },
            { 
            /* *struct _NDR64_POINTER_FORMAT */
                0x24,    /* FC64_IP */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT16) 0 /* 0x0 */,
                &__midl_frag263
            }
        },
        0x93    /* FC64_END */
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag262
    }
};

static const __midl_frag71_t __midl_frag71 =
{ 
/* SAFEARR_DISPATCH */
    { 
    /* SAFEARR_DISPATCH */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_DISPATCH */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag77,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag64_t __midl_frag64 =
{ 
/* SAFEARR_UNKNOWN */
    { 
    /* SAFEARR_UNKNOWN */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_UNKNOWN */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag99,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag63_t __midl_frag63 =
{ 
/*  */
    { 
    /* **FLAGGED_WORD_BLOB */
        0x20,    /* FC64_RP */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag60
    }
};

static const __midl_frag60_t __midl_frag60 =
{ 
/* **FLAGGED_WORD_BLOB */
    { 
    /* **FLAGGED_WORD_BLOB */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* **FLAGGED_WORD_BLOB */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag118
    },
    { 
    /*  */
        { 
        /* struct _NDR64_REPEAT_FORMAT */
            0x82,    /* FC64_VARIABLE_REPEAT */
            { 
            /* struct _NDR64_REPEAT_FORMAT */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT8) 0 /* 0x0 */
            },
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 1 /* 0x1 */
        },
        { 
        /*  */
            { 
            /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
                (NDR64_UINT32) 0 /* 0x0 */,
                (NDR64_UINT32) 0 /* 0x0 */
            },
            { 
            /* *FLAGGED_WORD_BLOB */
                0x22,    /* FC64_OP */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT16) 0 /* 0x0 */,
                &__midl_frag27
            }
        },
        0x93    /* FC64_END */
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag346
    }
};

static const __midl_frag59_t __midl_frag59 =
{ 
/* SAFEARR_BSTR */
    { 
    /* SAFEARR_BSTR */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_BSTR */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag63,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag58_t __midl_frag58 =
{ 
/* SAFEARRAYUNION */
    { 
    /* SAFEARRAYUNION */
        0x50,    /* FC64_ENCAPSULATED_UNION */
        (NDR64_UINT8) 7 /* 0x7 */,
        (NDR64_UINT8) 0 /* 0x0 */,
        0x6,    /* FC64_UINT32 */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM_SELECTOR */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT8) 7 /* 0x7 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 10 /* 0xa */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 8 /* 0x8 */,
        &__midl_frag59,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 13 /* 0xd */,
        &__midl_frag64,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 9 /* 0x9 */,
        &__midl_frag71,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 12 /* 0xc */,
        &__midl_frag78,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 36 /* 0x24 */,
        &__midl_frag83,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 32781 /* 0x800d */,
        &__midl_frag93,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16 /* 0x10 */,
        &__midl_frag101,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 2 /* 0x2 */,
        &__midl_frag106,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 3 /* 0x3 */,
        &__midl_frag111,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 20 /* 0x14 */,
        &__midl_frag116,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    (NDR64_UINT32) 4294967295 /* 0xffffffff */
};

static const __midl_frag57_t __midl_frag57 =
{ 
/* SAFEARRAYBOUND */
    { 
    /* SAFEARRAYBOUND */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* SAFEARRAYBOUND */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */
    }
};

static const __midl_frag56_t __midl_frag56 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x3,    /* FC64_UINT16 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 0 /* 0x0 */
    }
};

static const __midl_frag55_t __midl_frag55 =
{ 
/*  */
    { 
    /* struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag56
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag57
    }
};

static const __midl_frag54_t __midl_frag54 =
{ 
/* _wireSAFEARRAY */
    { 
    /* _wireSAFEARRAY */
        0x36,    /* FC64_CONF_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* _wireSAFEARRAY */
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 1 /* 0x1 */,
        (NDR64_UINT32) 56 /* 0x38 */,
        0,
        0,
        0,
        &__midl_frag55,
    },
    { 
    /*  */
        { 
        /* _wireSAFEARRAY */
            0x30,    /* FC64_STRUCT */
            (NDR64_UINT8) 1 /* 0x1 */,
            (NDR64_UINT16) 12 /* 0xc */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag58
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag45_t __midl_frag45 =
{ 
/* CY */
    { 
    /* CY */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* CY */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */
    }
};

static const __midl_frag36_t __midl_frag36 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x3,    /* FC64_UINT16 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */
    }
};

static const __midl_frag35_t __midl_frag35 =
{ 
/* __MIDL_IOleAutomationTypes_0004 */
    { 
    /* __MIDL_IOleAutomationTypes_0004 */
        0x51,    /* FC64_NON_ENCAPSULATED_UNION */
        (NDR64_UINT8) 7 /* 0x7 */,
        (NDR64_UINT8) 0 /* 0x0 */,
        0x6,    /* FC64_UINT32 */
        (NDR64_UINT32) 16 /* 0x10 */,
        &__midl_frag36,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM_SELECTOR */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT8) 7 /* 0x7 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 47 /* 0x2f */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 20 /* 0x14 */,
        &__midl_frag169,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 3 /* 0x3 */,
        &__midl_frag347,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 17 /* 0x11 */,
        &__midl_frag123,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 2 /* 0x2 */,
        &__midl_frag289,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 4 /* 0x4 */,
        &__midl_frag131,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 5 /* 0x5 */,
        &__midl_frag140,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 11 /* 0xb */,
        &__midl_frag289,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 10 /* 0xa */,
        &__midl_frag347,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 6 /* 0x6 */,
        &__midl_frag45,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 7 /* 0x7 */,
        &__midl_frag140,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 8 /* 0x8 */,
        &__midl_frag346,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 13 /* 0xd */,
        &__midl_frag144,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 9 /* 0x9 */,
        &__midl_frag262,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 8192 /* 0x2000 */,
        &__midl_frag150,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 36 /* 0x24 */,
        &__midl_frag121,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16420 /* 0x4024 */,
        &__midl_frag121,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16401 /* 0x4011 */,
        &__midl_frag122,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16386 /* 0x4002 */,
        &__midl_frag164,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16387 /* 0x4003 */,
        &__midl_frag172,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16404 /* 0x4014 */,
        &__midl_frag168,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16388 /* 0x4004 */,
        &__midl_frag130,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16389 /* 0x4005 */,
        &__midl_frag139,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16395 /* 0x400b */,
        &__midl_frag164,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16394 /* 0x400a */,
        &__midl_frag172,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16390 /* 0x4006 */,
        &__midl_frag138,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16391 /* 0x4007 */,
        &__midl_frag139,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16392 /* 0x4008 */,
        &__midl_frag141,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16397 /* 0x400d */,
        &__midl_frag143,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16393 /* 0x4009 */,
        &__midl_frag146,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 24576 /* 0x6000 */,
        &__midl_frag149,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16396 /* 0x400c */,
        &__midl_frag152,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16 /* 0x10 */,
        &__midl_frag163,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 18 /* 0x12 */,
        &__midl_frag289,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 19 /* 0x13 */,
        &__midl_frag347,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 21 /* 0x15 */,
        &__midl_frag169,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 22 /* 0x16 */,
        &__midl_frag347,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 23 /* 0x17 */,
        &__midl_frag347,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 14 /* 0xe */,
        &__midl_frag160,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16398 /* 0x400e */,
        &__midl_frag161,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16400 /* 0x4010 */,
        &__midl_frag162,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16402 /* 0x4012 */,
        &__midl_frag164,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16403 /* 0x4013 */,
        &__midl_frag172,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16405 /* 0x4015 */,
        &__midl_frag168,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16406 /* 0x4016 */,
        &__midl_frag172,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16407 /* 0x4017 */,
        &__midl_frag172,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 0 /* 0x0 */,
        0,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 1 /* 0x1 */,
        0,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    (NDR64_UINT32) 4294967295 /* 0xffffffff */
};

static const __midl_frag34_t __midl_frag34 =
{ 
/* _wireVARIANT */
    { 
    /* _wireVARIANT */
        0x34,    /* FC64_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* _wireVARIANT */
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        0,
        0,
        0,
    },
    { 
    /*  */
        { 
        /* _wireVARIANT */
            0x30,    /* FC64_STRUCT */
            (NDR64_UINT8) 3 /* 0x3 */,
            (NDR64_UINT16) 16 /* 0x10 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag35
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag32_t __midl_frag32 =
{ 
/* wireVARIANT */
    0xa2,    /* FC64_USER_MARSHAL */
    (NDR64_UINT8) 128 /* 0x80 */,
    (NDR64_UINT16) 1 /* 0x1 */,
    (NDR64_UINT16) 7 /* 0x7 */,
    (NDR64_UINT16) 8 /* 0x8 */,
    (NDR64_UINT32) 24 /* 0x18 */,
    (NDR64_UINT32) 0 /* 0x0 */,
    &__midl_frag153
};

static const __midl_frag31_t __midl_frag31 =
{ 
/* *wireVARIANT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag32
};

static const __midl_frag28_t __midl_frag28 =
{ 
/*  */
    { 
    /* struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 1 /* 0x1 */,
        { 
        /* struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 2 /* 0x2 */,
        &__midl_frag89
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 2 /* 0x2 */,
        &__midl_frag289
    }
};

static const __midl_frag27_t __midl_frag27 =
{ 
/* FLAGGED_WORD_BLOB */
    { 
    /* FLAGGED_WORD_BLOB */
        0x32,    /* FC64_CONF_STRUCT */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* FLAGGED_WORD_BLOB */
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag28
    }
};

static const __midl_frag24_t __midl_frag24 =
{ 
/* ExcuteJS */
    { 
    /* ExcuteJS */      /* procedure ExcuteJS */
        (NDR64_UINT32) 7209283 /* 0x6e0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, ClientMustSize, HasReturn, ServerCorrelation, ClientCorrelation */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* src */      /* parameter src */
        &__midl_frag342,
        { 
        /* src */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* result */      /* parameter result */
        &__midl_frag32,
        { 
        /* result */
            1,
            1,
            0,
            1,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], [out], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag20_t __midl_frag20 =
{ 
/* putref_HostApp */
    { 
    /* putref_HostApp */      /* procedure putref_HostApp */
        (NDR64_UINT32) 786755 /* 0xc0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* app */      /* parameter app */
        &__midl_frag262,
        { 
        /* app */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag7_t __midl_frag7 =
{ 
/* get_WebBrowser */
    { 
    /* get_WebBrowser */      /* procedure get_WebBrowser */
        (NDR64_UINT32) 655683 /* 0xa0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* pVal */      /* parameter pVal */
        &__midl_frag261,
        { 
        /* pVal */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag2_t __midl_frag2 =
{ 
/* CreateBrowser */
    { 
    /* CreateBrowser */      /* procedure CreateBrowser */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 40 /* 0x28 */ ,  /* Stack size */
        (NDR64_UINT32) 22 /* 0x16 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 4 /* 0x4 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* hWnd */      /* parameter hWnd */
        &__midl_frag347,
        { 
        /* hWnd */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* browserEmulation */      /* parameter browserEmulation */
        &__midl_frag347,
        { 
        /* browserEmulation */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* isNewWindow */      /* parameter isNewWindow */
        &__midl_frag289,
        { 
        /* isNewWindow */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag347,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        32 /* 0x20 */,   /* Stack offset */
    }
};

static const __midl_frag1_t __midl_frag1 =
(NDR64_UINT32) 0 /* 0x0 */;


#include "poppack.h"


static const USER_MARSHAL_ROUTINE_QUADRUPLE NDR64_UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            BSTR_UserSize64
            ,BSTR_UserMarshal64
            ,BSTR_UserUnmarshal64
            ,BSTR_UserFree64
            },
            {
            VARIANT_UserSize64
            ,VARIANT_UserMarshal64
            ,VARIANT_UserUnmarshal64
            ,VARIANT_UserFree64
            }

        };



/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IBrowserContainer, ver. 0.0,
   GUID={0xDB1BC009,0x56ED,0x4A59,{0xB4,0x10,0x1D,0x69,0xAF,0xE4,0x09,0x50}} */

#pragma code_seg(".orpc")
static const FormatInfoRef IBrowserContainer_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag2,
    &__midl_frag7,
    &__midl_frag293,
    &__midl_frag327,
    &__midl_frag20,
    &__midl_frag24,
    &__midl_frag235,
    &__midl_frag178,
    &__midl_frag274,
    &__midl_frag185,
    &__midl_frag311,
    &__midl_frag235,
    &__midl_frag311,
    &__midl_frag201,
    &__midl_frag215,
    &__midl_frag311,
    &__midl_frag231,
    &__midl_frag235
    };


static const MIDL_SYNTAX_INFO IBrowserContainer_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    IEInterface__MIDL_ProcFormatString.Format,
    &IBrowserContainer_FormatStringOffsetTable[-3],
    IEInterface__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IBrowserContainer_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IBrowserContainer_ProxyInfo =
    {
    &Object_StubDesc,
    IEInterface__MIDL_ProcFormatString.Format,
    &IBrowserContainer_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IBrowserContainer_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IBrowserContainer_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    IEInterface__MIDL_ProcFormatString.Format,
    (unsigned short *) &IBrowserContainer_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IBrowserContainer_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IBrowserEventHandler_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag238,
    &__midl_frag245,
    &__midl_frag311,
    &__midl_frag255,
    &__midl_frag293,
    &__midl_frag293,
    &__midl_frag269,
    &__midl_frag274,
    &__midl_frag277,
    &__midl_frag285,
    &__midl_frag293,
    &__midl_frag293,
    &__midl_frag327,
    &__midl_frag301,
    &__midl_frag311,
    &__midl_frag315,
    &__midl_frag323,
    &__midl_frag327,
    &__midl_frag333,
    &__midl_frag341
    };


static const MIDL_SYNTAX_INFO IBrowserEventHandler_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    IEInterface__MIDL_ProcFormatString.Format,
    &IBrowserEventHandler_FormatStringOffsetTable[-3],
    IEInterface__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IBrowserEventHandler_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IBrowserEventHandler_ProxyInfo =
    {
    &Object_StubDesc,
    IEInterface__MIDL_ProcFormatString.Format,
    &IBrowserEventHandler_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IBrowserEventHandler_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IBrowserEventHandler_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    IEInterface__MIDL_ProcFormatString.Format,
    (unsigned short *) &IBrowserEventHandler_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IBrowserEventHandler_SyntaxInfo
    };
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
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
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
static const FormatInfoRef IHttpMonitor_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    0
    };


static const MIDL_SYNTAX_INFO IHttpMonitor_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    IEInterface__MIDL_ProcFormatString.Format,
    &IHttpMonitor_FormatStringOffsetTable[-3],
    IEInterface__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &IHttpMonitor_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO IHttpMonitor_ProxyInfo =
    {
    &Object_StubDesc,
    IEInterface__MIDL_ProcFormatString.Format,
    &IHttpMonitor_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IHttpMonitor_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO IHttpMonitor_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    IEInterface__MIDL_ProcFormatString.Format,
    (unsigned short *) &IHttpMonitor_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)IHttpMonitor_SyntaxInfo
    };
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
    0x60001, /* Ndr library version */
    0,
    0x8000265, /* MIDL Version 8.0.613 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x2000001, /* MIDL flag */
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

