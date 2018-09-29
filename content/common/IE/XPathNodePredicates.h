#ifndef CONTENT_COMMON_IE_XPATH_NODE_PREDICATES_H_
#define CONTENT_COMMON_IE_XPATH_NODE_PREDICATES_H_
#pragma once
#include <string>
#include <vector>
#include <MsHTML.h>
#include "ATLInclude.h"

namespace IE
{
  class XPathNodePredicates
  {
  public:
    XPathNodePredicates();
    XPathNodePredicates(const XPathNodePredicates& other);
    ~XPathNodePredicates();

    void AppendPredicates(const wchar_t str);
    bool EndPendingString();
    void BeginMatch();
    bool MatchPredicate(CComPtr<IHTMLDOMNode> node);
  private:
    bool Parse();
  private:
    enum STATUS
    {
      STATUS_NONE = 0,
      STATUS_SELECT_ATTR = 1,
      STATUS_EQUAL = 2,
      STATUS_FIRST_QUOTE = 3,
      STATUS_END_QUOTE = 4
    };
    std::pair<std::wstring, std::wstring> Attributes;
    STATUS parsingStatus;
    unsigned int matchFrequency;
    unsigned int matchCount;
    std::wstring predicatesString;

  };
} //namespace IE
#endif