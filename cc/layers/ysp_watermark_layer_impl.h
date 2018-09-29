// Copyright 2016 The Redcore Authors. All rights reserved.
//ysp+ { watermark }

#ifndef CC_LAYERS_YSP_WATERMARK_LAYER_IMPL_H_
#define CC_LAYERS_YSP_WATERMARK_LAYER_IMPL_H_
#if defined(REDCORE) && defined(WATERMARK) && !defined(IE_REDCORE)
#include <memory>
#include <string>
#include <vector>

#include "base/macros.h"
#include "base/strings/string16.h"
#include "base/memory/ptr_util.h"
#include "base/time/time.h"
#include "cc/base/cc_export.h"
#include "cc/debug/debug_rect_history.h"
#include "cc/layers/layer_impl.h"
#include "cc/resources/memory_history.h"
#include "cc/resources/scoped_resource.h"

class SkCanvas;
class SkPaint;
class SkTypeface;
struct SkRect;

namespace cc {

class FrameRateCounter;
class PaintTimeCounter;

class CC_EXPORT YSPWatermarkLayerImpl : public LayerImpl {
 public:
  static scoped_ptr<YSPWatermarkLayerImpl> Create(
      LayerTreeImpl* tree_impl,
      int id) {
    return make_scoped_ptr(new YSPWatermarkLayerImpl(tree_impl, id));
  }
  ~YSPWatermarkLayerImpl() override;

  scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;

  bool WillDraw(DrawMode draw_mode,
                ResourceProvider* resource_provider) override;
  void AppendQuads(RenderPass* render_pass,
                   AppendQuadsData* append_quads_data) override;
  void UpdateWatermarkTexture(DrawMode draw_mode,
                              ResourceProvider* resource_provider);

  void ReleaseResources() override;

  gfx::Rect GetEnclosingRectInTargetSpace() const override;

  void SetTypeface(const skia::RefPtr<SkTypeface>& typeface);

  // LayerImpl overrides.
  void PushPropertiesTo(LayerImpl* layer) override;

 private:
  class Graph {
   public:
    Graph(double indicator_value, double start_upper_bound);

    // Eases the upper bound, which limits what is currently visible in the
    // graph, so that the graph always scales to either it's max or
    // default_upper_bound.
    double UpdateUpperBound();

    double value;
    double min;
    double max;

    double current_upper_bound;
    const double default_upper_bound;
    const double indicator;
  };

  YSPWatermarkLayerImpl(LayerTreeImpl* tree_impl, int id);

  const char* LayerTypeAsString() const override;

  void AsValueInto(base::trace_event::TracedValue* dict) const override;

  void DrawWatermarkContents(SkCanvas* canvas);

  int MeasureText(SkPaint* paint, const std::string& text, int size) const;
  void DrawText(SkCanvas* canvas,
    SkPaint* paint,
    const base::string16& text,
    SkPaint::Align align,
    int size,
    int x,
    int y) const;
  void DrawText(SkCanvas* canvas,
    SkPaint* paint,
    const base::string16& text,
    SkPaint::Align align,
    int size,
    const SkPoint& pos) const;

  SkRect DrawWatermark(SkCanvas* canvas) const;

  void AcquireResource(ResourceProvider* resource_provider);
  void ReleaseUnmatchedSizeResources(ResourceProvider* resource_provider);

  std::vector<scoped_ptr<ScopedResource>> resources_;
  skia::RefPtr<SkSurface> watermark_surface_;

  skia::RefPtr<SkTypeface> typeface_;

  float internal_contents_scale_;
  gfx::Size internal_content_bounds_;
  std::vector<DebugRect> paint_rects_;
  std::vector<base::string16> watermark_string_;
  uint32_t watermark_color_;
  int watermark_font_size_;

  DISALLOW_COPY_AND_ASSIGN(YSPWatermarkLayerImpl);
};

}  // namespace cc
#endif //WATERMARK
#endif  // CC_LAYERS_YSP_WATERMARK_LAYER_IMPL_H_
