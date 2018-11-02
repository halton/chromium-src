// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/IE/xpath_node_predicate_ie.h"

namespace ie {

const wchar_t kAttribute = L'@';
const wchar_t kEqual = L'=';
const wchar_t kDoubleQuote = L'\"';
const wchar_t kSingleQuote = L'\'';
const wchar_t kStringEnd = L'\0';

XpathNodePredicate::XpathNodePredicate()
    : status_(STATUS_NONE),
      match_frequency_(1),
      match_count_(0),
      predicate_string_(L"") {}

XpathNodePredicate::XpathNodePredicate(const XpathNodePredicate& other) =
    default;

XpathNodePredicate::~XpathNodePredicate() {}

void XpathNodePredicate::AppendPredicate(const wchar_t string) {
  predicate_string_.append(1, string);
}

bool XpathNodePredicate::EndPendingString() {
  bool ret = Parse();
  return ret;
}

void XpathNodePredicate::BeginMatch() {
  match_count_ = 0;
}

bool XpathNodePredicate::MatchPredicate(CComPtr<IHTMLDOMNode> node) {
  if (node == NULL)
    return false;

  long type = 0;
  node->get_nodeType(&type);
  if (type != 1)
    return false;

  CComPtr<IHTMLElement> html_element = NULL;
  node->QueryInterface(IID_IHTMLElement, (void**)&html_element);
  if (html_element == NULL)
    return false;

  CComBSTR key = L"";
  key = attribute_.first.c_str();
  variant_t value = L"";
  if (attribute_.first.empty() == false)
    html_element->getAttribute(key, 2, value.GetAddress());
  if (value.vt == VT_BSTR && value.bstrVal != NULL) {
    if (attribute_.second == value.bstrVal) {
      match_count_++;
      if (match_count_ == match_frequency_)
        return true;
    }
  }
  return false;
}

bool XpathNodePredicate::Parse() {
  if (predicate_string_.empty())
    return false;

  int len = predicate_string_.length();
  std::wstring attr_key = L"";
  std::wstring attr_value = L"";
  std::wstring match_num_string = L"";
  status_ = STATUS_NONE;
  int i = 0;
  for (i = 0; i <= len; i++) {
    wchar_t c = kStringEnd;
    if (i < len)
      c = predicate_string_.at(i);
    switch (c) {
      case kAttribute:
        status_ = STATUS_SELECT_ATTR;
        break;
      case kEqual:
        status_ = STATUS_EQUAL;
        break;
      case kDoubleQuote:
      case kSingleQuote:
        if (status_ != STATUS_EQUAL && status_ != STATUS_FIRST_QUOTE)
          return false;
        if (status_ != STATUS_FIRST_QUOTE)
          status_ = STATUS_FIRST_QUOTE;
        else
          status_ = STATUS_END_QUOTE;
        break;
      case kStringEnd:
        if ((status_ == STATUS_END_QUOTE || status_ == STATUS_SELECT_ATTR) &&
            attr_key.empty() == false)
          attribute_ = std::make_pair(attr_key, attr_value);
        else if (status_ == STATUS_NONE && match_num_string.empty() == false)
          match_frequency_ = _wtol(match_num_string.c_str());
        status_ = STATUS_NONE;
        break;
      default:
        if (status_ == STATUS_SELECT_ATTR)
          attr_key.append(1, c);
        else if (status_ == STATUS_FIRST_QUOTE)
          attr_value.append(1, c);
        else if (status_ == STATUS_NONE)
          match_num_string.append(1, c);
        break;
    }
  }
  return true;
}

}  // namespace ie
