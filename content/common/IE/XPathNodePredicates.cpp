#include "XPathNodePredicates.h"

namespace IE
{

  const wchar_t SELECT_ATTRIBUTE = L'@';
  const wchar_t EQUAL = L'=';
  const wchar_t DOUBLE_QUOTE = L'\"';
  const wchar_t QUOTE = L'\'';
  const wchar_t STRING_END = L'\0';

  XPathNodePredicates::XPathNodePredicates()
    :parsingStatus(STATUS_NONE),
    matchFrequency(1),
    matchCount(0),
    predicatesString(L"")
  {
  }

  XPathNodePredicates::XPathNodePredicates(const XPathNodePredicates& other) = default;

  XPathNodePredicates::~XPathNodePredicates()
  {
  }

  void XPathNodePredicates::AppendPredicates(const wchar_t str)
  {
    predicatesString.append(1, str);
  }

  bool XPathNodePredicates::EndPendingString()
  {
    bool ret = Parse();
    return ret;
  }

  void XPathNodePredicates::BeginMatch()
  {
    matchCount = 0;
  }

  bool XPathNodePredicates::MatchPredicate(CComPtr<IHTMLDOMNode> node)
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

    CComBSTR key = L"";
    key = Attributes.first.c_str();
    variant_t value=L"";
    if(Attributes.first.empty()==false)
      htmlEle->getAttribute(key, 2, value.GetAddress());
    if (value.vt == VT_BSTR && value.bstrVal != NULL)
    {
      if (Attributes.second == value.bstrVal)
      {
        matchCount++;
        if (matchCount == matchFrequency)
          return true;
      }
    }
    return false;
  }

  bool XPathNodePredicates::Parse()
  {
    if (predicatesString.empty())
      return false;
    int len = predicatesString.length();
    std::wstring attrKey = L"";
    std::wstring attrValue = L"";
    std::wstring matchNumStr = L"";
    parsingStatus = STATUS_NONE;
    int i = 0;
    for (i = 0; i <= len; i++)
    {
      wchar_t c = STRING_END;
      if (i < len)
        c = predicatesString.at(i);
      switch (c)
      {
      case SELECT_ATTRIBUTE:
        parsingStatus = STATUS_SELECT_ATTR;
        break;
      case EQUAL:
        parsingStatus = STATUS_EQUAL;
        break;
      case DOUBLE_QUOTE:
      case QUOTE:
        if (parsingStatus != STATUS_EQUAL &&
          parsingStatus != STATUS_FIRST_QUOTE)
          return false;
        if (parsingStatus != STATUS_FIRST_QUOTE)
          parsingStatus = STATUS_FIRST_QUOTE;
        else
          parsingStatus = STATUS_END_QUOTE;
        break;
      case STRING_END:
        if ((parsingStatus == STATUS_END_QUOTE || parsingStatus == STATUS_SELECT_ATTR) &&
          attrKey.empty() == false)
          Attributes = std::make_pair(attrKey, attrValue);
        else if (parsingStatus == STATUS_NONE && matchNumStr.empty() == false)
          matchFrequency = _wtol(matchNumStr.c_str());
        parsingStatus = STATUS_NONE;
        break;
      default:
        if (parsingStatus == STATUS_SELECT_ATTR)
          attrKey.append(1, c);
        else if (parsingStatus == STATUS_FIRST_QUOTE)
          attrValue.append(1, c);
        else if (parsingStatus == STATUS_NONE)
          matchNumStr.append(1, c);
        break;
      }
    }
    return true;
  }

} //namespace IE
