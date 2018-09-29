

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 8.00.0613 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for IEInterface.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0613
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data
    VC __declspec() decoration level:
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

//#if !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off )

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif /* __RPCPROXY_H_VERSION__ */


#include "IEInterface_h.h"

#define TYPE_FORMAT_STRING_SIZE   1101
#define PROC_FORMAT_STRING_SIZE   1543
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

// #if !defined(__RPC_WIN32__)
// #error  Invalid build platform for this stub.
// #endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const IEInterface_MIDL_PROC_FORMAT_STRING IEInterface__MIDL_ProcFormatString =
    {
        0,
        {

  /* Procedure CreateBrowser */

      0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/*  2 */  NdrFcLong( 0x0 ), /* 0 */
/*  6 */  NdrFcShort( 0x7 ),  /* 7 */
/*  8 */  NdrFcShort( 0x14 ), /* x86 Stack size/offset = 20 */
/* 10 */  NdrFcShort( 0x16 ), /* 22 */
/* 12 */  NdrFcShort( 0x8 ),  /* 8 */
/* 14 */  0x44,   /* Oi2 Flags:  has return, has ext, */
      0x4,    /* 4 */
/* 16 */  0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 18 */  NdrFcShort( 0x0 ),  /* 0 */
/* 20 */  NdrFcShort( 0x0 ),  /* 0 */
/* 22 */  NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter hWnd */

/* 24 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 26 */  NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 28 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter browserEmulation */

/* 30 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 32 */  NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 34 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter isNewWindow */

/* 36 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 38 */  NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 40 */  0x6,    /* FC_SHORT */
      0x0,    /* 0 */

  /* Return value */

/* 42 */  NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 44 */  NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 46 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure get_WebBrowser */

/* 48 */  0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 50 */  NdrFcLong( 0x0 ), /* 0 */
/* 54 */  NdrFcShort( 0x8 ),  /* 8 */
/* 56 */  NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 58 */  NdrFcShort( 0x0 ),  /* 0 */
/* 60 */  NdrFcShort( 0x8 ),  /* 8 */
/* 62 */  0x45,   /* Oi2 Flags:  srv must size, has return, has ext, */
      0x2,    /* 2 */
/* 64 */  0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 66 */  NdrFcShort( 0x0 ),  /* 0 */
/* 68 */  NdrFcShort( 0x0 ),  /* 0 */
/* 70 */  NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter pVal */

/* 72 */  NdrFcShort( 0x13 ), /* Flags:  must size, must free, out, */
/* 74 */  NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 76 */  NdrFcShort( 0x2 ),  /* Type Offset=2 */

  /* Return value */

/* 78 */  NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 80 */  NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 82 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure CloseBrowser */

/* 84 */  0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 86 */  NdrFcLong( 0x0 ), /* 0 */
/* 90 */  NdrFcShort( 0x9 ),  /* 9 */
/* 92 */  NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 94 */  NdrFcShort( 0x0 ),  /* 0 */
/* 96 */  NdrFcShort( 0x8 ),  /* 8 */
/* 98 */  0x44,   /* Oi2 Flags:  has return, has ext, */
      0x1,    /* 1 */
/* 100 */ 0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 102 */ NdrFcShort( 0x0 ),  /* 0 */
/* 104 */ NdrFcShort( 0x0 ),  /* 0 */
/* 106 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Return value */

/* 108 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 110 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 112 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure SetRect */

/* 114 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 116 */ NdrFcLong( 0x0 ), /* 0 */
/* 120 */ NdrFcShort( 0xa ),  /* 10 */
/* 122 */ NdrFcShort( 0x18 ), /* x86 Stack size/offset = 24 */
/* 124 */ NdrFcShort( 0x20 ), /* 32 */
/* 126 */ NdrFcShort( 0x8 ),  /* 8 */
/* 128 */ 0x44,   /* Oi2 Flags:  has return, has ext, */
      0x5,    /* 5 */
/* 130 */ 0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 132 */ NdrFcShort( 0x0 ),  /* 0 */
/* 134 */ NdrFcShort( 0x0 ),  /* 0 */
/* 136 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter left */

/* 138 */ NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 140 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 142 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter top */

/* 144 */ NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 146 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 148 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter right */

/* 150 */ NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 152 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 154 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter bottom */

/* 156 */ NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 158 */ NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 160 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Return value */

/* 162 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 164 */ NdrFcShort( 0x14 ), /* x86 Stack size/offset = 20 */
/* 166 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure putref_HostApp */

/* 168 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 170 */ NdrFcLong( 0x0 ), /* 0 */
/* 174 */ NdrFcShort( 0xb ),  /* 11 */
/* 176 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 178 */ NdrFcShort( 0x0 ),  /* 0 */
/* 180 */ NdrFcShort( 0x8 ),  /* 8 */
/* 182 */ 0x46,   /* Oi2 Flags:  clt must size, has return, has ext, */
      0x2,    /* 2 */
/* 184 */ 0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 186 */ NdrFcShort( 0x0 ),  /* 0 */
/* 188 */ NdrFcShort( 0x0 ),  /* 0 */
/* 190 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter app */

/* 192 */ NdrFcShort( 0xb ),  /* Flags:  must size, must free, in, */
/* 194 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 196 */ NdrFcShort( 0x6 ),  /* Type Offset=6 */

  /* Return value */

/* 198 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 200 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 202 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure ExcuteJS */

/* 204 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 206 */ NdrFcLong( 0x0 ), /* 0 */
/* 210 */ NdrFcShort( 0xc ),  /* 12 */
/* 212 */ NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 214 */ NdrFcShort( 0x0 ),  /* 0 */
/* 216 */ NdrFcShort( 0x8 ),  /* 8 */
/* 218 */ 0x47,   /* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
      0x3,    /* 3 */
/* 220 */ 0x8,    /* 8 */
      0x7,    /* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 222 */ NdrFcShort( 0x1 ),  /* 1 */
/* 224 */ NdrFcShort( 0x1 ),  /* 1 */
/* 226 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter src */

/* 228 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 230 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 232 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter result */

/* 234 */ NdrFcShort( 0x11b ),  /* Flags:  must size, must free, in, out, simple ref, */
/* 236 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 238 */ NdrFcShort( 0x416 ),  /* Type Offset=1046 */

  /* Return value */

/* 240 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 242 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 244 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure Show */

/* 246 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 248 */ NdrFcLong( 0x0 ), /* 0 */
/* 252 */ NdrFcShort( 0xd ),  /* 13 */
/* 254 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 256 */ NdrFcShort( 0x6 ),  /* 6 */
/* 258 */ NdrFcShort( 0x8 ),  /* 8 */
/* 260 */ 0x44,   /* Oi2 Flags:  has return, has ext, */
      0x2,    /* 2 */
/* 262 */ 0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 264 */ NdrFcShort( 0x0 ),  /* 0 */
/* 266 */ NdrFcShort( 0x0 ),  /* 0 */
/* 268 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter bShow */

/* 270 */ NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 272 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 274 */ 0x6,    /* FC_SHORT */
      0x0,    /* 0 */

  /* Return value */

/* 276 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 278 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 280 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure GetDocumentMode */

/* 282 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 284 */ NdrFcLong( 0x0 ), /* 0 */
/* 288 */ NdrFcShort( 0xe ),  /* 14 */
/* 290 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 292 */ NdrFcShort( 0x0 ),  /* 0 */
/* 294 */ NdrFcShort( 0x24 ), /* 36 */
/* 296 */ 0x44,   /* Oi2 Flags:  has return, has ext, */
      0x2,    /* 2 */
/* 298 */ 0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 300 */ NdrFcShort( 0x0 ),  /* 0 */
/* 302 */ NdrFcShort( 0x0 ),  /* 0 */
/* 304 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter mode */

/* 306 */ NdrFcShort( 0x2150 ), /* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 308 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 310 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Return value */

/* 312 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 314 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 316 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure SetDocumentMode */

/* 318 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 320 */ NdrFcLong( 0x0 ), /* 0 */
/* 324 */ NdrFcShort( 0xf ),  /* 15 */
/* 326 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 328 */ NdrFcShort( 0x8 ),  /* 8 */
/* 330 */ NdrFcShort( 0x8 ),  /* 8 */
/* 332 */ 0x44,   /* Oi2 Flags:  has return, has ext, */
      0x2,    /* 2 */
/* 334 */ 0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 336 */ NdrFcShort( 0x0 ),  /* 0 */
/* 338 */ NdrFcShort( 0x0 ),  /* 0 */
/* 340 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter mode */

/* 342 */ NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 344 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 346 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Return value */

/* 348 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 350 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 352 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure SetCookie */

/* 354 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 356 */ NdrFcLong( 0x0 ), /* 0 */
/* 360 */ NdrFcShort( 0x10 ), /* 16 */
/* 362 */ NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 364 */ NdrFcShort( 0x8 ),  /* 8 */
/* 366 */ NdrFcShort( 0x8 ),  /* 8 */
/* 368 */ 0x46,   /* Oi2 Flags:  clt must size, has return, has ext, */
      0x3,    /* 3 */
/* 370 */ 0x8,    /* 8 */
      0x5,    /* Ext Flags:  new corr desc, srv corr check, */
/* 372 */ NdrFcShort( 0x0 ),  /* 0 */
/* 374 */ NdrFcShort( 0x1 ),  /* 1 */
/* 376 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter json */

/* 378 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 380 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 382 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter flag */

/* 384 */ NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 386 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 388 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Return value */

/* 390 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 392 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 394 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure SetUserAgent */

/* 396 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 398 */ NdrFcLong( 0x0 ), /* 0 */
/* 402 */ NdrFcShort( 0x11 ), /* 17 */
/* 404 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 406 */ NdrFcShort( 0x0 ),  /* 0 */
/* 408 */ NdrFcShort( 0x8 ),  /* 8 */
/* 410 */ 0x46,   /* Oi2 Flags:  clt must size, has return, has ext, */
      0x2,    /* 2 */
/* 412 */ 0x8,    /* 8 */
      0x5,    /* Ext Flags:  new corr desc, srv corr check, */
/* 414 */ NdrFcShort( 0x0 ),  /* 0 */
/* 416 */ NdrFcShort( 0x1 ),  /* 1 */
/* 418 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter usStr */

/* 420 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 422 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 424 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Return value */

/* 426 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 428 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 430 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure FinishDownload */

/* 432 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 434 */ NdrFcLong( 0x0 ), /* 0 */
/* 438 */ NdrFcShort( 0x12 ), /* 18 */
/* 440 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 442 */ NdrFcShort( 0x6 ),  /* 6 */
/* 444 */ NdrFcShort( 0x8 ),  /* 8 */
/* 446 */ 0x44,   /* Oi2 Flags:  has return, has ext, */
      0x2,    /* 2 */
/* 448 */ 0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 450 */ NdrFcShort( 0x0 ),  /* 0 */
/* 452 */ NdrFcShort( 0x0 ),  /* 0 */
/* 454 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter isCancel */

/* 456 */ NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 458 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 460 */ 0x6,    /* FC_SHORT */
      0x0,    /* 0 */

  /* Return value */

/* 462 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 464 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 466 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure SetFunctionControl */

/* 468 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 470 */ NdrFcLong( 0x0 ), /* 0 */
/* 474 */ NdrFcShort( 0x13 ), /* 19 */
/* 476 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 478 */ NdrFcShort( 0x0 ),  /* 0 */
/* 480 */ NdrFcShort( 0x8 ),  /* 8 */
/* 482 */ 0x46,   /* Oi2 Flags:  clt must size, has return, has ext, */
      0x2,    /* 2 */
/* 484 */ 0x8,    /* 8 */
      0x5,    /* Ext Flags:  new corr desc, srv corr check, */
/* 486 */ NdrFcShort( 0x0 ),  /* 0 */
/* 488 */ NdrFcShort( 0x1 ),  /* 1 */
/* 490 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter jsonStr */

/* 492 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 494 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 496 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Return value */

/* 498 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 500 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 502 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure AutoLogin */

/* 504 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 506 */ NdrFcLong( 0x0 ), /* 0 */
/* 510 */ NdrFcShort( 0x14 ), /* 20 */
/* 512 */ NdrFcShort( 0x20 ), /* x86 Stack size/offset = 32 */
/* 514 */ NdrFcShort( 0x0 ),  /* 0 */
/* 516 */ NdrFcShort( 0x8 ),  /* 8 */
/* 518 */ 0x46,   /* Oi2 Flags:  clt must size, has return, has ext, */
      0x7,    /* 7 */
/* 520 */ 0x8,    /* 8 */
      0x5,    /* Ext Flags:  new corr desc, srv corr check, */
/* 522 */ NdrFcShort( 0x0 ),  /* 0 */
/* 524 */ NdrFcShort( 0x1 ),  /* 1 */
/* 526 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter frameXPath */

/* 528 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 530 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 532 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter userNameXPath */

/* 534 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 536 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 538 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter userNameValue */

/* 540 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 542 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 544 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter pwdXPath */

/* 546 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 548 */ NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 550 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter pwdValue */

/* 552 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 554 */ NdrFcShort( 0x14 ), /* x86 Stack size/offset = 20 */
/* 556 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter loginBtnXPath */

/* 558 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 560 */ NdrFcShort( 0x18 ), /* x86 Stack size/offset = 24 */
/* 562 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Return value */

/* 564 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 566 */ NdrFcShort( 0x1c ), /* x86 Stack size/offset = 28 */
/* 568 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure SetListenLoginXPath */

/* 570 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 572 */ NdrFcLong( 0x0 ), /* 0 */
/* 576 */ NdrFcShort( 0x15 ), /* 21 */
/* 578 */ NdrFcShort( 0x1c ), /* x86 Stack size/offset = 28 */
/* 580 */ NdrFcShort( 0x0 ),  /* 0 */
/* 582 */ NdrFcShort( 0x8 ),  /* 8 */
/* 584 */ 0x46,   /* Oi2 Flags:  clt must size, has return, has ext, */
      0x6,    /* 6 */
/* 586 */ 0x8,    /* 8 */
      0x5,    /* Ext Flags:  new corr desc, srv corr check, */
/* 588 */ NdrFcShort( 0x0 ),  /* 0 */
/* 590 */ NdrFcShort( 0x1 ),  /* 1 */
/* 592 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter url */

/* 594 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 596 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 598 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter frameXPath */

/* 600 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 602 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 604 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter userNameXPath */

/* 606 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 608 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 610 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter pwdXPath */

/* 612 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 614 */ NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 616 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter loginbtnXPath */

/* 618 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 620 */ NdrFcShort( 0x14 ), /* x86 Stack size/offset = 20 */
/* 622 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Return value */

/* 624 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 626 */ NdrFcShort( 0x18 ), /* x86 Stack size/offset = 24 */
/* 628 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure NotifyCommand */

/* 630 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 632 */ NdrFcLong( 0x0 ), /* 0 */
/* 636 */ NdrFcShort( 0x16 ), /* 22 */
/* 638 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 640 */ NdrFcShort( 0x0 ),  /* 0 */
/* 642 */ NdrFcShort( 0x8 ),  /* 8 */
/* 644 */ 0x46,   /* Oi2 Flags:  clt must size, has return, has ext, */
      0x2,    /* 2 */
/* 646 */ 0x8,    /* 8 */
      0x5,    /* Ext Flags:  new corr desc, srv corr check, */
/* 648 */ NdrFcShort( 0x0 ),  /* 0 */
/* 650 */ NdrFcShort( 0x1 ),  /* 1 */
/* 652 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter command */

/* 654 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 656 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 658 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Return value */

/* 660 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 662 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 664 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure GetDevToolsOpenStatus */

/* 666 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 668 */ NdrFcLong( 0x0 ), /* 0 */
/* 672 */ NdrFcShort( 0x17 ), /* 23 */
/* 674 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 676 */ NdrFcShort( 0x1a ), /* 26 */
/* 678 */ NdrFcShort( 0x22 ), /* 34 */
/* 680 */ 0x44,   /* Oi2 Flags:  has return, has ext, */
      0x2,    /* 2 */
/* 682 */ 0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 684 */ NdrFcShort( 0x0 ),  /* 0 */
/* 686 */ NdrFcShort( 0x0 ),  /* 0 */
/* 688 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter isOpen */

/* 690 */ NdrFcShort( 0x158 ),  /* Flags:  in, out, base type, simple ref, */
/* 692 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 694 */ 0x6,    /* FC_SHORT */
      0x0,    /* 0 */

  /* Return value */

/* 696 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 698 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 700 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure ShowDevTools */

/* 702 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 704 */ NdrFcLong( 0x0 ), /* 0 */
/* 708 */ NdrFcShort( 0x18 ), /* 24 */
/* 710 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 712 */ NdrFcShort( 0x6 ),  /* 6 */
/* 714 */ NdrFcShort( 0x8 ),  /* 8 */
/* 716 */ 0x44,   /* Oi2 Flags:  has return, has ext, */
      0x2,    /* 2 */
/* 718 */ 0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 720 */ NdrFcShort( 0x0 ),  /* 0 */
/* 722 */ NdrFcShort( 0x0 ),  /* 0 */
/* 724 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter show */

/* 726 */ NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 728 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 730 */ 0x6,    /* FC_SHORT */
      0x0,    /* 0 */

  /* Return value */

/* 732 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 734 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 736 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnBeforeNavigate */

/* 738 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 740 */ NdrFcLong( 0x0 ), /* 0 */
/* 744 */ NdrFcShort( 0x7 ),  /* 7 */
/* 746 */ NdrFcShort( 0x14 ), /* x86 Stack size/offset = 20 */
/* 748 */ NdrFcShort( 0x20 ), /* 32 */
/* 750 */ NdrFcShort( 0x22 ), /* 34 */
/* 752 */ 0x46,   /* Oi2 Flags:  clt must size, has return, has ext, */
      0x4,    /* 4 */
/* 754 */ 0x8,    /* 8 */
      0x5,    /* Ext Flags:  new corr desc, srv corr check, */
/* 756 */ NdrFcShort( 0x0 ),  /* 0 */
/* 758 */ NdrFcShort( 0x1 ),  /* 1 */
/* 760 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter url */

/* 762 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 764 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 766 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter navNewUrl */

/* 768 */ NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 770 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 772 */ 0x6,    /* FC_SHORT */
      0x0,    /* 0 */

  /* Parameter cancel */

/* 774 */ NdrFcShort( 0x158 ),  /* Flags:  in, out, base type, simple ref, */
/* 776 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 778 */ 0x6,    /* FC_SHORT */
      0x0,    /* 0 */

  /* Return value */

/* 780 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 782 */ NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 784 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnFinishNavigate */

/* 786 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 788 */ NdrFcLong( 0x0 ), /* 0 */
/* 792 */ NdrFcShort( 0x8 ),  /* 8 */
/* 794 */ NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 796 */ NdrFcShort( 0x0 ),  /* 0 */
/* 798 */ NdrFcShort( 0x8 ),  /* 8 */
/* 800 */ 0x46,   /* Oi2 Flags:  clt must size, has return, has ext, */
      0x3,    /* 3 */
/* 802 */ 0x8,    /* 8 */
      0x5,    /* Ext Flags:  new corr desc, srv corr check, */
/* 804 */ NdrFcShort( 0x0 ),  /* 0 */
/* 806 */ NdrFcShort( 0x1 ),  /* 1 */
/* 808 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter url */

/* 810 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 812 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 814 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter faviconUrls */

/* 816 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 818 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 820 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Return value */

/* 822 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 824 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 826 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnTitleChange */

/* 828 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 830 */ NdrFcLong( 0x0 ), /* 0 */
/* 834 */ NdrFcShort( 0x9 ),  /* 9 */
/* 836 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 838 */ NdrFcShort( 0x0 ),  /* 0 */
/* 840 */ NdrFcShort( 0x8 ),  /* 8 */
/* 842 */ 0x46,   /* Oi2 Flags:  clt must size, has return, has ext, */
      0x2,    /* 2 */
/* 844 */ 0x8,    /* 8 */
      0x5,    /* Ext Flags:  new corr desc, srv corr check, */
/* 846 */ NdrFcShort( 0x0 ),  /* 0 */
/* 848 */ NdrFcShort( 0x1 ),  /* 1 */
/* 850 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter title */

/* 852 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 854 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 856 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Return value */

/* 858 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 860 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 862 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnLoadUrlNewWnd */

/* 864 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 866 */ NdrFcLong( 0x0 ), /* 0 */
/* 870 */ NdrFcShort( 0xa ),  /* 10 */
/* 872 */ NdrFcShort( 0x18 ), /* x86 Stack size/offset = 24 */
/* 874 */ NdrFcShort( 0x22 ), /* 34 */
/* 876 */ NdrFcShort( 0x22 ), /* 34 */
/* 878 */ 0x47,   /* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
      0x5,    /* 5 */
/* 880 */ 0x8,    /* 8 */
      0x5,    /* Ext Flags:  new corr desc, srv corr check, */
/* 882 */ NdrFcShort( 0x0 ),  /* 0 */
/* 884 */ NdrFcShort( 0x1 ),  /* 1 */
/* 886 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter url */

/* 888 */ NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 890 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 892 */ NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter flag */

/* 894 */ NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 896 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 898 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter cancel */

/* 900 */ NdrFcShort( 0x158 ),  /* Flags:  in, out, base type, simple ref, */
/* 902 */ NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 904 */ 0x6,    /* FC_SHORT */
      0x0,    /* 0 */

  /* Parameter pDisp */

/* 906 */ NdrFcShort( 0x13 ), /* Flags:  must size, must free, out, */
/* 908 */ NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 910 */ NdrFcShort( 0x2 ),  /* Type Offset=2 */

  /* Return value */

/* 912 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 914 */ NdrFcShort( 0x14 ), /* x86 Stack size/offset = 20 */
/* 916 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnBrowserClosing */

/* 918 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 920 */ NdrFcLong( 0x0 ), /* 0 */
/* 924 */ NdrFcShort( 0xb ),  /* 11 */
/* 926 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 928 */ NdrFcShort( 0x0 ),  /* 0 */
/* 930 */ NdrFcShort( 0x8 ),  /* 8 */
/* 932 */ 0x44,   /* Oi2 Flags:  has return, has ext, */
      0x1,    /* 1 */
/* 934 */ 0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 936 */ NdrFcShort( 0x0 ),  /* 0 */
/* 938 */ NdrFcShort( 0x0 ),  /* 0 */
/* 940 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Return value */

/* 942 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 944 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 946 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnBeforeBrowserClose */

/* 948 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 950 */ NdrFcLong( 0x0 ), /* 0 */
/* 954 */ NdrFcShort( 0xc ),  /* 12 */
/* 956 */ NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 958 */ NdrFcShort( 0x0 ),  /* 0 */
/* 960 */ NdrFcShort( 0x8 ),  /* 8 */
/* 962 */ 0x44,   /* Oi2 Flags:  has return, has ext, */
      0x1,    /* 1 */
/* 964 */ 0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 966 */ NdrFcShort( 0x0 ),  /* 0 */
/* 968 */ NdrFcShort( 0x0 ),  /* 0 */
/* 970 */ NdrFcShort( 0x0 ),  /* 0 */

  /* Return value */

/* 972 */ NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 974 */ NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 976 */ 0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnShowContextMenu */

/* 978 */ 0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 980 */ NdrFcLong( 0x0 ), /* 0 */
/* 984 */ NdrFcShort( 0xd ),  /* 13 */
/* 986 */ NdrFcShort( 0x14 ), /* x86 Stack size/offset = 20 */
/* 988 */ NdrFcShort( 0x18 ), /* 24 */
/* 990 */ NdrFcShort( 0x8 ),  /* 8 */
/* 992 */ 0x44,   /* Oi2 Flags:  has return, has ext, */
      0x4,    /* 4 */
/* 994 */ 0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 996 */ NdrFcShort( 0x0 ),  /* 0 */
/* 998 */ NdrFcShort( 0x0 ),  /* 0 */
/* 1000 */  NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter dwID */

/* 1002 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 1004 */  NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 1006 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter x */

/* 1008 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 1010 */  NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 1012 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter y */

/* 1014 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 1016 */  NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 1018 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Return value */

/* 1020 */  NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 1022 */  NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 1024 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnIEServerWndCreated */

/* 1026 */  0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 1028 */  NdrFcLong( 0x0 ), /* 0 */
/* 1032 */  NdrFcShort( 0xe ),  /* 14 */
/* 1034 */  NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 1036 */  NdrFcShort( 0x8 ),  /* 8 */
/* 1038 */  NdrFcShort( 0x8 ),  /* 8 */
/* 1040 */  0x44,   /* Oi2 Flags:  has return, has ext, */
      0x2,    /* 2 */
/* 1042 */  0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 1044 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1046 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1048 */  NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter hwnd */

/* 1050 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 1052 */  NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 1054 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Return value */

/* 1056 */  NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 1058 */  NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 1060 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnDownLoadFile */

/* 1062 */  0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 1064 */  NdrFcLong( 0x0 ), /* 0 */
/* 1068 */  NdrFcShort( 0xf ),  /* 15 */
/* 1070 */  NdrFcShort( 0x14 ), /* x86 Stack size/offset = 20 */
/* 1072 */  NdrFcShort( 0x8 ),  /* 8 */
/* 1074 */  NdrFcShort( 0x8 ),  /* 8 */
/* 1076 */  0x46,   /* Oi2 Flags:  clt must size, has return, has ext, */
      0x4,    /* 4 */
/* 1078 */  0x8,    /* 8 */
      0x5,    /* Ext Flags:  new corr desc, srv corr check, */
/* 1080 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1082 */  NdrFcShort( 0x1 ),  /* 1 */
/* 1084 */  NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter url */

/* 1086 */  NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 1088 */  NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 1090 */  NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter status */

/* 1092 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 1094 */  NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 1096 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter variant */

/* 1098 */  NdrFcShort( 0x10b ),  /* Flags:  must size, must free, in, simple ref, */
/* 1100 */  NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 1102 */  NdrFcShort( 0x430 ),  /* Type Offset=1072 */

  /* Return value */

/* 1104 */  NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 1106 */  NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 1108 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnCanGoBackOrForward */

/* 1110 */  0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 1112 */  NdrFcLong( 0x0 ), /* 0 */
/* 1116 */  NdrFcShort( 0x10 ), /* 16 */
/* 1118 */  NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 1120 */  NdrFcShort( 0x34 ), /* 52 */
/* 1122 */  NdrFcShort( 0x3c ), /* 60 */
/* 1124 */  0x44,   /* Oi2 Flags:  has return, has ext, */
      0x3,    /* 3 */
/* 1126 */  0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 1128 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1130 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1132 */  NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter back */

/* 1134 */  NdrFcShort( 0x158 ),  /* Flags:  in, out, base type, simple ref, */
/* 1136 */  NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 1138 */  0x6,    /* FC_SHORT */
      0x0,    /* 0 */

  /* Parameter forward */

/* 1140 */  NdrFcShort( 0x158 ),  /* Flags:  in, out, base type, simple ref, */
/* 1142 */  NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 1144 */  0x6,    /* FC_SHORT */
      0x0,    /* 0 */

  /* Return value */

/* 1146 */  NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 1148 */  NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 1150 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnGoBack */

/* 1152 */  0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 1154 */  NdrFcLong( 0x0 ), /* 0 */
/* 1158 */  NdrFcShort( 0x11 ), /* 17 */
/* 1160 */  NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 1162 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1164 */  NdrFcShort( 0x8 ),  /* 8 */
/* 1166 */  0x44,   /* Oi2 Flags:  has return, has ext, */
      0x1,    /* 1 */
/* 1168 */  0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 1170 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1172 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1174 */  NdrFcShort( 0x0 ),  /* 0 */

  /* Return value */

/* 1176 */  NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 1178 */  NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 1180 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnGoForward */

/* 1182 */  0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 1184 */  NdrFcLong( 0x0 ), /* 0 */
/* 1188 */  NdrFcShort( 0x12 ), /* 18 */
/* 1190 */  NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 1192 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1194 */  NdrFcShort( 0x8 ),  /* 8 */
/* 1196 */  0x44,   /* Oi2 Flags:  has return, has ext, */
      0x1,    /* 1 */
/* 1198 */  0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 1200 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1202 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1204 */  NdrFcShort( 0x0 ),  /* 0 */

  /* Return value */

/* 1206 */  NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 1208 */  NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 1210 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnRequestChangePos */

/* 1212 */  0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 1214 */  NdrFcLong( 0x0 ), /* 0 */
/* 1218 */  NdrFcShort( 0x13 ), /* 19 */
/* 1220 */  NdrFcShort( 0x18 ), /* x86 Stack size/offset = 24 */
/* 1222 */  NdrFcShort( 0x20 ), /* 32 */
/* 1224 */  NdrFcShort( 0x8 ),  /* 8 */
/* 1226 */  0x44,   /* Oi2 Flags:  has return, has ext, */
      0x5,    /* 5 */
/* 1228 */  0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 1230 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1232 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1234 */  NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter left */

/* 1236 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 1238 */  NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 1240 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter top */

/* 1242 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 1244 */  NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 1246 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter width */

/* 1248 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 1250 */  NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 1252 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter height */

/* 1254 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 1256 */  NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 1258 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Return value */

/* 1260 */  NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 1262 */  NdrFcShort( 0x14 ), /* x86 Stack size/offset = 20 */
/* 1264 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnGetMainWndPos */

/* 1266 */  0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 1268 */  NdrFcLong( 0x0 ), /* 0 */
/* 1272 */  NdrFcShort( 0x14 ), /* 20 */
/* 1274 */  NdrFcShort( 0x18 ), /* x86 Stack size/offset = 24 */
/* 1276 */  NdrFcShort( 0x70 ), /* 112 */
/* 1278 */  NdrFcShort( 0x78 ), /* 120 */
/* 1280 */  0x44,   /* Oi2 Flags:  has return, has ext, */
      0x5,    /* 5 */
/* 1282 */  0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 1284 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1286 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1288 */  NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter left */

/* 1290 */  NdrFcShort( 0x158 ),  /* Flags:  in, out, base type, simple ref, */
/* 1292 */  NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 1294 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter top */

/* 1296 */  NdrFcShort( 0x158 ),  /* Flags:  in, out, base type, simple ref, */
/* 1298 */  NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 1300 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter width */

/* 1302 */  NdrFcShort( 0x158 ),  /* Flags:  in, out, base type, simple ref, */
/* 1304 */  NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 1306 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter height */

/* 1308 */  NdrFcShort( 0x158 ),  /* Flags:  in, out, base type, simple ref, */
/* 1310 */  NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 1312 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Return value */

/* 1314 */  NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 1316 */  NdrFcShort( 0x14 ), /* x86 Stack size/offset = 20 */
/* 1318 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnUpdateCookie */

/* 1320 */  0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 1322 */  NdrFcLong( 0x0 ), /* 0 */
/* 1326 */  NdrFcShort( 0x15 ), /* 21 */
/* 1328 */  NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 1330 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1332 */  NdrFcShort( 0x8 ),  /* 8 */
/* 1334 */  0x46,   /* Oi2 Flags:  clt must size, has return, has ext, */
      0x2,    /* 2 */
/* 1336 */  0x8,    /* 8 */
      0x5,    /* Ext Flags:  new corr desc, srv corr check, */
/* 1338 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1340 */  NdrFcShort( 0x1 ),  /* 1 */
/* 1342 */  NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter cookie */

/* 1344 */  NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 1346 */  NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 1348 */  NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Return value */

/* 1350 */  NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 1352 */  NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 1354 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnGetLoginInfo */

/* 1356 */  0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 1358 */  NdrFcLong( 0x0 ), /* 0 */
/* 1362 */  NdrFcShort( 0x16 ), /* 22 */
/* 1364 */  NdrFcShort( 0x14 ), /* x86 Stack size/offset = 20 */
/* 1366 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1368 */  NdrFcShort( 0x8 ),  /* 8 */
/* 1370 */  0x46,   /* Oi2 Flags:  clt must size, has return, has ext, */
      0x4,    /* 4 */
/* 1372 */  0x8,    /* 8 */
      0x5,    /* Ext Flags:  new corr desc, srv corr check, */
/* 1374 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1376 */  NdrFcShort( 0x1 ),  /* 1 */
/* 1378 */  NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter url */

/* 1380 */  NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 1382 */  NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 1384 */  NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter userName */

/* 1386 */  NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 1388 */  NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 1390 */  NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Parameter userPwd */

/* 1392 */  NdrFcShort( 0x8b ), /* Flags:  must size, must free, in, by val, */
/* 1394 */  NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 1396 */  NdrFcShort( 0x32 ), /* Type Offset=50 */

  /* Return value */

/* 1398 */  NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 1400 */  NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 1402 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnRequestAcceleratorFromKeyboard */

/* 1404 */  0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 1406 */  NdrFcLong( 0x0 ), /* 0 */
/* 1410 */  NdrFcShort( 0x17 ), /* 23 */
/* 1412 */  NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 1414 */  NdrFcShort( 0x10 ), /* 16 */
/* 1416 */  NdrFcShort( 0x8 ),  /* 8 */
/* 1418 */  0x44,   /* Oi2 Flags:  has return, has ext, */
      0x3,    /* 3 */
/* 1420 */  0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 1422 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1424 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1426 */  NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter keyCode */

/* 1428 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 1430 */  NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 1432 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter modifiers */

/* 1434 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 1436 */  NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 1438 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Return value */

/* 1440 */  NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 1442 */  NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 1444 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnRequestAcceleratorFromMouseWheel */

/* 1446 */  0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 1448 */  NdrFcLong( 0x0 ), /* 0 */
/* 1452 */  NdrFcShort( 0x18 ), /* 24 */
/* 1454 */  NdrFcShort( 0x18 ), /* x86 Stack size/offset = 24 */
/* 1456 */  NdrFcShort( 0x20 ), /* 32 */
/* 1458 */  NdrFcShort( 0x8 ),  /* 8 */
/* 1460 */  0x44,   /* Oi2 Flags:  has return, has ext, */
      0x5,    /* 5 */
/* 1462 */  0x8,    /* 8 */
      0x1,    /* Ext Flags:  new corr desc, */
/* 1464 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1466 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1468 */  NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter delta */

/* 1470 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 1472 */  NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 1474 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter Pt_x */

/* 1476 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 1478 */  NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 1480 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter Pt_y */

/* 1482 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 1484 */  NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 1486 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Parameter modifiers */

/* 1488 */  NdrFcShort( 0x48 ), /* Flags:  in, base type, */
/* 1490 */  NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 1492 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Return value */

/* 1494 */  NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 1496 */  NdrFcShort( 0x14 ), /* x86 Stack size/offset = 20 */
/* 1498 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

  /* Procedure OnWinDomainAuthenticate */

/* 1500 */  0x33,   /* FC_AUTO_HANDLE */
      0x6c,   /* Old Flags:  object, Oi2 */
/* 1502 */  NdrFcLong( 0x0 ), /* 0 */
/* 1506 */  NdrFcShort( 0x19 ), /* 25 */
/* 1508 */  NdrFcShort( 0x10 ), /* x86 Stack size/offset = 16 */
/* 1510 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1512 */  NdrFcShort( 0x8 ),  /* 8 */
/* 1514 */  0x47,   /* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
      0x3,    /* 3 */
/* 1516 */  0x8,    /* 8 */
      0x7,    /* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 1518 */  NdrFcShort( 0x1 ),  /* 1 */
/* 1520 */  NdrFcShort( 0x1 ),  /* 1 */
/* 1522 */  NdrFcShort( 0x0 ),  /* 0 */

  /* Parameter userName */

/* 1524 */  NdrFcShort( 0x11b ),  /* Flags:  must size, must free, in, out, simple ref, */
/* 1526 */  NdrFcShort( 0x4 ),  /* x86 Stack size/offset = 4 */
/* 1528 */  NdrFcShort( 0x442 ),  /* Type Offset=1090 */

  /* Parameter userPwd */

/* 1530 */  NdrFcShort( 0x11b ),  /* Flags:  must size, must free, in, out, simple ref, */
/* 1532 */  NdrFcShort( 0x8 ),  /* x86 Stack size/offset = 8 */
/* 1534 */  NdrFcShort( 0x442 ),  /* Type Offset=1090 */

  /* Return value */

/* 1536 */  NdrFcShort( 0x70 ), /* Flags:  out, return, base type, */
/* 1538 */  NdrFcShort( 0xc ),  /* x86 Stack size/offset = 12 */
/* 1540 */  0x8,    /* FC_LONG */
      0x0,    /* 0 */

      0x0
        }
    };

static const IEInterface_MIDL_TYPE_FORMAT_STRING IEInterface__MIDL_TypeFormatString =
    {
        0,
        {
      NdrFcShort( 0x0 ),  /* 0 */
/*  2 */
      0x11, 0x10, /* FC_RP [pointer_deref] */
/*  4 */  NdrFcShort( 0x2 ),  /* Offset= 2 (6) */
/*  6 */
      0x2f,   /* FC_IP */
      0x5a,   /* FC_CONSTANT_IID */
/*  8 */  NdrFcLong( 0x20400 ), /* 132096 */
/* 12 */  NdrFcShort( 0x0 ),  /* 0 */
/* 14 */  NdrFcShort( 0x0 ),  /* 0 */
/* 16 */  0xc0,   /* 192 */
      0x0,    /* 0 */
/* 18 */  0x0,    /* 0 */
      0x0,    /* 0 */
/* 20 */  0x0,    /* 0 */
      0x0,    /* 0 */
/* 22 */  0x0,    /* 0 */
      0x46,   /* 70 */
/* 24 */
      0x12, 0x0,  /* FC_UP */
/* 26 */  NdrFcShort( 0xe ),  /* Offset= 14 (40) */
/* 28 */
      0x1b,   /* FC_CARRAY */
      0x1,    /* 1 */
/* 30 */  NdrFcShort( 0x2 ),  /* 2 */
/* 32 */  0x9,    /* Corr desc: FC_ULONG */
      0x0,    /*  */
/* 34 */  NdrFcShort( 0xfffc ), /* -4 */
/* 36 */  NdrFcShort( 0x1 ),  /* Corr flags:  early, */
/* 38 */  0x6,    /* FC_SHORT */
      0x5b,   /* FC_END */
/* 40 */
      0x17,   /* FC_CSTRUCT */
      0x3,    /* 3 */
/* 42 */  NdrFcShort( 0x8 ),  /* 8 */
/* 44 */  NdrFcShort( 0xfff0 ), /* Offset= -16 (28) */
/* 46 */  0x8,    /* FC_LONG */
      0x8,    /* FC_LONG */
/* 48 */  0x5c,   /* FC_PAD */
      0x5b,   /* FC_END */
/* 50 */  0xb4,   /* FC_USER_MARSHAL */
      0x83,   /* 131 */
/* 52 */  NdrFcShort( 0x0 ),  /* 0 */
/* 54 */  NdrFcShort( 0x4 ),  /* 4 */
/* 56 */  NdrFcShort( 0x0 ),  /* 0 */
/* 58 */  NdrFcShort( 0xffde ), /* Offset= -34 (24) */
/* 60 */
      0x11, 0x0,  /* FC_RP */
/* 62 */  NdrFcShort( 0x3d8 ),  /* Offset= 984 (1046) */
/* 64 */
      0x13, 0x0,  /* FC_OP */
/* 66 */  NdrFcShort( 0x3c0 ),  /* Offset= 960 (1026) */
/* 68 */
      0x2b,   /* FC_NON_ENCAPSULATED_UNION */
      0x9,    /* FC_ULONG */
/* 70 */  0x7,    /* Corr desc: FC_USHORT */
      0x0,    /*  */
/* 72 */  NdrFcShort( 0xfff8 ), /* -8 */
/* 74 */  NdrFcShort( 0x1 ),  /* Corr flags:  early, */
/* 76 */  NdrFcShort( 0x2 ),  /* Offset= 2 (78) */
/* 78 */  NdrFcShort( 0x10 ), /* 16 */
/* 80 */  NdrFcShort( 0x2f ), /* 47 */
/* 82 */  NdrFcLong( 0x14 ),  /* 20 */
/* 86 */  NdrFcShort( 0x800b ), /* Simple arm type: FC_HYPER */
/* 88 */  NdrFcLong( 0x3 ), /* 3 */
/* 92 */  NdrFcShort( 0x8008 ), /* Simple arm type: FC_LONG */
/* 94 */  NdrFcLong( 0x11 ),  /* 17 */
/* 98 */  NdrFcShort( 0x8001 ), /* Simple arm type: FC_BYTE */
/* 100 */ NdrFcLong( 0x2 ), /* 2 */
/* 104 */ NdrFcShort( 0x8006 ), /* Simple arm type: FC_SHORT */
/* 106 */ NdrFcLong( 0x4 ), /* 4 */
/* 110 */ NdrFcShort( 0x800a ), /* Simple arm type: FC_FLOAT */
/* 112 */ NdrFcLong( 0x5 ), /* 5 */
/* 116 */ NdrFcShort( 0x800c ), /* Simple arm type: FC_DOUBLE */
/* 118 */ NdrFcLong( 0xb ), /* 11 */
/* 122 */ NdrFcShort( 0x8006 ), /* Simple arm type: FC_SHORT */
/* 124 */ NdrFcLong( 0xa ), /* 10 */
/* 128 */ NdrFcShort( 0x8008 ), /* Simple arm type: FC_LONG */
/* 130 */ NdrFcLong( 0x6 ), /* 6 */
/* 134 */ NdrFcShort( 0xe8 ), /* Offset= 232 (366) */
/* 136 */ NdrFcLong( 0x7 ), /* 7 */
/* 140 */ NdrFcShort( 0x800c ), /* Simple arm type: FC_DOUBLE */
/* 142 */ NdrFcLong( 0x8 ), /* 8 */
/* 146 */ NdrFcShort( 0xe2 ), /* Offset= 226 (372) */
/* 148 */ NdrFcLong( 0xd ), /* 13 */
/* 152 */ NdrFcShort( 0xe0 ), /* Offset= 224 (376) */
/* 154 */ NdrFcLong( 0x9 ), /* 9 */
/* 158 */ NdrFcShort( 0xff68 ), /* Offset= -152 (6) */
/* 160 */ NdrFcLong( 0x2000 ),  /* 8192 */
/* 164 */ NdrFcShort( 0xe6 ), /* Offset= 230 (394) */
/* 166 */ NdrFcLong( 0x24 ),  /* 36 */
/* 170 */ NdrFcShort( 0x30e ),  /* Offset= 782 (952) */
/* 172 */ NdrFcLong( 0x4024 ),  /* 16420 */
/* 176 */ NdrFcShort( 0x308 ),  /* Offset= 776 (952) */
/* 178 */ NdrFcLong( 0x4011 ),  /* 16401 */
/* 182 */ NdrFcShort( 0x306 ),  /* Offset= 774 (956) */
/* 184 */ NdrFcLong( 0x4002 ),  /* 16386 */
/* 188 */ NdrFcShort( 0x304 ),  /* Offset= 772 (960) */
/* 190 */ NdrFcLong( 0x4003 ),  /* 16387 */
/* 194 */ NdrFcShort( 0x302 ),  /* Offset= 770 (964) */
/* 196 */ NdrFcLong( 0x4014 ),  /* 16404 */
/* 200 */ NdrFcShort( 0x300 ),  /* Offset= 768 (968) */
/* 202 */ NdrFcLong( 0x4004 ),  /* 16388 */
/* 206 */ NdrFcShort( 0x2fe ),  /* Offset= 766 (972) */
/* 208 */ NdrFcLong( 0x4005 ),  /* 16389 */
/* 212 */ NdrFcShort( 0x2fc ),  /* Offset= 764 (976) */
/* 214 */ NdrFcLong( 0x400b ),  /* 16395 */
/* 218 */ NdrFcShort( 0x2e6 ),  /* Offset= 742 (960) */
/* 220 */ NdrFcLong( 0x400a ),  /* 16394 */
/* 224 */ NdrFcShort( 0x2e4 ),  /* Offset= 740 (964) */
/* 226 */ NdrFcLong( 0x4006 ),  /* 16390 */
/* 230 */ NdrFcShort( 0x2ee ),  /* Offset= 750 (980) */
/* 232 */ NdrFcLong( 0x4007 ),  /* 16391 */
/* 236 */ NdrFcShort( 0x2e4 ),  /* Offset= 740 (976) */
/* 238 */ NdrFcLong( 0x4008 ),  /* 16392 */
/* 242 */ NdrFcShort( 0x2e6 ),  /* Offset= 742 (984) */
/* 244 */ NdrFcLong( 0x400d ),  /* 16397 */
/* 248 */ NdrFcShort( 0x2e4 ),  /* Offset= 740 (988) */
/* 250 */ NdrFcLong( 0x4009 ),  /* 16393 */
/* 254 */ NdrFcShort( 0x2e2 ),  /* Offset= 738 (992) */
/* 256 */ NdrFcLong( 0x6000 ),  /* 24576 */
/* 260 */ NdrFcShort( 0x2e0 ),  /* Offset= 736 (996) */
/* 262 */ NdrFcLong( 0x400c ),  /* 16396 */
/* 266 */ NdrFcShort( 0x2de ),  /* Offset= 734 (1000) */
/* 268 */ NdrFcLong( 0x10 ),  /* 16 */
/* 272 */ NdrFcShort( 0x8002 ), /* Simple arm type: FC_CHAR */
/* 274 */ NdrFcLong( 0x12 ),  /* 18 */
/* 278 */ NdrFcShort( 0x8006 ), /* Simple arm type: FC_SHORT */
/* 280 */ NdrFcLong( 0x13 ),  /* 19 */
/* 284 */ NdrFcShort( 0x8008 ), /* Simple arm type: FC_LONG */
/* 286 */ NdrFcLong( 0x15 ),  /* 21 */
/* 290 */ NdrFcShort( 0x800b ), /* Simple arm type: FC_HYPER */
/* 292 */ NdrFcLong( 0x16 ),  /* 22 */
/* 296 */ NdrFcShort( 0x8008 ), /* Simple arm type: FC_LONG */
/* 298 */ NdrFcLong( 0x17 ),  /* 23 */
/* 302 */ NdrFcShort( 0x8008 ), /* Simple arm type: FC_LONG */
/* 304 */ NdrFcLong( 0xe ), /* 14 */
/* 308 */ NdrFcShort( 0x2bc ),  /* Offset= 700 (1008) */
/* 310 */ NdrFcLong( 0x400e ),  /* 16398 */
/* 314 */ NdrFcShort( 0x2c0 ),  /* Offset= 704 (1018) */
/* 316 */ NdrFcLong( 0x4010 ),  /* 16400 */
/* 320 */ NdrFcShort( 0x2be ),  /* Offset= 702 (1022) */
/* 322 */ NdrFcLong( 0x4012 ),  /* 16402 */
/* 326 */ NdrFcShort( 0x27a ),  /* Offset= 634 (960) */
/* 328 */ NdrFcLong( 0x4013 ),  /* 16403 */
/* 332 */ NdrFcShort( 0x278 ),  /* Offset= 632 (964) */
/* 334 */ NdrFcLong( 0x4015 ),  /* 16405 */
/* 338 */ NdrFcShort( 0x276 ),  /* Offset= 630 (968) */
/* 340 */ NdrFcLong( 0x4016 ),  /* 16406 */
/* 344 */ NdrFcShort( 0x26c ),  /* Offset= 620 (964) */
/* 346 */ NdrFcLong( 0x4017 ),  /* 16407 */
/* 350 */ NdrFcShort( 0x266 ),  /* Offset= 614 (964) */
/* 352 */ NdrFcLong( 0x0 ), /* 0 */
/* 356 */ NdrFcShort( 0x0 ),  /* Offset= 0 (356) */
/* 358 */ NdrFcLong( 0x1 ), /* 1 */
/* 362 */ NdrFcShort( 0x0 ),  /* Offset= 0 (362) */
/* 364 */ NdrFcShort( 0xffff ), /* Offset= -1 (363) */
/* 366 */
      0x15,   /* FC_STRUCT */
      0x7,    /* 7 */
/* 368 */ NdrFcShort( 0x8 ),  /* 8 */
/* 370 */ 0xb,    /* FC_HYPER */
      0x5b,   /* FC_END */
/* 372 */
      0x13, 0x0,  /* FC_OP */
/* 374 */ NdrFcShort( 0xfeb2 ), /* Offset= -334 (40) */
/* 376 */
      0x2f,   /* FC_IP */
      0x5a,   /* FC_CONSTANT_IID */
/* 378 */ NdrFcLong( 0x0 ), /* 0 */
/* 382 */ NdrFcShort( 0x0 ),  /* 0 */
/* 384 */ NdrFcShort( 0x0 ),  /* 0 */
/* 386 */ 0xc0,   /* 192 */
      0x0,    /* 0 */
/* 388 */ 0x0,    /* 0 */
      0x0,    /* 0 */
/* 390 */ 0x0,    /* 0 */
      0x0,    /* 0 */
/* 392 */ 0x0,    /* 0 */
      0x46,   /* 70 */
/* 394 */
      0x13, 0x10, /* FC_OP [pointer_deref] */
/* 396 */ NdrFcShort( 0x2 ),  /* Offset= 2 (398) */
/* 398 */
      0x13, 0x0,  /* FC_OP */
/* 400 */ NdrFcShort( 0x216 ),  /* Offset= 534 (934) */
/* 402 */
      0x2a,   /* FC_ENCAPSULATED_UNION */
      0x49,   /* 73 */
/* 404 */ NdrFcShort( 0x18 ), /* 24 */
/* 406 */ NdrFcShort( 0xa ),  /* 10 */
/* 408 */ NdrFcLong( 0x8 ), /* 8 */
/* 412 */ NdrFcShort( 0x5a ), /* Offset= 90 (502) */
/* 414 */ NdrFcLong( 0xd ), /* 13 */
/* 418 */ NdrFcShort( 0x7e ), /* Offset= 126 (544) */
/* 420 */ NdrFcLong( 0x9 ), /* 9 */
/* 424 */ NdrFcShort( 0x9e ), /* Offset= 158 (582) */
/* 426 */ NdrFcLong( 0xc ), /* 12 */
/* 430 */ NdrFcShort( 0xc8 ), /* Offset= 200 (630) */
/* 432 */ NdrFcLong( 0x24 ),  /* 36 */
/* 436 */ NdrFcShort( 0x124 ),  /* Offset= 292 (728) */
/* 438 */ NdrFcLong( 0x800d ),  /* 32781 */
/* 442 */ NdrFcShort( 0x140 ),  /* Offset= 320 (762) */
/* 444 */ NdrFcLong( 0x10 ),  /* 16 */
/* 448 */ NdrFcShort( 0x15a ),  /* Offset= 346 (794) */
/* 450 */ NdrFcLong( 0x2 ), /* 2 */
/* 454 */ NdrFcShort( 0x174 ),  /* Offset= 372 (826) */
/* 456 */ NdrFcLong( 0x3 ), /* 3 */
/* 460 */ NdrFcShort( 0x18e ),  /* Offset= 398 (858) */
/* 462 */ NdrFcLong( 0x14 ),  /* 20 */
/* 466 */ NdrFcShort( 0x1a8 ),  /* Offset= 424 (890) */
/* 468 */ NdrFcShort( 0xffff ), /* Offset= -1 (467) */
/* 470 */
      0x1b,   /* FC_CARRAY */
      0x3,    /* 3 */
/* 472 */ NdrFcShort( 0x4 ),  /* 4 */
/* 474 */ 0x19,   /* Corr desc:  field pointer, FC_ULONG */
      0x0,    /*  */
/* 476 */ NdrFcShort( 0x0 ),  /* 0 */
/* 478 */ NdrFcShort( 0x1 ),  /* Corr flags:  early, */
/* 480 */
      0x4b,   /* FC_PP */
      0x5c,   /* FC_PAD */
/* 482 */
      0x48,   /* FC_VARIABLE_REPEAT */
      0x49,   /* FC_FIXED_OFFSET */
/* 484 */ NdrFcShort( 0x4 ),  /* 4 */
/* 486 */ NdrFcShort( 0x0 ),  /* 0 */
/* 488 */ NdrFcShort( 0x1 ),  /* 1 */
/* 490 */ NdrFcShort( 0x0 ),  /* 0 */
/* 492 */ NdrFcShort( 0x0 ),  /* 0 */
/* 494 */ 0x13, 0x0,  /* FC_OP */
/* 496 */ NdrFcShort( 0xfe38 ), /* Offset= -456 (40) */
/* 498 */
      0x5b,   /* FC_END */

      0x8,    /* FC_LONG */
/* 500 */ 0x5c,   /* FC_PAD */
      0x5b,   /* FC_END */
/* 502 */
      0x16,   /* FC_PSTRUCT */
      0x3,    /* 3 */
/* 504 */ NdrFcShort( 0x8 ),  /* 8 */
/* 506 */
      0x4b,   /* FC_PP */
      0x5c,   /* FC_PAD */
/* 508 */
      0x46,   /* FC_NO_REPEAT */
      0x5c,   /* FC_PAD */
/* 510 */ NdrFcShort( 0x4 ),  /* 4 */
/* 512 */ NdrFcShort( 0x4 ),  /* 4 */
/* 514 */ 0x11, 0x0,  /* FC_RP */
/* 516 */ NdrFcShort( 0xffd2 ), /* Offset= -46 (470) */
/* 518 */
      0x5b,   /* FC_END */

      0x8,    /* FC_LONG */
/* 520 */ 0x8,    /* FC_LONG */
      0x5b,   /* FC_END */
/* 522 */
      0x21,   /* FC_BOGUS_ARRAY */
      0x3,    /* 3 */
/* 524 */ NdrFcShort( 0x0 ),  /* 0 */
/* 526 */ 0x19,   /* Corr desc:  field pointer, FC_ULONG */
      0x0,    /*  */
/* 528 */ NdrFcShort( 0x0 ),  /* 0 */
/* 530 */ NdrFcShort( 0x1 ),  /* Corr flags:  early, */
/* 532 */ NdrFcLong( 0xffffffff ),  /* -1 */
/* 536 */ NdrFcShort( 0x0 ),  /* Corr flags:  */
/* 538 */ 0x4c,   /* FC_EMBEDDED_COMPLEX */
      0x0,    /* 0 */
/* 540 */ NdrFcShort( 0xff5c ), /* Offset= -164 (376) */
/* 542 */ 0x5c,   /* FC_PAD */
      0x5b,   /* FC_END */
/* 544 */
      0x1a,   /* FC_BOGUS_STRUCT */
      0x3,    /* 3 */
/* 546 */ NdrFcShort( 0x8 ),  /* 8 */
/* 548 */ NdrFcShort( 0x0 ),  /* 0 */
/* 550 */ NdrFcShort( 0x6 ),  /* Offset= 6 (556) */
/* 552 */ 0x8,    /* FC_LONG */
      0x36,   /* FC_POINTER */
/* 554 */ 0x5c,   /* FC_PAD */
      0x5b,   /* FC_END */
/* 556 */
      0x11, 0x0,  /* FC_RP */
/* 558 */ NdrFcShort( 0xffdc ), /* Offset= -36 (522) */
/* 560 */
      0x21,   /* FC_BOGUS_ARRAY */
      0x3,    /* 3 */
/* 562 */ NdrFcShort( 0x0 ),  /* 0 */
/* 564 */ 0x19,   /* Corr desc:  field pointer, FC_ULONG */
      0x0,    /*  */
/* 566 */ NdrFcShort( 0x0 ),  /* 0 */
/* 568 */ NdrFcShort( 0x1 ),  /* Corr flags:  early, */
/* 570 */ NdrFcLong( 0xffffffff ),  /* -1 */
/* 574 */ NdrFcShort( 0x0 ),  /* Corr flags:  */
/* 576 */ 0x4c,   /* FC_EMBEDDED_COMPLEX */
      0x0,    /* 0 */
/* 578 */ NdrFcShort( 0xfdc4 ), /* Offset= -572 (6) */
/* 580 */ 0x5c,   /* FC_PAD */
      0x5b,   /* FC_END */
/* 582 */
      0x1a,   /* FC_BOGUS_STRUCT */
      0x3,    /* 3 */
/* 584 */ NdrFcShort( 0x8 ),  /* 8 */
/* 586 */ NdrFcShort( 0x0 ),  /* 0 */
/* 588 */ NdrFcShort( 0x6 ),  /* Offset= 6 (594) */
/* 590 */ 0x8,    /* FC_LONG */
      0x36,   /* FC_POINTER */
/* 592 */ 0x5c,   /* FC_PAD */
      0x5b,   /* FC_END */
/* 594 */
      0x11, 0x0,  /* FC_RP */
/* 596 */ NdrFcShort( 0xffdc ), /* Offset= -36 (560) */
/* 598 */
      0x1b,   /* FC_CARRAY */
      0x3,    /* 3 */
/* 600 */ NdrFcShort( 0x4 ),  /* 4 */
/* 602 */ 0x19,   /* Corr desc:  field pointer, FC_ULONG */
      0x0,    /*  */
/* 604 */ NdrFcShort( 0x0 ),  /* 0 */
/* 606 */ NdrFcShort( 0x1 ),  /* Corr flags:  early, */
/* 608 */
      0x4b,   /* FC_PP */
      0x5c,   /* FC_PAD */
/* 610 */
      0x48,   /* FC_VARIABLE_REPEAT */
      0x49,   /* FC_FIXED_OFFSET */
/* 612 */ NdrFcShort( 0x4 ),  /* 4 */
/* 614 */ NdrFcShort( 0x0 ),  /* 0 */
/* 616 */ NdrFcShort( 0x1 ),  /* 1 */
/* 618 */ NdrFcShort( 0x0 ),  /* 0 */
/* 620 */ NdrFcShort( 0x0 ),  /* 0 */
/* 622 */ 0x13, 0x0,  /* FC_OP */
/* 624 */ NdrFcShort( 0x192 ),  /* Offset= 402 (1026) */
/* 626 */
      0x5b,   /* FC_END */

      0x8,    /* FC_LONG */
/* 628 */ 0x5c,   /* FC_PAD */
      0x5b,   /* FC_END */
/* 630 */
      0x1a,   /* FC_BOGUS_STRUCT */
      0x3,    /* 3 */
/* 632 */ NdrFcShort( 0x8 ),  /* 8 */
/* 634 */ NdrFcShort( 0x0 ),  /* 0 */
/* 636 */ NdrFcShort( 0x6 ),  /* Offset= 6 (642) */
/* 638 */ 0x8,    /* FC_LONG */
      0x36,   /* FC_POINTER */
/* 640 */ 0x5c,   /* FC_PAD */
      0x5b,   /* FC_END */
/* 642 */
      0x11, 0x0,  /* FC_RP */
/* 644 */ NdrFcShort( 0xffd2 ), /* Offset= -46 (598) */
/* 646 */
      0x2f,   /* FC_IP */
      0x5a,   /* FC_CONSTANT_IID */
/* 648 */ NdrFcLong( 0x2f ),  /* 47 */
/* 652 */ NdrFcShort( 0x0 ),  /* 0 */
/* 654 */ NdrFcShort( 0x0 ),  /* 0 */
/* 656 */ 0xc0,   /* 192 */
      0x0,    /* 0 */
/* 658 */ 0x0,    /* 0 */
      0x0,    /* 0 */
/* 660 */ 0x0,    /* 0 */
      0x0,    /* 0 */
/* 662 */ 0x0,    /* 0 */
      0x46,   /* 70 */
/* 664 */
      0x1b,   /* FC_CARRAY */
      0x0,    /* 0 */
/* 666 */ NdrFcShort( 0x1 ),  /* 1 */
/* 668 */ 0x19,   /* Corr desc:  field pointer, FC_ULONG */
      0x0,    /*  */
/* 670 */ NdrFcShort( 0x4 ),  /* 4 */
/* 672 */ NdrFcShort( 0x1 ),  /* Corr flags:  early, */
/* 674 */ 0x1,    /* FC_BYTE */
      0x5b,   /* FC_END */
/* 676 */
      0x1a,   /* FC_BOGUS_STRUCT */
      0x3,    /* 3 */
/* 678 */ NdrFcShort( 0x10 ), /* 16 */
/* 680 */ NdrFcShort( 0x0 ),  /* 0 */
/* 682 */ NdrFcShort( 0xa ),  /* Offset= 10 (692) */
/* 684 */ 0x8,    /* FC_LONG */
      0x8,    /* FC_LONG */
/* 686 */ 0x4c,   /* FC_EMBEDDED_COMPLEX */
      0x0,    /* 0 */
/* 688 */ NdrFcShort( 0xffd6 ), /* Offset= -42 (646) */
/* 690 */ 0x36,   /* FC_POINTER */
      0x5b,   /* FC_END */
/* 692 */
      0x13, 0x0,  /* FC_OP */
/* 694 */ NdrFcShort( 0xffe2 ), /* Offset= -30 (664) */
/* 696 */
      0x1b,   /* FC_CARRAY */
      0x3,    /* 3 */
/* 698 */ NdrFcShort( 0x4 ),  /* 4 */
/* 700 */ 0x19,   /* Corr desc:  field pointer, FC_ULONG */
      0x0,    /*  */
/* 702 */ NdrFcShort( 0x0 ),  /* 0 */
/* 704 */ NdrFcShort( 0x1 ),  /* Corr flags:  early, */
/* 706 */
      0x4b,   /* FC_PP */
      0x5c,   /* FC_PAD */
/* 708 */
      0x48,   /* FC_VARIABLE_REPEAT */
      0x49,   /* FC_FIXED_OFFSET */
/* 710 */ NdrFcShort( 0x4 ),  /* 4 */
/* 712 */ NdrFcShort( 0x0 ),  /* 0 */
/* 714 */ NdrFcShort( 0x1 ),  /* 1 */
/* 716 */ NdrFcShort( 0x0 ),  /* 0 */
/* 718 */ NdrFcShort( 0x0 ),  /* 0 */
/* 720 */ 0x13, 0x0,  /* FC_OP */
/* 722 */ NdrFcShort( 0xffd2 ), /* Offset= -46 (676) */
/* 724 */
      0x5b,   /* FC_END */

      0x8,    /* FC_LONG */
/* 726 */ 0x5c,   /* FC_PAD */
      0x5b,   /* FC_END */
/* 728 */
      0x1a,   /* FC_BOGUS_STRUCT */
      0x3,    /* 3 */
/* 730 */ NdrFcShort( 0x8 ),  /* 8 */
/* 732 */ NdrFcShort( 0x0 ),  /* 0 */
/* 734 */ NdrFcShort( 0x6 ),  /* Offset= 6 (740) */
/* 736 */ 0x8,    /* FC_LONG */
      0x36,   /* FC_POINTER */
/* 738 */ 0x5c,   /* FC_PAD */
      0x5b,   /* FC_END */
/* 740 */
      0x11, 0x0,  /* FC_RP */
/* 742 */ NdrFcShort( 0xffd2 ), /* Offset= -46 (696) */
/* 744 */
      0x1d,   /* FC_SMFARRAY */
      0x0,    /* 0 */
/* 746 */ NdrFcShort( 0x8 ),  /* 8 */
/* 748 */ 0x1,    /* FC_BYTE */
      0x5b,   /* FC_END */
/* 750 */
      0x15,   /* FC_STRUCT */
      0x3,    /* 3 */
/* 752 */ NdrFcShort( 0x10 ), /* 16 */
/* 754 */ 0x8,    /* FC_LONG */
      0x6,    /* FC_SHORT */
/* 756 */ 0x6,    /* FC_SHORT */
      0x4c,   /* FC_EMBEDDED_COMPLEX */
/* 758 */ 0x0,    /* 0 */
      NdrFcShort( 0xfff1 ), /* Offset= -15 (744) */
      0x5b,   /* FC_END */
/* 762 */
      0x1a,   /* FC_BOGUS_STRUCT */
      0x3,    /* 3 */
/* 764 */ NdrFcShort( 0x18 ), /* 24 */
/* 766 */ NdrFcShort( 0x0 ),  /* 0 */
/* 768 */ NdrFcShort( 0xa ),  /* Offset= 10 (778) */
/* 770 */ 0x8,    /* FC_LONG */
      0x36,   /* FC_POINTER */
/* 772 */ 0x4c,   /* FC_EMBEDDED_COMPLEX */
      0x0,    /* 0 */
/* 774 */ NdrFcShort( 0xffe8 ), /* Offset= -24 (750) */
/* 776 */ 0x5c,   /* FC_PAD */
      0x5b,   /* FC_END */
/* 778 */
      0x11, 0x0,  /* FC_RP */
/* 780 */ NdrFcShort( 0xfefe ), /* Offset= -258 (522) */
/* 782 */
      0x1b,   /* FC_CARRAY */
      0x0,    /* 0 */
/* 784 */ NdrFcShort( 0x1 ),  /* 1 */
/* 786 */ 0x19,   /* Corr desc:  field pointer, FC_ULONG */
      0x0,    /*  */
/* 788 */ NdrFcShort( 0x0 ),  /* 0 */
/* 790 */ NdrFcShort( 0x1 ),  /* Corr flags:  early, */
/* 792 */ 0x1,    /* FC_BYTE */
      0x5b,   /* FC_END */
/* 794 */
      0x16,   /* FC_PSTRUCT */
      0x3,    /* 3 */
/* 796 */ NdrFcShort( 0x8 ),  /* 8 */
/* 798 */
      0x4b,   /* FC_PP */
      0x5c,   /* FC_PAD */
/* 800 */
      0x46,   /* FC_NO_REPEAT */
      0x5c,   /* FC_PAD */
/* 802 */ NdrFcShort( 0x4 ),  /* 4 */
/* 804 */ NdrFcShort( 0x4 ),  /* 4 */
/* 806 */ 0x13, 0x0,  /* FC_OP */
/* 808 */ NdrFcShort( 0xffe6 ), /* Offset= -26 (782) */
/* 810 */
      0x5b,   /* FC_END */

      0x8,    /* FC_LONG */
/* 812 */ 0x8,    /* FC_LONG */
      0x5b,   /* FC_END */
/* 814 */
      0x1b,   /* FC_CARRAY */
      0x1,    /* 1 */
/* 816 */ NdrFcShort( 0x2 ),  /* 2 */
/* 818 */ 0x19,   /* Corr desc:  field pointer, FC_ULONG */
      0x0,    /*  */
/* 820 */ NdrFcShort( 0x0 ),  /* 0 */
/* 822 */ NdrFcShort( 0x1 ),  /* Corr flags:  early, */
/* 824 */ 0x6,    /* FC_SHORT */
      0x5b,   /* FC_END */
/* 826 */
      0x16,   /* FC_PSTRUCT */
      0x3,    /* 3 */
/* 828 */ NdrFcShort( 0x8 ),  /* 8 */
/* 830 */
      0x4b,   /* FC_PP */
      0x5c,   /* FC_PAD */
/* 832 */
      0x46,   /* FC_NO_REPEAT */
      0x5c,   /* FC_PAD */
/* 834 */ NdrFcShort( 0x4 ),  /* 4 */
/* 836 */ NdrFcShort( 0x4 ),  /* 4 */
/* 838 */ 0x13, 0x0,  /* FC_OP */
/* 840 */ NdrFcShort( 0xffe6 ), /* Offset= -26 (814) */
/* 842 */
      0x5b,   /* FC_END */

      0x8,    /* FC_LONG */
/* 844 */ 0x8,    /* FC_LONG */
      0x5b,   /* FC_END */
/* 846 */
      0x1b,   /* FC_CARRAY */
      0x3,    /* 3 */
/* 848 */ NdrFcShort( 0x4 ),  /* 4 */
/* 850 */ 0x19,   /* Corr desc:  field pointer, FC_ULONG */
      0x0,    /*  */
/* 852 */ NdrFcShort( 0x0 ),  /* 0 */
/* 854 */ NdrFcShort( 0x1 ),  /* Corr flags:  early, */
/* 856 */ 0x8,    /* FC_LONG */
      0x5b,   /* FC_END */
/* 858 */
      0x16,   /* FC_PSTRUCT */
      0x3,    /* 3 */
/* 860 */ NdrFcShort( 0x8 ),  /* 8 */
/* 862 */
      0x4b,   /* FC_PP */
      0x5c,   /* FC_PAD */
/* 864 */
      0x46,   /* FC_NO_REPEAT */
      0x5c,   /* FC_PAD */
/* 866 */ NdrFcShort( 0x4 ),  /* 4 */
/* 868 */ NdrFcShort( 0x4 ),  /* 4 */
/* 870 */ 0x13, 0x0,  /* FC_OP */
/* 872 */ NdrFcShort( 0xffe6 ), /* Offset= -26 (846) */
/* 874 */
      0x5b,   /* FC_END */

      0x8,    /* FC_LONG */
/* 876 */ 0x8,    /* FC_LONG */
      0x5b,   /* FC_END */
/* 878 */
      0x1b,   /* FC_CARRAY */
      0x7,    /* 7 */
/* 880 */ NdrFcShort( 0x8 ),  /* 8 */
/* 882 */ 0x19,   /* Corr desc:  field pointer, FC_ULONG */
      0x0,    /*  */
/* 884 */ NdrFcShort( 0x0 ),  /* 0 */
/* 886 */ NdrFcShort( 0x1 ),  /* Corr flags:  early, */
/* 888 */ 0xb,    /* FC_HYPER */
      0x5b,   /* FC_END */
/* 890 */
      0x16,   /* FC_PSTRUCT */
      0x3,    /* 3 */
/* 892 */ NdrFcShort( 0x8 ),  /* 8 */
/* 894 */
      0x4b,   /* FC_PP */
      0x5c,   /* FC_PAD */
/* 896 */
      0x46,   /* FC_NO_REPEAT */
      0x5c,   /* FC_PAD */
/* 898 */ NdrFcShort( 0x4 ),  /* 4 */
/* 900 */ NdrFcShort( 0x4 ),  /* 4 */
/* 902 */ 0x13, 0x0,  /* FC_OP */
/* 904 */ NdrFcShort( 0xffe6 ), /* Offset= -26 (878) */
/* 906 */
      0x5b,   /* FC_END */

      0x8,    /* FC_LONG */
/* 908 */ 0x8,    /* FC_LONG */
      0x5b,   /* FC_END */
/* 910 */
      0x15,   /* FC_STRUCT */
      0x3,    /* 3 */
/* 912 */ NdrFcShort( 0x8 ),  /* 8 */
/* 914 */ 0x8,    /* FC_LONG */
      0x8,    /* FC_LONG */
/* 916 */ 0x5c,   /* FC_PAD */
      0x5b,   /* FC_END */
/* 918 */
      0x1b,   /* FC_CARRAY */
      0x3,    /* 3 */
/* 920 */ NdrFcShort( 0x8 ),  /* 8 */
/* 922 */ 0x7,    /* Corr desc: FC_USHORT */
      0x0,    /*  */
/* 924 */ NdrFcShort( 0xffd8 ), /* -40 */
/* 926 */ NdrFcShort( 0x1 ),  /* Corr flags:  early, */
/* 928 */ 0x4c,   /* FC_EMBEDDED_COMPLEX */
      0x0,    /* 0 */
/* 930 */ NdrFcShort( 0xffec ), /* Offset= -20 (910) */
/* 932 */ 0x5c,   /* FC_PAD */
      0x5b,   /* FC_END */
/* 934 */
      0x1a,   /* FC_BOGUS_STRUCT */
      0x3,    /* 3 */
/* 936 */ NdrFcShort( 0x28 ), /* 40 */
/* 938 */ NdrFcShort( 0xffec ), /* Offset= -20 (918) */
/* 940 */ NdrFcShort( 0x0 ),  /* Offset= 0 (940) */
/* 942 */ 0x6,    /* FC_SHORT */
      0x6,    /* FC_SHORT */
/* 944 */ 0x8,    /* FC_LONG */
      0x8,    /* FC_LONG */
/* 946 */ 0x4c,   /* FC_EMBEDDED_COMPLEX */
      0x0,    /* 0 */
/* 948 */ NdrFcShort( 0xfdde ), /* Offset= -546 (402) */
/* 950 */ 0x5c,   /* FC_PAD */
      0x5b,   /* FC_END */
/* 952 */
      0x13, 0x0,  /* FC_OP */
/* 954 */ NdrFcShort( 0xfeea ), /* Offset= -278 (676) */
/* 956 */
      0x13, 0x8,  /* FC_OP [simple_pointer] */
/* 958 */ 0x1,    /* FC_BYTE */
      0x5c,   /* FC_PAD */
/* 960 */
      0x13, 0x8,  /* FC_OP [simple_pointer] */
/* 962 */ 0x6,    /* FC_SHORT */
      0x5c,   /* FC_PAD */
/* 964 */
      0x13, 0x8,  /* FC_OP [simple_pointer] */
/* 966 */ 0x8,    /* FC_LONG */
      0x5c,   /* FC_PAD */
/* 968 */
      0x13, 0x8,  /* FC_OP [simple_pointer] */
/* 970 */ 0xb,    /* FC_HYPER */
      0x5c,   /* FC_PAD */
/* 972 */
      0x13, 0x8,  /* FC_OP [simple_pointer] */
/* 974 */ 0xa,    /* FC_FLOAT */
      0x5c,   /* FC_PAD */
/* 976 */
      0x13, 0x8,  /* FC_OP [simple_pointer] */
/* 978 */ 0xc,    /* FC_DOUBLE */
      0x5c,   /* FC_PAD */
/* 980 */
      0x13, 0x0,  /* FC_OP */
/* 982 */ NdrFcShort( 0xfd98 ), /* Offset= -616 (366) */
/* 984 */
      0x13, 0x10, /* FC_OP [pointer_deref] */
/* 986 */ NdrFcShort( 0xfd9a ), /* Offset= -614 (372) */
/* 988 */
      0x13, 0x10, /* FC_OP [pointer_deref] */
/* 990 */ NdrFcShort( 0xfd9a ), /* Offset= -614 (376) */
/* 992 */
      0x13, 0x10, /* FC_OP [pointer_deref] */
/* 994 */ NdrFcShort( 0xfc24 ), /* Offset= -988 (6) */
/* 996 */
      0x13, 0x10, /* FC_OP [pointer_deref] */
/* 998 */ NdrFcShort( 0xfda4 ), /* Offset= -604 (394) */
/* 1000 */
      0x13, 0x10, /* FC_OP [pointer_deref] */
/* 1002 */  NdrFcShort( 0x2 ),  /* Offset= 2 (1004) */
/* 1004 */
      0x13, 0x0,  /* FC_OP */
/* 1006 */  NdrFcShort( 0x14 ), /* Offset= 20 (1026) */
/* 1008 */
      0x15,   /* FC_STRUCT */
      0x7,    /* 7 */
/* 1010 */  NdrFcShort( 0x10 ), /* 16 */
/* 1012 */  0x6,    /* FC_SHORT */
      0x1,    /* FC_BYTE */
/* 1014 */  0x1,    /* FC_BYTE */
      0x8,    /* FC_LONG */
/* 1016 */  0xb,    /* FC_HYPER */
      0x5b,   /* FC_END */
/* 1018 */
      0x13, 0x0,  /* FC_OP */
/* 1020 */  NdrFcShort( 0xfff4 ), /* Offset= -12 (1008) */
/* 1022 */
      0x13, 0x8,  /* FC_OP [simple_pointer] */
/* 1024 */  0x2,    /* FC_CHAR */
      0x5c,   /* FC_PAD */
/* 1026 */
      0x1a,   /* FC_BOGUS_STRUCT */
      0x7,    /* 7 */
/* 1028 */  NdrFcShort( 0x20 ), /* 32 */
/* 1030 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1032 */  NdrFcShort( 0x0 ),  /* Offset= 0 (1032) */
/* 1034 */  0x8,    /* FC_LONG */
      0x8,    /* FC_LONG */
/* 1036 */  0x6,    /* FC_SHORT */
      0x6,    /* FC_SHORT */
/* 1038 */  0x6,    /* FC_SHORT */
      0x6,    /* FC_SHORT */
/* 1040 */  0x4c,   /* FC_EMBEDDED_COMPLEX */
      0x0,    /* 0 */
/* 1042 */  NdrFcShort( 0xfc32 ), /* Offset= -974 (68) */
/* 1044 */  0x5c,   /* FC_PAD */
      0x5b,   /* FC_END */
/* 1046 */  0xb4,   /* FC_USER_MARSHAL */
      0x83,   /* 131 */
/* 1048 */  NdrFcShort( 0x1 ),  /* 1 */
/* 1050 */  NdrFcShort( 0x10 ), /* 16 */
/* 1052 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1054 */  NdrFcShort( 0xfc22 ), /* Offset= -990 (64) */
/* 1056 */
      0x11, 0xc,  /* FC_RP [alloced_on_stack] [simple_pointer] */
/* 1058 */  0x8,    /* FC_LONG */
      0x5c,   /* FC_PAD */
/* 1060 */
      0x11, 0x8,  /* FC_RP [simple_pointer] */
/* 1062 */  0x6,    /* FC_SHORT */
      0x5c,   /* FC_PAD */
/* 1064 */
      0x11, 0x0,  /* FC_RP */
/* 1066 */  NdrFcShort( 0x6 ),  /* Offset= 6 (1072) */
/* 1068 */
      0x12, 0x0,  /* FC_UP */
/* 1070 */  NdrFcShort( 0xffd4 ), /* Offset= -44 (1026) */
/* 1072 */  0xb4,   /* FC_USER_MARSHAL */
      0x83,   /* 131 */
/* 1074 */  NdrFcShort( 0x1 ),  /* 1 */
/* 1076 */  NdrFcShort( 0x10 ), /* 16 */
/* 1078 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1080 */  NdrFcShort( 0xfff4 ), /* Offset= -12 (1068) */
/* 1082 */
      0x11, 0x8,  /* FC_RP [simple_pointer] */
/* 1084 */  0x8,    /* FC_LONG */
      0x5c,   /* FC_PAD */
/* 1086 */
      0x11, 0x0,  /* FC_RP */
/* 1088 */  NdrFcShort( 0x2 ),  /* Offset= 2 (1090) */
/* 1090 */  0xb4,   /* FC_USER_MARSHAL */
      0x83,   /* 131 */
/* 1092 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1094 */  NdrFcShort( 0x4 ),  /* 4 */
/* 1096 */  NdrFcShort( 0x0 ),  /* 0 */
/* 1098 */  NdrFcShort( 0xfd2a ), /* Offset= -726 (372) */

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
    48,
    84,
    114,
    168,
    204,
    246,
    282,
    318,
    354,
    396,
    432,
    468,
    504,
    570,
    630,
    666,
    702
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
    738,
    786,
    828,
    864,
    918,
    948,
    978,
    1026,
    1062,
    1110,
    1152,
    1182,
    1212,
    1266,
    1320,
    1356,
    1404,
    1446,
    1500
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
CINTERFACE_PROXY_VTABLE(26) _IBrowserEventHandlerProxyVtbl =
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
    (void *) (INT_PTR) -1 /* IBrowserEventHandler::OnWinDomainAuthenticate */
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
    NdrStubCall2
};

CInterfaceStubVtbl _IBrowserEventHandlerStubVtbl =
{
    &IID_IBrowserEventHandler,
    &IBrowserEventHandler_ServerInfo,
    26,
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
    0x8000265, /* MIDL Version 8.0.613 */
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


#define _IEInterface_CHECK_IID(n) IID_GENERIC_CHECK_IID( _IEInterface, pIID, n)

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


//#endif /* !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_) */

