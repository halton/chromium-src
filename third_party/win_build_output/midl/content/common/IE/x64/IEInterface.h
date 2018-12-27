

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __IEInterface_h__
#define __IEInterface_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IBrowserContainer_FWD_DEFINED__
#define __IBrowserContainer_FWD_DEFINED__
typedef interface IBrowserContainer IBrowserContainer;

#endif 	/* __IBrowserContainer_FWD_DEFINED__ */


#ifndef __IBrowserEventHandler_FWD_DEFINED__
#define __IBrowserEventHandler_FWD_DEFINED__
typedef interface IBrowserEventHandler IBrowserEventHandler;

#endif 	/* __IBrowserEventHandler_FWD_DEFINED__ */


#ifndef __IHttpMonitor_FWD_DEFINED__
#define __IHttpMonitor_FWD_DEFINED__
typedef interface IHttpMonitor IHttpMonitor;

#endif 	/* __IHttpMonitor_FWD_DEFINED__ */


#ifndef __BrowserContainer_FWD_DEFINED__
#define __BrowserContainer_FWD_DEFINED__

#ifdef __cplusplus
typedef class BrowserContainer BrowserContainer;
#else
typedef struct BrowserContainer BrowserContainer;
#endif /* __cplusplus */

#endif 	/* __BrowserContainer_FWD_DEFINED__ */


#ifndef __BrowserHost_FWD_DEFINED__
#define __BrowserHost_FWD_DEFINED__

#ifdef __cplusplus
typedef class BrowserHost BrowserHost;
#else
typedef struct BrowserHost BrowserHost;
#endif /* __cplusplus */

#endif 	/* __BrowserHost_FWD_DEFINED__ */


#ifndef __HttpMonitor_FWD_DEFINED__
#define __HttpMonitor_FWD_DEFINED__

#ifdef __cplusplus
typedef class HttpMonitor HttpMonitor;
#else
typedef struct HttpMonitor HttpMonitor;
#endif /* __cplusplus */

#endif 	/* __HttpMonitor_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IBrowserContainer_INTERFACE_DEFINED__
#define __IBrowserContainer_INTERFACE_DEFINED__

/* interface IBrowserContainer */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IBrowserContainer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DB1BC009-56ED-4A59-B410-1D69AFE40950")
    IBrowserContainer : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateBrowser( 
            /* [in] */ long hWnd,
            /* [in] */ int browserEmulation,
            /* [in] */ VARIANT_BOOL isNewWindow) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_WebBrowser( 
            /* [out] */ IDispatch **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CloseBrowser( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetRect( 
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ int right,
            /* [in] */ int bottom) = 0;
        
        virtual /* [helpstring][id][propputref] */ HRESULT STDMETHODCALLTYPE putref_HostApp( 
            /* [in] */ IDispatch *app) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ExcuteJS( 
            /* [in] */ BSTR src,
            /* [out][in] */ VARIANT *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Show( 
            /* [in] */ VARIANT_BOOL bShow) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDocumentMode( 
            /* [out] */ int *mode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDocumentMode( 
            /* [in] */ int mode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetCookie( 
            /* [in] */ BSTR json,
            /* [in] */ int flag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetUserAgent( 
            /* [in] */ BSTR usStr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FinishDownload( 
            /* [in] */ VARIANT_BOOL isCancel) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetFunctionControl( 
            /* [in] */ BSTR jsonStr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AutoLogin( 
            /* [in] */ BSTR frameXPath,
            /* [in] */ BSTR userNameXPath,
            /* [in] */ BSTR userNameValue,
            /* [in] */ BSTR pwdXPath,
            /* [in] */ BSTR pwdValue,
            /* [in] */ BSTR loginBtnXPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetListenLoginXPath( 
            /* [in] */ BSTR url,
            /* [in] */ BSTR frameXPath,
            /* [in] */ BSTR userNameXPath,
            /* [in] */ BSTR pwdXPath,
            /* [in] */ BSTR loginbtnXPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE NotifyCommand( 
            /* [in] */ BSTR command) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDevToolsOpenStatus( 
            /* [out][in] */ VARIANT_BOOL *isOpen) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowDevTools( 
            /* [in] */ VARIANT_BOOL show) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IBrowserContainerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBrowserContainer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBrowserContainer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBrowserContainer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IBrowserContainer * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IBrowserContainer * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IBrowserContainer * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IBrowserContainer * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateBrowser )( 
            IBrowserContainer * This,
            /* [in] */ long hWnd,
            /* [in] */ int browserEmulation,
            /* [in] */ VARIANT_BOOL isNewWindow);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_WebBrowser )( 
            IBrowserContainer * This,
            /* [out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CloseBrowser )( 
            IBrowserContainer * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetRect )( 
            IBrowserContainer * This,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ int right,
            /* [in] */ int bottom);
        
        /* [helpstring][id][propputref] */ HRESULT ( STDMETHODCALLTYPE *putref_HostApp )( 
            IBrowserContainer * This,
            /* [in] */ IDispatch *app);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ExcuteJS )( 
            IBrowserContainer * This,
            /* [in] */ BSTR src,
            /* [out][in] */ VARIANT *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Show )( 
            IBrowserContainer * This,
            /* [in] */ VARIANT_BOOL bShow);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDocumentMode )( 
            IBrowserContainer * This,
            /* [out] */ int *mode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetDocumentMode )( 
            IBrowserContainer * This,
            /* [in] */ int mode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetCookie )( 
            IBrowserContainer * This,
            /* [in] */ BSTR json,
            /* [in] */ int flag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetUserAgent )( 
            IBrowserContainer * This,
            /* [in] */ BSTR usStr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FinishDownload )( 
            IBrowserContainer * This,
            /* [in] */ VARIANT_BOOL isCancel);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetFunctionControl )( 
            IBrowserContainer * This,
            /* [in] */ BSTR jsonStr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AutoLogin )( 
            IBrowserContainer * This,
            /* [in] */ BSTR frameXPath,
            /* [in] */ BSTR userNameXPath,
            /* [in] */ BSTR userNameValue,
            /* [in] */ BSTR pwdXPath,
            /* [in] */ BSTR pwdValue,
            /* [in] */ BSTR loginBtnXPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetListenLoginXPath )( 
            IBrowserContainer * This,
            /* [in] */ BSTR url,
            /* [in] */ BSTR frameXPath,
            /* [in] */ BSTR userNameXPath,
            /* [in] */ BSTR pwdXPath,
            /* [in] */ BSTR loginbtnXPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *NotifyCommand )( 
            IBrowserContainer * This,
            /* [in] */ BSTR command);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDevToolsOpenStatus )( 
            IBrowserContainer * This,
            /* [out][in] */ VARIANT_BOOL *isOpen);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowDevTools )( 
            IBrowserContainer * This,
            /* [in] */ VARIANT_BOOL show);
        
        END_INTERFACE
    } IBrowserContainerVtbl;

    interface IBrowserContainer
    {
        CONST_VTBL struct IBrowserContainerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBrowserContainer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBrowserContainer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBrowserContainer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBrowserContainer_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IBrowserContainer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IBrowserContainer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IBrowserContainer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IBrowserContainer_CreateBrowser(This,hWnd,browserEmulation,isNewWindow)	\
    ( (This)->lpVtbl -> CreateBrowser(This,hWnd,browserEmulation,isNewWindow) ) 

#define IBrowserContainer_get_WebBrowser(This,pVal)	\
    ( (This)->lpVtbl -> get_WebBrowser(This,pVal) ) 

#define IBrowserContainer_CloseBrowser(This)	\
    ( (This)->lpVtbl -> CloseBrowser(This) ) 

#define IBrowserContainer_SetRect(This,left,top,right,bottom)	\
    ( (This)->lpVtbl -> SetRect(This,left,top,right,bottom) ) 

#define IBrowserContainer_putref_HostApp(This,app)	\
    ( (This)->lpVtbl -> putref_HostApp(This,app) ) 

#define IBrowserContainer_ExcuteJS(This,src,result)	\
    ( (This)->lpVtbl -> ExcuteJS(This,src,result) ) 

#define IBrowserContainer_Show(This,bShow)	\
    ( (This)->lpVtbl -> Show(This,bShow) ) 

#define IBrowserContainer_GetDocumentMode(This,mode)	\
    ( (This)->lpVtbl -> GetDocumentMode(This,mode) ) 

#define IBrowserContainer_SetDocumentMode(This,mode)	\
    ( (This)->lpVtbl -> SetDocumentMode(This,mode) ) 

#define IBrowserContainer_SetCookie(This,json,flag)	\
    ( (This)->lpVtbl -> SetCookie(This,json,flag) ) 

#define IBrowserContainer_SetUserAgent(This,usStr)	\
    ( (This)->lpVtbl -> SetUserAgent(This,usStr) ) 

#define IBrowserContainer_FinishDownload(This,isCancel)	\
    ( (This)->lpVtbl -> FinishDownload(This,isCancel) ) 

#define IBrowserContainer_SetFunctionControl(This,jsonStr)	\
    ( (This)->lpVtbl -> SetFunctionControl(This,jsonStr) ) 

#define IBrowserContainer_AutoLogin(This,frameXPath,userNameXPath,userNameValue,pwdXPath,pwdValue,loginBtnXPath)	\
    ( (This)->lpVtbl -> AutoLogin(This,frameXPath,userNameXPath,userNameValue,pwdXPath,pwdValue,loginBtnXPath) ) 

#define IBrowserContainer_SetListenLoginXPath(This,url,frameXPath,userNameXPath,pwdXPath,loginbtnXPath)	\
    ( (This)->lpVtbl -> SetListenLoginXPath(This,url,frameXPath,userNameXPath,pwdXPath,loginbtnXPath) ) 

#define IBrowserContainer_NotifyCommand(This,command)	\
    ( (This)->lpVtbl -> NotifyCommand(This,command) ) 

#define IBrowserContainer_GetDevToolsOpenStatus(This,isOpen)	\
    ( (This)->lpVtbl -> GetDevToolsOpenStatus(This,isOpen) ) 

#define IBrowserContainer_ShowDevTools(This,show)	\
    ( (This)->lpVtbl -> ShowDevTools(This,show) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBrowserContainer_INTERFACE_DEFINED__ */


#ifndef __IBrowserEventHandler_INTERFACE_DEFINED__
#define __IBrowserEventHandler_INTERFACE_DEFINED__

/* interface IBrowserEventHandler */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IBrowserEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E74F8824-D752-48A2-82E0-7DF0FDAAB786")
    IBrowserEventHandler : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnBeforeNavigate( 
            /* [in] */ BSTR url,
            /* [in] */ VARIANT_BOOL navNewUrl,
            /* [out][in] */ VARIANT_BOOL *cancel) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnFinishNavigate( 
            /* [in] */ BSTR url,
            /* [in] */ BSTR faviconUrls) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnTitleChange( 
            /* [in] */ BSTR title) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnLoadUrlNewWnd( 
            /* [in] */ BSTR url,
            /* [in] */ int flag,
            /* [out][in] */ VARIANT_BOOL *cancel,
            /* [out] */ IDispatch **pDisp) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnBrowserClosing( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnBeforeBrowserClose( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnShowContextMenu( 
            /* [in] */ int dwID,
            /* [in] */ int x,
            /* [in] */ int y) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnIEServerWndCreated( 
            /* [in] */ int hwnd) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnDownLoadFile( 
            /* [in] */ BSTR url,
            /* [in] */ int status,
            /* [in] */ VARIANT *variant) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnCanGoBackOrForward( 
            /* [out][in] */ VARIANT_BOOL *back,
            /* [out][in] */ VARIANT_BOOL *forward) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnGoBack( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnGoForward( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnRequestChangePos( 
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ int width,
            /* [in] */ int height) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnGetMainWndPos( 
            /* [out][in] */ int *left,
            /* [out][in] */ int *top,
            /* [out][in] */ int *width,
            /* [out][in] */ int *height) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnUpdateCookie( 
            /* [in] */ BSTR cookie) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnGetLoginInfo( 
            /* [in] */ BSTR url,
            /* [in] */ BSTR userName,
            /* [in] */ BSTR userPwd) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnRequestAcceleratorFromKeyboard( 
            /* [in] */ int keyCode,
            /* [in] */ int modifiers) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnRequestAcceleratorFromMouseWheel( 
            /* [in] */ int delta,
            /* [in] */ int Pt_x,
            /* [in] */ int Pt_y,
            /* [in] */ int modifiers) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnWinDomainAuthenticate( 
            /* [out][in] */ BSTR *userName,
            /* [out][in] */ BSTR *userPwd) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnQueryPrivateDNS( 
            /* [in] */ BSTR host,
            /* [out][in] */ BSTR *ipList) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IBrowserEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBrowserEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBrowserEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBrowserEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IBrowserEventHandler * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IBrowserEventHandler * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IBrowserEventHandler * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IBrowserEventHandler * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnBeforeNavigate )( 
            IBrowserEventHandler * This,
            /* [in] */ BSTR url,
            /* [in] */ VARIANT_BOOL navNewUrl,
            /* [out][in] */ VARIANT_BOOL *cancel);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnFinishNavigate )( 
            IBrowserEventHandler * This,
            /* [in] */ BSTR url,
            /* [in] */ BSTR faviconUrls);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnTitleChange )( 
            IBrowserEventHandler * This,
            /* [in] */ BSTR title);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnLoadUrlNewWnd )( 
            IBrowserEventHandler * This,
            /* [in] */ BSTR url,
            /* [in] */ int flag,
            /* [out][in] */ VARIANT_BOOL *cancel,
            /* [out] */ IDispatch **pDisp);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnBrowserClosing )( 
            IBrowserEventHandler * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnBeforeBrowserClose )( 
            IBrowserEventHandler * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnShowContextMenu )( 
            IBrowserEventHandler * This,
            /* [in] */ int dwID,
            /* [in] */ int x,
            /* [in] */ int y);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnIEServerWndCreated )( 
            IBrowserEventHandler * This,
            /* [in] */ int hwnd);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnDownLoadFile )( 
            IBrowserEventHandler * This,
            /* [in] */ BSTR url,
            /* [in] */ int status,
            /* [in] */ VARIANT *variant);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnCanGoBackOrForward )( 
            IBrowserEventHandler * This,
            /* [out][in] */ VARIANT_BOOL *back,
            /* [out][in] */ VARIANT_BOOL *forward);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnGoBack )( 
            IBrowserEventHandler * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnGoForward )( 
            IBrowserEventHandler * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnRequestChangePos )( 
            IBrowserEventHandler * This,
            /* [in] */ int left,
            /* [in] */ int top,
            /* [in] */ int width,
            /* [in] */ int height);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnGetMainWndPos )( 
            IBrowserEventHandler * This,
            /* [out][in] */ int *left,
            /* [out][in] */ int *top,
            /* [out][in] */ int *width,
            /* [out][in] */ int *height);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnUpdateCookie )( 
            IBrowserEventHandler * This,
            /* [in] */ BSTR cookie);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnGetLoginInfo )( 
            IBrowserEventHandler * This,
            /* [in] */ BSTR url,
            /* [in] */ BSTR userName,
            /* [in] */ BSTR userPwd);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnRequestAcceleratorFromKeyboard )( 
            IBrowserEventHandler * This,
            /* [in] */ int keyCode,
            /* [in] */ int modifiers);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnRequestAcceleratorFromMouseWheel )( 
            IBrowserEventHandler * This,
            /* [in] */ int delta,
            /* [in] */ int Pt_x,
            /* [in] */ int Pt_y,
            /* [in] */ int modifiers);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnWinDomainAuthenticate )( 
            IBrowserEventHandler * This,
            /* [out][in] */ BSTR *userName,
            /* [out][in] */ BSTR *userPwd);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnQueryPrivateDNS )( 
            IBrowserEventHandler * This,
            /* [in] */ BSTR host,
            /* [out][in] */ BSTR *ipList);
        
        END_INTERFACE
    } IBrowserEventHandlerVtbl;

    interface IBrowserEventHandler
    {
        CONST_VTBL struct IBrowserEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBrowserEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBrowserEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBrowserEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBrowserEventHandler_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IBrowserEventHandler_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IBrowserEventHandler_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IBrowserEventHandler_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IBrowserEventHandler_OnBeforeNavigate(This,url,navNewUrl,cancel)	\
    ( (This)->lpVtbl -> OnBeforeNavigate(This,url,navNewUrl,cancel) ) 

#define IBrowserEventHandler_OnFinishNavigate(This,url,faviconUrls)	\
    ( (This)->lpVtbl -> OnFinishNavigate(This,url,faviconUrls) ) 

#define IBrowserEventHandler_OnTitleChange(This,title)	\
    ( (This)->lpVtbl -> OnTitleChange(This,title) ) 

#define IBrowserEventHandler_OnLoadUrlNewWnd(This,url,flag,cancel,pDisp)	\
    ( (This)->lpVtbl -> OnLoadUrlNewWnd(This,url,flag,cancel,pDisp) ) 

#define IBrowserEventHandler_OnBrowserClosing(This)	\
    ( (This)->lpVtbl -> OnBrowserClosing(This) ) 

#define IBrowserEventHandler_OnBeforeBrowserClose(This)	\
    ( (This)->lpVtbl -> OnBeforeBrowserClose(This) ) 

#define IBrowserEventHandler_OnShowContextMenu(This,dwID,x,y)	\
    ( (This)->lpVtbl -> OnShowContextMenu(This,dwID,x,y) ) 

#define IBrowserEventHandler_OnIEServerWndCreated(This,hwnd)	\
    ( (This)->lpVtbl -> OnIEServerWndCreated(This,hwnd) ) 

#define IBrowserEventHandler_OnDownLoadFile(This,url,status,variant)	\
    ( (This)->lpVtbl -> OnDownLoadFile(This,url,status,variant) ) 

#define IBrowserEventHandler_OnCanGoBackOrForward(This,back,forward)	\
    ( (This)->lpVtbl -> OnCanGoBackOrForward(This,back,forward) ) 

#define IBrowserEventHandler_OnGoBack(This)	\
    ( (This)->lpVtbl -> OnGoBack(This) ) 

#define IBrowserEventHandler_OnGoForward(This)	\
    ( (This)->lpVtbl -> OnGoForward(This) ) 

#define IBrowserEventHandler_OnRequestChangePos(This,left,top,width,height)	\
    ( (This)->lpVtbl -> OnRequestChangePos(This,left,top,width,height) ) 

#define IBrowserEventHandler_OnGetMainWndPos(This,left,top,width,height)	\
    ( (This)->lpVtbl -> OnGetMainWndPos(This,left,top,width,height) ) 

#define IBrowserEventHandler_OnUpdateCookie(This,cookie)	\
    ( (This)->lpVtbl -> OnUpdateCookie(This,cookie) ) 

#define IBrowserEventHandler_OnGetLoginInfo(This,url,userName,userPwd)	\
    ( (This)->lpVtbl -> OnGetLoginInfo(This,url,userName,userPwd) ) 

#define IBrowserEventHandler_OnRequestAcceleratorFromKeyboard(This,keyCode,modifiers)	\
    ( (This)->lpVtbl -> OnRequestAcceleratorFromKeyboard(This,keyCode,modifiers) ) 

#define IBrowserEventHandler_OnRequestAcceleratorFromMouseWheel(This,delta,Pt_x,Pt_y,modifiers)	\
    ( (This)->lpVtbl -> OnRequestAcceleratorFromMouseWheel(This,delta,Pt_x,Pt_y,modifiers) ) 

#define IBrowserEventHandler_OnWinDomainAuthenticate(This,userName,userPwd)	\
    ( (This)->lpVtbl -> OnWinDomainAuthenticate(This,userName,userPwd) ) 

#define IBrowserEventHandler_OnQueryPrivateDNS(This,host,ipList)	\
    ( (This)->lpVtbl -> OnQueryPrivateDNS(This,host,ipList) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBrowserEventHandler_INTERFACE_DEFINED__ */


#ifndef __IHttpMonitor_INTERFACE_DEFINED__
#define __IHttpMonitor_INTERFACE_DEFINED__

/* interface IHttpMonitor */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IHttpMonitor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8C11DD57-767B-480E-A72B-920A78D06D73")
    IHttpMonitor : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IHttpMonitorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHttpMonitor * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHttpMonitor * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHttpMonitor * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IHttpMonitor * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IHttpMonitor * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IHttpMonitor * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IHttpMonitor * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IHttpMonitorVtbl;

    interface IHttpMonitor
    {
        CONST_VTBL struct IHttpMonitorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHttpMonitor_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHttpMonitor_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHttpMonitor_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IHttpMonitor_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IHttpMonitor_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IHttpMonitor_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IHttpMonitor_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHttpMonitor_INTERFACE_DEFINED__ */



#ifndef __IEBrowserLib_LIBRARY_DEFINED__
#define __IEBrowserLib_LIBRARY_DEFINED__

/* library IEBrowserLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_IEBrowserLib;

EXTERN_C const CLSID CLSID_BrowserContainer;

#ifdef __cplusplus

class DECLSPEC_UUID("5C304C78-AD5B-4674-BADA-C96683423219")
BrowserContainer;
#endif

EXTERN_C const CLSID CLSID_BrowserHost;

#ifdef __cplusplus

class DECLSPEC_UUID("44EC5D79-21E6-45E3-BB21-C363BA401190")
BrowserHost;
#endif

EXTERN_C const CLSID CLSID_HttpMonitor;

#ifdef __cplusplus

class DECLSPEC_UUID("1144F1D5-3CDB-4525-83D4-78D4A40A3901")
HttpMonitor;
#endif
#endif /* __IEBrowserLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


