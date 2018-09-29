// Copyright 2016 The Redcore Authors. All rights reserved.
//ysp+ { watermark }

#ifndef CC_YSP_WATERMARK_LAYER_H_
#define CC_YSP_WATERMARK_LAYER_H_
#if defined(REDCORE) && defined(WATERMARK) && !defined(IE_REDCORE)
#include <string>

#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/layers/layer.h"
#include "third_party/skia/include/core/SkTypeface.h"

namespace cc {

class CC_EXPORT YSPWatermarkLayer : public Layer {
public:
  static scoped_refptr<YSPWatermarkLayer> Create(
    const LayerSettings& settings);

  void PrepareForCalculateDrawProperties(
    const gfx::Size& device_viewport, float device_scale_factor);

  scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;

  // Layer overrides.
  void PushPropertiesTo(LayerImpl* layer) override;

protected:
  explicit YSPWatermarkLayer(const LayerSettings& settings);
  bool HasDrawableContent() const override;

private:
  ~YSPWatermarkLayer() override;

  skia::RefPtr<SkTypeface> typeface_;

  DISALLOW_COPY_AND_ASSIGN(YSPWatermarkLayer);
};

}  // namespace cc
#endif //WATERMARK
#endif  // CC_YSP_WATERMARK_LAYER_H_
