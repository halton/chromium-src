/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebDevToolsAgentImpl_h
#define WebDevToolsAgentImpl_h

#include <memory>

#include "core/CoreExport.h"
#include "core/inspector/InspectorLayerTreeAgent.h"
#include "core/inspector/InspectorPageAgent.h"
#include "core/inspector/InspectorSession.h"
#include "core/inspector/InspectorTracingAgent.h"
#include "mojo/public/cpp/bindings/associated_binding.h"
#include "platform/heap/Handle.h"
#include "platform/wtf/Forward.h"
#include "public/platform/WebSize.h"
#include "public/platform/WebThread.h"
#include "public/web/devtools_agent.mojom-blink.h"

namespace blink {

class ClientMessageLoopAdapter;
class GraphicsLayer;
class InspectedFrames;
class InspectorOverlayAgent;
class InspectorResourceContainer;
class InspectorResourceContentLoader;
class LocalFrame;
class WebLayerTreeView;
class WebLocalFrameImpl;

class CORE_EXPORT WebDevToolsAgentImpl final
    : public GarbageCollectedFinalized<WebDevToolsAgentImpl>,
      public mojom::blink::DevToolsAgent,
      public InspectorTracingAgent::Client,
      public InspectorPageAgent::Client,
      public InspectorSession::Client,
      public InspectorLayerTreeAgent::Client,
      private WebThread::TaskObserver {
 public:
  // ------ Deprecated ------
  // These public methods and client are temporary,
  // until shared/service workers inspection migrates to Mojo.
  class WorkerClient {
   public:
    virtual ~WorkerClient() {}
    virtual bool SendProtocolMessage(int session_id,
                                     int call_id,
                                     const String& response,
                                     const String& state) = 0;
    virtual void ResumeStartup() = 0;
  };
  void Attach(int session_id);
  void Reattach(int session_id, const String& saved_state);
  void Detach(int session_id);
  void DispatchOnInspectorBackend(int session_id,
                                  int call_id,
                                  const String& method,
                                  const String& message);
  // ------ End deprecated ------

  static WebDevToolsAgentImpl* CreateForFrame(WebLocalFrameImpl*);
  static WebDevToolsAgentImpl* CreateForWorker(WebLocalFrameImpl*,
                                               WorkerClient*);
  ~WebDevToolsAgentImpl() override;
  virtual void Trace(blink::Visitor*);

  void WillBeDestroyed();
  void FlushProtocolNotifications();
  void PaintOverlay();
  void LayoutOverlay();
  bool HandleInputEvent(const WebInputEvent&);
  void DispatchBufferedTouchEvents();
  void BindRequest(mojom::blink::DevToolsAgentAssociatedRequest);

  // Instrumentation from web/ layer.
  void DidCommitLoadForLocalFrame(LocalFrame*);
  void DidStartProvisionalLoad(LocalFrame*);
  bool ScreencastEnabled();
  void LayerTreeViewChanged(WebLayerTreeView*);
  void RootLayerCleared();
  String EvaluateInOverlayForTesting(const String& script);

 private:
  friend class ClientMessageLoopAdapter;

  WebDevToolsAgentImpl(WebLocalFrameImpl*,
                       bool include_view_agents,
                       WorkerClient*);

  // mojom::blink::DevToolsAgent implementation.
  void AttachDevToolsSession(
      mojom::blink::DevToolsSessionHostAssociatedPtrInfo,
      mojom::blink::DevToolsSessionAssociatedRequest main_session,
      mojom::blink::DevToolsSessionRequest io_session,
      const String& reattach_state) override;

  // InspectorTracingAgent::Client implementation.
  void ShowReloadingBlanket() override;
  void HideReloadingBlanket() override;

  // InspectorPageAgent::Client implementation.
  void PageLayoutInvalidated(bool resized) override;

  // InspectorLayerTreeAgent::Client implementation.
  bool IsInspectorLayer(GraphicsLayer*) override;

  // InspectorSession::Client implementation.
  void SendProtocolMessage(int session_id,
                           int call_id,
                           const String& response,
                           const String& state) override;

  // WebThread::TaskObserver implementation.
  void WillProcessTask() override;
  void DidProcessTask() override;

  InspectorSession* InitializeSession(int session_id,
                                      String* state);
  void DetachSession(int session_id);
  void DestroySession(int session_id);
  void DispatchMessageFromFrontend(int session_id,
                                   const String& method,
                                   const String& message);

  bool Attached() const { return !!sessions_.size(); }
  void InspectElementAt(int session_id, const WebPoint&);

  class Session;
  class IOSession;

  mojo::AssociatedBinding<mojom::blink::DevToolsAgent> binding_;
  // TODO(dgozman): drop session ids after workers migrate to Mojo.
  int last_session_id_ = 0;
  HeapHashMap<int, Member<Session>> main_sessions_;
  HashMap<int, IOSession*> io_sessions_;
  HashMap<int, mojom::blink::DevToolsSessionHostAssociatedPtr> hosts_;

  WorkerClient* worker_client_;
  Member<WebLocalFrameImpl> web_local_frame_impl_;

  Member<CoreProbeSink> probe_sink_;
  Member<InspectorResourceContentLoader> resource_content_loader_;
  Member<InspectedFrames> inspected_frames_;
  Member<InspectorResourceContainer> resource_container_;

  HeapHashMap<int, Member<InspectorPageAgent>> page_agents_;
  HeapHashMap<int, Member<InspectorNetworkAgent>> network_agents_;
  HeapHashMap<int, Member<InspectorTracingAgent>> tracing_agents_;
  HeapHashMap<int, Member<InspectorOverlayAgent>> overlay_agents_;

  HeapHashMap<int, Member<InspectorSession>> sessions_;
  bool include_view_agents_;
  int layer_tree_id_;
};

}  // namespace blink

#endif
