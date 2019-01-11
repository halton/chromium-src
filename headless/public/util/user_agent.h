// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HEADLESS_PUBLIC_UTIL_USER_AGENT_H_
#define HEADLESS_PUBLIC_UTIL_USER_AGENT_H_

#include <string>

namespace headless {

#ifdef REDCORE
std::string BuildUserAgentFromProduct(const std::string& product,
                                      const std::string& ysp_product);
std::string BuildUserAgentFromOSAndProduct(const std::string& os_info,
                                           const std::string& product,
                                           const std::string& ysp_product);
#else
std::string BuildUserAgentFromProduct(const std::string& product);
std::string BuildUserAgentFromOSAndProduct(const std::string& os_info,
                                           const std::string& product);
#endif  // if defined(REDCORE)

}  // namespace headless

#endif  // HEADLESS_PUBLIC_UTIL_USER_AGENT_H_
