

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.xx.xxxx */
/* at a redacted point in time
 */
/* Compiler settings for ../../chrome/app/IEInterface.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.xx.xxxx 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        EXTERN_C __declspec(selectany) const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif // !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IBrowserContainer,0x66707C49,0xE1DC,0x42EC,0xAD,0x0C,0xC9,0xF0,0xB0,0x8F,0x78,0x13);


MIDL_DEFINE_GUID(IID, IID_IBrowserEventHandler,0x3130B1FC,0x06E5,0x4497,0xAD,0x49,0x12,0x10,0xB8,0x63,0x10,0xE0);


MIDL_DEFINE_GUID(IID, IID_IHttpMonitor,0x7B210509,0x86BE,0x47DE,0x85,0x0E,0x91,0xF6,0xC1,0xEA,0xB7,0x71);


MIDL_DEFINE_GUID(IID, LIBID_IEBrowserLib,0x435380BE,0xBB0E,0x408D,0x92,0x44,0xC6,0x65,0x72,0x06,0x83,0xC6);


MIDL_DEFINE_GUID(CLSID, CLSID_BrowserContainer,0xA8CEEC22,0xAF5E,0x44E2,0xA7,0x9D,0x25,0x19,0xC3,0xF2,0x7E,0xA6);


MIDL_DEFINE_GUID(CLSID, CLSID_BrowserHost,0xD4806415,0x6D82,0x452D,0xB4,0xCC,0xA5,0x7E,0xA4,0x6B,0x85,0xF9);


MIDL_DEFINE_GUID(CLSID, CLSID_HttpMonitor,0x9306A4A9,0x3154,0x4E07,0x88,0x55,0x2B,0xC7,0xAA,0x5A,0x8D,0xD4);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



