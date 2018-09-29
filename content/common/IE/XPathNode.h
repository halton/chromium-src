#ifndef CONTENT_COMMON_IE_XPATH_NODE_H_
#define CONTENT_COMMON_IE_XPATH_NODE_H_
#pragma once
#include <string>
#include <vector>
#include "XPathNodePredicates.h"
#include <MsHTML.h>
#include "ATLInclude.h"

namespace IE
{
  class XPathNode
  {
  public:
    enum NODETYPE
    {
      NODE_TYPE_NONE = 0,
      NODE_TYPE_SELECT_NODE = 1,
      NODE_TYPE_SELECT_ANY_NODE = 2,
      NODE_TYPE_ANY_VALUE = 3
    };

    XPathNode();
    XPathNode(const XPathNode& other);
    virtual ~XPathNode();

    void AppendNodeString(const wchar_t str);
    bool EndPendingString();
    void BeginMatch();
    bool MatchNode(CComPtr<IHTMLDOMNode> node);
    XPathNode::NODETYPE GetNodeType();
  private:
    bool Parse();
    void SetNodeType(XPathNode::NODETYPE type);
    void SetNodeValue(const std::wstring& value);
    void AppendNodeValue(const wchar_t str);
    void AppendPredicates(const XPathNodePredicates& pred);
    bool MatchNodeAttribute(CComPtr<IHTMLDOMNode> node);
  private:
    enum STATUS
    {
      KEY_STATUS_NONE = 0,
      KEY_SELECT_NODE = 1,
      KEY_SELECT_ANY_NODE = 2,
      KEY_LEFT_BRACKET = 3,
      KEY_RIGHT_BRACKET = 4,
      KEY_ATTRIBUTE = 5,
      KEY_FIRST_QUOTE = 6,
      KEY_END_QUOTE = 7
    };
    XPathNode::NODETYPE nodeType;
    std::wstring nodeValue;
    std::wstring nodeSourceString;
    STATUS parseStatus;
    XPathNodePredicates* pNodePredicatesTemp;
    std::vector<XPathNodePredicates> nodePredicates;

  };
} //namespace IE
#endif