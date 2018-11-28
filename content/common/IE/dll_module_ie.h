// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_IE_DLL_MODULE_IE_H_
#define CONTENT_COMMON_IE_DLL_MODULE_IE_H_

#include "content/common/IE/IEInterface.h"
#include "content/common/IE/atl_include.h"

namespace ie {
// class IEProcessModule : public CAtlExeModuleT< IEProcessModule >
//{
// public:
//  DECLARE_LIBID(LIBID_IEBrowserLib)
//};

class DllModule : public CAtlDllModuleT<DllModule> {
 public:
  DECLARE_LIBID(LIBID_IEBrowserLib)
};

}  // namespace ie

#endif  // CONTENT_COMMON_IE_DLL_MODULE_IE_H_
