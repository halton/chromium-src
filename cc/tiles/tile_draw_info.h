// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TILES_TILE_DRAW_INFO_H_
#define CC_TILES_TILE_DRAW_INFO_H_

#include <memory>

#include "base/trace_event/trace_event_argument.h"
#include "cc/resources/resource_provider.h"
#include "cc/resources/scoped_resource.h"
#include "components/viz/common/resources/platform_color.h"
#include "components/viz/common/resources/resource_format_utils.h"

namespace cc {

// This class holds all the state relevant to drawing a tile.
class CC_EXPORT TileDrawInfo {
 public:
  enum Mode { RESOURCE_MODE, SOLID_COLOR_MODE, OOM_MODE };

  TileDrawInfo();
  ~TileDrawInfo();

  Mode mode() const { return mode_; }

  bool IsReadyToDraw() const {
    switch (mode_) {
      case RESOURCE_MODE:
        return is_resource_ready_to_draw_;
      case SOLID_COLOR_MODE:
      case OOM_MODE:
        return true;
    }
    NOTREACHED();
    return false;
  }
  bool NeedsRaster() const {
    switch (mode_) {
      case RESOURCE_MODE:
        return !resource_;
      case SOLID_COLOR_MODE:
        return false;
      case OOM_MODE:
        return true;
    }
    NOTREACHED();
    return false;
  }

  viz::ResourceId resource_id() const {
    DCHECK(mode_ == RESOURCE_MODE);
    DCHECK(resource_);
    return resource_->id();
  }

  const gfx::Size& resource_size() const {
    DCHECK(mode_ == RESOURCE_MODE);
    DCHECK(resource_);
    return resource_->size();
  }

  SkColor solid_color() const {
    DCHECK(mode_ == SOLID_COLOR_MODE);
    return solid_color_;
  }

  bool contents_swizzled() const { return contents_swizzled_; }

  bool requires_resource() const {
    return mode_ == RESOURCE_MODE || mode_ == OOM_MODE;
  }

  inline bool has_resource() const { return !!resource_; }

  inline bool has_compressed_resource() const {
    return resource_ ? IsResourceFormatCompressed(resource_->format()) : false;
  }

  bool is_checker_imaged() const {
    DCHECK(!resource_is_checker_imaged_ || resource_);
    return resource_is_checker_imaged_;
  }

  void SetSolidColorForTesting(SkColor color) { set_solid_color(color); }

  void AsValueInto(base::trace_event::TracedValue* state) const;

 private:
  friend class Tile;
  friend class TileManager;

  const Resource* resource() const { return resource_; }

  void set_resource(Resource* resource, bool resource_is_checker_imaged) {
    DCHECK(!resource_is_checker_imaged || resource)
        << "Need to have a resource for it to be checker-imaged";

    mode_ = RESOURCE_MODE;
    is_resource_ready_to_draw_ = false;
    resource_is_checker_imaged_ = resource_is_checker_imaged;
    resource_ = resource;
  }

  void set_resource_ready_for_draw() {
    is_resource_ready_to_draw_ = true;
  }

  Resource* TakeResource();

  void set_solid_color(const SkColor& color) {
    mode_ = SOLID_COLOR_MODE;
    solid_color_ = color;
  }

  void set_oom() { mode_ = OOM_MODE; }

  Mode mode_ = RESOURCE_MODE;
  SkColor solid_color_ = SK_ColorWHITE;
  Resource* resource_ = nullptr;
  bool contents_swizzled_ = false;
  bool is_resource_ready_to_draw_ = false;

  // Set to true if |resource_| was rasterized with checker-imaged content. The
  // flag can only be true iff we have a valid |resource_|.
  bool resource_is_checker_imaged_ = false;
};

}  // namespace cc

#endif  // CC_TILES_TILE_DRAW_INFO_H_
