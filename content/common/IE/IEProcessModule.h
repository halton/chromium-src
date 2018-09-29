#ifndef CONTENT_COMMON_IE_IE_PROCESS_MODULE_H_
#define CONTENT_COMMON_IE_IE_PROCESS_MODULE_H_
#pragma once
#include "ATLInclude.h"
#include "IEInterface_h.h"

namespace IE {
  //class IEProcessModule : public CAtlExeModuleT< IEProcessModule >
  //{
  //public:
  //  DECLARE_LIBID(LIBID_IEBrowserLib)
  //};

  class IEDLLModule : public CAtlDllModuleT< IEDLLModule >
  {
  public:
    DECLARE_LIBID(LIBID_IEBrowserLib)
  };
} //namespace IE
#endif