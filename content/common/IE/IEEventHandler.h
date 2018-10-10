#ifndef CONTENT_COMMON_IE_IE_EVENT_HANDLER_H_
#define CONTENT_COMMON_IE_IE_EVENT_HANDLER_H_
#pragma once
#include <windows.h>
#include <string>
#include <MsHTML.h>
#include "ATLInclude.h"

namespace IE {
	class IEEventHandler
	{
	public:
		IEEventHandler() {};
		virtual ~IEEventHandler() {};

		virtual bool OnBeforeNavigate(std::wstring url, bool clickUrl) = 0;
		virtual void OnFinishNavigate(std::wstring url, std::wstring faviconUrls) = 0;
		virtual void OnTitleChange(std::wstring title) = 0;
		virtual bool OnLoadUrlNewWnd(std::wstring url, long flag, IDispatch** pDisp) = 0;
		virtual void OnBrowserClosing() = 0;
		virtual void SetIEHwnd(HWND hwnd)=0;
		virtual bool OnShowContxtMenu(int id, POINT pt)= 0;
		virtual bool OnDownLoadFile(const std::wstring& url, int status, VARIANT variant) = 0;
		virtual void GetGoBackOrForwardState(bool* goBack, bool* goForward) = 0;
		virtual void GoBack() = 0;
		virtual void GoForward() = 0;
		virtual void OnRequestChangePos(int left, int top, int width, int height) = 0;
		virtual void OnGetMainWndPos(RECT* rect)=0;
		virtual void OnIEServerCreated(HWND hwnd)=0;
		virtual void OnUpdateCookie(const std::wstring& cookie)=0;
		virtual void OnClickElement(CComPtr<IHTMLElement> clickElement, bool byEnterKey) = 0;
		virtual void OnGetLoginInfo(std::wstring url, std::wstring userName, std::wstring userPwd) = 0;
		virtual void OnRequestAcceleratorFromKeyboard(int keyCode, int modifiers) = 0;
		virtual void OnRequestAcceleratorFromMouseWheel(int delta, int Pt_x, int Pt_y, int modifiers) = 0;
		virtual void OnWindowDomainAuthenticate(std::wstring* userName, std::wstring* userPwd) = 0;
		virtual void OnQueryPrivateDNS(const std::wstring& host, std::wstring* ipListJsonStr) = 0;
	};
} //namespace IE

#endif

