// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/browser_main.h"

#include <memory>

#include "base/trace_event/trace_event.h"
#include "content/browser/browser_main_runner_impl.h"
#include "content/common/content_constants_internal.h"

#if defined(IE_REDCORE)
#include "content/common/IE/atl_include.h"
#include "content/common/IE/dll_module_ie.h"
#endif

namespace content {

namespace {

// Generates a pair of BrowserMain async events. We don't use the TRACE_EVENT0
// macro because the tracing infrastructure doesn't expect synchronous events
// around the main loop of a thread.
class ScopedBrowserMainEvent {
 public:
  ScopedBrowserMainEvent() {
    TRACE_EVENT_ASYNC_BEGIN0("startup", "BrowserMain", 0);
  }
  ~ScopedBrowserMainEvent() {
    TRACE_EVENT_ASYNC_END0("startup", "BrowserMain", 0);
  }
};

}  // namespace

// Main routine for running as the Browser process.
int BrowserMain(const MainFunctionParams& parameters) {
  ScopedBrowserMainEvent scoped_browser_main_event;

#if defined(IE_REDCORE)
  ie::DllModule _AtlModule;
  HRESULT hr = OleInitialize(NULL);

  if (!SUCCEEDED(hr)) {
    return 0;
  }
#endif

  base::trace_event::TraceLog::GetInstance()->set_process_name("Browser");
  base::trace_event::TraceLog::GetInstance()->SetProcessSortIndex(
      kTraceEventBrowserProcessSortIndex);

  std::unique_ptr<BrowserMainRunnerImpl> main_runner(
      BrowserMainRunnerImpl::Create());

  int exit_code = main_runner->Initialize(parameters);
  if (exit_code >= 0)
    return exit_code;

  exit_code = main_runner->Run();

  main_runner->Shutdown();
#if defined(IE_REDCORE)
  OleUninitialize();
#endif
  return exit_code;
}

}  // namespace content
