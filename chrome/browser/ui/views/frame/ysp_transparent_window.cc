// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(REDCORE) && defined(WATERMARK) && defined(IE_REDCORE)

#include "chrome/browser/ui/views/frame/ysp_transparent_window.h"

#include <WinUser.h>
#include <comdef.h>
#include <gdiplus.h>
#include <timeapi.h>
#include <string>

#include "base/strings/utf_string_conversions.h"
#include "base/win/windows_version.h"
#include "content/public/browser/browser_thread.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkDevice.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkRefCnt.h"
#include "third_party/skia/include/core/SkTypeface.h"
#include "third_party/skia/include/core/SkTypes.h"
#include "ui/gfx/canvas.h"

#ifndef min
#define min
#endif

#ifndef max
#define max
#endif

using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")
#pragma warning(disable : 4302)

YspTransparentWindow::YspTransparentWindow()
    : self_ptr_(this),
      self_hwnd_(NULL),
      parent_hwnd_(NULL),
      is_hide_(true),
      water_mark_font_size_(0),
      water_mark_color_(0),
      mask_color_(0),
      alpha_value_(0),
      delay_show_timer_(new base::Timer(false, false)) {
  GdiplusStartupInput gdi_input;
  GdiplusStartup(&gdi_token_, &gdi_input, NULL);
}

YspTransparentWindow::~YspTransparentWindow() {
  YSPLoginManager::GetInstance()->RemoveObserver(this);

  if (self_hwnd_) {
    ::DestroyWindow(self_hwnd_);
  }
  GdiplusShutdown(gdi_token_);
}

bool YspTransparentWindow::Init(HWND parent_HWND) {
  parent_hwnd_ = parent_HWND;
  YSPLoginManager::GetInstance()->AddObserver(this);

  HINSTANCE hInstance = GetModuleHandle(0);

  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
  wcex.lpszMenuName = L"YspTransparentWindow";
  wcex.lpszClassName = L"YspTransparentWindow";
  wcex.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));

  RegisterClassExW(&wcex);

  self_hwnd_ = CreateWindowW(L"YspTransparentWindow", L"YspTransparentWindow",
                             WS_POPUP, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
                             nullptr, nullptr, hInstance, nullptr);

  SetWindowLong(self_hwnd_, GWL_USERDATA, (long)this);

  LONG nRet = ::GetWindowLong(self_hwnd_, GWL_EXSTYLE);
  nRet = nRet | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE;
  ::SetWindowLong(self_hwnd_, GWL_EXSTYLE, nRet);

  ::SetWindowPos(self_hwnd_, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE);

  SetWaterMark(std::vector<base::string16>(), 0, 0);

  return true;
}

void YspTransparentWindow::SetRect(const RECT& rect) {
  RECT old_rect;
  GetWindowRect(self_hwnd_, &old_rect);
  int result = memcmp((const void*)&rect, (const void*)&old_rect, sizeof(RECT));
  if (result != 0)
    ::SetWindowPos(self_hwnd_, HWND_TOP, rect.left, rect.top,
                   rect.right - rc.left, rect.bottom - rc.top,
                   SWP_NOACTIVATE | SWP_NOZORDER);
}

void YspTransparentWindow::Show(bool is_show) {
  if (is_show)
    if (is_hide_) {
      base::TimeDelta delayTime = base::TimeDelta::FromMilliseconds(0);
      delay_show_timer_->Stop();
      delay_show_timer_->Start(
          FROM_HERE, delayTime,
          base::Bind(&YspTransparentWindow::DelayShowWindow,
                     self_ptr_.GetWeakPtr()));
    } else {
      ::SetWindowPos(self_hwnd_, HWND_TOP, 0, 0, 0, 0,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE |
                         SWP_NOZORDER);
    }
  else {
    delay_show_timer_->Stop();
    ::SetWindowPos(self_hwnd_, HWND_TOP, 0, 0, 0, 0,
                   SWP_NOACTIVATE | SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE);
    is_hide_ = true;
  }
}

void YspTransparentWindow::GotoTop(bool is_top_most) {
  ::SetWindowPos(self_hwnd_, HWND_NOTOPMOST, 0, 0, 0, 0,
                 SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
  if (is_top_most)
    ::SetWindowPos(self_hwnd_, HWND_TOPMOST, 0, 0, 0, 0,
                   SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
  else
    ::SetWindowPos(self_hwnd_, HWND_TOP, 0, 0, 0, 0,
                   SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
}

bool YspTransparentWindow::IsVisible() {
  if (self_hwnd_ == NULL)
    return false;
  return ::IsWindowVisible(self_hwnd_) == TRUE;
}

void YspTransparentWindow::OnConfigDataUpdated(const std::string& type,
                                               const std::string& data) {
  if (type == "accountInfo" || type == "strategy") {
    uint32_t color = YSPLoginManager::GetInstance()->GetWatermarkColor();
    int size = YSPLoginManager::GetInstance()->GetWatermarkFontSize();
    std::vector<base::string16> context =
        YSPLoginManager::GetInstance()->GetWatermarkString();

    SetWaterMark(context, size, color);
  }
}

void YspTransparentWindow::OnLoginRequestFailure(const std::string& error) {}

void YspTransparentWindow::OnLoginResponseParseFailure(
    const std::string& error) {}

void YspTransparentWindow::OnLoginFailure(base::string16 message) {
  SetWaterMark(std::vector<base::string16>(), 0, 0);
  EnableScreenCapture(true);
}

void YspTransparentWindow::OnLoginSuccess(const base::string16& name,
                                          const std::string& head_image_url) {
  uint32_t color = YSPLoginManager::GetInstance()->GetWatermarkColor();
  int size = YSPLoginManager::GetInstance()->GetWatermarkFontSize();
  std::vector<base::string16> context =
      YSPLoginManager::GetInstance()->GetWatermarkString();

  SetWaterMark(context, size, color);

  EnableScreenCapture(
      YSPLoginManager::GetInstance()->GetScreenCaptureEnabled());
}

void YspTransparentWindow::OnLogout() {
  SetWaterMark(std::vector<base::string16>(), 0, 0);
  EnableScreenCapture(true);
}

HWND YspTransparentWindow::GetHwnd() {
  return self_hwnd_;
}

void YspTransparentWindow::DelayShowWindow() {
  if (IsWindow(self_hwnd_) && IsWindow(parent_hwnd_) &&
      ::IsWindowVisible(parent_hwnd_)) {
    ::SetWindowPos(self_hwnd_, HWND_TOP, 0, 0, 0, 0,
                   SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
    is_hide_ = false;
  }
}

void YspTransparentWindow::SetWaterMark(
    const std::vector<base::string16>& water_mark_string,
    const int& font_size,
    const DWORD& font_color) {
  bool isChange = false;
  water_mark_string_.assign(water_mark_string.begin(), water_mark_string.end());
  water_mark_font_size_ = font_size;

  int alpha = 0;
  alpha = (font_color >> 24) & 0xff;
  int r = font_color & 0xff;
  int g = (font_color >> 8) & 0xff;
  int b = (font_color >> 16) & 0xff;
  if (water_mark_color_ != RGB(r, g, b)) {
    water_mark_color_ = RGB(r, g, b);
    isChange = true;
  }

  int mask_r = 0;
  if (r < 255)
    mask_r = r + 1;
  else
    mask_r = r - 1;
  mask_color_ = RGB(mask_r, g, b);

  if (IsWindow(self_hwnd_) == FALSE)
    return;

  if (isChange) {
    //先设置为完全透明，在绘制完镂空色后再设置需要的透明度，防止如果新设置的镂空色和之前不同时能够直接看到之前的镂空色
    ::SetLayeredWindowAttributes(self_hwnd_, mask_color_, 0,
                                 LWA_COLORKEY | LWA_ALPHA);
  }

  RECT rect = {0, 0, 0, 0};
  GetClientRect(self_hwnd_, &rect);
  ::InvalidateRect(self_hwnd_, &rect, TRUE);
  ::UpdateWindow(self_hwnd_);
  ::SetLayeredWindowAttributes(self_hwnd_, mask_color_, alpha,
                               LWA_COLORKEY | LWA_ALPHA);
}

void YspTransparentWindow::DrawWaterMarkUseSkia(HDC hdc,
                                                int width,
                                                int height) {
  int dist = 0;
  if (width > height)
    dist = width;
  else
    dist = height;

  skia::RefPtr<SkCanvas> skCanvas =
      skia::AdoptRef(skia::CreateBitmapCanvas(3 * dist, 3 * dist, false));
  skCanvas->drawColor(SkColorSetARGB(255, GetRValue(mask_color_),
                                     GetGValue(mask_color_),
                                     GetBValue(mask_color_)));
  if (water_mark_font_size_ > 0) {
    SkPaint paint;
    paint.setColor(SkColorSetARGB(255, GetRValue(water_mark_color_),
                                  GetGValue(water_mark_color_),
                                  GetBValue(water_mark_color_)));
    paint.setTextSize(water_mark_font_size_);
    paint.setAntiAlias(false);
    paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);

    HGDIOBJ hFont = GetStockObject(DEFAULT_GUI_FONT);
    LOGFONT lf = {0};
    GetObject(hFont, sizeof(LOGFONT), &lf);
    DeleteObject(hFont);
    std::string fontName = "";
    fontName = base::UTF16ToASCII(lf.lfFaceName);
    skia::RefPtr<SkTypeface> face = skia::AdoptRef(
        SkTypeface::CreateFromName(fontName.c_str(), SkTypeface::kNormal));
    paint.setTypeface(face.get());

    int row = water_mark_string_.size();
    SkScalar row_height = 0.0;
    SkRect layoutRect = SkRect::MakeEmpty();
    SkRect* stringRects = new SkRect[row];
    for (int i = 0; i < row; i++) {
      paint.measureText(water_mark_string_[i].c_str(),
                        water_mark_string_[i].size() * sizeof(wchar_t),
                        &stringRects[i]);
      row_height = stringRects[i].height();
      layoutRect.fBottom += stringRects[i].height();
      if (stringRects[i].width() > layoutRect.width()) {
        layoutRect.fRight = stringRects[i].width();
      }
    }

    const int h_space = 75;
    const int v_space = 60;

    skCanvas->save();
    skCanvas->rotate(-30);
    SkScalar x = 0.0;
    SkScalar y = 0.0;
    while (y < 3 * dist) {
      int ox = x;
      while (ox < 3 * dist) {
        int oy = y;
        for (int i = 0; i < row; i++) {
          int xx = ox + (layoutRect.width() - stringRects[i].width()) / 2;
          skCanvas->drawText(water_mark_string_[i].c_str(),
                             water_mark_string_[i].size() * sizeof(wchar_t), xx,
                             oy, paint);
          oy += 1.5 * row_height;
        }
        ox += h_space + layoutRect.width();
      }

      x += layoutRect.width();
      while (x > 0.0) {
        x -= h_space;
        x -= layoutRect.width();
      }
      y += v_space + 1.5 * layoutRect.height();
    }

    delete[] stringRects;
    skCanvas->restore();
  }
  skCanvas->flush();
  const SkBitmap& skbmp = skCanvas->getDevice()->accessBitmap(false);
  skbmp.lockPixels();

  BITMAPINFO bmi;
  memset(&bmi, 0, sizeof(bmi));
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = skbmp.width();
  bmi.bmiHeader.biHeight = -skbmp.height();  // top-down image
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = 0;
  int ret = 0;
  ret = SetDIBitsToDevice(hdc, 0, 0, width, height, 1.5 * dist, 1.5 * dist, 0,
                          skbmp.height(), (unsigned char*)skbmp.getPixels(),
                          &bmi, DIB_RGB_COLORS);
  skbmp.unlockPixels();
}

void YspTransparentWindow::DrawWaterMarkUseGdi(HDC hdc, int width, int height) {
  int dist = 0;
  if (width > height)
    dist = width;
  else
    dist = height;

  HDC memdc = ::CreateCompatibleDC(hdc);
  HBITMAP hBitmap = ::CreateCompatibleBitmap(hdc, 2 * dist, 2 * dist);
  HBITMAP hOldBitmap = (HBITMAP)::SelectObject(memdc, hBitmap);

  Graphics* memGraph = Graphics::FromHDC(memdc);
  memGraph->SetTextRenderingHint(
      Gdiplus::TextRenderingHint::TextRenderingHintSingleBitPerPixel);
  memGraph->SetCompositingMode(CompositingModeSourceCopy);
  memGraph->SetCompositingQuality(CompositingQualityHighSpeed);
  memGraph->SetPixelOffsetMode(PixelOffsetModeNone);
  memGraph->SetSmoothingMode(SmoothingModeNone);
  memGraph->SetInterpolationMode(InterpolationModeDefault);

  Color bgColor;
  bgColor.SetFromCOLORREF(mask_color_);
  memGraph->Clear(bgColor);

  HGDIOBJ hFont = GetStockObject(DEFAULT_GUI_FONT);
  LOGFONT lf = {0};
  int ret = GetObject(hFont, sizeof(LOGFONT), &lf);
  DeleteObject(hFont);
  if (ret > 0) {
    Gdiplus::FontFamily fontFanily(lf.lfFaceName);
    if (fontFanily.IsAvailable() == FALSE) {
      delete memGraph;
      BitBlt(hdc, 0, 0, width, height, memdc, 0, 0, SRCCOPY);
      ::SelectObject(memdc, hOldBitmap);
      DeleteObject(hBitmap);
      DeleteDC(memdc);
      return;
    }
    Gdiplus::Font font(&fontFanily, water_mark_font_size_, 0,
                       Gdiplus::UnitPixel);
    if (font.IsAvailable()) {
      int row = water_mark_string_.size();
      Gdiplus::REAL row_height = 0.0;
      Gdiplus::RectF layoutRect(0.0, 0.0, 0.0, 0.0);
      Gdiplus::RectF* stringRects = new Gdiplus::RectF[row];
      for (int i = 0; i < row; i++) {
        memGraph->MeasureString(water_mark_string_[i].c_str(), -1, &font,
                                PointF(0, 0), &stringRects[i]);
        row_height = stringRects[i].Height;
        layoutRect.Height += stringRects[i].Height;
        if (stringRects[i].Width > layoutRect.Width) {
          layoutRect.Width = stringRects[i].Width;
        }
      }

      Color font_color;
      font_color.SetFromCOLORREF(water_mark_color_);
      Gdiplus::SolidBrush br(font_color);
      memGraph->RotateTransform(-30.0);
      StringFormat stringformat(Gdiplus::StringAlignment::StringAlignmentNear);

      const int h_space = 75;
      const int v_space = 60;

      Gdiplus::REAL x = 0.0;
      Gdiplus::REAL y = 0.0;
      while (y < 2 * dist) {
        int ox = x;
        while (ox < 2 * dist) {
          int oy = y;
          for (int i = 0; i < row; i++) {
            int xx = ox + (layoutRect.Width - stringRects[i].Width) / 2;
            memGraph->DrawString(water_mark_string_[i].c_str(), -1, &font,
                                 PointF(xx, oy), &stringformat, &br);
            oy += row_height;
          }
          ox += h_space + layoutRect.Width;
        }

        x += layoutRect.Width;
        while (x > 0.0) {
          x -= h_space;
          x -= layoutRect.Width;
        }
        y += v_space + layoutRect.Height;
      }

      delete[] stringRects;
      memGraph->ResetTransform();
    }

    BitBlt(hdc, 0, 0, width, height, memdc, 0.58 * dist, 0, SRCCOPY);

    delete memGraph;
    ::SelectObject(memdc, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(memdc);
  }
}

LRESULT YspTransparentWindow::WndProc(HWND hWnd,
                                      UINT message,
                                      WPARAM wParam,
                                      LPARAM lParam) {
  switch (message) {
    case WM_PAINT: {
      YspTransparentWindow* pTransWnd =
          (YspTransparentWindow*)GetWindowLong(hWnd, GWL_USERDATA);
      if (pTransWnd == NULL)
        break;
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);

      if (pTransWnd->water_mark_string_.empty()) {
        EndPaint(hWnd, &ps);
        break;
      }

      DWORD time = ::timeGetTime();
      RECT rc;
      ::GetClientRect(hWnd, &rc);

      base::win::Version ver = base::win::GetVersion();
      // ver = base::win::Version::VERSION_SERVER_2003;
      if (ver < base::win::VERSION_WIN8) {
        pTransWnd->DrawWaterMarkUseGdi(hdc, rc.right, rc.bottom);
      } else if (ver >= base::win::VERSION_WIN8) {
        pTransWnd->DrawWaterMarkUseSkia(hdc, rc.right, rc.bottom);
      }

      time = ::timeGetTime() - time;
      // LOG(INFO) << time;
      EndPaint(hWnd, &ps);
    } break;
    case WM_ERASEBKGND:
      return TRUE;
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

void YspTransparentWindow::EnableScreenCapture(bool isEnable) {
  typedef BOOL(WINAPI * SetWindowDisplayAffinityPtr)(HWND, DWORD);
  SetWindowDisplayAffinityPtr func_ptr =
      reinterpret_cast<SetWindowDisplayAffinityPtr>(GetProcAddress(
          GetModuleHandleA("user32.dll"), "SetWindowDisplayAffinity"));
  if (func_ptr) {
    if (isEnable)
      func_ptr(GetHwnd(), WDA_NONE);
    else
      func_ptr(GetHwnd(), WDA_MONITOR);
  }
}
#endif
