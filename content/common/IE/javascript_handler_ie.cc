// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/IE/javascript_handler_ie.h"

#include "base/win/atl.h"

namespace ie {

JavaScriptHandler::JavaScriptHandler() {
  com_ref_count_ = 0;
  InitExternalFunMap();
}

JavaScriptHandler::~JavaScriptHandler() {}

HRESULT JavaScriptHandler::QueryInterface(REFIID riid, void** ppv_object) {
  if (riid == __uuidof(IUnknown)) {
    (*ppv_object) = static_cast<IUnknown*>(this);
  } else if (riid == __uuidof(IDispatch)) {
    (*ppv_object) = static_cast<IDispatch*>(this);
  } else {
    return E_NOINTERFACE;
  }

  AddRef();
  return S_OK;
}

ULONG JavaScriptHandler::AddRef() {
  InterlockedIncrement(&com_ref_count_);
  return com_ref_count_;
}

ULONG JavaScriptHandler::Release() {
  InterlockedDecrement(&com_ref_count_);
  return com_ref_count_;
}

HRESULT JavaScriptHandler::GetTypeInfoCount(UINT* pctinfo) {
  return E_NOTIMPL;
}

HRESULT JavaScriptHandler::GetTypeInfo(UINT iTInfo,
                                       LCID lcid,
                                       ITypeInfo** ppTInfo) {
  return E_NOTIMPL;
}

HRESULT JavaScriptHandler::GetIDsOfNames(REFIID riid,
                                         LPOLESTR* rgszNames,
                                         UINT cNames,
                                         LCID lcid,
                                         DISPID* rgDispId) {
  current_function_call_ = FunctionCall();
  if (rgszNames == NULL || rgszNames[0] == NULL || rgDispId == NULL)
    return E_INVALIDARG;
  std::map<std::wstring, std::pair<long, ExterFun>>::const_iterator iter =
      external_function_map_.find(std::wstring(rgszNames[0]));
  if (iter == external_function_map_.end())
    return E_UNEXPECTED;

  *rgDispId = iter->second.first;
  current_function_call_.id = iter->second.first;
  current_function_call_.exter_func = iter->second.second;
  return S_OK;
}

HRESULT JavaScriptHandler::Invoke(DISPID dispIdMember,
                                  REFIID riid,
                                  LCID lcid,
                                  WORD wFlags,
                                  DISPPARAMS* pDispParams,
                                  VARIANT* pVarResult,
                                  EXCEPINFO* pExcepInfo,
                                  UINT* puArgErr) {
  if (dispIdMember == current_function_call_.id) {
    std::vector<variant_t> args;
    int i = pDispParams->cArgs - 1;
    for (; i >= 0; i--) {
      args.push_back(pDispParams->rgvarg[i]);
    }
    variant_t ret;
    HRESULT hr = (this->*(current_function_call_.exter_func))(args, &ret);
    if (pVarResult)
      *pVarResult = ret.Detach();
    return hr;
  }

  return E_NOTIMPL;
}

void JavaScriptHandler::InitExternalFunMap() {
  // external_function_map_.insert(make_pair(L"sayHello", make_pair(0,
  // &JavaScriptHandler::SayHello)));
  external_function_map_.insert(
      make_pair(L"AutoCompleteSaveForm",
                make_pair(0, &JavaScriptHandler::AutoCompleteSaveForm)));
}

HRESULT JavaScriptHandler::SayHello(std::vector<variant_t> params,
                                    variant_t* result) {
  return S_OK;
}

HRESULT JavaScriptHandler::AutoCompleteSaveForm(std::vector<variant_t> params,
                                                variant_t* result) {
  return S_OK;
}

}  // namespace ie
