#ifndef CONTENT_COMMON_IE_JAVA_SCRIPT_HANDLER_H_
#define CONTENT_COMMON_IE_JAVA_SCRIPT_HANDLER_H_
#pragma once
#include <OAIdl.h>
#include <map>
#include <string>
#include <vector>

namespace IE {
  const long VALID_EXTERNAL_FUNCTION_ID = -1000;


  class JavaScriptHandler :
    public IDispatch
  {
  public:
    JavaScriptHandler();
    virtual ~JavaScriptHandler();

    //------IUnknown------
     HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
      void**ppvObject) override;

     ULONG STDMETHODCALLTYPE AddRef(void) override;

     ULONG STDMETHODCALLTYPE Release(void) override;

    //------IDispatch------
     HRESULT STDMETHODCALLTYPE GetTypeInfoCount(
      /* [out] */ __RPC__out UINT *pctinfo)  override;

     HRESULT STDMETHODCALLTYPE GetTypeInfo(
      /* [in] */ UINT iTInfo,
      /* [in] */ LCID lcid,
      /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo) override;

     HRESULT STDMETHODCALLTYPE GetIDsOfNames(
      /* [in] */ __RPC__in REFIID riid,
      /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
      /* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
      /* [in] */ LCID lcid,
      /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) override;

     /* [local] */ HRESULT STDMETHODCALLTYPE Invoke(
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
      _Out_opt_  UINT *puArgErr) override;

  private:
    void InitExternalFunMap();
  private:
    //JaveScript external Function C++ definition
    typedef  HRESULT(JavaScriptHandler::*ExterFun)(std::vector<variant_t>, variant_t*);
    HRESULT SayHello(std::vector<variant_t>params, variant_t* pResult);
    HRESULT AutoCompleteSaveForm(std::vector<variant_t>params, variant_t* pResult);
  private:
    volatile long comRefCount;
    std::map<std::wstring, std::pair<long, ExterFun>> externalFunMap;

    struct StruFunCall
    {
      long FunctionID;
      ExterFun pFUn;
      StruFunCall()
      {
        FunctionID = VALID_EXTERNAL_FUNCTION_ID;
        pFUn = NULL;
      }
    };
    StruFunCall curFunCall;
  };
} //namespace IE

#endif
