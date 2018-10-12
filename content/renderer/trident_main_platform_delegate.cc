// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/renderer/trident_main_platform_delegate.h"

#include <dwrite.h>

#include "base/command_line.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/strings/string16.h"
#include "base/win/scoped_comptr.h"
#include "base/win/win_util.h"
#include "base/win/windows_version.h"
#include "content/child/dwrite_font_proxy/dwrite_font_proxy_init_win.h"
#include "content/common/IE/BrowserProcess.h"
#include "content/common/font_warmup_win.h"
#include "content/public/common/content_switches.h"

namespace content {
TridentMainPlatformDelegate::TridentMainPlatformDelegate(
    const MainFunctionParams& parameters)
    : parameters_(parameters) {}

TridentMainPlatformDelegate::~TridentMainPlatformDelegate() {}

void TridentMainPlatformDelegate::PlatformInitialize() {
  IE::BrowserProcess::RegisterClassObject();
}

void TridentMainPlatformDelegate::PlatformUninitialize() {
  IE::BrowserProcess::UnregisterClassObject();
}

}  // namespace content
