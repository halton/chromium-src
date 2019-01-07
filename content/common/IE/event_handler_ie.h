// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_IE_IE_EVENT_HANDLER_IE_H_
#define CONTENT_COMMON_IE_IE_EVENT_HANDLER_IE_H_

// atl.h need be in front of the COM headers
#include "base/win/atl.h"

#include <MsHTML.h>
#include <windows.h>

#include <string>

namespace ie {

class EventHandler {
 public:
  EventHandler(){};
  virtual ~EventHandler(){};

  virtual bool OnBeforeNavigate(const std::wstring& url, bool click_url) = 0;
  virtual void OnFinishNavigate(const std::wstring& url,
                                const std::wstring& favicon_urls) = 0;
  virtual void OnTitleChange(const std::wstring& title) = 0;
  virtual bool OnLoadUrlNewWnd(const std::wstring& url,
                               long flag,
                               IDispatch** dispatch) = 0;
  virtual void OnBrowserClosing() = 0;
  virtual void SetIEHwnd(HWND hwnd) = 0;
  virtual bool OnShowContxtMenu(int id, POINT pt) = 0;
  virtual bool OnDownLoadFile(const std::wstring& url,
                              int status,
                              VARIANT variant) = 0;
  virtual void GetGoBackOrForwardState(bool* go_back, bool* go_forward) = 0;
  virtual void GoBack() = 0;
  virtual void GoForward() = 0;
  virtual void OnRequestChangePos(int left, int top, int width, int height) = 0;
  virtual void OnGetMainWndPos(RECT* rect) = 0;
  virtual void OnIEServerCreated(HWND hwnd) = 0;
  virtual void OnUpdateCookie(const std::wstring& cookie) = 0;
  virtual void OnClickElement(CComPtr<IHTMLElement> click_element,
                              bool by_enter_key) = 0;
  virtual void OnGetLoginInfo(const std::wstring& url,
                              const std::wstring& username,
                              const std::wstring& password) = 0;
  virtual void OnRequestAcceleratorFromKeyboard(int key_code,
                                                int modifiers) = 0;
  virtual void OnRequestAcceleratorFromMouseWheel(int delta,
                                                  int point_x,
                                                  int point_y,
                                                  int modifiers) = 0;
  virtual void OnWindowDomainAuthenticate(std::wstring* username,
                                          std::wstring* password) = 0;
  virtual void OnQueryPrivateDNS(const std::wstring& host,
                                 std::wstring* ip_list_json_string) = 0;
};

}  // namespace ie

#endif  // CONTENT_COMMON_IE_IE_EVENT_HANDLER_IE_H_
