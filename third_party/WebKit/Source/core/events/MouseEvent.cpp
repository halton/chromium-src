/*
 * Copyright (C) 2001 Peter Kelly (pmk@post.com)
 * Copyright (C) 2001 Tobias Anton (anton@stud.fbi.fh-darmstadt.de)
 * Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
 * Copyright (C) 2003, 2005, 2006, 2008 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "core/events/MouseEvent.h"

#include "core/dom/Element.h"
#include "core/dom/events/EventDispatcher.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/LocalFrameView.h"
#include "core/frame/UseCounter.h"
#include "core/input/InputDeviceCapabilities.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutView.h"
#include "core/paint/PaintLayer.h"
#include "core/svg/SVGElement.h"
#include "platform/bindings/DOMWrapperWorld.h"
#include "platform/bindings/ScriptState.h"
#include "public/platform/WebPointerProperties.h"

namespace blink {

namespace {

DoubleSize ContentsScrollOffset(AbstractView* abstract_view) {
  if (!abstract_view || !abstract_view->IsLocalDOMWindow())
    return DoubleSize();
  LocalFrame* frame = ToLocalDOMWindow(abstract_view)->GetFrame();
  if (!frame)
    return DoubleSize();
  ScrollableArea* scrollable_area =
      frame->View()->LayoutViewportScrollableArea();
  if (!scrollable_area)
    return DoubleSize();
  float scale_factor = frame->PageZoomFactor();
  return DoubleSize(scrollable_area->ScrollOffsetInt().Width() / scale_factor,
                    scrollable_area->ScrollOffsetInt().Height() / scale_factor);
}

float PageZoomFactor(const UIEvent* event) {
  if (!event->view() || !event->view()->IsLocalDOMWindow())
    return 1;
  LocalFrame* frame = ToLocalDOMWindow(event->view())->GetFrame();
  if (!frame)
    return 1;
  return frame->PageZoomFactor();
}

const LayoutObject* FindTargetLayoutObject(Node*& target_node) {
  LayoutObject* layout_object = target_node->GetLayoutObject();
  if (!layout_object || !layout_object->IsSVG())
    return layout_object;
  // If this is an SVG node, compute the offset to the padding box of the
  // outermost SVG root (== the closest ancestor that has a CSS layout box.)
  while (!layout_object->IsSVGRoot())
    layout_object = layout_object->Parent();
  // Update the target node to point to the SVG root.
  target_node = layout_object->GetNode();
  DCHECK(!target_node ||
         (target_node->IsSVGElement() &&
          ToSVGElement(*target_node).IsOutermostSVGSVGElement()));
  return layout_object;
}

}  // namespace

MouseEvent* MouseEvent::Create(ScriptState* script_state,
                               const AtomicString& type,
                               const MouseEventInit& initializer) {
  if (script_state && script_state->World().IsIsolatedWorld())
    UIEventWithKeyState::DidCreateEventInIsolatedWorld(
        initializer.ctrlKey(), initializer.altKey(), initializer.shiftKey(),
        initializer.metaKey());
  return new MouseEvent(type, initializer);
}

MouseEvent* MouseEvent::Create(const AtomicString& event_type,
                               AbstractView* view,
                               const WebMouseEvent& event,
                               int detail,
                               const String& canvas_region_id,
                               Node* related_target) {
  bool is_mouse_enter_or_leave = event_type == EventTypeNames::mouseenter ||
                                 event_type == EventTypeNames::mouseleave;
  bool is_cancelable = !is_mouse_enter_or_leave;
  bool is_bubbling = !is_mouse_enter_or_leave;
  return new MouseEvent(event_type, is_bubbling, is_cancelable, view, event,
                        detail, canvas_region_id, related_target);
}

MouseEvent* MouseEvent::Create(const AtomicString& event_type,
                               AbstractView* view,
                               Event* underlying_event,
                               SimulatedClickCreationScope creation_scope) {
  WebInputEvent::Modifiers modifiers = WebInputEvent::kNoModifiers;
  if (UIEventWithKeyState* key_state_event =
          FindEventWithKeyState(underlying_event)) {
    modifiers = key_state_event->GetModifiers();
  }

  SyntheticEventType synthetic_type = kPositionless;
  double screen_x = 0;
  double screen_y = 0;
  if (underlying_event && underlying_event->IsMouseEvent()) {
    synthetic_type = kRealOrIndistinguishable;
    MouseEvent* mouse_event = ToMouseEvent(underlying_event);
    screen_x = mouse_event->screenX();
    screen_y = mouse_event->screenY();
  }

  TimeTicks timestamp = underlying_event ? underlying_event->PlatformTimeStamp()
                                         : CurrentTimeTicks();
  MouseEvent* created_event = new MouseEvent(
      event_type, true, true, view, 0, screen_x, screen_y, 0, 0, 0, 0,
      modifiers, 0, 0, nullptr, timestamp, synthetic_type, String());

  created_event->SetTrusted(creation_scope ==
                            SimulatedClickCreationScope::kFromUserAgent);
  created_event->SetUnderlyingEvent(underlying_event);
  if (synthetic_type == kRealOrIndistinguishable) {
    MouseEvent* mouse_event = ToMouseEvent(created_event->UnderlyingEvent());
    created_event->InitCoordinates(mouse_event->clientX(),
                                   mouse_event->clientY());
  }

  return created_event;
}

MouseEvent::MouseEvent()
    : position_type_(PositionType::kPosition),
      has_cached_relative_position_(false),
      button_(0),
      buttons_(0),
      related_target_(nullptr),
      synthetic_event_type_(kRealOrIndistinguishable) {}

MouseEvent::MouseEvent(const AtomicString& event_type,
                       bool can_bubble,
                       bool cancelable,
                       AbstractView* abstract_view,
                       const WebMouseEvent& event,
                       int detail,
                       const String& region,
                       EventTarget* related_target)
    : UIEventWithKeyState(
          event_type,
          can_bubble,
          cancelable,
          abstract_view,
          detail,
          static_cast<WebInputEvent::Modifiers>(event.GetModifiers()),
          TimeTicks::FromSeconds(event.TimeStampSeconds()),
          abstract_view
              ? abstract_view->GetInputDeviceCapabilities()->FiresTouchEvents(
                    event.FromTouch())
              : nullptr),
      screen_location_(event.PositionInScreen().x, event.PositionInScreen().y),
      movement_delta_(event.MovementInRootFrame()),
      position_type_(PositionType::kPosition),
      button_(static_cast<short>(event.button)),
      buttons_(WebInputEventModifiersToButtons(event.GetModifiers())),
      related_target_(related_target),
      synthetic_event_type_(event.FromTouch() ? kFromTouch
                                              : kRealOrIndistinguishable),
      region_(region),
      menu_source_type_(event.menu_source_type) {
  FloatPoint root_frame_coordinates = event.PositionInRootFrame();
  InitCoordinatesFromRootFrame(root_frame_coordinates.X(),
                               root_frame_coordinates.Y());
}

MouseEvent::MouseEvent(const AtomicString& event_type,
                       bool can_bubble,
                       bool cancelable,
                       AbstractView* abstract_view,
                       int detail,
                       double screen_x,
                       double screen_y,
                       double window_x,
                       double window_y,
                       double movement_x,
                       double movement_y,
                       WebInputEvent::Modifiers modifiers,
                       short button,
                       unsigned short buttons,
                       EventTarget* related_target,
                       TimeTicks platform_time_stamp,
                       SyntheticEventType synthetic_event_type,
                       const String& region)
    : UIEventWithKeyState(
          event_type,
          can_bubble,
          cancelable,
          abstract_view,
          detail,
          modifiers,
          platform_time_stamp,
          abstract_view
              ? abstract_view->GetInputDeviceCapabilities()->FiresTouchEvents(
                    synthetic_event_type == kFromTouch)
              : nullptr),
      screen_location_(screen_x, screen_y),
      movement_delta_(movement_x, movement_y),
      position_type_(synthetic_event_type == kPositionless
                         ? PositionType::kPositionless
                         : PositionType::kPosition),
      button_(button),
      buttons_(buttons),
      related_target_(related_target),
      synthetic_event_type_(synthetic_event_type),
      region_(region) {
  InitCoordinatesFromRootFrame(window_x, window_y);
}

MouseEvent::MouseEvent(const AtomicString& event_type,
                       const MouseEventInit& initializer,
                       TimeTicks platform_time_stamp)
    : UIEventWithKeyState(event_type, initializer, platform_time_stamp),
      screen_location_(
          DoublePoint(initializer.screenX(), initializer.screenY())),
      movement_delta_(
          IntPoint(initializer.movementX(), initializer.movementY())),
      position_type_(PositionType::kPosition),
      button_(initializer.button()),
      buttons_(initializer.buttons()),
      related_target_(initializer.relatedTarget()),
      synthetic_event_type_(kRealOrIndistinguishable),
      region_(initializer.region()) {
  InitCoordinates(initializer.clientX(), initializer.clientY());
}

void MouseEvent::InitCoordinates(const double client_x, const double client_y) {
  // Set up initial values for coordinates.
  // Correct values are computed lazily, see computeRelativePosition.
  client_location_ = DoublePoint(client_x, client_y);
  page_location_ = client_location_ + ContentsScrollOffset(view());

  layer_location_ = page_location_;
  offset_location_ = page_location_;

  ComputePageLocation();
  has_cached_relative_position_ = false;
}

void MouseEvent::InitCoordinatesFromRootFrame(double window_x,
                                              double window_y) {
  DoublePoint adjusted_page_location;
  DoubleSize scroll_offset;

  LocalFrame* frame = view() && view()->IsLocalDOMWindow()
                          ? ToLocalDOMWindow(view())->GetFrame()
                          : nullptr;
  if (frame && HasPosition()) {
    scroll_offset = ContentsScrollOffset(view());
    if (LocalFrameView* frame_view = frame->View()) {
      adjusted_page_location =
          frame_view->RootFrameToDocument(FloatPoint(window_x, window_y));
      float scale_factor = 1 / frame->PageZoomFactor();
      if (scale_factor != 1.0f)
        adjusted_page_location.Scale(scale_factor, scale_factor);
    }
  }

  client_location_ = adjusted_page_location - scroll_offset;
  page_location_ = adjusted_page_location;

  // Set up initial values for coordinates.
  // Correct values are computed lazily, see computeRelativePosition.
  layer_location_ = page_location_;
  offset_location_ = page_location_;

  ComputePageLocation();
  has_cached_relative_position_ = false;
}

MouseEvent::~MouseEvent() = default;

unsigned short MouseEvent::WebInputEventModifiersToButtons(unsigned modifiers) {
  unsigned short buttons = 0;

  if (modifiers & WebInputEvent::kLeftButtonDown)
    buttons |=
        static_cast<unsigned short>(WebPointerProperties::Buttons::kLeft);
  if (modifiers & WebInputEvent::kRightButtonDown) {
    buttons |=
        static_cast<unsigned short>(WebPointerProperties::Buttons::kRight);
  }
  if (modifiers & WebInputEvent::kMiddleButtonDown) {
    buttons |=
        static_cast<unsigned short>(WebPointerProperties::Buttons::kMiddle);
  }
  if (modifiers & WebInputEvent::kBackButtonDown)
    buttons |=
        static_cast<unsigned short>(WebPointerProperties::Buttons::kBack);
  if (modifiers & WebInputEvent::kForwardButtonDown) {
    buttons |=
        static_cast<unsigned short>(WebPointerProperties::Buttons::kForward);
  }

  return buttons;
}

void MouseEvent::initMouseEvent(ScriptState* script_state,
                                const AtomicString& type,
                                bool can_bubble,
                                bool cancelable,
                                AbstractView* view,
                                int detail,
                                int screen_x,
                                int screen_y,
                                int client_x,
                                int client_y,
                                bool ctrl_key,
                                bool alt_key,
                                bool shift_key,
                                bool meta_key,
                                short button,
                                EventTarget* related_target,
                                unsigned short buttons) {
  if (IsBeingDispatched())
    return;

  if (script_state && script_state->World().IsIsolatedWorld())
    UIEventWithKeyState::DidCreateEventInIsolatedWorld(ctrl_key, alt_key,
                                                       shift_key, meta_key);

  InitModifiers(ctrl_key, alt_key, shift_key, meta_key);
  InitMouseEventInternal(type, can_bubble, cancelable, view, detail, screen_x,
                         screen_y, client_x, client_y, GetModifiers(), button,
                         related_target, nullptr, buttons);
}

void MouseEvent::InitMouseEventInternal(
    const AtomicString& type,
    bool can_bubble,
    bool cancelable,
    AbstractView* view,
    int detail,
    double screen_x,
    double screen_y,
    double client_x,
    double client_y,
    WebInputEvent::Modifiers modifiers,
    short button,
    EventTarget* related_target,
    InputDeviceCapabilities* source_capabilities,
    unsigned short buttons) {
  InitUIEventInternal(type, can_bubble, cancelable, related_target, view,
                      detail, source_capabilities);

  screen_location_ = DoublePoint(screen_x, screen_y);
  button_ = button;
  buttons_ = buttons;
  related_target_ = related_target;
  modifiers_ = modifiers;

  InitCoordinates(client_x, client_y);

  // FIXME: SyntheticEventType is not set to RealOrIndistinguishable here.
}

const AtomicString& MouseEvent::InterfaceName() const {
  return EventNames::MouseEvent;
}

bool MouseEvent::IsMouseEvent() const {
  return true;
}

short MouseEvent::button() const {
  const AtomicString& event_name = type();
  if (button_ == -1 || event_name == EventTypeNames::mousemove ||
      event_name == EventTypeNames::mouseleave ||
      event_name == EventTypeNames::mouseenter ||
      event_name == EventTypeNames::mouseover ||
      event_name == EventTypeNames::mouseout) {
    return 0;
  }
  return button_;
}

unsigned MouseEvent::which() const {
  // For the DOM, the return values for left, middle and right mouse buttons are
  // 0, 1, 2, respectively.
  // For the Netscape "which" property, the return values for left, middle and
  // right mouse buttons are 1, 2, 3, respectively.
  // So we must add 1.
  return (unsigned)(button_ + 1);
}

Node* MouseEvent::toElement() const {
  // MSIE extension - "the object toward which the user is moving the mouse
  // pointer"
  if (type() == EventTypeNames::mouseout ||
      type() == EventTypeNames::mouseleave)
    return relatedTarget() ? relatedTarget()->ToNode() : nullptr;

  return target() ? target()->ToNode() : nullptr;
}

Node* MouseEvent::fromElement() const {
  // MSIE extension - "object from which activation or the mouse pointer is
  // exiting during the event" (huh?)
  if (type() != EventTypeNames::mouseout &&
      type() != EventTypeNames::mouseleave)
    return relatedTarget() ? relatedTarget()->ToNode() : nullptr;

  return target() ? target()->ToNode() : nullptr;
}

void MouseEvent::Trace(blink::Visitor* visitor) {
  visitor->Trace(related_target_);
  UIEventWithKeyState::Trace(visitor);
}

DispatchEventResult MouseEvent::DispatchEvent(EventDispatcher& dispatcher) {
  GetEventPath().AdjustForRelatedTarget(dispatcher.GetNode(), relatedTarget());

  bool is_click = type() == EventTypeNames::click;
  bool send_to_disabled_form_controls =
      RuntimeEnabledFeatures::SendMouseEventsDisabledFormControlsEnabled();

  if (send_to_disabled_form_controls && is_click &&
      GetEventPath().DisabledFormControlExistsInPath()) {
    return DispatchEventResult::kCanceledBeforeDispatch;
  }

  if (!isTrusted())
    return dispatcher.Dispatch();

  if (!send_to_disabled_form_controls &&
      IsDisabledFormControl(&dispatcher.GetNode())) {
    if (GetEventPath().HasEventListenersInPath(type())) {
      UseCounter::Count(dispatcher.GetNode().GetDocument(),
                        WebFeature::kDispatchMouseEventOnDisabledFormControl);
      if (type() == EventTypeNames::mousedown ||
          type() == EventTypeNames::mouseup) {
        UseCounter::Count(
            dispatcher.GetNode().GetDocument(),
            WebFeature::kDispatchMouseUpDownEventOnDisabledFormControl);
      }
    }
    return DispatchEventResult::kCanceledBeforeDispatch;
  }

  if (type().IsEmpty())
    return DispatchEventResult::kNotCanceled;  // Shouldn't happen.

  DCHECK(!target() || target() != relatedTarget());

  EventTarget* related_target = relatedTarget();

  DispatchEventResult dispatch_result = dispatcher.Dispatch();

  if (!is_click || detail() != 2)
    return dispatch_result;

  // Special case: If it's a double click event, we also send the dblclick
  // event. This is not part of the DOM specs, but is used for compatibility
  // with the ondblclick="" attribute. This is treated as a separate event in
  // other DOM-compliant browsers like Firefox, and so we do the same.
  MouseEvent* double_click_event = MouseEvent::Create();
  double_click_event->InitMouseEventInternal(
      EventTypeNames::dblclick, bubbles(), cancelable(), view(), detail(),
      screenX(), screenY(), clientX(), clientY(), GetModifiers(), button(),
      related_target, sourceCapabilities(), buttons());
  double_click_event->SetComposed(composed());

  // Inherit the trusted status from the original event.
  double_click_event->SetTrusted(isTrusted());
  if (DefaultHandled())
    double_click_event->SetDefaultHandled();
  DispatchEventResult double_click_dispatch_result =
      EventDispatcher::DispatchEvent(dispatcher.GetNode(), double_click_event);
  if (double_click_dispatch_result != DispatchEventResult::kNotCanceled)
    return double_click_dispatch_result;
  return dispatch_result;
}

void MouseEvent::ComputePageLocation() {
  LocalFrame* frame = view() && view()->IsLocalDOMWindow()
                          ? ToLocalDOMWindow(view())->GetFrame()
                          : nullptr;
  if (frame && frame->View())
    absolute_location_ = frame->View()->DocumentToAbsolute(page_location_);
  else
    absolute_location_ = page_location_;

  float scale_factor = PageZoomFactor(this);
  absolute_location_.Scale(scale_factor, scale_factor);
}

void MouseEvent::ReceivedTarget() {
  has_cached_relative_position_ = false;
}

void MouseEvent::ComputeRelativePosition() {
  Node* target_node = target() ? target()->ToNode() : nullptr;
  if (!target_node)
    return;

  // Compute coordinates that are based on the target.
  layer_location_ = page_location_;
  offset_location_ = page_location_;

  // Must have an updated layout tree for this math to work correctly.
  target_node->GetDocument().UpdateStyleAndLayoutIgnorePendingStylesheets();

  // Adjust offsetLocation to be relative to the target's padding box.
  if (const LayoutObject* layout_object = FindTargetLayoutObject(target_node)) {
    FloatPoint local_pos = layout_object->AbsoluteToLocal(
        FloatPoint(AbsoluteLocation()), kUseTransforms);

    // Adding this here to address crbug.com/570666. Basically we'd like to
    // find the local coordinates relative to the padding box not the border
    // box.
    if (layout_object->IsBoxModelObject()) {
      const LayoutBoxModelObject* layout_box =
          ToLayoutBoxModelObject(layout_object);
      local_pos.Move(-layout_box->BorderLeft(), -layout_box->BorderTop());
    }

    offset_location_ = DoublePoint(local_pos);
    float scale_factor = 1 / PageZoomFactor(this);
    if (scale_factor != 1.0f)
      offset_location_.Scale(scale_factor, scale_factor);
  }

  // Adjust layerLocation to be relative to the layer.
  // FIXME: event.layerX and event.layerY are poorly defined,
  // and probably don't always correspond to PaintLayer offsets.
  // https://bugs.webkit.org/show_bug.cgi?id=21868
  Node* n = target_node;
  while (n && !n->GetLayoutObject())
    n = n->parentNode();

  if (n) {
    if (LocalFrameView* view = n->GetLayoutObject()->View()->GetFrameView())
      layer_location_ = view->DocumentToAbsolute(page_location_);

    // FIXME: This logic is a wrong implementation of convertToLayerCoords.
    for (PaintLayer* layer = n->GetLayoutObject()->EnclosingLayer(); layer;
         layer = layer->Parent()) {
      layer_location_ -= DoubleSize(layer->Location().X().ToDouble(),
                                    layer->Location().Y().ToDouble());
    }
  }

  has_cached_relative_position_ = true;
}

int MouseEvent::layerX() {
  if (!has_cached_relative_position_)
    ComputeRelativePosition();

  // TODO(mustaq): Remove the PointerEvent specific code when mouse has
  // fractional coordinates. See crbug.com/655786.
  return IsPointerEvent() ? layer_location_.X()
                          : static_cast<int>(layer_location_.X());
}

int MouseEvent::layerY() {
  if (!has_cached_relative_position_)
    ComputeRelativePosition();

  // TODO(mustaq): Remove the PointerEvent specific code when mouse has
  // fractional coordinates. See crbug.com/655786.
  return IsPointerEvent() ? layer_location_.Y()
                          : static_cast<int>(layer_location_.Y());
}

int MouseEvent::offsetX() {
  if (!HasPosition())
    return 0;
  if (!has_cached_relative_position_)
    ComputeRelativePosition();
  return std::round(offset_location_.X());
}

int MouseEvent::offsetY() {
  if (!HasPosition())
    return 0;
  if (!has_cached_relative_position_)
    ComputeRelativePosition();
  return std::round(offset_location_.Y());
}

}  // namespace blink
