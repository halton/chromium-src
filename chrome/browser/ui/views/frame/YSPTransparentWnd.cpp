#if defined(REDCORE) && defined(WATERMARK) && defined(IE_REDCORE)

#include "YSPTransparentWnd.h"
#include <WinUser.h>
#include <timeapi.h>

#include "ui/gfx/canvas.h"
#include "third_party/skia/include/core/SkTypes.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkDevice.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkTypeface.h"
#include "third_party/skia/include/core/SkRefCnt.h"
#include "base/strings/utf_string_conversions.h"
#include "base/win/windows_version.h"

#ifndef min
#define min
#endif

#ifndef max
#define max
#endif

#include <comdef.h>
#include <gdiplus.h>
#include <string>
#include "content/public/browser/browser_thread.h"

using namespace Gdiplus;
#pragma comment (lib, "gdiplus.lib")

#pragma warning ( disable : 4302 )

YspTransparentWnd::YspTransparentWnd()
	:weakFactory(this),
	hwnd(NULL),
	hParentHwnd(NULL),
	isHide(true),
	waterMarkFontSize(0),
	waterMarkColor(0),
	maskColor(0),
	alphaValue(0),
	delayShowTimer(new base::Timer(false, false))
{
	GdiplusStartupInput m_GdiInput;
	GdiplusStartup(&m_GdiToken, &m_GdiInput, NULL);
}

YspTransparentWnd::~YspTransparentWnd()
{
	YSPLoginManager::GetInstance()->RemoveObserver(this);

	if (hwnd)
	{
		::DestroyWindow(hwnd);
	}
	GdiplusShutdown(m_GdiToken);
}

bool YspTransparentWnd::Init(HWND parentHWND)
{
	hParentHwnd = parentHWND;
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

	hwnd = CreateWindowW(L"YspTransparentWindow", L"YspTransparentWindow", WS_POPUP,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	SetWindowLong(hwnd, GWL_USERDATA, (long)this);

	LONG nRet = ::GetWindowLong(hwnd, GWL_EXSTYLE);
	nRet = nRet | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE;
	::SetWindowLong(hwnd, GWL_EXSTYLE, nRet);

	::SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE);

	SetWaterMark(std::vector<base::string16>(), 0, 0);

	return true;
}

void YspTransparentWnd::SetRect(const RECT & rc)
{
	RECT oldRc;
	GetWindowRect(hwnd,&oldRc);
	int result=memcmp((const void*)&rc, (const void*)&oldRc,sizeof(RECT));
	if(result!=0)
		::SetWindowPos(hwnd, HWND_TOP, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, SWP_NOACTIVATE| SWP_NOZORDER);
}

void YspTransparentWnd::Show(bool isShow)
{
	if (isShow)
		if (isHide) {
			base::TimeDelta delayTime = base::TimeDelta::FromMilliseconds(0);
			delayShowTimer->Stop();
			delayShowTimer->Start(FROM_HERE, delayTime, base::Bind(&YspTransparentWnd::DelayShowWindow, weakFactory.GetWeakPtr()));
		}
		else {
			::SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE| SWP_NOZORDER);
		}
	else
	{
		delayShowTimer->Stop();
		::SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE);
		isHide = true;
	}
}

void YspTransparentWnd::GotoTop(bool isTopMost)
{
	::SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE  | SWP_NOSIZE | SWP_NOMOVE);
	if(isTopMost)
		::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE  | SWP_NOSIZE | SWP_NOMOVE);
	else
		::SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
}

bool YspTransparentWnd::IsVisible()
{
	if (hwnd == NULL) return false;
	return ::IsWindowVisible(hwnd) == TRUE;
}

void YspTransparentWnd::OnConfigDataUpdated(const std::string& type,
	const std::string& data)
{
	if (type == "accountInfo" || type == "strategy")
	{
		uint32_t color = YSPLoginManager::GetInstance()->GetWatermarkColor();
		int size = YSPLoginManager::GetInstance()->GetWatermarkFontSize();
		std::vector<base::string16> context = YSPLoginManager::GetInstance()->GetWatermarkString();

		SetWaterMark(context, size, color);
	}
}

void YspTransparentWnd::OnLoginRequestFailure(const std::string& error)
{
}

void YspTransparentWnd::OnLoginResponseParseFailure(const std::string& error)
{
}

void YspTransparentWnd::OnLoginFailure(base::string16 message)
{
	SetWaterMark(std::vector<base::string16>(), 0, 0);
	EnableScreenCapture(true);
}

void YspTransparentWnd::OnLoginSuccess(const base::string16 & name, const std::string & head_image_url)
{
	uint32_t color= YSPLoginManager::GetInstance()->GetWatermarkColor();
	int size=YSPLoginManager::GetInstance()->GetWatermarkFontSize();
	std::vector<base::string16> context=YSPLoginManager::GetInstance()->GetWatermarkString();

	SetWaterMark(context, size, color);

	EnableScreenCapture(YSPLoginManager::GetInstance()->GetScreenCaptureEnabled());
}

void YspTransparentWnd::OnLogout()
{
	SetWaterMark(std::vector<base::string16>(), 0, 0);
	EnableScreenCapture(true);
}

HWND YspTransparentWnd::GetHwnd()
{
	return hwnd;
}

void YspTransparentWnd::DelayShowWindow()
{
	if (IsWindow(hwnd) && IsWindow(hParentHwnd) &&
		::IsWindowVisible(hParentHwnd))
	{
		::SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
		isHide = false;
	}
}

void YspTransparentWnd::SetWaterMark(const std::vector<base::string16> & waterMarkStr, const int & fontSize,
	const DWORD & fontColor)
{
	bool isChange = false;
	waterMarkString.assign(waterMarkStr.begin(), waterMarkStr.end());
	waterMarkFontSize = fontSize;

	int alpha = 0;
	alpha = (fontColor >> 24) & 0xff;
	int r = fontColor & 0xff; 
	int g = (fontColor >> 8) & 0xff;
	int b = (fontColor >> 16) & 0xff;
	if (waterMarkColor != RGB(r, g, b))
	{
		waterMarkColor = RGB(r, g, b);
		isChange = true;
	}
	
	int mask_r = 0;
	if (r < 255)
		mask_r = r + 1;
	else
		mask_r = r - 1;
	maskColor = RGB(mask_r, g, b);

	if (IsWindow(hwnd) == FALSE)
		return;
	
	if (isChange)
	{
		//先设置为完全透明，在绘制完镂空色后再设置需要的透明度，防止如果新设置的镂空色和之前不同时能够直接看到之前的镂空色
		::SetLayeredWindowAttributes(hwnd, maskColor, 0, LWA_COLORKEY | LWA_ALPHA);
	}

	RECT rect = { 0,0,0,0 };
	GetClientRect(hwnd, &rect);
	::InvalidateRect(hwnd, &rect, TRUE);
	::UpdateWindow(hwnd);
	::SetLayeredWindowAttributes(hwnd, maskColor, alpha, LWA_COLORKEY | LWA_ALPHA);
}

void YspTransparentWnd::DrawWaterMarkUseSkia(HDC hdc, int width, int height)
{
	int dist = 0;
	if (width > height)
		dist = width;
	else
		dist = height;

	skia::RefPtr<SkCanvas> skCanvas = skia::AdoptRef(skia::CreateBitmapCanvas(3 * dist, 3 * dist, false));
	skCanvas->drawColor(SkColorSetARGB(255, GetRValue(maskColor), GetGValue(maskColor), GetBValue(maskColor)));
	if (waterMarkFontSize > 0)
	{
		SkPaint paint;
		paint.setColor(SkColorSetARGB(255, GetRValue(waterMarkColor), GetGValue(waterMarkColor), GetBValue(waterMarkColor)));
		paint.setTextSize(waterMarkFontSize);
		paint.setAntiAlias(false);
		paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);

		HGDIOBJ hFont = GetStockObject(DEFAULT_GUI_FONT);
		LOGFONT lf = { 0 };
		GetObject(hFont, sizeof(LOGFONT), &lf);
		DeleteObject(hFont);
		std::string fontName = "";
		fontName = base::UTF16ToASCII(lf.lfFaceName);
		skia::RefPtr<SkTypeface> face = skia::AdoptRef(SkTypeface::CreateFromName(fontName.c_str(), SkTypeface::kNormal));
		paint.setTypeface(face.get());

		int row = waterMarkString.size();
		SkScalar row_height = 0.0;
		SkRect layoutRect = SkRect::MakeEmpty();
		SkRect* stringRects = new SkRect[row];
		for (int i = 0; i < row; i++)
		{
			paint.measureText(waterMarkString[i].c_str(), waterMarkString[i].size() * sizeof(wchar_t), &stringRects[i]);
			row_height = stringRects[i].height();
			layoutRect.fBottom += stringRects[i].height();
			if (stringRects[i].width() > layoutRect.width())
			{
				layoutRect.fRight = stringRects[i].width();
			}
		}

		const int h_space = 75;
		const int v_space = 60;

		skCanvas->save();
		skCanvas->rotate(-30);
		SkScalar x = 0.0;
		SkScalar y = 0.0;
		while (y < 3 * dist)
		{
			int ox = x;
			while (ox < 3 * dist)
			{
				int oy = y;
				for (int i = 0; i < row; i++)
				{
					int xx = ox + (layoutRect.width() - stringRects[i].width()) / 2;
					skCanvas->drawText(waterMarkString[i].c_str(), waterMarkString[i].size() * sizeof(wchar_t), xx, oy, paint);
					oy += 1.5 * row_height;
				}
				ox += h_space + layoutRect.width();
			}

			x += layoutRect.width();
			while (x > 0.0)
			{
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
	bmi.bmiHeader.biHeight = -skbmp.height(); // top-down image  
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;
	int ret = 0;
	ret = SetDIBitsToDevice(hdc,
		0, 0,
		width, height,
		1.5*dist, 1.5*dist,
		0, skbmp.height(),
		(unsigned char *)skbmp.getPixels(),
		&bmi,
		DIB_RGB_COLORS);
	skbmp.unlockPixels();
}

void YspTransparentWnd::DrawWaterMarkUseGdi(HDC hdc, int width, int height)
{
	int dist = 0;
	if (width > height)
		dist = width;
	else
		dist = height;

	HDC memdc = ::CreateCompatibleDC(hdc);
	HBITMAP hBitmap = ::CreateCompatibleBitmap(hdc, 2 * dist, 2 * dist);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(memdc, hBitmap);

	Graphics* memGraph = Graphics::FromHDC(memdc);
	memGraph->SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintSingleBitPerPixel);
	memGraph->SetCompositingMode(CompositingModeSourceCopy);
	memGraph->SetCompositingQuality(CompositingQualityHighSpeed);
	memGraph->SetPixelOffsetMode(PixelOffsetModeNone);
	memGraph->SetSmoothingMode(SmoothingModeNone);
	memGraph->SetInterpolationMode(InterpolationModeDefault);

	Color bgColor;
	bgColor.SetFromCOLORREF(maskColor);
	memGraph->Clear(bgColor);

	HGDIOBJ hFont = GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT lf = { 0 };
	int ret = GetObject(hFont, sizeof(LOGFONT), &lf);
	DeleteObject(hFont);
	if (ret > 0)
	{
		Gdiplus::FontFamily fontFanily(lf.lfFaceName);
		if (fontFanily.IsAvailable() == FALSE)
		{
			delete memGraph;
			BitBlt(hdc, 0, 0, width, height, memdc, 0, 0, SRCCOPY);
			::SelectObject(memdc, hOldBitmap);
			DeleteObject(hBitmap);
			DeleteDC(memdc);
			return;
		}
		Gdiplus::Font font(&fontFanily, waterMarkFontSize, 0, Gdiplus::UnitPixel);
		if (font.IsAvailable())
		{
			int row = waterMarkString.size();
			Gdiplus::REAL row_height = 0.0;
			Gdiplus::RectF layoutRect(0.0, 0.0, 0.0, 0.0);
			Gdiplus::RectF* stringRects = new Gdiplus::RectF[row];
			for (int i = 0; i < row; i++)
			{
				memGraph->MeasureString(waterMarkString[i].c_str(), -1, &font, PointF(0, 0), &stringRects[i]);
				row_height = stringRects[i].Height;
				layoutRect.Height += stringRects[i].Height;
				if (stringRects[i].Width > layoutRect.Width)
				{
					layoutRect.Width = stringRects[i].Width;
				}
			}

			Color fontColor;
			fontColor.SetFromCOLORREF(waterMarkColor);
			Gdiplus::SolidBrush br(fontColor);
			memGraph->RotateTransform(-30.0);
			StringFormat stringformat(Gdiplus::StringAlignment::StringAlignmentNear);

			const int h_space = 75;
			const int v_space = 60;

			Gdiplus::REAL x = 0.0;
			Gdiplus::REAL y = 0.0;
			while (y < 2 * dist)
			{
				int ox = x;
				while (ox < 2 * dist)
				{
					int oy = y;
					for (int i = 0; i < row; i++)
					{
						int xx = ox + (layoutRect.Width - stringRects[i].Width) / 2;
						memGraph->DrawString(waterMarkString[i].c_str(), -1, &font, PointF(xx, oy), &stringformat, &br);
						oy += row_height;
					}
					ox += h_space + layoutRect.Width;
				}

				x += layoutRect.Width;
				while (x > 0.0)
				{
					x -= h_space;
					x -= layoutRect.Width;
				}
				y += v_space + layoutRect.Height;
			}

			delete[] stringRects;
			memGraph->ResetTransform();
		}

		BitBlt(hdc, 0, 0,width, height, memdc, 0.58*dist, 0, SRCCOPY);

		delete memGraph;
		::SelectObject(memdc, hOldBitmap);
		DeleteObject(hBitmap);
		DeleteDC(memdc);
	}
}

LRESULT YspTransparentWnd::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		YspTransparentWnd* pTransWnd=(YspTransparentWnd*)GetWindowLong(hWnd, GWL_USERDATA);
		if(pTransWnd==NULL)
			break;
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		
		if (pTransWnd->waterMarkString.empty())
		{
			EndPaint(hWnd, &ps);
			break;
		}

		DWORD time = ::timeGetTime();
		RECT rc;
		::GetClientRect(hWnd, &rc);

		base::win::Version ver = base::win::GetVersion();
		//ver = base::win::Version::VERSION_SERVER_2003;
		if (ver < base::win::VERSION_WIN8)
		{
			pTransWnd->DrawWaterMarkUseGdi(hdc, rc.right, rc.bottom);
		}
		else if (ver >= base::win::VERSION_WIN8)
		{
			pTransWnd->DrawWaterMarkUseSkia(hdc, rc.right, rc.bottom);
		}

		time=::timeGetTime() - time;
		//LOG(INFO) << time;
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_ERASEBKGND:
		return TRUE;
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void YspTransparentWnd::EnableScreenCapture(bool isEnable)
{
	typedef BOOL(WINAPI *SetWindowDisplayAffinityPtr)(HWND, DWORD);
	SetWindowDisplayAffinityPtr func_ptr =reinterpret_cast<SetWindowDisplayAffinityPtr>(
			GetProcAddress(GetModuleHandleA("user32.dll"), "SetWindowDisplayAffinity"));
	if (func_ptr)
	{
		if (isEnable)
			func_ptr(GetHwnd(), WDA_NONE);
		else
			func_ptr(GetHwnd(), WDA_MONITOR);
	}
}
#endif
