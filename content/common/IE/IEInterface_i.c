

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IBrowserContainer,0xDB1BC009,0x56ED,0x4A59,0xB4,0x10,0x1D,0x69,0xAF,0xE4,0x09,0x50);


MIDL_DEFINE_GUID(IID, IID_IBrowserEventHandler,0xE74F8824,0xD752,0x48A2,0x82,0xE0,0x7D,0xF0,0xFD,0xAA,0xB7,0x86);


MIDL_DEFINE_GUID(IID, IID_IHttpMonitor,0x8C11DD57,0x767B,0x480E,0xA7,0x2B,0x92,0x0A,0x78,0xD0,0x6D,0x73);


MIDL_DEFINE_GUID(IID, LIBID_IEBrowserLib,0xBD1246AA,0x9706,0x435F,0x9C,0x5C,0x86,0xF1,0x77,0x85,0x0F,0x9E);


MIDL_DEFINE_GUID(CLSID, CLSID_BrowserContainer,0x5C304C78,0xAD5B,0x4674,0xBA,0xDA,0xC9,0x66,0x83,0x42,0x32,0x19);


MIDL_DEFINE_GUID(CLSID, CLSID_BrowserHost,0x44EC5D79,0x21E6,0x45E3,0xBB,0x21,0xC3,0x63,0xBA,0x40,0x11,0x90);


MIDL_DEFINE_GUID(CLSID, CLSID_HttpMonitor,0x1144F1D5,0x3CDB,0x4525,0x83,0xD4,0x78,0xD4,0xA4,0x0A,0x39,0x01);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



