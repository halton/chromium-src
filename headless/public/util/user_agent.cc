// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "headless/public/util/user_agent.h"

#include "content/public/common/user_agent.h"

namespace headless {

#ifdef REDCORE
std::string BuildUserAgentFromProduct(const std::string& product,
                                      const std::string& ysp_product) {
  return content::BuildUserAgentFromProduct(product, ysp_product);
}

std::string BuildUserAgentFromOSAndProduct(const std::string& os_info,
                                           const std::string& product,
                                           const std::string& ysp_product) {
  return content::BuildUserAgentFromOSAndProduct(os_info, product, ysp_product);
#else
std::string BuildUserAgentFromProduct(const std::string& product) {
  return content::BuildUserAgentFromProduct(product);
}

std::string BuildUserAgentFromOSAndProduct(const std::string& os_info,
                                           const std::string& product) {
  return content::BuildUserAgentFromOSAndProduct(os_info, product);
#endif  // if defined(REDCORE)
}

}  // namespace headless
