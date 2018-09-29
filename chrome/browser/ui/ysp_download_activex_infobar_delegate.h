#ifdef IE_REDCORE
//ysp+ {Activex distribute}
#ifndef CHROME_BROWSER_UI_VIEWS_YSP_DOWNLOAD_ACTIVEX_INFOBAR_DELEGATE_H_
#define CHROME_BROWSER_UI_VIEWS_YSP_DOWNLOAD_ACTIVEX_INFOBAR_DELEGATE_H_

#include "base/location.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "base/metrics/histogram_macros.h"
#include "chrome/grit/generated_resources.h"
#include "components/infobars/core/infobar.h"
#include "components/infobars/core/infobar_delegate.h"
#include "chrome/browser/infobars/infobar_service.h"
#include "content/public/browser/web_contents.h"
#include "components/download/public/common/download_item.h"
#include "components/download/public/common/download_interrupt_reasons.h"
#include <string>
#include "base/memory/singleton.h"

class BrowserView;

struct ActivexDownloadInfo
{
	std::wstring displayName;
	std::wstring fileName;
	std::wstring downloadUrl;
	std::wstring md5;
	ActivexDownloadInfo();
	~ActivexDownloadInfo();
	ActivexDownloadInfo(const ActivexDownloadInfo& other);
};

class DownloadActivexInfobarDelegate 
	: public infobars::InfoBarDelegate
{
public:
	enum InfoBarButton {
		BUTTON_NONE = 0,
		BUTTON_OK = 1 << 0,
		BUTTON_CANCEL = 1 << 1,
		BUTTON_NO_PROMPT = 1 << 2,
	};

	DownloadActivexInfobarDelegate(BrowserView* pView, const ActivexDownloadInfo& info);
	~DownloadActivexInfobarDelegate() override;

	static void Create(BrowserView* pView, const ActivexDownloadInfo& info);
	static void ClearAllActivexInfoBar(BrowserView* pView);

	base::string16 GetMessageText() const;
	int GetButtons() const;
	bool OKButtonTriggersUACPrompt() const;
	base::string16 GetButtonLabel(InfoBarButton button) const;
	base::string16 GetLinkText() const;
	bool Accept();
	bool Cancel();
	bool NoPrompt();
	GURL GetLinkURL() const;
	bool LinkClicked(WindowOpenDisposition disposition);

private:
	static std::unique_ptr<infobars::InfoBar> CreateInfoBar(
		std::unique_ptr<DownloadActivexInfobarDelegate> delegate);

	// InfoBarDelegate:
	Type GetInfoBarType() const override;
	infobars::InfoBarDelegate::InfoBarIdentifier GetIdentifier() const override;
	int GetIconId() const override;
	// gfx::VectorIcon& GetVectorIcon() const override;
	bool ShouldExpire(const NavigationDetails& details) const override;
	bool EqualsDelegate(infobars::InfoBarDelegate* delegate) const override;
	DownloadActivexInfobarDelegate* AsDownloadActivexInfobarDelegate() override;

	void DownloadStarted(download::DownloadItem* item,
												download::DownloadInterruptReason interrupt_reason);
	void CloseInfobar();
private:
	BrowserView* pBrowserView;
	ActivexDownloadInfo activexInfo;
};

class ActivexDownloadObserver : public download::DownloadItem::Observer
{
public:
	static ActivexDownloadObserver* GetInstance();

	// content::DownloadItem::Observer:
	 void OnDownloadUpdated(download::DownloadItem* download) override;
	 void OnDownloadRemoved(download::DownloadItem* download) override;

private:
	friend struct base::DefaultSingletonTraits<ActivexDownloadObserver>;
	ActivexDownloadObserver();
	~ActivexDownloadObserver() override;
};

#endif
#endif
