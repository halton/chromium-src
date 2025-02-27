// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/media/router/test/mock_media_router.h"

namespace media_router {

// static
std::unique_ptr<KeyedService> MockMediaRouter::Create(
    content::BrowserContext* context) {
  return base::MakeUnique<MockMediaRouter>();
}

MockMediaRouter::MockMediaRouter() {}

MockMediaRouter::~MockMediaRouter() {}

}  // namespace media_router
