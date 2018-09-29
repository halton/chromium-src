#include "XPathNode.h"

namespace IE
{

  const wchar_t SELECT_NODE = L'/';
  const wchar_t LEFT_BRACKET = L'[';
  const wchar_t RIGHT_BRACKET = L']';
  const wchar_t ATTRIBUTE = L'@';
  const wchar_t DOUBLE_QUOTE = L'\"';
  const wchar_t QUOTE = L'\'';
  const wchar_t STRING_END = L'\0';

  XPathNode::XPathNode()
    :nodeType(NODE_TYPE_NONE),
    nodeValue(L""),
    nodeSourceString(L""),
    parseStatus(KEY_STATUS_NONE),
    pNodePredicatesTemp(NULL)
  {
  }

  XPathNode::XPathNode(const XPathNode& other) = default;

  XPathNode::~XPathNode()
  {
  }

  void XPathNode::AppendNodeString(const wchar_t str)
  {
    nodeSourceString.append(1, str);
  }

  bool XPathNode::EndPendingString()
  {
    bool ret = Parse();
    return ret;
  }

  void XPathNode::BeginMatch()
  {
    std::vector<XPathNodePredicates>::iterator iter = nodePredicates.begin();
    for (; iter != nodePredicates.end(); iter++)
    {
      iter->BeginMatch();
    }
  }

  bool XPathNode::MatchNode(CComPtr<IHTMLDOMNode> node)
  {
    if (node == NULL)
      return false;
    CComBSTR nodeName=L"";
    node->get_nodeName(&nodeName);
    nodeName.ToLower();
    if (nodeValue != L"*" &&
      nodeValue != nodeName.m_str)
      return false;

    if (nodePredicates.empty())
      return true;

    bool isMathAttr = false;
    isMathAttr = MatchNodeAttribute(node);
    return isMathAttr;
  }

  XPathNode::NODETYPE XPathNode::GetNodeType()
  {
    return nodeType;
  }

  void XPathNode::SetNodeType(XPathNode::NODETYPE type)
  {
    nodeType = type;
  }

  void XPathNode::SetNodeValue(const std::wstring & value)
  {
    nodeValue = value;
  }

  void XPathNode::AppendNodeValue(const wchar_t str)
  {
    nodeValue.append(1, str);
  }


  void XPathNode::AppendPredicates(const XPathNodePredicates& pred)
  {
    nodePredicates.push_back(pred);
  }

  bool XPathNode::MatchNodeAttribute(CComPtr<IHTMLDOMNode> node)
  {
    if (node == NULL)
      return false;

    long type = 0;
    node->get_nodeType(&type);
    if (type != 1)
      return false;

    CComPtr<IHTMLElement> htmlEle = NULL;
    node->QueryInterface(IID_IHTMLElement, (void**)&htmlEle);
    if (htmlEle == NULL)
      return false;
    std::vector<XPathNodePredicates>::iterator iter = nodePredicates.begin();
    for (; iter != nodePredicates.end(); iter++)
    {
      bool ret = iter->MatchPredicate(node);
      if (ret == false)
        return false;
    }
    return true;
  }

  bool XPathNode::Parse()
  {
    if (nodeSourceString.empty())
      return false;
    int len = nodeSourceString.length();
    parseStatus = KEY_STATUS_NONE;
    bool maybeSelectAnyNode = false;
    pNodePredicatesTemp = NULL;
    int i = 0;
    for (i = 0; i <= len; i++)
    {
      wchar_t c = STRING_END;
      if (i < len)
        c = nodeSourceString.at(i);
      switch (c)
      {
      case SELECT_NODE:   // keyword"/"
        if (maybeSelectAnyNode == false)
        {
          parseStatus = KEY_SELECT_NODE;
          SetNodeType(XPathNode::NODE_TYPE_SELECT_NODE);
          maybeSelectAnyNode = true;
        }
        else if (maybeSelectAnyNode == true)  // keyword"//"
        {
          parseStatus = KEY_SELECT_ANY_NODE;
          SetNodeType(XPathNode::NODE_TYPE_SELECT_ANY_NODE);
          maybeSelectAnyNode = false;
        }
        break;
      case ATTRIBUTE: // keyword"@"
        if (parseStatus == KEY_LEFT_BRACKET)
          pNodePredicatesTemp->AppendPredicates(c);
        else
        {
          parseStatus = KEY_ATTRIBUTE;
          pNodePredicatesTemp = new XPathNodePredicates;
          pNodePredicatesTemp->AppendPredicates(c);
        }
        break;
      case LEFT_BRACKET:  //keyword"["
        if (parseStatus == KEY_FIRST_QUOTE &&
          pNodePredicatesTemp)
        {
          pNodePredicatesTemp->AppendPredicates(c);
          break;
        }
        parseStatus = KEY_LEFT_BRACKET;
        pNodePredicatesTemp = new XPathNodePredicates;
        break;
      case RIGHT_BRACKET: //keyword"]"
        if (parseStatus == KEY_FIRST_QUOTE &&
          pNodePredicatesTemp)
        {
          pNodePredicatesTemp->AppendPredicates(c);
          break;
        }
        parseStatus = KEY_RIGHT_BRACKET;
        if (pNodePredicatesTemp == NULL)
          return false;
        if (pNodePredicatesTemp->EndPendingString())
          AppendPredicates(std::move(*pNodePredicatesTemp));
        else
        {
          delete pNodePredicatesTemp;
          pNodePredicatesTemp = NULL;
          return false;
        }
        delete pNodePredicatesTemp;
        pNodePredicatesTemp = NULL;
        break;
      case DOUBLE_QUOTE:
      case QUOTE:
        if (parseStatus == KEY_LEFT_BRACKET)
        {
          parseStatus = KEY_FIRST_QUOTE;
          if (pNodePredicatesTemp)
            pNodePredicatesTemp->AppendPredicates(c);
        }
        else if (parseStatus == KEY_FIRST_QUOTE)
        {
          parseStatus = KEY_END_QUOTE;
          if (pNodePredicatesTemp)
            pNodePredicatesTemp->AppendPredicates(c);
        }
        break;
      case STRING_END:  // keyword"\0"
        maybeSelectAnyNode = false;
        if (parseStatus == KEY_ATTRIBUTE)
        {
          if (pNodePredicatesTemp &&
            pNodePredicatesTemp->EndPendingString())
            AppendPredicates(std::move(*pNodePredicatesTemp));
          delete pNodePredicatesTemp;
          pNodePredicatesTemp = NULL;
        }
        parseStatus = KEY_STATUS_NONE;
        break;
      default:
        maybeSelectAnyNode = false;
        if (parseStatus == KEY_SELECT_NODE ||
          parseStatus == KEY_SELECT_ANY_NODE)
          AppendNodeValue(c);
        if (parseStatus == KEY_LEFT_BRACKET ||
          parseStatus == KEY_ATTRIBUTE ||
          parseStatus == KEY_FIRST_QUOTE)
          pNodePredicatesTemp->AppendPredicates(c);
        break;
      }
    }
    return true;
  }

} //namespace IE
