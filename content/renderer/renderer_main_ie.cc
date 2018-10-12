// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stddef.h>
#include <utility>

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/debug/debugger.h"
#include "base/debug/leak_annotations.h"
#include "base/feature_list.h"
#include "base/i18n/rtl.h"
#include "base/message_loop/message_loop.h"
#include "base/metrics/field_trial.h"
#include "base/metrics/histogram.h"
#include "base/metrics/statistics_recorder.h"
#include "base/pending_task.h"
#include "base/strings/string_util.h"
#include "base/sys_info.h"
#include "base/threading/platform_thread.h"
#include "base/time/time.h"
#include "base/timer/hi_res_timer_manager.h"
#include "base/trace_event/trace_event.h"
#include "build/build_config.h"
#include "components/scheduler/renderer/renderer_scheduler.h"
#include "components/startup_metric_utils/common/startup_metric_messages.h"
#include "content/child/child_process.h"
#include "content/common/IE/IEProcessModule.h"
#include "content/common/content_constants_internal.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/main_function_params.h"
#include "content/public/renderer/content_renderer_client.h"
#include "content/renderer/render_process_impl.h"
#include "content/renderer/render_thread_impl.h"
#include "content/renderer/renderer_main_platform_delegate.h"
#include "content/renderer/trident_main_platform_delegate.h"
#include "third_party/skia/include/core/SkGraphics.h"
#include "ui/base/ui_base_switches.h"

namespace content {
namespace {
static void HandleTridentErrorTestParameters(
    const base::CommandLine& command_line) {
  if (command_line.HasSwitch(switches::kTridentStartupDialog))
    ChildProcess::WaitForDebugger("Trident");
}
}  // namespace

int TridentMain(const MainFunctionParams& parameters) {
  TRACE_EVENT_ASYNC_BEGIN0("startup", "TridentMain", 0);

  IE::IEDLLModule _AtlModule;
  if (FAILED(OleInitialize(NULL)))
    return -1;

  base::trace_event::TraceLog::GetInstance()->SetProcessName("Trident");
  base::trace_event::TraceLog::GetInstance()->SetProcessSortIndex(
      kTraceEventRendererProcessSortIndex);

  const base::CommandLine& parsed_command_line = parameters.command_line;
  HandleTridentErrorTestParameters(parsed_command_line);

  TridentMainPlatformDelegate platform(parameters);
  scoped_ptr<base::MessageLoop> main_message_loop(new base::MessageLoopForUI());
  base::PlatformThread::SetName("CrTridentMain");
  platform.PlatformInitialize();
  TRACE_EVENT_ASYNC_BEGIN0("toplevel", "TridentMain.START_MSG_LOOP", 0);
  base::MessageLoop::current()->Run();
  TRACE_EVENT_ASYNC_END0("toplevel", "TridentMain.START_MSG_LOOP", 0);
  platform.PlatformUninitialize();
  OleUninitialize();
  TRACE_EVENT_ASYNC_END0("startup", "TridentMain", 0);
  return 0;
}
}  // namespace content
