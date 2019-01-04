// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_IE_JAVASCRIPT_HANDLER_IE_H_
#define CONTENT_COMMON_IE_JAVASCRIPT_HANDLER_IE_H_

#include <comdef.h>

#include <map>
#include <string>
#include <vector>

#include "base/win/atl.h"

namespace ie {

const long VALID_EXTERNAL_FUNCTION_ID = -1000;

class JavaScriptHandler : public IDispatch {
 public:
  JavaScriptHandler();
  virtual ~JavaScriptHandler();

  //------IUnknown------
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                           void** ppv_object) override;

  ULONG STDMETHODCALLTYPE AddRef() override;

  ULONG STDMETHODCALLTYPE Release() override;

  //------IDispatch------
  HRESULT STDMETHODCALLTYPE GetTypeInfoCount(
      /* [out] */ __RPC__out UINT* pctinfo) override;

  HRESULT STDMETHODCALLTYPE GetTypeInfo(
      /* [in] */ UINT iTInfo,
      /* [in] */ LCID lcid,
      /* [out] */ __RPC__deref_out_opt ITypeInfo** ppTInfo) override;

  HRESULT STDMETHODCALLTYPE GetIDsOfNames(
      /* [in] */ __RPC__in REFIID riid,
      /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR* rgszNames,
      /* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
      /* [in] */ LCID lcid,
      /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID* rgDispId)
      override;

  /* [local] */ HRESULT STDMETHODCALLTYPE Invoke(
      /* [annotation][in] */
      _In_ DISPID dispIdMember,
      /* [annotation][in] */
      _In_ REFIID riid,
      /* [annotation][in] */
      _In_ LCID lcid,
      /* [annotation][in] */
      _In_ WORD wFlags,
      /* [annotation][out][in] */
      _In_ DISPPARAMS* pDispParams,
      /* [annotation][out] */
      _Out_opt_ VARIANT* pVarResult,
      /* [annotation][out] */
      _Out_opt_ EXCEPINFO* pExcepInfo,
      /* [annotation][out] */
      _Out_opt_ UINT* puArgErr) override;

 private:
  void InitExternalFunMap();

 private:
  // JaveScript external Function C++ definition
  typedef HRESULT (JavaScriptHandler::*ExterFun)(std::vector<variant_t>,
                                                 variant_t*);
  HRESULT SayHello(std::vector<variant_t> params, variant_t* result);
  HRESULT AutoCompleteSaveForm(std::vector<variant_t> params,
                               variant_t* result);

 private:
  volatile long com_ref_count_;
  std::map<std::wstring, std::pair<long, ExterFun>> external_function_map_;

  struct FunctionCall {
    long id;
    ExterFun exter_func;
    FunctionCall() {
      id = VALID_EXTERNAL_FUNCTION_ID;
      exter_func = NULL;
    }
  };
  FunctionCall current_function_call_;
};
}  // namespace ie

#endif  // CONTENT_COMMON_IE_JAVASCRIPT_HANDLER_IE_H_
