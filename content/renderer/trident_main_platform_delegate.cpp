#ifdef REDCORE
#ifdef IE_REDCORE
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
#include "content/common/font_warmup_win.h"
#include "content/public/common/content_switches.h"
#include "content/common/IE/BrowserProcess.h"

namespace content {
  TridentMainPlatformDelegate::TridentMainPlatformDelegate(
    const MainFunctionParams& parameters)
    : parameters_(parameters){
  }

  TridentMainPlatformDelegate::~TridentMainPlatformDelegate() {
  }

  void TridentMainPlatformDelegate::PlatformInitialize() {
    IE::BrowserProcess::RegisterClassObject();
  }

  void TridentMainPlatformDelegate::PlatformUninitialize() {
    IE::BrowserProcess::UnregisterClassObject();
  }

} //namespace content
#endif
#endif
