// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_IE_RENDER_MODE_IE_H_
#define CONTENT_COMMON_IE_RENDER_MODE_IE_H_

#include "content/common/IE/common_ie.h"

namespace ie {

enum RenderCoreType {
  NONE_CORE = -1,
  BLINK_CORE = 0,
  IE_CORE = 1
};

struct RenderMode {
  RenderCoreType core;
  ie::Version version;
  ie::Emulation emulation;
  RenderMode() {
    core = NONE_CORE;
    version = DOCNONE;
    emulation = EMULATION_NONE;
  }

  RenderMode(const RenderMode& mode) {
    memcpy(this, &mode, sizeof(RenderMode));
  }

  bool operator==(const RenderMode& mode) const {
    if (this->core == BLINK_CORE && this->core == mode.core)
      return true;
    if ((this->core == mode.core) && (this->version == mode.version) &&
        (this->emulation == mode.emulation))
      return true;
    return false;
  }

  bool operator!=(const RenderMode& mode) const {
    bool ret = (*this == mode);
    return !ret;
  }
};

}  // namespace ie

#endif  // CONTENT_COMMON_IE_VERSION_IE_H_
