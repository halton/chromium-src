#ifdef IE_REDCORE
#include "chrome/browser/ui/ysp_download_activex_infobar_delegate.h"
#include "ui/base/l10n/l10n_util.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "content/public/browser/download_manager.h"
#include "components/download/public/common/download_url_parameters.h"
#include "base/memory/weak_ptr.h"
#include "url/gurl.h"
#include "base/files/File_path.h"
#include "base/files/file_util.h"
#include "base/values.h"
#include "base/path_service.h"
#include "base/strings/utf_string_conversions.h"
#include "components/prefs/scoped_user_pref_update.h"
#include "chrome/common/pref_names.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/browser/browser_process.h"
#include "components/infobars/core/infobar.h"

DownloadActivexInfobarDelegate::DownloadActivexInfobarDelegate(BrowserView* pView , const ActivexDownloadInfo& info)
	:pBrowserView(pView),
	activexInfo(info)
{
}

DownloadActivexInfobarDelegate::~DownloadActivexInfobarDelegate()
{
}

void DownloadActivexInfobarDelegate::Create(BrowserView* pView, const ActivexDownloadInfo& info)
{
	if (pView == NULL)
		return;
	content::WebContents* webcontent = pView->GetActiveWebContents();
	if (webcontent == NULL)
		return;
	InfoBarService* infobar_service = NULL;
	infobar_service = InfoBarService::FromWebContents(webcontent);
	if (infobar_service==NULL)
		return;
	infobar_service->AddInfoBar(
		DownloadActivexInfobarDelegate::CreateInfoBar(std::unique_ptr<DownloadActivexInfobarDelegate>(
			new DownloadActivexInfobarDelegate(pView, info))));
}

void DownloadActivexInfobarDelegate::ClearAllActivexInfoBar(BrowserView* pView)
{
	TabStripModel* pTabModel = NULL;
	pTabModel = pView->browser()->tab_strip_model();
	if (pTabModel == NULL)
		return;
	int count = pTabModel->count();
	int i = 0;
	for (i = 0; i < count; i++)		//Get All InfoBarService
	{
		content::WebContents* webcontent = pTabModel->GetWebContentsAt(i);
		if (webcontent == NULL)
			continue;
		InfoBarService* infobar_service = NULL;
		infobar_service = InfoBarService::FromWebContents(webcontent);
		if (infobar_service == NULL)
			continue;
		std::vector<infobars::InfoBar*> willRemoveInfoBar;
		size_t j = 0;
		for (j = 0; j < infobar_service->infobar_count(); j++)	//Find All ActivexInfobar for every InfoBarService
		{
			infobars::InfoBar* bar=  infobar_service->infobar_at(j);
			if(bar==NULL)
				continue;
			if (bar->delegate()->GetIdentifier() ==
				infobars::InfoBarDelegate::DOWNLOAD_ACTIVEX_INFOBAR_DELEGATE)
				willRemoveInfoBar.push_back(bar);
		}
		std::vector<infobars::InfoBar*>::iterator iter = willRemoveInfoBar.begin();
		for (; iter != willRemoveInfoBar.end(); iter++)	//Remove ActivexInfobar
		{
			infobar_service->RemoveInfoBar(*iter);
		}
	}
}

	ActivexDownloadInfo::ActivexDownloadInfo()
	{
		displayName = L"";
		fileName = L"";
		downloadUrl = L"";
		md5 = L"";
	}
	ActivexDownloadInfo::ActivexDownloadInfo(const ActivexDownloadInfo& other) = default;
    ActivexDownloadInfo::~ActivexDownloadInfo(){}
	
infobars::InfoBarDelegate::Type DownloadActivexInfobarDelegate::GetInfoBarType() const
{
	return InfoBarDelegate::WARNING_TYPE;
}

//just for link error.
// gfx::VectorIcon& GetVectorIcon() const 
// {   gfx::VectorIcon vector_icon;
// 	return vector_icon;
// }

infobars::InfoBarDelegate::InfoBarIdentifier DownloadActivexInfobarDelegate::GetIdentifier() const
{
	return infobars::InfoBarDelegate::DOWNLOAD_ACTIVEX_INFOBAR_DELEGATE;
}

int DownloadActivexInfobarDelegate::GetIconId() const
{
	return infobars::InfoBarDelegate::kNoIconID;
}

bool DownloadActivexInfobarDelegate::ShouldExpire(const NavigationDetails & details) const
{
	return false;
}

bool DownloadActivexInfobarDelegate::EqualsDelegate(InfoBarDelegate* delegate) const {
	DownloadActivexInfobarDelegate* download_activex_delegate =
		delegate->AsDownloadActivexInfobarDelegate();
	return download_activex_delegate &&
		(download_activex_delegate->GetMessageText() == GetMessageText());
}

DownloadActivexInfobarDelegate* DownloadActivexInfobarDelegate::AsDownloadActivexInfobarDelegate()
{
	return this;
}

base::string16 DownloadActivexInfobarDelegate::GetMessageText() const
{
	std::wstring txt= l10n_util::GetStringFUTF16(IDS_YSP_ACTIVEX_DOWNLOAD_INFOBAR_MESSAGE, activexInfo.displayName);
	return txt;
}

int DownloadActivexInfobarDelegate::GetButtons() const {
	return BUTTON_OK | BUTTON_CANCEL | BUTTON_NO_PROMPT;
}

base::string16 DownloadActivexInfobarDelegate::GetButtonLabel(InfoBarButton button) const
{
	if (button == BUTTON_OK)
		return l10n_util::GetStringUTF16(IDS_YSP_ACTIVEX_DOWNLOAD_BUTTON_OK);
	else if (button == BUTTON_CANCEL)
		return l10n_util::GetStringUTF16(IDS_YSP_ACTIVEX_DOWNLOAD_BUTTON_CANCEL);
	else if (button == BUTTON_NO_PROMPT)
		return l10n_util::GetStringUTF16(IDS_YSP_ACTIVEX_DOWNLOAD_BUTTON_NO_PROMPT);
	return L"";
}

bool DownloadActivexInfobarDelegate::OKButtonTriggersUACPrompt() const
{
	return false;
}

base::string16 DownloadActivexInfobarDelegate::GetLinkText() const {
	return base::string16();
}

// comment just for cmp
bool DownloadActivexInfobarDelegate::Accept()
{
	// GURL gurl(activexInfo.downloadUrl);
	// content::WebContents* webcontent = pBrowserView->GetActiveWebContents();
	// if (webcontent == NULL)
	// 	return false;
	// content::DownloadManager* manager =
	// 	content::BrowserContext::GetDownloadManager(webcontent->GetBrowserContext());
	// if (manager == NULL)
	// 	return false;
	// std::unique_ptr<download::DownloadUrlParameters> downloadParam(download::DownloadUrlParameters::FromWebContents(webcontent, gurl));
	// base::FilePath path;
	// base::PathService::Get(chrome::DIR_USER_DATA, &path);
	// base::string16 strPath = path.value();
	// strPath += L"\\Activex\\";
	// strPath +=activexInfo.fileName;
	// path=base::FilePath(strPath);
	// if (base::PathExists(path.DirName()) == false)
	// 	base::CreateDirectory(path.DirName());
	// downloadParam->set_file_path(path);
	// downloadParam->set_callback(
	// 	base::Bind(&DownloadActivexInfobarDelegate::DownloadStarted, base::Unretained(this)));
	// manager->DownloadUrl(std::move(downloadParam));
	return false;
}

bool DownloadActivexInfobarDelegate::Cancel()
{
	return true;
}

bool DownloadActivexInfobarDelegate::NoPrompt()
{
	if (activexInfo.md5 != L"")
	{
		std::string md5 = base::UTF16ToASCII(activexInfo.md5);
		PrefService* pref_service = g_browser_process->local_state();
		DictionaryPrefUpdate update(pref_service, prefs::kYSPActivexNoPromptInfo);
		update->SetBoolean(md5, true);
	}

	return true;
}

GURL DownloadActivexInfobarDelegate::GetLinkURL() const {
	return GURL();
}

bool DownloadActivexInfobarDelegate::LinkClicked(WindowOpenDisposition disposition) {
	infobar()->owner()->OpenURL(GetLinkURL(), disposition);
	return false;
}

void DownloadActivexInfobarDelegate::DownloadStarted(download::DownloadItem * item, download::DownloadInterruptReason interrupt_reason)
{
	if (interrupt_reason != download::DOWNLOAD_INTERRUPT_REASON_NONE) 
	{
		std::string err = "";
		err=DownloadInterruptReasonToString(interrupt_reason);
		LOG(WARNING) << "Download activex " << activexInfo.displayName
			<<" from " << activexInfo.downloadUrl <<" failed reason: " << err;
		CloseInfobar();
		return;
	}
	item->set_is_update(true);
	item->SetOpenWhenComplete(true);
	// for cmp
	// item->SetIsTemporary(false);
	item->AddObserver(ActivexDownloadObserver::GetInstance());
	CloseInfobar();
}

void DownloadActivexInfobarDelegate::CloseInfobar()
{
	content::WebContents* webcontent = pBrowserView->GetActiveWebContents();
	InfoBarService* infobar_service = NULL;
	infobar_service = InfoBarService::FromWebContents(webcontent);
	infobar_service->RemoveInfoBar(infobar());
}

ActivexDownloadObserver * ActivexDownloadObserver::GetInstance()
{
	return base::Singleton<ActivexDownloadObserver>::get();
}

void ActivexDownloadObserver::OnDownloadUpdated(download::DownloadItem * download)
{
	download::DownloadItem::DownloadState state = download->GetState();
	switch (state)
	{
	case download::DownloadItem::IN_PROGRESS:
		break;
	case download::DownloadItem::COMPLETE:
		download->RemoveObserver(this);
		break;
	case download::DownloadItem::CANCELLED:
		download->RemoveObserver(this);
		break;
	case download::DownloadItem::INTERRUPTED:
		break;
	case download::DownloadItem::MAX_DOWNLOAD_STATE:
		break;
	default:
		break;
	}
}

void ActivexDownloadObserver::OnDownloadRemoved(download::DownloadItem * download)
{
	download->RemoveObserver(this);
}

ActivexDownloadObserver::ActivexDownloadObserver()
{
}

ActivexDownloadObserver::~ActivexDownloadObserver()
{
}
#endif
