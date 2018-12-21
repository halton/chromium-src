// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef REDCORE

// ysp+ { login }

#ifndef CHROME_BROWSER_UI_VIEWS_YSP_ACCOUNT_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_YSP_ACCOUNT_VIEW_H_

#include "ui/views/view.h"
#include "ui/views/view_targeter_delegate.h"
#include "chrome/browser/ysp_login/ysp_login_manager.h"
#include <string>

class BrowserView;

namespace views {
  class ImageView;
  class Label;
}

class YSPAccountView : public views::View,
                       public views::ViewTargeterDelegate,
                       public YSPLoginManagerObserver {
 public:
  // The YSPAccountView's class name.
  static const char kViewClassName[];

  YSPAccountView(BrowserView* browser_view);
  ~YSPAccountView() override;

  void Init();

  // views::View:
  void OnPaint(gfx::Canvas* canvas) override;
  void Layout() override;
  void OnThemeChanged() override;
  const char* GetClassName() const override;
  bool OnMousePressed(const ui::MouseEvent& event) override;
  void OnMouseReleased(const ui::MouseEvent& event) override;
  void OnMouseMoved(const ui::MouseEvent& event) override;
  void OnMouseEntered(const ui::MouseEvent& event) override;
  void OnMouseExited(const ui::MouseEvent& event) override;

  // YSPLoginManagerObserver:
  void OnConfigDataUpdated(const std::string& type,
                           const std::string& data) override;
  void OnLoginRequestFailure(const std::string& error) override;
  void OnLoginResponseParseFailure(const std::string& error) override;
  void OnLoginFailure(const base::string16& message) override;
  void OnLoginSuccess(const base::string16& name,
    const std::string& head_image_url) override;
  void OnLogout() override;

  // ViewTargeterDelegate
  View* TargetForRect(View* root, const gfx::Rect& rect) override;

  base::string16 GetUserName() { return user_name_; }
  const gfx::ImageSkia* GetHeadImage();

protected:
  void CreateViews();
  void DownloadImage(const std::string& url);
  void DidDownloadFavicon(
    int id,
    int http_status_code,
    const GURL& image_url,
    const std::vector<SkBitmap>& bitmaps,
    const std::vector<gfx::Size>& original_bitmap_sizes);
  void ResetFeatureState();
  void UpdateScreenCaptureState(bool enable);
#if defined(IE_REDCORE)
  void BeginCheckFileMD5(const ActivexDownloadInfo info);
  void SumMD5(const ActivexDownloadInfo info);
  void OnSumMD5(std::string md5, const ActivexDownloadInfo info);
#endif

private:
  void ShowDefaultAvatar();
  BrowserView* browser_view_;
  views::ImageView* head_view_;
  views::Label*     name_label_;
  std::unique_ptr<gfx::ImageSkia> head_image_;
  base::string16 user_name_;
  std::string head_image_url_;
  base::WeakPtrFactory<YSPAccountView> weakFactoryForFile;
  base::WeakPtrFactory<YSPAccountView> weakFactoryForUI;

  DISALLOW_COPY_AND_ASSIGN(YSPAccountView);
};

#endif  // CHROME_BROWSER_UI_VIEWS_YSP_ACCOUNT_VIEW_H_

#endif
