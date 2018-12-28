; 卸载界面，安装包和更新包共用逻辑

Var un_common_close_button_black_image
Var un_common_close_button_white_image
Var un_common_ok_button_image
Var un_common_cancel_button_black_image
Var un_common_cancel_button_white_image
Var un_common_quit_button_image
Var un_common_checkbox_unchecked_image
Var un_common_checkbox_checked_image
Var un_common_load_bg_image
Var un_common_load_fg_image
Var un_common_msgbox_bg_image

Var un_common_msgbox
Var un_common_msgbox_close_button
Var un_common_msgbox_ok_button

;------------------------------------------------卸载界面逻辑-----------------------------------------------------------

;------------------------资源变量------------------------

Var un_uninstall_page_bg_image
Var un_dont_uninstall_button_image
Var un_confirm_uninstall_button_image

;------------------------控件变量------------------------

Var un_uninstall_page_dialog
Var un_uninstall_page_close_button
Var un_del_user_data_checkbox
Var un_dont_uninstall_button
Var un_confirm_uninstall_button

;------------------------逻辑变量------------------------

Var un_is_del_user_data
Var un_common_msgbox_caption_font
Var un_common_msgbox_text_font
Var un_common_title_font

;--------------------------------------------------------

Function un.onInit
  ; 控制客户端安装的目录，chromium目前只支持User目录和ProgramFiles，通过enterplorer_config.nsh中的变量控制
  ${If} ${SYSTEM_INSTALL_LEVEL} == "TRUE"
    Strcpy $INSTDIR '$PROGRAMFILES\AllMobilize\Enterplorer'
  ${Else}
    Strcpy $INSTDIR '$LOCALAPPDATA\AllMobilize\Enterplorer'
  ${endif}

  InitPluginsDir

  File `/ONAME=$PLUGINSDIR\common_close_button_black.bmp` `images\common_close_button_black.bmp`
  File `/ONAME=$PLUGINSDIR\common_close_button_white.bmp` `images\common_close_button_white.bmp`
  File `/ONAME=$PLUGINSDIR\common_ok_button.bmp` `images\common_ok_button.bmp`
  File `/ONAME=$PLUGINSDIR\common_cancel_button_black.bmp` `images\common_cancel_button_black.bmp`
  File `/ONAME=$PLUGINSDIR\common_cancel_button_white.bmp` `images\common_cancel_button_white.bmp`
  File `/ONAME=$PLUGINSDIR\common_quit_button.bmp` `images\common_quit_button.bmp`
  File `/ONAME=$PLUGINSDIR\common_checkbox_unchecked.bmp` `images\common_checkbox_unchecked.bmp`
  File `/ONAME=$PLUGINSDIR\common_checkbox_checked.bmp` `images\common_checkbox_checked.bmp`
  File `/ONAME=$PLUGINSDIR\common_load_bg.bmp` `images\common_load_bg.bmp`
  File `/ONAME=$PLUGINSDIR\common_load_fg.bmp` `images\common_load_fg.bmp`
  File `/ONAME=$PLUGINSDIR\bg_common_msgbox.bmp` `images\bg_common_msgbox.bmp`
  File `/ONAME=$PLUGINSDIR\bg_uninstall_page.bmp` `images\bg_uninstall_page.bmp`
  File `/ONAME=$PLUGINSDIR\dont_uninstall.bmp` `images\dont_uninstall.bmp`
  File `/ONAME=$PLUGINSDIR\confirm_uninstall.bmp` `images\confirm_uninstall.bmp`

  nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_close_button_black.bmp"
  pop $un_common_close_button_black_image
  nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_close_button_white.bmp"
  pop $un_common_close_button_white_image
  nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_ok_button.bmp"
  pop $un_common_ok_button_image
  nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_cancel_button_black.bmp"
  pop $un_common_cancel_button_black_image
  nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_cancel_button_white.bmp"
  pop $un_common_cancel_button_white_image
  nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_quit_button.bmp"
  pop $un_common_quit_button_image
  nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_checkbox_unchecked.bmp"
  pop $un_common_checkbox_unchecked_image
  nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_checkbox_checked.bmp"
  pop $un_common_checkbox_checked_image
  nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_load_bg.bmp"
  pop $un_common_load_bg_image
  nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_load_fg.bmp"
  pop $un_common_load_fg_image
  nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\bg_common_msgbox.bmp"
  pop $un_common_msgbox_bg_image

  nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\bg_uninstall_page.bmp"
  pop $un_uninstall_page_bg_image
  nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\dont_uninstall.bmp"
  pop $un_dont_uninstall_button_image
  nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\confirm_uninstall.bmp"
  pop $un_confirm_uninstall_button_image
FunctionEnd

Function un.onGUIInit
  GetFunctionAddress $R0 un.onImageCallback
  GetFunctionAddress $R1 un.onButtonClick
  nsiskin::Init /NOUNLOAD $R0 $R1 280
  CreateFont $un_common_msgbox_caption_font "Microsoft YaHei" "12" "600"
  CreateFont $un_common_msgbox_text_font "Microsoft YaHei" "10" "400"
  CreateFont $un_common_title_font "Microsoft YaHei" "12" "450"
  ${SetWindowSize} $HWNDPARENT 560 280

  ; 隐藏一些既有控件
  GetDlgItem $0 $HWNDPARENT 1034
  ${DestroyWindow} $0
  GetDlgItem $0 $HWNDPARENT 1035
  ${DestroyWindow} $0
  GetDlgItem $0 $HWNDPARENT 1036
  ${DestroyWindow} $0
  GetDlgItem $0 $HWNDPARENT 1037
  ${DestroyWindow} $0
  GetDlgItem $0 $HWNDPARENT 1038
  ${DestroyWindow} $0
  GetDlgItem $0 $HWNDPARENT 1039
  ${DestroyWindow} $0
  GetDlgItem $0 $HWNDPARENT 1256
  ${DestroyWindow} $0
  GetDlgItem $0 $HWNDPARENT 1028
  ${DestroyWindow} $0
  GetDlgItem $0 $HWNDPARENT 1018
  ${DestroyWindow} $0
  GetDlgItem $0 $HWNDPARENT 1045
  ${DestroyWindow} $0
  GetDlgItem $0 $HWNDPARENT 1
  ${DestroyWindow} $0
  GetDlgItem $0 $HWNDPARENT 2
  ${DestroyWindow} $0
  GetDlgItem $0 $HWNDPARENT 3
  ${DestroyWindow} $0
FunctionEnd

Function un.onImageCallback
  Pop $0

  ${If} $0 = $un_common_msgbox
    push $un_common_msgbox_bg_image
  ${ElseIf} $0 = $un_common_msgbox_close_button
    push $un_common_close_button_black_image
  ${ElseIf} $0 = $un_common_msgbox_ok_button
    push $un_common_ok_button_image
  ${EndIf}

  ${If} $0 = $un_uninstall_page_dialog
    push $un_uninstall_page_bg_image
  ${ElseIf} $0 = $un_uninstall_page_close_button
    push $un_common_close_button_white_image
  ${ElseIf} $0 = $un_del_user_data_checkbox
    ${If} $un_is_del_user_data == '0'
      push $un_common_checkbox_unchecked_image
    ${Else}
      push $un_common_checkbox_checked_image
    ${EndIf}
  ${ElseIf} $0 = $un_dont_uninstall_button
    push $un_dont_uninstall_button_image
  ${ElseIf} $0 = $un_confirm_uninstall_button
    push $un_confirm_uninstall_button_image
  ${EndIf}
FunctionEnd

Function un.UninstallPage
  nsDialogs::Create 1044
  Pop $un_uninstall_page_dialog
  ${If} $un_uninstall_page_dialog == error
    Abort
  ${EndIf}

  ${SetWindowSize} $un_uninstall_page_dialog 560 280
  nsiskin::CreateButton /NOUNLOAD $un_uninstall_page_dialog 530 16 12 12 ""
  Pop $un_uninstall_page_close_button

  nsiskin::CreateButton /NOUNLOAD $un_uninstall_page_dialog 38 222 150 15 ""
  Pop $un_del_user_data_checkbox
  StrCpy $un_is_del_user_data 0

  nsiskin::CreateButton /NOUNLOAD $un_uninstall_page_dialog 286 212 112 36 "" 0
  Pop $un_dont_uninstall_button

  nsiskin::CreateButton /NOUNLOAD $un_uninstall_page_dialog 418 212 112 36 "" 0
  Pop $un_confirm_uninstall_button

  nsiskin::Attach /NOUNLOAD $un_uninstall_page_dialog
  nsDialogs::Show
FunctionEnd

Function un.onButtonClick
  Pop $0

  ${If} $0 = $un_uninstall_page_close_button
    call un.onClickClose
  ${ElseIf} $0 = $un_del_user_data_checkbox
    call un.onClickDelUserDataCheckbox
  ${ElseIf} $0 = $un_dont_uninstall_button
    call un.onClickDontUninstallButton
  ${ElseIf} $0 = $un_confirm_uninstall_button
    call un.onClickConfirmUninstallButton
  ${EndIf}
FunctionEnd

Function un.onClickClose
  nsYSP::EndApplication
FunctionEnd

Function un.onClickDelUserDataCheckbox
  ${If} $un_is_del_user_data == '1'
    StrCpy $un_is_del_user_data 0
  ${Else}
    StrCpy $un_is_del_user_data 1
  ${EndIf}
FunctionEnd

Function un.onClickDontUninstallButton
  nsYSP::EndApplication
FunctionEnd

Function un.onClickConfirmUninstallButton
  nsYSP::FindProc "enterplorer.exe"
  Pop $R0
  ${If} $R0 == "1"
    push "请关闭所有 $(ProductName) 窗口并重试。"
    call un.CommonMsgBox
  ${Else}
    ${NSD_CreateTimer} un.UninstallTimer 50
  ${EndIf}
FunctionEnd

Function un.UninstallTimer
  ${NSD_KillTimer} un.UninstallTimer
  GetFunctionAddress $0 un.MainUninstallLogic
  BgWorker::CallAndWait
  nsYSP::EndApplication
FunctionEnd

Function un.CommonMsgBox
  Pop $R0

  nsiskin::CreateWindow /NOUNLOAD $HWNDPARENT 0 0 400 180 "提示"
  pop $un_common_msgbox

  System::Call 'User32::CreateWindowEx(ii00000004, t"STATIC", t"卸载提示", i0x50020000, i40, i30, i72, i20, i$un_common_msgbox, i1130, i0, i0) i.R1'
  SendMessage $R1 ${WM_SETFONT} $un_common_msgbox_caption_font 0

  System::Call 'User32::CreateWindowEx(i00000004, t"STATIC", t"$R0", i0x50020000, i40, i60, i320, i60, i$un_common_msgbox, i1130, i0, i0) i.R1'
  SendMessage $R1 ${WM_SETFONT} $un_common_msgbox_text_font 0

  nsiskin::CreateButton /NOUNLOAD $un_common_msgbox 366 20 12 12 "" 2
  Pop $un_common_msgbox_close_button

  nsiskin::CreateButton /NOUNLOAD $un_common_msgbox 304 130 76 30 "" 2
  Pop $un_common_msgbox_ok_button

  EnableWindow $HWNDPARENT 0
  ShowWindow $un_common_msgbox ${SW_SHOW}
FunctionEnd

;------------------------------------------------卸载后台逻辑-----------------------------------------------------------

Function un.MainUninstallLogic
  ${If} $un_is_del_user_data == '1'
    ExecWait "$INSTDIR\Application\${CHROME_VERSION}\Installer\setup.exe --uninstall --force-uninstall --delete-profile" $0
  ${Else}
    ExecWait "$INSTDIR\Application\${CHROME_VERSION}\Installer\setup.exe --uninstall --force-uninstall" $0
  ${EndIf}

  IntCmp $0 19 caseEqual caseLess caseMore
  caseEqual:
    Delete "$SMPROGRAMS\$(ProductName)\$(ShortcutName).lnk"
    Delete "$SMPROGRAMS\$(ProductName)\$(Shortcut_Unintall)"
    RMDir /r "$SMPROGRAMS\$(ProductName)"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_96DPI_PIXEL" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_BEHAVIORS" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_BROWSER_EMULATION" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_DISABLE_MK_PROTOCOL" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_DISABLE_SQM_UPLOAD_FOR_APP" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_INTERNET_SHELL_FOLDERS" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_LEGACY_DLCONTROL_BEHAVIORS" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_LOCALMACHINE_LOCKDOWN" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_MIME_HANDLING" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_MIME_SNIFFING" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_NINPUT_LEGACYMODE" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_OBJECT_CACHING" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_PROTOCOL_LOCKDOWN" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_RESTRICT_ACTIVEXINSTALL" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_SAFE_BINDTOOBJECT" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_SCRIPTURL_MITIGATION" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_SECURITYBAND" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_SPELLCHECKING" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_TABBED_BROWSING" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_WEBOC_DOCUMENT_ZOOM" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_WEBOC_POPUPMANAGEMENT" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_WINDOW_RESTRICTIONS" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_XSSFILTER" "enterplorer.exe"
    DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_ZONE_ELEVATION" "enterplorer.exe"
  caseLess:
  caseMore:
  SetAutoClose true
FunctionEnd
;----------------------------------------------------------------------------------------------------------------------------------
