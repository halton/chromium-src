#include "ATLInclude.h"
#include "JavaScriptHandler.h"

namespace IE {
  using namespace std;

  JavaScriptHandler::JavaScriptHandler()
  {
    comRefCount = 0;
    InitExternalFunMap();
  }


  JavaScriptHandler::~JavaScriptHandler()
  {
  }

  HRESULT JavaScriptHandler::QueryInterface(REFIID riid, void ** ppvObject)
  {
    if (riid == __uuidof(IUnknown))
    {
      (*ppvObject) = static_cast<IUnknown*>(this);
    }
    else if (riid == __uuidof(IDispatch))
    {
      (*ppvObject) = static_cast<IDispatch*>(this);
    }
    else
    {
      return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
  }

  ULONG JavaScriptHandler::AddRef(void)
  {
    InterlockedIncrement(&comRefCount);
    return comRefCount;
  }

  ULONG JavaScriptHandler::Release(void)
  {
    InterlockedDecrement(&comRefCount);
    return comRefCount;
  }

  HRESULT JavaScriptHandler::GetTypeInfoCount(UINT * pctinfo)
  {
    return E_NOTIMPL;
  }

  HRESULT JavaScriptHandler::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo)
  {
    return E_NOTIMPL;
  }

  HRESULT JavaScriptHandler::GetIDsOfNames(REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId)
  {
    curFunCall = StruFunCall();
    if (rgszNames == NULL || rgszNames[0] == NULL || rgDispId == NULL)
      return E_INVALIDARG;
    map<wstring, pair<long, ExterFun>>::const_iterator iter = externalFunMap.find(wstring(rgszNames[0]));
    if (iter == externalFunMap.end())
      return E_UNEXPECTED;
    *rgDispId = iter->second.first;
    curFunCall.FunctionID = iter->second.first;
    curFunCall.pFUn = iter->second.second;
    return S_OK;
  }

  HRESULT JavaScriptHandler::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr)
  {
    if (dispIdMember == curFunCall.FunctionID)
    {
      vector<variant_t> args;
      int i = pDispParams->cArgs - 1;
      for (; i >= 0; i--)
      {
        args.push_back(pDispParams->rgvarg[i]);
      }
      variant_t ret;
      HRESULT hr = (this->*(curFunCall.pFUn))(args, &ret);
      if(pVarResult)
        *pVarResult = ret.Detach();
      return hr;
    }
    return E_NOTIMPL;
  }

  void JavaScriptHandler::InitExternalFunMap()
  {
    //externalFunMap.insert(make_pair(L"sayHello", make_pair(0, &JavaScriptHandler::SayHello)));
    externalFunMap.insert(make_pair(L"AutoCompleteSaveForm", make_pair(0, &JavaScriptHandler::AutoCompleteSaveForm)));
  }

  HRESULT JavaScriptHandler::SayHello(std::vector<variant_t> params, variant_t * pResult)
  {

    return S_OK;
  }

  HRESULT JavaScriptHandler::AutoCompleteSaveForm(std::vector<variant_t> params, variant_t * pResult)
  {
    return S_OK;
  }

} //namespace IE