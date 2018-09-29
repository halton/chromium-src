#if defined(REDCORE) && defined(WATERMARK) && defined(IE_REDCORE)

#ifndef CHROME_BROWSER_UI_VIEWS_FRAME_YSP_TRANSPRENTWND_H_
#define CHROME_BROWSER_UI_VIEWS_FRAME_YSP_TRANSPRENTWND_H_

#include <windows.h>
#include "base/memory/weak_ptr.h"
#include "base/timer/timer.h"
#include "components/ysp_login/ysp_login_manager.h"

class YspTransparentWnd:public YSPLoginManagerObserver
{
public:
	YspTransparentWnd();
	~YspTransparentWnd();

	bool Init(HWND parentHWND);
	void SetRect(const RECT& rc);
	void Show(bool isShow);
	void GotoTop(bool isTopMost);
	bool IsVisible();

	//YSPLoginManagerObserver
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
	void SetWaterMark(const std::vector<base::string16>& waterMarkStr, const int& fontSize, const DWORD& fontColor);
	void DrawWaterMarkUseSkia(HDC hdc, int width, int height);
	void DrawWaterMarkUseGdi(HDC hdc, int width, int height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void EnableScreenCapture(bool isEnable);
private:
	ULONG_PTR m_GdiToken;
	HWND hwnd;
	HWND hParentHwnd;
	base::WeakPtrFactory<YspTransparentWnd> weakFactory;
	bool isHide;
	COLORREF waterMarkColor;
	COLORREF maskColor;
	int alphaValue;
	int waterMarkFontSize;
	std::vector<base::string16> waterMarkString;
	scoped_ptr<base::Timer> delayShowTimer;
};

#endif
#endif
