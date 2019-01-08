// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/dom_distiller/core/dom_distiller_constants.h"

namespace dom_distiller {

#ifdef REDCORE
const char kChromeUIDomDistillerURL[] = "ep://dom-distiller/";
#else
const char kChromeUIDomDistillerURL[] = "chrome://dom-distiller/";
#endif // REDCORE
const char kChromeUIDomDistillerHost[] = "dom-distiller";

}  // namespace dom_distiller
