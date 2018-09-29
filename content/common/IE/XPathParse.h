#ifndef CONTENT_COMMON_IE_XPATH_PARSE_H_
#define CONTENT_COMMON_IE_XPATH_PARSE_H_
#pragma once
#include <string>
#include <vector>
#include <list>
#include "XPathNode.h"
#include "ATLInclude.h"
#include <MsHTML.h>

namespace IE
{

  enum MATCHRESULT
  {
    RESULT_NONE=0,
    RESULT_NOT_MATCH=1,
    RESULT_NODE_MATCH=2,
    RESULT_NODE_PIECE_MATCH=3,
    RESULT_ALL_NODE_MATCH=4
  };

  class XPathParse
  {
  public:
    XPathParse();
    virtual ~XPathParse();

    bool Parse(const std::wstring& xpathStr);
    void BeginMatch();
    MATCHRESULT StepMatch(CComPtr<IHTMLDOMNode> node);
  private:
    enum STATUS
    {
      KEY_STATUS_NONE = 0,
      KEY_SELECT_NODE = 1,
      KEY_SELECT_ANY_NODE = 2
    };
    XPathNode* pNodeTemp;
    // XPathNodePredicates* pNodePredicatesTemp;
    std::wstring xpathString;
    STATUS parsingStatus;
    std::list<XPathNode> nodeList;
    std::list<XPathNode>::iterator matchIter;
  };
} //namespace IE
#endif