// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_IE_XPATH_NODE_IE_H_
#define CONTENT_COMMON_IE_XPATH_NODE_IE_H_

#include "base/win/atl.h"

#include <string>
#include <vector>

#include "content/common/IE/xpath_node_predicate_ie.h"

namespace ie {

class XpathNode {
 public:
  enum NODETYPE {
    NODE_TYPE_NONE = 0,
    NODE_TYPE_SELECT_NODE = 1,
    NODE_TYPE_SELECT_ANY_NODE = 2,
    NODE_TYPE_ANY_VALUE = 3
  };

  XpathNode();
  XpathNode(const XpathNode& other);
  virtual ~XpathNode();

  void AppendNodeString(const wchar_t string);
  bool EndPendingString();
  void BeginMatch();
  bool MatchNode(CComPtr<IHTMLDOMNode> node);
  XpathNode::NODETYPE GetNodeType();

 private:
  bool Parse();
  void SetNodeType(XpathNode::NODETYPE type);
  void SetNodeValue(const std::wstring& value);
  void AppendNodeValue(const wchar_t string);
  void AppendPredicate(const XpathNodePredicate& predicate);
  bool MatchNodeAttribute(CComPtr<IHTMLDOMNode> node);

 private:
  enum Status {
    KEY_STATUS_NONE = 0,
    KEY_SELECT_NODE = 1,
    KEY_SELECT_ANY_NODE = 2,
    KEY_LEFT_BRACKET = 3,
    KEY_RIGHT_BRACKET = 4,
    KEY_ATTRIBUTE = 5,
    KEY_FIRST_QUOTE = 6,
    KEY_END_QUOTE = 7
  };

  XpathNode::NODETYPE type_;
  std::wstring value_;
  std::wstring source_string_;
  Status status_;
  XpathNodePredicate* predicate_temp_;
  std::vector<XpathNodePredicate> predicates_;
};

}  // namespace ie

#endif  // CONTENT_COMMON_IE_XPATH_NODE_IE_H_
