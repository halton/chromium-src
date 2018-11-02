// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/IE/xpath_node_ie.h"

namespace ie {

const wchar_t kSelectNode = L'/';
const wchar_t kLeftBracket = L'[';
const wchar_t kRightBracket = L']';
const wchar_t kAttribute = L'@';
const wchar_t kDoubleQuote = L'\"';
const wchar_t kSingleQuote = L'\'';
const wchar_t kStringEnd = L'\0';

XpathNode::XpathNode()
    : type_(NODE_TYPE_NONE),
      value_(L""),
      source_string_(L""),
      status_(KEY_STATUS_NONE),
      predicate_temp_(NULL) {}

XpathNode::XpathNode(const XpathNode& other) = default;

XpathNode::~XpathNode() {}

void XpathNode::AppendNodeString(const wchar_t string) {
  source_string_.append(1, string);
}

bool XpathNode::EndPendingString() {
  bool ret = Parse();
  return ret;
}

void XpathNode::BeginMatch() {
  std::vector<XpathNodePredicate>::iterator iter = predicates_.begin();
  for (; iter != predicates_.end(); iter++) {
    iter->BeginMatch();
  }
}

bool XpathNode::MatchNode(CComPtr<IHTMLDOMNode> node) {
  if (node == NULL)
    return false;

  CComBSTR node_name = L"";
  node->get_nodeName(&node_name);
  node_name.ToLower();
  if (value_ != L"*" && value_ != node_name.m_str)
    return false;

  if (predicates_.empty())
    return true;

  return MatchNodeAttribute(node);
}

XpathNode::NODETYPE XpathNode::GetNodeType() {
  return type_;
}

void XpathNode::SetNodeType(XpathNode::NODETYPE type) {
  type_ = type;
}

void XpathNode::SetNodeValue(const std::wstring& value) {
  value_ = value;
}

void XpathNode::AppendNodeValue(const wchar_t string) {
  value_.append(1, string);
}

void XpathNode::AppendPredicate(const XpathNodePredicate& predicate) {
  predicates_.push_back(predicate);
}

bool XpathNode::MatchNodeAttribute(CComPtr<IHTMLDOMNode> node) {
  if (node == NULL)
    return false;

  long type = 0;
  node->get_nodeType(&type);
  if (type != 1)
    return false;

  CComPtr<IHTMLElement> element = NULL;
  node->QueryInterface(IID_IHTMLElement, (void**)&element);
  if (element == NULL)
    return false;

  std::vector<XpathNodePredicate>::iterator iter = predicates_.begin();
  for (; iter != predicates_.end(); iter++) {
    bool ret = iter->MatchPredicate(node);
    if (ret == false)
      return false;
  }

  return true;
}

bool XpathNode::Parse() {
  if (source_string_.empty())
    return false;

  int len = source_string_.length();
  status_ = KEY_STATUS_NONE;
  bool maybe_select_any_node = false;
  predicate_temp_ = NULL;
  int i = 0;
  for (i = 0; i <= len; i++) {
    wchar_t c = kStringEnd;
    if (i < len)
      c = source_string_.at(i);
    switch (c) {
      case kSelectNode:  // keyword"/"
        if (maybe_select_any_node == false) {
          status_ = KEY_SELECT_NODE;
          SetNodeType(XpathNode::NODE_TYPE_SELECT_NODE);
          maybe_select_any_node = true;
        } else if (maybe_select_any_node == true) {
          // keyword"//"
          status_ = KEY_SELECT_ANY_NODE;
          SetNodeType(XpathNode::NODE_TYPE_SELECT_ANY_NODE);
          maybe_select_any_node = false;
        }
        break;
      case kAttribute:  // keyword"@"
        if (status_ == KEY_LEFT_BRACKET) {
          predicate_temp_->AppendPredicate(c);
        } else {
          status_ = KEY_ATTRIBUTE;
          predicate_temp_ = new XpathNodePredicate;
          predicate_temp_->AppendPredicate(c);
        }
        break;
      case kLeftBracket:  // keyword"["
        if (status_ == KEY_FIRST_QUOTE && predicate_temp_) {
          predicate_temp_->AppendPredicate(c);
          break;
        }
        status_ = KEY_LEFT_BRACKET;
        predicate_temp_ = new XpathNodePredicate;
        break;
      case kRightBracket:  // keyword"]"
        if (status_ == KEY_FIRST_QUOTE && predicate_temp_) {
          predicate_temp_->AppendPredicate(c);
          break;
        }
        status_ = KEY_RIGHT_BRACKET;
        if (predicate_temp_ == NULL)
          return false;
        if (predicate_temp_->EndPendingString()) {
          AppendPredicate(std::move(*predicate_temp_));
        } else {
          delete predicate_temp_;
          predicate_temp_ = NULL;
          return false;
        }
        delete predicate_temp_;
        predicate_temp_ = NULL;
        break;
      case kDoubleQuote:
      case kSingleQuote:
        if (status_ == KEY_LEFT_BRACKET) {
          status_ = KEY_FIRST_QUOTE;
          if (predicate_temp_)
            predicate_temp_->AppendPredicate(c);
        } else if (status_ == KEY_FIRST_QUOTE) {
          status_ = KEY_END_QUOTE;
          if (predicate_temp_)
            predicate_temp_->AppendPredicate(c);
        }
        break;
      case kStringEnd:
        maybe_select_any_node = false;
        if (status_ == KEY_ATTRIBUTE) {
          if (predicate_temp_ && predicate_temp_->EndPendingString())
            AppendPredicate(std::move(*predicate_temp_));
          delete predicate_temp_;
          predicate_temp_ = NULL;
        }
        status_ = KEY_STATUS_NONE;
        break;
      default:
        maybe_select_any_node = false;
        if (status_ == KEY_SELECT_NODE || status_ == KEY_SELECT_ANY_NODE)
          AppendNodeValue(c);
        if (status_ == KEY_LEFT_BRACKET || status_ == KEY_ATTRIBUTE ||
            status_ == KEY_FIRST_QUOTE)
          predicate_temp_->AppendPredicate(c);
        break;
    }
  }
  return true;
}

}  // namespace ie
