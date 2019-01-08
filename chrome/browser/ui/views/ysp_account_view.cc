// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// ysp+ { login }

#include "chrome/browser/ui/views/ysp_account_view.h"

#include "ui/gfx/canvas.h"
#include "ui/gfx/path.h"
#include "ui/gfx/scoped_canvas.h"
#include "ui/gfx/skia_util.h"
#include "ui/base/theme_provider.h"
#include "chrome/browser/themes/theme_properties.h"
#include "chrome/browser/ui/layout_constants.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/toolbar/toolbar_view.h"
#include "third_party/skia/include/pathops/SkPathOps.h"
#include "chrome/grit/generated_resources.h"
#include "chrome/grit/theme_resources.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/views/controls/image_view.h"
#include "ui/views/controls/label.h"
#include "ui/views/layout/grid_layout.h"
#include "chrome/browser/ui/views/ysp_login_view.h"
#include "chrome/browser/ui/views/tabs/tab_strip.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/views/location_bar/location_bar_view.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/constants.h"
#include "extensions/common/extension.h"
#include "chrome/browser/extensions/extension_service.h"
#include "components/prefs/pref_service.h"
#include "chrome/common/pref_names.h"
#include "chrome/browser/ui/view_ids.h"
#if defined(IE_REDCORE)
#include "ui/views/win/hwnd_util.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/ui/ysp_download_activex_infobar_delegate.h"
#endif
#include "base/json/json_writer.h"
#include "base/values.h"
#include "chrome/browser/ysp_update/ysp_pac_manager.h"
#include "base/md5.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/path_service.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/common/chrome_paths.h"
#include "ui/base/window_open_disposition.h"

const char YSPAccountView::kViewClassName[] = "YSPAccountView";

class YSPAvatarView : public views::Label {
 public:
  YSPAvatarView(const base::string16& text);
  ~YSPAvatarView() override{};
  void SetCircle(bool circle) { is_circle_ = circle; }

  void OnPaint(gfx::Canvas* canvas) override;

 private:
  bool is_circle_;
};

YSPAvatarView::YSPAvatarView(const base::string16& text)
    : views::Label(text), is_circle_(false) {}

void YSPAvatarView::OnPaint(gfx::Canvas* canvas) {
  if (is_circle_) {
    gfx::Rect image_bounds(bounds());
    int corner_radius = image_bounds.width() / 2;
    SkPath clip_path;
    SkRect clip_rect =
        SkRect::MakeXYWH(image_bounds.x(), image_bounds.y(),
                         image_bounds.width(), image_bounds.height());
    clip_path.addRoundRect(clip_rect, SkIntToScalar(corner_radius),
                           SkIntToScalar(corner_radius));
    canvas->ClipPath(clip_path, true);
  }
  views::Label::OnPaint(canvas);
}

YSPAccountView::YSPAccountView(BrowserView* browser_view)
    : browser_view_(browser_view),
      head_view_(nullptr),
      head_image_(nullptr),
      weak_factory_for_file_(this),
      weak_factory_for_ui_(this) {
  set_id(ViewID::VIEW_ID_YSP_ACCOUNT_VIEW);
  YSPLoginManager::GetInstance()->AddObserver(this);
  set_notify_enter_exit_on_child(true);
  SetEventTargeter(
      std::unique_ptr<views::ViewTargeter>(new views::ViewTargeter(this)));
}

YSPAccountView::~YSPAccountView() {
  YSPLoginManager::GetInstance()->RemoveObserver(this);
}

void YSPAccountView::Init() {
  CreateViews();
}

void YSPAccountView::CreateViews() {
  ui::ResourceBundle& bundle = ui::ResourceBundle::GetSharedInstance();
  head_view_ = new views::ImageView;
  head_view_->SetImage(*bundle.GetImageSkiaNamed(IDR_YSP_LOGIN_AVATAR));
  head_view_->SetImageSize(gfx::Size(kAvatarImageSize, kAvatarImageSize));
  head_view_->SetDrawCircle(true);
  AddChildView(head_view_);

  const gfx::FontList& small_font =
      ui::ResourceBundle::GetSharedInstance().GetFontList(
          ui::ResourceBundle::SmallFont);

  name_label_ = new YSPAvatarView(base::string16());
  name_label_->SetFontList(small_font.DeriveWithSizeDelta(18 - small_font.GetFontSize()));
  name_label_->SetVisible(false);
  name_label_->SetEnabledColor(SK_ColorWHITE);
  name_label_->SetAutoColorReadabilityEnabled(false);
  reinterpret_cast<YSPAvatarView *>(name_label_)->SetCircle(true);
  AddChildView(name_label_);
}

void YSPAccountView::OnPaint(gfx::Canvas* canvas) {
}

void YSPAccountView::Layout() {
  int x = (width() - kAvatarImageSize) / 2;
  int y = (height() - kAvatarImageSize) / 2;
  head_view_->SetBounds(x, y, kAvatarImageSize, kAvatarImageSize);
  head_view_->SetDrawCircle(true);
  if (name_label_->visible()) {
    name_label_->SetBounds(x, y, kAvatarImageSize, kAvatarImageSize);
  }
}

void YSPAccountView::OnThemeChanged() {
}

const char* YSPAccountView::GetClassName() const {
  return kViewClassName;
}

bool YSPAccountView::OnMousePressed(const ui::MouseEvent& event) {
  // TODO (LIUWEI): work around when personal settings are not ready
  const GURL url("ep://me");
  const content::Referrer ref(url, blink::kWebReferrerPolicyDefault);
  content::OpenURLParams param(url,
                               ref,
                               WindowOpenDisposition::SINGLETON_TAB,
                               ui::PAGE_TRANSITION_AUTO_TOPLEVEL,
                               false);
  // FIXME(mtz): Disable click on login icon.
  browser_view_->browser()->OpenURL(param);

  return false;
}

void YSPAccountView::OnMouseReleased(const ui::MouseEvent& event) {
  views::View::OnMouseReleased(event);
}

void YSPAccountView::OnMouseMoved(const ui::MouseEvent& event) {
  // TODO (LIUWEI) move to platform specific file
#if defined(OS_WIN)
  SetCursor(LoadCursor(NULL, IDC_HAND));
#endif
}

void YSPAccountView::OnMouseEntered(const ui::MouseEvent& event) {
  // TODO (LIUWEI) move to platform specific file
#if defined(OS_WIN)
  SetCursor(LoadCursor(NULL, IDC_HAND));
#endif
}

void YSPAccountView::OnMouseExited(const ui::MouseEvent& event) {
  views::View::OnMouseExited(event);
}

// YSPLoginManagerObserver:
void YSPAccountView::OnLoginRequestFailure(const std::string& error) {
  DLOG(INFO) << "YSPAccountView::OnLoginRequestFailure";
}

void YSPAccountView::OnLoginResponseParseFailure(const std::string& error) {
  DLOG(INFO) << "YSPAccountView::OnLoginResponseParseFailure";
}

void YSPAccountView::OnLoginFailure(const base::string16& message) {
  DLOG(INFO) << "YSPAccountView::OnLoginFailure";
}

void YSPAccountView::OnConfigDataUpdated(const std::string& type,
                                         const std::string& data) {
  if (type == "pc") {
#if defined(IE_REDCORE)
    PrefService* pref_service = g_browser_process->local_state();
    const base::DictionaryValue* activex_no_prompt_info =
        pref_service->GetDictionary(prefs::kYSPActivexNoPromptInfo);
    std::vector<ActivexDownloadInfo> info = YSPLoginManager::GetInstance()->GetActivexDownloadInfo();
    std::vector<ActivexDownloadInfo>::iterator iter = info.begin();
    for (; iter != info.end(); iter++) {
      std::string md5 = base::UTF16ToASCII(iter->md5);
      if (activex_no_prompt_info->HasKey(md5))
        continue;
      BeginCheckFileMD5(*iter);
    }
#endif
  }
}

void YSPAccountView::OnLoginSuccess(const base::string16& name,
  const std::string& head_image_url) {
  DLOG(INFO) << "YSPAccountView::OnLoginSuccess";
  if (!browser_view_)
    return;

  user_name_ = name;
  head_image_url_ = head_image_url;
  // TODO: (LIUWEI) Update user head image
  Browser* browser = browser_view_->browser();
  if (browser) {
    browser->SetPopup();
  }
  if (!head_image_url.empty()) {
    DownloadImage(head_image_url);
  } else {
    ShowDefaultAvatar();
  }

#ifdef IE_REDCORE
  content::WebContents* web_contents = browser_view_->GetActiveWebContents();
  if (web_contents)
    YSPPACManager::GetInstance()->RequestPAC(web_contents);

  PrefService* pref_service = g_browser_process->local_state();
  const base::DictionaryValue* activex_no_prompt_info =
      pref_service->GetDictionary(prefs::kYSPActivexNoPromptInfo);
  std::vector<ActivexDownloadInfo> info = YSPLoginManager::GetInstance()->GetActivexDownloadInfo();
  std::vector<ActivexDownloadInfo>::iterator iter = info.begin();
  for (; iter != info.end(); iter++) {
    std::string md5 = base::UTF16ToASCII(iter->md5);
    if (activex_no_prompt_info->HasKey(md5))
      continue;
    BeginCheckFileMD5(*iter);
  }
#endif
  InvalidateLayout();
  browser_view_->frame()->non_client_view()->Layout();
  browser_view_->frame()->non_client_view()->SchedulePaint();
}

void YSPAccountView::OnLogout() {
  DLOG(INFO) << "YSPAccountView::OnLogout";

  name_label_->SetVisible(false);
  ui::ResourceBundle& bundle = ui::ResourceBundle::GetSharedInstance();
  head_view_->SetImage(*bundle.GetImageSkiaNamed(IDR_YSP_LOGIN_AVATAR));
  head_image_.reset();
  ResetFeatureState();
  user_name_.clear();
  if (browser_view_) {
    InvalidateLayout();
    browser_view_->frame()->non_client_view()->Layout();
    browser_view_->frame()->non_client_view()->SchedulePaint();
  }
#if defined(IE_REDCORE)
  DownloadActivexInfobarDelegate::ClearAllActivexInfoBar(browser_view_);
#endif
}

const gfx::ImageSkia* YSPAccountView::GetHeadImage() {
  if (!head_image_.get())
    return ui::ResourceBundle::GetSharedInstance().GetImageSkiaNamed(IDR_YSP_LOGIN_AVATAR);

  return head_image_.get();
}

void YSPAccountView::DownloadImage(const std::string& url) {
  content::WebContents* web_contents = browser_view_->GetActiveWebContents();
  if (!web_contents)
    return;

  web_contents->DownloadImage(
      GURL(url), true, 0, true,
      base::Bind(&YSPAccountView::DidDownloadFavicon, base::Unretained(this)));
}

void YSPAccountView::DidDownloadFavicon(
  int id,
  int http_status_code,
  const GURL& image_url,
  const std::vector<SkBitmap>& bitmaps,
  const std::vector<gfx::Size>& original_bitmap_sizes) {
  DLOG(INFO) << "YSPAccountView::DidDownloadFavicon";
  if (head_view_ && bitmaps.size() > 0) {
    name_label_->SetVisible(false);
    head_image_.reset(new gfx::ImageSkia(gfx::ImageSkiaRep(bitmaps[0], 1)));
    head_view_->SetImage(*head_image_);
    if (YSPLoginView::IsShowing())
      YSPLoginView::SetAndRefreshAvatar(head_image_.get());
  } else {
    ShowDefaultAvatar();
  }
  Layout();
}

void YSPAccountView::ResetFeatureState() {
  if (browser_view_)
    browser_view_->toolbar()->location_bar()->SetAddressBarEnable(true);

  UpdateScreenCaptureState(true);
}

void YSPAccountView::UpdateScreenCaptureState(bool enable) {
#if defined(IE_REDCORE)
  HWND hwnd = views::HWNDForNativeWindow(browser_view_->GetNativeWindow());
  DLOG(INFO) << "YSPAccountView::UpdateScreenCaptureState hwnd: " << hwnd;
  SetWindowDisplayAffinityPtr func_ptr =
      reinterpret_cast<SetWindowDisplayAffinityPtr>(
      GetProcAddress(GetModuleHandleA("user32.dll"), "SetWindowDisplayAffinity"));
  if(func_ptr) {
    int num = func_ptr(hwnd, (enable ? 0 /*WDA_NONE*/ : 1 /*WDA_MONITOR*/));
    if (!num)
      LOG(INFO) << "SetWindowDisplayAffinity error !";
  }
#endif
}

#if defined(IE_REDCORE)
void YSPAccountView::BeginCheckFileMD5(const ActivexDownloadInfo info) {
  content::BrowserThread::PostTask(
      content::BrowserThread::IO, FROM_HERE,
      base::Bind(&YSPAccountView::SumMD5, weak_factory_for_file_.GetWeakPtr(),
                 info));
}

void YSPAccountView::SumMD5(const  ActivexDownloadInfo info) {
  base::TimeTicks tickst = base::TimeTicks::Now();
  std::string md5 = "";
  base::FilePath path;
  base::PathService::Get(chrome::DIR_USER_DATA, &path);
  base::string16 str_path = path.value();
  str_path += L"\\Activex\\" + info.filename;
  FILE* file = _wfopen(str_path.c_str(), L"rb");
  if (file == NULL) {
    content::BrowserThread::PostTask(
        content::BrowserThread::UI, FROM_HERE,
        base::Bind(&YSPAccountView::OnSumMD5, weak_factory_for_ui_.GetWeakPtr(),
                   md5, info));
    return;
  }
  fseek(file, 0L, SEEK_END);
  int sz = ftell(file);
  fseek(file, 0L, SEEK_SET);
  if (sz == 0)  {
    content::BrowserThread::PostTask(
        content::BrowserThread::UI, FROM_HERE,
        base::Bind(&YSPAccountView::OnSumMD5, weak_factory_for_ui_.GetWeakPtr(),
                   md5, info));
    return;
  }
  char* buff = new char[sz];
  memset(buff, 0, sz);
  fread(buff, 1, sz, file);
  fclose(file);
  base::MD5Digest  digest = { 0 };
  base::MD5Sum(buff, sz, &digest);
  delete[] buff;
  md5 = base::MD5DigestToBase16(digest);
  base::TimeTicks endtime = base::TimeTicks::Now();
  base::TimeDelta delta = endtime - tickst;
  int ms = 0;
  ms = delta.InMilliseconds();
  LOG(INFO) << "SUM " << info.filename << " md5 used " << ms << " ms";
  content::BrowserThread::PostTask(
      content::BrowserThread::UI, FROM_HERE,
      base::Bind(&YSPAccountView::OnSumMD5, weak_factory_for_ui_.GetWeakPtr(),
                 md5, info));
}

void YSPAccountView::OnSumMD5(std::string md5, const  ActivexDownloadInfo info) {
  std::wstring md5_wstr = base::ASCIIToUTF16(md5);
  if (md5_wstr != info.md5)
    DownloadActivexInfobarDelegate::Create(browser_view_, info);
}
#endif

void YSPAccountView::ShowDefaultAvatar() {
  base::string16 first_name(user_name_, 0, 1);
  name_label_->SetText(first_name);
  name_label_->SetVisible(true);
  ui::ResourceBundle& bundle = ui::ResourceBundle::GetSharedInstance();
  head_view_->SetImage(*bundle.GetImageSkiaNamed(IDR_YSP_LOGIN_AVATAR_L));
}

views::View* YSPAccountView::TargetForRect(View* root, const gfx::Rect& rect) {
  return this;
}
