// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_IE_COMMON_IE_H_
#define CONTENT_COMMON_IE_COMMON_IE_H_

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

enum DocumentMode {
  IE11 = 0x20000,
  IE10 = 0x19000,
  IE9 = 0x18000,
  IE8 = 0x15000,
  IE7 = 0x13000,
  IE5 = 0x11000
};

enum NewWindowFlag {
  TRANSITION = 1,
  POPUP = 2,
  FROMDIALOG = 4
};

}  // namespace ie

#endif  // CONTENT_COMMON_IE_COMMON_IE_H_
