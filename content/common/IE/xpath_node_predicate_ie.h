// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_IE_XPATH_NODE_PREDICATES_IE_H_
#define CONTENT_COMMON_IE_XPATH_NODE_PREDICATES_IE_H_

#include <MsHTML.h>
#include <string>
#include <vector>

#include "content/common/IE/atl_include.h"

namespace ie {

class XpathNodePredicate {
 public:
  XpathNodePredicate();
  XpathNodePredicate(const XpathNodePredicate& other);
  ~XpathNodePredicate();

  void AppendPredicate(const wchar_t string);
  bool EndPendingString();
  void BeginMatch();
  bool MatchPredicate(CComPtr<IHTMLDOMNode> node);

 private:
  bool Parse();

 private:
  enum Status {
    STATUS_NONE = 0,
    STATUS_SELECT_ATTR = 1,
    STATUS_EQUAL = 2,
    STATUS_FIRST_QUOTE = 3,
    STATUS_END_QUOTE = 4
  };

  std::pair<std::wstring, std::wstring> attribute_;
  Status status_;
  unsigned int match_frequency_;
  unsigned int match_count_;
  std::wstring predicate_string_;
};

}  // namespace ie

#endif  // CONTENT_COMMON_IE_XPATH_NODE_PREDICATES_IE_H_
