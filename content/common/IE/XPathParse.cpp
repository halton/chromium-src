#include "XPathParse.h"

namespace IE
{

  const wchar_t SELECT_NODE = L'/';
  const wchar_t STRING_END = L'\0';


  XPathParse::XPathParse()
    :pNodeTemp(NULL),
    // pNodePredicatesTemp(NULL),
    xpathString(L""),
    parsingStatus(KEY_STATUS_NONE)
  {
    matchIter = nodeList.end();
  }


  XPathParse::~XPathParse()
  {
  }

  bool XPathParse::Parse(const std::wstring & xpathStr)
  {
    nodeList.clear();
    matchIter = nodeList.end();

    if (xpathStr.empty())
      return false;

    parsingStatus = KEY_STATUS_NONE;
    xpathString = xpathStr;
    int len = xpathString.length();
    bool maybeSelectAnyNode = false;
    int i = 0;
    for (i = 0; i <= len; i++)
    {
      wchar_t c = STRING_END;
      if (i < len)
        c = xpathString.at(i);
      switch (c)
      {
      case SELECT_NODE:   // keyword"/"
        if (parsingStatus != KEY_STATUS_NONE
          && maybeSelectAnyNode == false &&
          pNodeTemp && pNodeTemp->EndPendingString())
        {
          nodeList.push_back(std::move(*pNodeTemp));
          delete pNodeTemp;
          pNodeTemp = NULL;
          parsingStatus = KEY_STATUS_NONE;
        }
        if (maybeSelectAnyNode == false)
        {
          maybeSelectAnyNode = true;
          parsingStatus = KEY_SELECT_NODE;
          pNodeTemp = new XPathNode;
          pNodeTemp->AppendNodeString(c);
        }
        else if (maybeSelectAnyNode == true)  // keyword"//"
        {
          parsingStatus = KEY_SELECT_ANY_NODE;
          maybeSelectAnyNode = false;
          pNodeTemp->AppendNodeString(c);
        }
        break;
      case STRING_END:  // keyword"\0"
        maybeSelectAnyNode = false;
        if (parsingStatus != KEY_STATUS_NONE &&
          pNodeTemp && pNodeTemp->EndPendingString())
        {
          nodeList.push_back(std::move(*pNodeTemp));
          delete pNodeTemp;
          pNodeTemp = NULL;
        }
        parsingStatus = KEY_STATUS_NONE;
        break;
      default:
        maybeSelectAnyNode = false;
        if (pNodeTemp == NULL)
          pNodeTemp = new XPathNode;
        pNodeTemp->AppendNodeString(c);
        break;
      }
    }
    return true;
  }

  void XPathParse::BeginMatch()
  {
    matchIter = nodeList.begin();
    std::list<XPathNode>::iterator nodeIter = nodeList.begin();
    for (; nodeIter != nodeList.begin(); nodeIter++)
    {
      nodeIter->BeginMatch();
    }
  }

  MATCHRESULT XPathParse::StepMatch(CComPtr<IHTMLDOMNode> node)
  {
    if (node == NULL || matchIter == nodeList.end())
      return RESULT_NONE;

    bool isMath = matchIter->MatchNode(node);
    if (isMath == false)
    {
      if (matchIter->GetNodeType() == XPathNode::NODE_TYPE_SELECT_ANY_NODE)
        return RESULT_NODE_PIECE_MATCH;
      else
        return RESULT_NOT_MATCH;
    }
    matchIter++;
    if (matchIter == nodeList.end())
      return RESULT_ALL_NODE_MATCH;
    return RESULT_NODE_MATCH;
  }

} //namespace IE

