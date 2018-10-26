// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(REDCORE) && defined(WATERMARK) && !defined(IE_REDCORE)
// ysp+ { watermark }

#include "cc/layers/ysp_watermark_layer_impl.h"
#include <stddef.h>
#include <stdint.h>

#include <algorithm>
#include <vector>

#include "base/numerics/safe_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/debug/debug_colors.h"
#include "cc/debug/frame_rate_counter.h"
#include "cc/output/begin_frame_args.h"
#include "cc/quads/texture_draw_quad.h"
#include "cc/resources/memory_history.h"
#include "cc/trees/layer_tree_host_impl.h"
#include "cc/trees/layer_tree_impl.h"
#include "skia/ext/platform_canvas.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkPath.h"
#include "third_party/skia/include/core/SkTypeface.h"
#include "third_party/skia/include/effects/SkColorMatrixFilter.h"
#include "third_party/skia/include/effects/SkGradientShader.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/geometry/size_conversions.h"

namespace cc {

static inline SkPaint CreatePaint() {
  SkPaint paint;
#if (SK_R32_SHIFT || SK_B32_SHIFT != 16)
  // The SkCanvas is in RGBA but the shader is expecting BGRA, so we need to
  // swizzle our colors when drawing to the SkCanvas.
  SkColorMatrix swizzle_matrix;
  for (int i = 0; i < 20; ++i)
    swizzle_matrix.fMat[i] = 0;
  swizzle_matrix.fMat[0 + 5 * 2] = 1;
  swizzle_matrix.fMat[1 + 5 * 1] = 1;
  swizzle_matrix.fMat[2 + 5 * 0] = 1;
  swizzle_matrix.fMat[3 + 5 * 3] = 1;

  skia::RefPtr<SkColorFilter> filter =
      skia::AdoptRef(SkColorMatrixFilter::Create(swizzle_matrix));
  paint.setColorFilter(filter.get());
#endif
  paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
  return paint;
}

YSPWatermarkLayerImpl::Graph::Graph(double indicator_value,
                                    double start_upper_bound)
    : value(0.0),
      min(0.0),
      max(0.0),
      current_upper_bound(start_upper_bound),
      default_upper_bound(start_upper_bound),
      indicator(indicator_value) {}

double YSPWatermarkLayerImpl::Graph::UpdateUpperBound() {
  double target_upper_bound = std::max(max, default_upper_bound);
  current_upper_bound += (target_upper_bound - current_upper_bound) * 0.5;
  return current_upper_bound;
}

YSPWatermarkLayerImpl::YSPWatermarkLayerImpl(LayerTreeImpl* tree_impl, int id)
    : LayerImpl(tree_impl, id), internal_contents_scale_(1.f) {}

YSPWatermarkLayerImpl::~YSPWatermarkLayerImpl() {}

scoped_ptr<LayerImpl> YSPWatermarkLayerImpl::CreateLayerImpl(
    LayerTreeImpl* tree_impl) {
  return YSPWatermarkLayerImpl::Create(tree_impl, id());
}

void YSPWatermarkLayerImpl::AcquireResource(
    ResourceProvider* resource_provider) {
  watermark_string_ = ResourceBundle::GetSharedInstance().watermark_string();
  watermark_color_ = ResourceBundle::GetSharedInstance().watermark_color();
  watermark_font_size_ =
      ResourceBundle::GetSharedInstance().watermark_font_size();

  for (auto& resource : resources_) {
    if (!resource_provider->InUseByConsumer(resource->id())) {
      resource.swap(resources_.back());
      return;
    }
  }

  scoped_ptr<ScopedResource> resource =
      ScopedResource::Create(resource_provider);
  resource->Allocate(internal_content_bounds_,
                     ResourceProvider::TEXTURE_HINT_IMMUTABLE,
                     resource_provider->best_texture_format());
  resources_.push_back(std::move(resource));
}

void YSPWatermarkLayerImpl::ReleaseUnmatchedSizeResources(
    ResourceProvider* resource_provider) {
  auto it_erase =
      std::remove_if(resources_.begin(), resources_.end(),
                     [this](const scoped_ptr<ScopedResource>& resource) {
                       return internal_content_bounds_ != resource->size();
                     });
  resources_.erase(it_erase, resources_.end());
}

bool YSPWatermarkLayerImpl::WillDraw(DrawMode draw_mode,
                                     ResourceProvider* resource_provider) {
  if (draw_mode == DRAW_MODE_RESOURCELESS_SOFTWARE)
    return false;

  internal_contents_scale_ = GetIdealContentsScale();
  internal_content_bounds_ =
      gfx::ScaleToCeiledSize(bounds(), internal_contents_scale_);
  internal_content_bounds_.SetToMin(
      gfx::Size(resource_provider->max_texture_size(),
                resource_provider->max_texture_size()));

  ReleaseUnmatchedSizeResources(resource_provider);
  AcquireResource(resource_provider);
  return LayerImpl::WillDraw(draw_mode, resource_provider);
}

void YSPWatermarkLayerImpl::AppendQuads(RenderPass* render_pass,
                                        AppendQuadsData* append_quads_data) {
  if (!resources_.back()->id())
    return;

  SharedQuadState* shared_quad_state =
      render_pass->CreateAndAppendSharedQuadState();
  PopulateScaledSharedQuadState(shared_quad_state, internal_contents_scale_);

  gfx::Rect quad_rect(internal_content_bounds_);
  gfx::Rect opaque_rect(contents_opaque() ? quad_rect : gfx::Rect());
  gfx::Rect visible_quad_rect(quad_rect);
  bool premultiplied_alpha = true;
  gfx::PointF uv_top_left(0.f, 0.f);
  gfx::PointF uv_bottom_right(1.f, 1.f);
  const float vertex_opacity[] = {1.f, 1.f, 1.f, 1.f};
  bool flipped = false;
  bool nearest_neighbor = false;
  TextureDrawQuad* quad =
      render_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
  quad->SetNew(shared_quad_state, quad_rect, opaque_rect, visible_quad_rect,
               resources_.back()->id(), premultiplied_alpha, uv_top_left,
               uv_bottom_right, SK_ColorTRANSPARENT, vertex_opacity, flipped,
               nearest_neighbor);
  ValidateQuadResources(quad);
}

void YSPWatermarkLayerImpl::UpdateWatermarkTexture(
    DrawMode draw_mode,
    ResourceProvider* resource_provider) {
  if (draw_mode == DRAW_MODE_RESOURCELESS_SOFTWARE || !resources_.back()->id())
    return;

  SkISize canvas_size;
  if (watermark_surface_)
    canvas_size = watermark_surface_->getCanvas()->getBaseLayerSize();
  else
    canvas_size.set(0, 0);

  if (canvas_size.width() != internal_content_bounds_.width() ||
      canvas_size.height() != internal_content_bounds_.height() ||
      !watermark_surface_) {
    TRACE_EVENT0("cc", "ResizeHudCanvas");

    watermark_surface_ = skia::AdoptRef(SkSurface::NewRasterN32Premul(
        internal_content_bounds_.width(), internal_content_bounds_.height()));
  }

  {
    TRACE_EVENT0("cc", "DrawHudContents");
    watermark_surface_->getCanvas()->clear(SkColorSetARGB(0, 0, 0, 0));
    watermark_surface_->getCanvas()->save();
    watermark_surface_->getCanvas()->scale(internal_contents_scale_,
                                           internal_contents_scale_);

    DrawWatermark(watermark_surface_->getCanvas());

    watermark_surface_->getCanvas()->restore();
  }

  TRACE_EVENT0("cc", "UploadHudTexture");
  SkImageInfo info;
  size_t row_bytes = 0;
  const void* pixels =
      watermark_surface_->getCanvas()->peekPixels(&info, &row_bytes);
  DCHECK(pixels);
  DCHECK(info.colorType() == kN32_SkColorType);
  resource_provider->CopyToResource(resources_.back()->id(),
                                    static_cast<const uint8_t*>(pixels),
                                    internal_content_bounds_);
}

void YSPWatermarkLayerImpl::ReleaseResources() {
  resources_.clear();
}

gfx::Rect YSPWatermarkLayerImpl::GetEnclosingRectInTargetSpace() const {
  DCHECK_GT(internal_contents_scale_, 0.f);
  return GetScaledEnclosingRectInTargetSpace(internal_contents_scale_);
}

void YSPWatermarkLayerImpl::SetTypeface(
    const skia::RefPtr<SkTypeface>& typeface) {
  if (typeface_ == typeface)
    return;

  DCHECK(typeface_.get() == nullptr);
  typeface_ = typeface;
  NoteLayerPropertyChanged();
}

void YSPWatermarkLayerImpl::PushPropertiesTo(LayerImpl* layer) {
  LayerImpl::PushPropertiesTo(layer);

  YSPWatermarkLayerImpl* layer_impl =
      static_cast<YSPWatermarkLayerImpl*>(layer);

  layer_impl->SetTypeface(typeface_);
}

void YSPWatermarkLayerImpl::DrawWatermarkContents(SkCanvas* canvas) {
  const LayerTreeDebugState& debug_state = layer_tree_impl()->debug_state();
  if (!debug_state.show_watermark)
    return;

  DrawWatermark(canvas);
}

int YSPWatermarkLayerImpl::MeasureText(SkPaint* paint,
                                       const std::string& text,
                                       int size) const {
  DCHECK(typeface_.get());
  const bool anti_alias = paint->isAntiAlias();
  paint->setAntiAlias(true);
  paint->setTextSize(size);
  paint->setTypeface(typeface_.get());
  SkScalar text_width = paint->measureText(text.c_str(), text.length());

  paint->setAntiAlias(anti_alias);
  return SkScalarCeilToInt(text_width);
}

void YSPWatermarkLayerImpl::DrawText(SkCanvas* canvas,
                                     SkPaint* paint,
                                     const base::string16& text,
                                     SkPaint::Align align,
                                     int size,
                                     int x,
                                     int y) const {
  DCHECK(typeface_.get());
  const bool anti_alias = paint->isAntiAlias();
  paint->setAntiAlias(true);

  paint->setTextSize(size);
  paint->setTextAlign(align);
  paint->setTypeface(typeface_.get());
  canvas->drawText(text.c_str(), text.length() * 2, x, y, *paint);

  paint->setAntiAlias(anti_alias);
}

void YSPWatermarkLayerImpl::DrawText(SkCanvas* canvas,
                                     SkPaint* paint,
                                     const base::string16& text,
                                     SkPaint::Align align,
                                     int size,
                                     const SkPoint& pos) const {
  DrawText(canvas, paint, text, align, size, pos.x(), pos.y());
}

SkRect YSPWatermarkLayerImpl::DrawWatermark(SkCanvas* canvas) const {
  const int kPadding = 4;
  const int kGap = 6;

  const int kTitleFontHeight = 13;

  const int kGraphWidth = 60;

  const int kHistogramWidth = 37;

  int width = bounds().width();
  int height = bounds().height();
  int left = 0;
  SkRect area = SkRect::MakeXYWH(0, 0, width, height);

  if (watermark_string_.size() == 0 || typeface_.get() == nullptr)
    return area;

  SkPaint paint = CreatePaint();
  paint.setTextSize(watermark_font_size_);
  paint.setTextAlign(SkPaint::kLeft_Align);
  paint.setTypeface(typeface_.get());

  SkRect title_bounds = SkRect::MakeXYWH(
      left + kPadding, kPadding, kGraphWidth + kHistogramWidth + kGap + 2,
      kTitleFontHeight);
  SkRect text_bounds = SkRect::MakeXYWH(
      left + kPadding, title_bounds.bottom() + 2 * kPadding,
      kGraphWidth + kHistogramWidth + kGap + 2, watermark_font_size_);

  // const base::string16 value_text = L"SAMPLE WATERMARK";
  int watermark_item_loop = 0;
  int max_display_width = 0;
  for (; watermark_item_loop < static_cast<int>(watermark_string_.size());
       watermark_item_loop++) {
    int display_text_width =
        paint.measureText(watermark_string_[watermark_item_loop].c_str(),
                          watermark_string_[watermark_item_loop].length() * 2);
    if (display_text_width > max_display_width)
      max_display_width = display_text_width;
  }
  watermark_item_loop = 0;

  paint.setColor(watermark_color_);
  int kColWidth = watermark_font_size_ * 2 + max_display_width;
  int kRowHeight = watermark_font_size_ * 1.5;
  int ox = text_bounds.left() - (float)height / 1.4;
  int dist = sqrt(width * width + height * height);
  int y = text_bounds.bottom();
  int xMax = ox + dist;
  int yMax = y + dist;
  while (y < yMax) {
    int x = ox;
    int display_text_width =
        paint.measureText(watermark_string_[watermark_item_loop].c_str(),
                          watermark_string_[watermark_item_loop].length() * 2);
    x += (kColWidth - display_text_width) / 2;
    while (x < xMax) {
      canvas->save();
      canvas->rotate(SkIntToScalar(-45));
      DrawText(canvas, &paint, watermark_string_[watermark_item_loop],
               SkPaint::kLeft_Align, watermark_font_size_, x, y);
      canvas->restore();
      x += kColWidth;
    }
    y += kRowHeight;
    watermark_item_loop = (watermark_item_loop + 1) % watermark_string_.size();
  }

  return area;
}

const char* YSPWatermarkLayerImpl::LayerTypeAsString() const {
  return "cc::YSPWatermarkLayerImpl";
}

void YSPWatermarkLayerImpl::AsValueInto(
    base::trace_event::TracedValue* dict) const {
  LayerImpl::AsValueInto(dict);
  dict->SetString("layer_name", "Heads Up Display Layer");
}

}  // namespace cc
#endif  // WATERMARK
