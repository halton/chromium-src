// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// ysp+ { watermark }
#if defined(REDCORE) && defined(WATERMARK) && !defined(IE_REDCORE)
#include "cc/layers/ysp_watermark_layer.h"
#include <algorithm>
#include "base/trace_event/trace_event.h"
#include "cc/layers/ysp_watermark_layer_impl.h"
#include "cc/trees/layer_tree_host.h"

namespace cc {

scoped_refptr<YSPWatermarkLayer> YSPWatermarkLayer::Create(
    const LayerSettings& settings) {
  return make_scoped_refptr(new YSPWatermarkLayer(settings));
}

YSPWatermarkLayer::YSPWatermarkLayer(const LayerSettings& settings)
    : Layer(settings),
      typeface_(skia::AdoptRef(
#if defined(OS_MACOSX)
          SkTypeface::CreateFromName("Hiragino Sans GB",
                                     SkTypeface::kNormal))) {
#else
          SkTypeface::CreateFromName("SimHei", SkTypeface::kNormal))) {
#endif
  if (!typeface_) {
    typeface_ = skia::AdoptRef(
        SkTypeface::CreateFromName("monospace", SkTypeface::kBold));
  }
  DCHECK(typeface_.get());
  SetIsDrawable(true);
  UpdateDrawsContent(HasDrawableContent());
}

YSPWatermarkLayer::~YSPWatermarkLayer() {}

void YSPWatermarkLayer::PrepareForCalculateDrawProperties(
    const gfx::Size& device_viewport,
    float device_scale_factor) {
  gfx::Size device_viewport_in_layout_pixels =
      gfx::Size(device_viewport.width() / device_scale_factor,
                device_viewport.height() / device_scale_factor);

  gfx::Size bounds;
  gfx::Transform matrix;
  matrix.MakeIdentity();

  if (layer_tree_host()->debug_state().show_watermark) {
    int max_texture_size =
        layer_tree_host()->GetRendererCapabilities().max_texture_size;
    bounds.SetSize(
        std::min(max_texture_size, device_viewport_in_layout_pixels.width()),
        std::min(max_texture_size, device_viewport_in_layout_pixels.height()));
  } else {
    int size = 256;
    bounds.SetSize(size, size);
    matrix.Translate(device_viewport_in_layout_pixels.width() - size, 0.0);
  }

  SetBounds(bounds);
  SetTransform(matrix);
}

bool YSPWatermarkLayer::HasDrawableContent() const {
  return true;
}

scoped_ptr<LayerImpl> YSPWatermarkLayer::CreateLayerImpl(
    LayerTreeImpl* tree_impl) {
  return YSPWatermarkLayerImpl::Create(tree_impl, layer_id_);
}

void YSPWatermarkLayer::PushPropertiesTo(LayerImpl* layer) {
  Layer::PushPropertiesTo(layer);
  TRACE_EVENT0("cc", "YSPWatermarkLayer::PushPropertiesTo");
  YSPWatermarkLayerImpl* layer_impl =
      static_cast<YSPWatermarkLayerImpl*>(layer);

  layer_impl->SetTypeface(typeface_);
}

}  // namespace cc
#endif  // WATERMARK
