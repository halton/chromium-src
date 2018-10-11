// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_FRAME_YSP_TRANSPRENT_WINDOW_H_
#define CHROME_BROWSER_UI_VIEWS_FRAME_YSP_TRANSPRENT_WINDOW_H_

#if defined(REDCORE) && defined(WATERMARK) && defined(IE_REDCORE)

#include <windows.h>

#include "base/memory/weak_ptr.h"
#include "base/timer/timer.h"
#include "chrome/browser/ysp_login/ysp_login_manager.h"

class YspTransparentWindow : public YSPLoginManagerObserver {
 public:
  YspTransparentWindow();
  ~YspTransparentWindow();

  bool Init(HWND parent_HWND);
  void SetRect(const RECT& rect);
  void Show(bool is_show);
  void GotoTop(bool is_top_most);
  bool IsVisible();

  // YSPLoginManagerObserver
  void OnConfigDataUpdated(const std::string& type,
                           const std::string& data) override;
  void OnLoginRequestFailure(const std::string& error) override;
  void OnLoginResponseParseFailure(const std::string& error) override;
  void OnLoginFailure(base::string16 message) override;
  void OnLoginSuccess(const base::string16& name,
                      const std::string& head_image_url) override;
  void OnLogout() override;

 private:
  HWND GetHwnd();
  void DelayShowWindow();
  void SetWaterMark(const std::vector<base::string16>& water_mark_string,
                    const int& font_size,
                    const DWORD& font_color);
  void DrawWaterMarkUseSkia(HDC hdc, int width, int height);
  void DrawWaterMarkUseGdi(HDC hdc, int width, int height);
  void EnableScreenCapture(bool isEnable);
  static LRESULT CALLBACK WndProc(HWND hWnd,
                                  UINT message,
                                  WPARAM wParam,
                                  LPARAM lParam);

 private:
  ULONG_PTR gdi_token_;
  HWND self_hwnd_;
  HWND parent_hwnd_;
  base::WeakPtrFactory<YspTransparentWindow> self_ptr_;
  bool is_hide_;
  COLORREF water_mark_color_;
  COLORREF mask_color_;
  int alpha_value_;
  int water_mark_font_size_;
  std::vector<base::string16> water_mark_string_;
  scoped_ptr<base::Timer> delay_show_timer_;
};

#endif  // defined(REDCORE) && defined(WATERMARK) && defined(IE_REDCORE)
#endif  // CHROME_BROWSER_UI_VIEWS_FRAME_YSP_TRANSPRENT_WINDOW_H_
