/*
 * Copyright (c) 2013, Opera Software ASA. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Opera Software ASA nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef VTTCue_h
#define VTTCue_h

#include "core/html/track/TextTrackCue.h"
#include "platform/heap/Handle.h"
#include "platform/wtf/Allocator.h"

namespace blink {

class Document;
class DoubleOrAutoKeyword;
class ExecutionContext;
class VTTCue;
class VTTRegion;
class VTTScanner;

using VTTRegionMap = HeapHashMap<String, Member<VTTRegion>>;

struct VTTDisplayParameters {
  STACK_ALLOCATED();
  VTTDisplayParameters();

  FloatPoint position;
  double size;
  CSSValueID direction;
  CSSValueID text_align;
  CSSValueID writing_mode;
  double snap_to_lines_position;
  int line_align;
};

class VTTCueBox final : public HTMLDivElement {
 public:
  static VTTCueBox* Create(Document& document) {
    return new VTTCueBox(document);
  }

  void ApplyCSSProperties(const VTTDisplayParameters&);

 private:
  explicit VTTCueBox(Document&);

  LayoutObject* CreateLayoutObject(const ComputedStyle&) override;

  // The computed line position for snap-to-lines layout, and NaN for
  // non-snap-to-lines layout where no adjustment should take place.
  // This is set in applyCSSProperties and propagated to LayoutVTTCue.
  float snap_to_lines_position_;
  int line_align_;
};

class VTTCue final : public TextTrackCue {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static VTTCue* Create(Document& document,
                        double start_time,
                        double end_time,
                        const String& text) {
    return new VTTCue(document, start_time, end_time, text);
  }

  ~VTTCue() override;

  VTTRegion* region() const { return region_; }
  void setRegion(VTTRegion*);

  const String& vertical() const;
  void setVertical(const String&);

  bool snapToLines() const { return snap_to_lines_; }
  void setSnapToLines(bool);

  void line(DoubleOrAutoKeyword&) const;
  void setLine(const DoubleOrAutoKeyword&);

  const String& lineAlign() const;
  void setLineAlign(const String&);

  void position(DoubleOrAutoKeyword&) const;
  void setPosition(const DoubleOrAutoKeyword&, ExceptionState&);

  const String& positionAlign() const;
  void setPositionAlign(const String&);

  double size() const { return cue_size_; }
  void setSize(double, ExceptionState&);

  const String& align() const;
  void setAlign(const String&);

  const String& text() const { return text_; }
  void setText(const String&);

  void ParseSettings(const VTTRegionMap*, const String&);

  // Applies CSS override style from user settings.
  void ApplyUserOverrideCSSProperties();

  DocumentFragment* getCueAsHTML();

  void UpdateDisplay(HTMLDivElement& container) override;

  void UpdatePastAndFutureNodes(double movie_time) override;

  void RemoveDisplayTree(RemovalNotification) override;

  double CalculateComputedLine() const;

  enum WritingDirection {
    kHorizontal = 0,
    kVerticalGrowingLeft,
    kVerticalGrowingRight,
    kNumberOfWritingDirections
  };
  WritingDirection GetWritingDirection() const { return writing_direction_; }

  enum class TextAlignment {
    kStart = 0,
    kCenter,
    kEnd,
    kLeft,
    kRight,
    kNumberOfAlignments
  };
  TextAlignment GetTextAlignment() const { return text_alignment_; }

  enum class PositionAlignment { kLineLeft, kCenter, kLineRight, kAuto };
  PositionAlignment GetPositionAlignment() const { return position_alignment_; }

  enum class LineAlignment { kStart, kCenter, kEnd };
  LineAlignment GetLineAlignment() const { return line_alignment_; }

  ExecutionContext* GetExecutionContext() const override;

#ifndef NDEBUG
  String ToString() const override;
#endif

  virtual void Trace(blink::Visitor*);

 private:
  VTTCue(Document&, double start_time, double end_time, const String& text);

  Document& GetDocument() const;

  VTTCueBox* GetDisplayTree();

  void CueDidChange(
      CueMutationAffectsOrder = kCueMutationDoesNotAffectOrder) override;

  void CreateVTTNodeTree();
  void CopyVTTNodeToDOMTree(ContainerNode* vtt_node, ContainerNode* root);

  bool LineIsAuto() const;
  bool TextPositionIsAuto() const;

  VTTDisplayParameters CalculateDisplayParameters() const;
  double CalculateComputedPosition() const;
  LineAlignment CalculateComputedLineAlignment() const;
  PositionAlignment CalculateComputedPositionAlignment() const;

  enum CueSetting {
    kNone,
    kVertical,
    kLine,
    kPosition,
    kSize,
    kAlign,
    kRegionId
  };
  CueSetting SettingName(VTTScanner&) const;

  String text_;
  double line_position_;
  double text_position_;
  double cue_size_;
  WritingDirection writing_direction_;
  TextAlignment text_alignment_;
  PositionAlignment position_alignment_;
  LineAlignment line_alignment_;

  Member<VTTRegion> region_;
  Member<DocumentFragment> vtt_node_tree_;
  Member<HTMLDivElement> cue_background_box_;
  Member<VTTCueBox> display_tree_;

  bool snap_to_lines_ : 1;
  bool display_tree_should_change_ : 1;
};

// VTTCue is currently the only TextTrackCue subclass.
DEFINE_TYPE_CASTS(VTTCue, TextTrackCue, cue, true, true);

}  // namespace blink

#endif  // VTTCue_h
