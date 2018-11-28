// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/renderer/trident_main_platform_delegate.h"

#include <dwrite.h>

#include "base/command_line.h"
#include "base/logging.h"
#include "base/strings/string16.h"
#include "base/win/win_util.h"
#include "base/win/windows_version.h"
#include "content/common/IE/browser_process_ie.h"
#include "content/public/common/content_switches.h"

namespace content {
TridentMainPlatformDelegate::TridentMainPlatformDelegate(
    const MainFunctionParams& parameters)
    : parameters_(parameters) {}

TridentMainPlatformDelegate::~TridentMainPlatformDelegate() {}

void TridentMainPlatformDelegate::PlatformInitialize() {
  ie::BrowserProcess::RegisterClassObject();
}

void TridentMainPlatformDelegate::PlatformUninitialize() {
  ie::BrowserProcess::UnregisterClassObject();
}

}  // namespace content
