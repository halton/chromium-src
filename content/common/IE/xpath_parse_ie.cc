// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/IE/xpath_parse_ie.h"

namespace ie {

const wchar_t kSelectNode = L'/';
const wchar_t kStringEnd = L'\0';

XpathParse::XpathParse()
    : xpath_node_(NULL),
      // pNodePredicatesTemp(NULL),
      xpath_(L""),
      status_(KEY_STATUS_NONE) {
  match_iter_ = node_list_.end();
}

XpathParse::~XpathParse() {}

bool XpathParse::Parse(const std::wstring& xpath) {
  node_list_.clear();
  match_iter_ = node_list_.end();

  if (xpath.empty())
    return false;

  status_ = KEY_STATUS_NONE;
  xpath_ = xpath;

  bool maybe_select_any_node = false;
  for (size_t i = 0; i <= xpath_.length(); i++) {
    wchar_t c = kStringEnd;
    if (i < xpath_.length())
      c = xpath_.at(i);
    switch (c) {
      case kSelectNode:  // keyword"/"
        if (status_ != KEY_STATUS_NONE && maybe_select_any_node == false &&
            xpath_node_ && xpath_node_->EndPendingString()) {
          node_list_.push_back(std::move(*xpath_node_));
          delete xpath_node_;
          xpath_node_ = NULL;
          status_ = KEY_STATUS_NONE;
        }
        if (maybe_select_any_node == false) {
          maybe_select_any_node = true;
          status_ = KEY_SELECT_NODE;
          xpath_node_ = new XpathNode;
          xpath_node_->AppendNodeString(c);
        } else if (maybe_select_any_node == true) {
          // keyword"//"
          status_ = KEY_SELECT_ANY_NODE;
          maybe_select_any_node = false;
          xpath_node_->AppendNodeString(c);
        }
        break;
      case kStringEnd:
        maybe_select_any_node = false;
        if (status_ != KEY_STATUS_NONE && xpath_node_ &&
            xpath_node_->EndPendingString()) {
          node_list_.push_back(std::move(*xpath_node_));
          delete xpath_node_;
          xpath_node_ = NULL;
        }
        status_ = KEY_STATUS_NONE;
        break;
      default:
        maybe_select_any_node = false;
        if (xpath_node_ == NULL)
          xpath_node_ = new XpathNode;
        xpath_node_->AppendNodeString(c);
        break;
    }
  }
  return true;
}

void XpathParse::BeginMatch() {
  match_iter_ = node_list_.begin();
  std::list<XpathNode>::iterator node_iter = node_list_.begin();
  for (; node_iter != node_list_.begin(); node_iter++) {
    node_iter->BeginMatch();
  }
}

MatchResult XpathParse::StepMatch(CComPtr<IHTMLDOMNode> node) {
  if (node == NULL || match_iter_ == node_list_.end())
    return RESULT_NONE;

  if (match_iter_->MatchNode(node) == false) {
    if (match_iter_->GetNodeType() == XpathNode::NODE_TYPE_SELECT_ANY_NODE)
      return RESULT_NODE_PIECE_MATCH;
    else
      return RESULT_NOT_MATCH;
  }
  match_iter_++;
  if (match_iter_ == node_list_.end())
    return RESULT_ALL_NODE_MATCH;

  return RESULT_NODE_MATCH;
}

}  // namespace ie
