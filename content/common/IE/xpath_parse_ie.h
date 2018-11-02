// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_IE_XPATH_PARSE_IE_H_
#define CONTENT_COMMON_IE_XPATH_PARSE_IE_H_

#include <MsHTML.h>
#include <list>
#include <string>
#include <vector>

#include "content/common/IE/atl_include.h"
#include "content/common/IE/xpath_node_ie.h"

namespace ie {

enum MatchResult {
  RESULT_NONE = 0,
  RESULT_NOT_MATCH = 1,
  RESULT_NODE_MATCH = 2,
  RESULT_NODE_PIECE_MATCH = 3,
  RESULT_ALL_NODE_MATCH = 4
};

class XpathParse {
 public:
  XpathParse();
  virtual ~XpathParse();

  bool Parse(const std::wstring& xpath);
  void BeginMatch();
  MatchResult StepMatch(CComPtr<IHTMLDOMNode> node);

 private:
  enum Status {
    KEY_STATUS_NONE = 0,
    KEY_SELECT_NODE = 1,
    KEY_SELECT_ANY_NODE = 2
  };

  XpathNode* xpath_node_;
  // XpathNodePredicate* pNodePredicatesTemp;
  std::wstring xpath_;
  Status status_;
  std::list<XpathNode> node_list_;
  std::list<XpathNode>::iterator match_iter_;
};

}  // namespace ie

#endif  // CONTENT_COMMON_IE_XPATH_PARSE_IE_H_
