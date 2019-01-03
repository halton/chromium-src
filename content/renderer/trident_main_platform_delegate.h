// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_TRIDENT_MAIN_PLATFORM_DELEGATE_H_
#define CONTENT_TRIDENT_MAIN_PLATFORM_DELEGATE_H_

#include "content/common/content_export.h"
#include "content/public/common/main_function_params.h"

namespace content {

class CONTENT_EXPORT TridentMainPlatformDelegate {
 public:
  explicit TridentMainPlatformDelegate(const MainFunctionParams& parameters);
  ~TridentMainPlatformDelegate();

  void PlatformInitialize();
  void PlatformUninitialize();

 protected:
  const MainFunctionParams& parameters() const { return parameters_; }

 private:
  const MainFunctionParams& parameters_;

  DISALLOW_COPY_AND_ASSIGN(TridentMainPlatformDelegate);
 };

}  // namespace content

#endif  // CONTENT_RENDERER_RENDERER_MAIN_PLATFORM_DELEGATE_H_
