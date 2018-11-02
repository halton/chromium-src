// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_IE_VERSION_IE_H_
#define CONTENT_COMMON_IE_VERSION_IE_H_

namespace ie {

enum Version {
  DOCNONE = -1,
  DOCSYS = 1,
  DOC6 = 5,
  DOC7 = 7,
  DOC8 = 8,
  DOC9 = 9,
  DOC10 = 10,
  DOC11 = 11
};

enum Emulation {
  EMULATION_NONE = -1,
  EMULATION7 = 7,
  EMULATION8 = 8,
  EMULATION9 = 9,
  EMULATION10 = 10,
  EMULATION11 = 11
};

enum DownloadStatus {
  NONESTATUS = 0,
  BEGINDOWNLOAD = 1,
  DOWNLOADINGDATA = 2,
  ENDDOWNLOAD = 3,
  DOWNLOADERROR = 4
};

}  // namespace ie

enum RenderCore { NONE_CORE = -1, BLINK_CORE = 0, IE_CORE = 1 };

struct RendererMode {
  RenderCore core;
  ie::Version version;
  ie::Emulation emulation;
  RendererMode() {
    core = NONE_CORE;
    version = ie::DOCNONE;
    emulation = ie::EMULATION_NONE;
  }

  bool operator==(const RendererMode& mode) const {
    if (this->core == BLINK_CORE && this->core == mode.core)
      return true;
    if ((this->core == mode.core) && (this->version == mode.version) &&
        (this->emulation == mode.emulation))
      return true;
    return false;
  }

  bool operator!=(const RendererMode& mode) const {
    bool ret = (*this == mode);
    return !ret;
  }
};

#endif  // CONTENT_COMMON_IE_VERSION_IE_H_
