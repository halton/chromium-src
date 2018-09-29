#ifndef CONTENT_COMMON_IE_IE_VERSION_H_
#define CONTENT_COMMON_IE_IE_VERSION_H_

namespace IE
{
  enum IEVersion
  {
    DOCNONE=-1,
    DOCSYS=1,
    DOC6=5,
    DOC7=7,
    DOC8=8,
    DOC9=9,
    DOC10=10,
    DOC11=11
  };

  enum IEEmulation
  {
    EMULATION_NONE=-1,
    EMULATION7 = 7,
    EMULATION8 = 8,
    EMULATION9 = 9,
    EMULATION10=10,
    EMULATION11=11
  };

  enum IEDownloadStatus
  {
    NONESTATUS = 0,
    BEGINDOWNLOAD = 1,
    DOWNLOADINGDATA = 2,
    ENDDOWNLOAD = 3,
    DOWNLOADERROR=4
  };
} //namespace IE

enum RenderCore
{
  NONE_CORE = -1,
  BLINK_CORE = 0,
  IE_CORE=1
};

struct RendererMode
{
  RenderCore core;
  IE::IEVersion ver;
  IE::IEEmulation emulation;
  RendererMode()
  {
    core = NONE_CORE;
    ver = IE::DOCNONE;
    emulation = IE::EMULATION_NONE;
  }

  bool operator==(const RendererMode &mode)const
  {
    if (this->core == BLINK_CORE &&
      this->core == mode.core)
      return true;
    if ((this->core == mode.core) &&
      (this->ver == mode.ver) &&
      (this->emulation == mode.emulation))
      return true;
    return false;
  }

  bool operator!=(const RendererMode &mode)const
  {
    bool ret = (*this == mode);
    return !ret;
  }
};
#endif