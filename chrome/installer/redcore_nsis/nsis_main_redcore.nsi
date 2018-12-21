!addplugindir Plugins

; Set compress algorithm
SetCompress Auto
SetCompressor /FINAL /SOLID zlib

; MUI 1.67 compatible ------
!include "MUI.nsh"
!include "nsDialogs.nsh"
!include "WordFunc.nsh"
!include "FileFunc.nsh"
!include "version.nsh"
!include "str_contains.nsh"
!include "is_flash_ax_Inst.nsh"
!include "text_log.nsh"
!include "redcore_config.nsh"
!include "EnvVarUpdate.nsh"

!define SWP_NOMOVE 0x0002
!macro _SetWindowSize CONTROL Width Height
  System::Call user32::SetWindowPos(i${CONTROL},i0,i0,i0,i${Width},i${Height},i${SWP_NOMOVE})
!macroend
!define SetWindowSize `!insertmacro _SetWindowSize`

!macro _DestroyWindow CONTROL
  System::Call user32::DestroyWindow(i${CONTROL})
!macroend
!define DestroyWindow `!insertmacro _DestroyWindow`

!define MUI_ABORTWARNING

; MUI Settings
!define MUI_ICON "install.ico"
!define MUI_UNICON "uninstall.ico"
!define UNINSTALLER_NAME  "uninstall.exe"

!define MUI_CUSTOMFUNCTION_GUIINIT onGUIInit

Page custom WelcomePage
Page custom InstallPage

UninstPage custom un.UninstallPage

; Language files
!insertmacro MUI_LANGUAGE "SimpChinese"
!insertmacro MUI_LANGUAGE "English"

; Grand permissions to UAC prompt.
RequestExecutionLevel admin

;add english support
LangString ProductName ${LANG_ENGLISH} "Enterplorer Browser"
LangString MUI_TEXT_DIRECTORY_TITLE ${LANG_ENGLISH} ""
LangString MUI_TEXT_DIRECTORY_SUBTITLE ${LANG_ENGLISH} "Welcome to Enterplorer Browser!"
LangString DirPageTop ${LANG_ENGLISH} "Setup will install $(^NameDA) in the following folder,$\r$\n$\r$\nTo install in a different folder, click [Browser(B)] and select another folder."
LangString Shortcut_Unintall ${LANG_ENGLISH} "Uninstall.lnk"
LangString NewerVersionInstalled ${LANG_ENGLISH} "There is a newer version installed."
LangString ShortcutDescription ${LANG_ENGLISH} "Browse the web"
LangString NoFlashAxNotify ${LANG_ENGLISH} "There is no flashplayer activex installed in the system,please install or contact the administrator."
LangString ShortcutName ${LANG_ENGLISH} "Enterplorer"
;add chinese support
LangString ProductName ${LANG_SIMPCHINESE} "Enterplorer企业浏览器"
LangString MUI_TEXT_DIRECTORY_TITLE ${LANG_SIMPCHINESE} ""
LangString MUI_TEXT_DIRECTORY_SUBTITLE ${LANG_SIMPCHINESE} "欢迎使用Enterplorer企业浏览器！"
LangString DirPageTop ${LANG_SIMPCHINESE} "$(^NameDA) 将安装在下列文件夹，$\r$\n$\r$\n要安装到不同文件夹，单击 [浏览(B)] 并选择其他文件夹。"
LangString Shortcut_Unintall ${LANG_SIMPCHINESE} "卸载.lnk"
LangString NewerVersionInstalled ${LANG_SIMPCHINESE} "本机已安装了浏览器的较新版本，如果要安装旧版本，请先将新版本卸载。"
LangString ShortcutDescription ${LANG_SIMPCHINESE} "访问互联网"
LangString NoFlashAxNotify ${LANG_SIMPCHINESE} "您的系统中没有安装FlashPlayer ActiveX插件，请自行安装或联系管理员"
LangString ShortcutName ${LANG_SIMPCHINESE} "Enterplorer"

; Adds fields in the Version Tab of the File Properties 
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Enterplorer Browser"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Enterplorer Browser"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Redcore"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright (C)2016 Redcore, All rights reserved."
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Enterplorer Browser"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${CHROME_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${CHROME_VERSION}"

VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "ProductName" "Enterplorer企业浏览器"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "Comments" "Enterplorer企业浏览器"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "CompanyName" "红芯时代"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "LegalCopyright" "版权所有 (C)2016 红芯时代，保留所有权利。"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "FileDescription" "Enterplorer企业浏览器"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "FileVersion" "${CHROME_VERSION}"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "ProductVersion" "${CHROME_VERSION}"

VIProductVersion "${CHROME_VERSION}"

Var WINDOWS_Version ;WINDOWS版本

Name "$(ProductName)"
OutFile "install_${CHROME_VERSION}.exe"
DirText "$(DirPageTop)"

;------------------------------------------------安装卸载共用变量-----------------------------------------------------------

;------------------------资源变量------------------------

Var common_close_button_black_image
Var common_close_button_white_image
Var common_ok_button_image
Var common_cancel_button_black_image
Var common_cancel_button_white_image
Var common_quit_button_image
Var common_checkbox_unchecked_image
Var common_checkbox_checked_image
Var common_load_bg_image
Var common_load_fg_image

Var common_msgbox_bg_image

;------------------------控件变量------------------------

Var common_msgbox
Var common_msgbox_close_button
Var common_msgbox_ok_button

Var quit_msgbox
Var quit_msgbox_close_button
Var quit_msgbox_cancel_button
Var quit_msgbox_quit_button

;------------------------逻辑变量------------------------

Var common_msgbox_caption_font
Var common_msgbox_text_font
Var common_title_font

;------------------------------------------------安装界面逻辑-----------------------------------------------------------

;------------------------资源变量------------------------

Var welcome_page_bg_image
Var install_page_bg_image
Var quick_install_button_image

;------------------------控件变量------------------------

Var welcome_page_dialog
Var welcome_page_close_button
Var set_defalut_checkbox
Var quick_install_button
Var install_title_label

Var install_page_dialog
Var install_page_close_button
Var install_page_progress_bar
Var install_page_progress_label
Var install_page_finish

;------------------------逻辑变量------------------------

Var is_set_default
Var InstallPercent
Var MainInstallLogicComplete
Var DelayTime

;--------------------------------------------------------

Function .onInit

    ;User data folder as default  
	${if} ${SYSTEM_INSTALL_LEVEL} == "TRUE"
	  Strcpy $INSTDIR '$PROGRAMFILES\AllMobilize\Redcore'
    ${else}
	  Strcpy $INSTDIR '$LOCALAPPDATA\AllMobilize\Redcore'
    ${endif}
  
    Delete "$INSTDIR\InstallLog.log"
    ${LogSetFileName} "$INSTDIR\InstallLog.log"
    ${LogSetOn}

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

    File `/ONAME=$PLUGINSDIR\bg_welcome_page.bmp` `images\bg_welcome_page.bmp`
    File `/ONAME=$PLUGINSDIR\quick_install_button.bmp` `images\quick_install_button.bmp`

    File `/ONAME=$PLUGINSDIR\bg_install_page.bmp` `images\bg_install_page.bmp`
    
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_close_button_black.bmp"
    Pop $common_close_button_black_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_close_button_white.bmp"
    Pop $common_close_button_white_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_ok_button.bmp"
    Pop $common_ok_button_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_cancel_button_black.bmp"
    Pop $common_cancel_button_black_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_cancel_button_white.bmp"
    Pop $common_cancel_button_white_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_quit_button.bmp"
    Pop $common_quit_button_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_checkbox_unchecked.bmp"
    Pop $common_checkbox_unchecked_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_checkbox_checked.bmp"
    Pop $common_checkbox_checked_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_load_bg.bmp"
    Pop $common_load_bg_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_load_fg.bmp"
    Pop $common_load_fg_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\bg_common_msgbox.bmp"
    Pop $common_msgbox_bg_image

    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\bg_welcome_page.bmp"
    Pop $welcome_page_bg_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\quick_install_button.bmp"
    Pop $quick_install_button_image

    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\bg_install_page.bmp"
    Pop $install_page_bg_image
    
FunctionEnd

Function onGUIInit
    GetFunctionAddress $R0 onImageCallback
    GetFunctionAddress $R1 onButtonClick
    nsiskin::Init /NOUNLOAD $R0 $R1 280
    CreateFont $common_msgbox_caption_font "Microsoft YaHei" "12" "600"
    CreateFont $common_msgbox_text_font "Microsoft YaHei" "10" "400"
    CreateFont $common_title_font "Microsoft YaHei" "12" "450"
    ${SetWindowSize} $HWNDPARENT 560 280

    ;隐藏一些既有控件
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

Function onImageCallback
    Pop $0

    ${If} $0 = $common_msgbox
        push $common_msgbox_bg_image
    ${ElseIf} $0 = $common_msgbox_close_button
        push $common_close_button_black_image
    ${ElseIf} $0 = $common_msgbox_ok_button
        push $common_ok_button_image
    ${EndIf}

    ${If} $0 = $quit_msgbox
        push $common_msgbox_bg_image
    ${ElseIf} $0 = $quit_msgbox_close_button
        push $common_close_button_black_image
    ${ElseIf} $0 = $quit_msgbox_cancel_button
        push $common_cancel_button_white_image
    ${ElseIf} $0 = $quit_msgbox_quit_button
        push $common_quit_button_image
    ${EndIf}

    ${If} $0 = $welcome_page_dialog
        push $welcome_page_bg_image
    ${ElseIf} $0 = $welcome_page_close_button
        push $common_close_button_white_image
    ${ElseIf} $0 = $set_defalut_checkbox
        push '5'
        ${If} $is_set_default == '1'
            push $common_checkbox_checked_image
        ${Else}
            push $common_checkbox_unchecked_image
        ${EndIf}
    ${ElseIf} $0 = $quick_install_button
        push $quick_install_button_image
    ${EndIf}

    ${If} $0 = $install_page_dialog
        push $install_page_bg_image
    ${ElseIf} $0 = $install_page_close_button
        push $common_close_button_white_image
    ${ElseIf} $0 = $install_page_progress_bar
        push $common_load_fg_image
        push $InstallPercent
        push 4
        push $common_load_bg_image
    ${EndIf}

FunctionEnd

Function onButtonClick
    Pop $0

    ${If} $0 = $welcome_page_close_button
        call onClose
    ${ElseIf} $0 = $quick_install_button
        call CheckVersion
    ${ElseIf} $0 = $set_defalut_checkbox
        call onClickSetDefault
    ${EndIf}

    ${If} $0 = $install_page_close_button
        call onClose
    ${EndIf}
FunctionEnd

Function onClickNext
  StrCpy $R9 1
  Call RelGotoPage
  Abort
FunctionEnd

Function RelGotoPage
  IntCmp $R9 0 0 Move Move
    StrCmp $R9 "X" 0 Move
      StrCpy $R9 "120"
  Move:
  SendMessage $HWNDPARENT "0x408" "$R9" ""
FunctionEnd

Function onClickSetDefault
    ${If} $is_set_default == '1'
        StrCpy $is_set_default 0
    ${Else}
        StrCpy $is_set_default 1
    ${EndIf}
FunctionEnd

Function onClose
    ${If} $InstallPercent == 100
        nsYSP::EndApplication
    ${EndIf}

    nsiskin::CreateWindow /NOUNLOAD $HWNDPARENT 0 0 400 180 "提示"
    pop $quit_msgbox

    System::Call 'User32::CreateWindowEx(ii00000004, t"STATIC", t"安装提示", i0x50020000, i40, i30, i72, i20, i$quit_msgbox, i1130, i0, i0) i.R1'
    SendMessage $R1 ${WM_SETFONT} $common_msgbox_caption_font 0

    System::Call 'User32::CreateWindowEx(i00000004, t"STATIC", t"确定要退出安装吗", i0x50020000, i40, i60, i320, i60, i$quit_msgbox, i1130, i0, i0) i.R1'
    SendMessage $R1 ${WM_SETFONT} $common_msgbox_text_font 0
    
    nsiskin::CreateButton /NOUNLOAD $quit_msgbox 366 20 12 12 "" 2
    Pop $quit_msgbox_close_button

    nsiskin::CreateButton /NOUNLOAD $quit_msgbox 208 130 76 30 "" 2
    Pop $quit_msgbox_cancel_button

    nsiskin::CreateButton /NOUNLOAD $quit_msgbox 304 130 76 30 "" 1
    Pop $quit_msgbox_quit_button

    EnableWindow $HWNDPARENT 0
    ShowWindow $quit_msgbox ${SW_SHOW}
FunctionEnd

Function InstallBgTimer
  ${NSD_KillTimer} InstallBgTimer

  GetFunctionAddress $0 MainInstallLogic
  BgWorker::CallAndWait

  call CheckFlashAx

  ;下面两行程序创建不了开始程序
  ;GetFunctionAddress $0 CreateShortCuts
  ;BgWorker::CallAndWait
  ;所以干脆直接调用，CreateShortCuts耗时很短，不会引起阻塞
  call CreateShortCuts

  GetFunctionAddress $0 WriteUninstaller
  BgWorker::CallAndWait

  StrCpy $MainInstallLogicComplete 1
FunctionEnd

Function InstallFgTimer
    ${If} $InstallPercent == 0
        nsYSP::Init /NOUNLOAD $install_page_dialog 515 240 40 25
        StrCpy $DelayTime 0
    ${EndIf}
  
    nsYSP::DrawNum /NOUNLOAD $install_page_dialog 515 240 40 25 $InstallPercent

    IntCmp $InstallPercent 90 caseEqual caseLess caseMore
caseLess:
    IntOp $InstallPercent $InstallPercent + 2
    goto CompareEnd
caseEqual:
caseMore:
    ${If} $MainInstallLogicComplete == 1
        StrCpy $InstallPercent 100
        ;${NSD_SetText} $install_page_progress_label "安装完成！"
		ShowWindow $install_page_progress_label ${SW_HIDE}
		ShowWindow $install_page_finish ${SW_SHOW}
        IntOp $DelayTime $DelayTime + 1
    ${EndIf}

CompareEnd:
    System::Call 'User32::InvalidateRect(i$install_page_progress_bar, i0, i1) i.R9'
    ${If} $DelayTime == 10
        ;nsYSP::EndApplication
    ${EndIf}
FunctionEnd

Function WelcomePage
    nsDialogs::Create 1044
    Pop $welcome_page_dialog
    ${If} $welcome_page_dialog == error
        Abort
    ${EndIf}
    ${SetWindowSize} $welcome_page_dialog 560 280
	
	${NSD_CreateLabel} 15 15 280 30 $(MUI_TEXT_DIRECTORY_SUBTITLE)
    Pop $install_title_label
	SetCtlColors $install_title_label 0xFFFFFF  transparent
	SendMessage $install_title_label ${WM_SETFONT} $common_title_font 0
    
    nsiskin::CreateButton /NOUNLOAD $welcome_page_dialog 530 16 12 12 ""
    Pop $welcome_page_close_button

    nsiskin::CreateButton /NOUNLOAD $welcome_page_dialog 199 70 162 162 "" 0
    Pop $quick_install_button

    ;nsiskin::CreateButton /NOUNLOAD $welcome_page_dialog 30 275 150 15 ""
    ;Pop $set_defalut_checkbox
    ;StrCpy $is_set_default 1
     
    nsiskin::Attach /NOUNLOAD $welcome_page_dialog
    
    nsDialogs::Show
    ${NSD_FreeImage} $1
FunctionEnd

Function InstallPage
    nsDialogs::Create 1044
    Pop $install_page_dialog
    ${If} $install_page_dialog == error
        Abort
    ${EndIf}
    ${SetWindowSize} $install_page_dialog 560 280
	
	${NSD_CreateLabel} 15 15 250 30 "$(ProductName)-安装"
    Pop $install_title_label
	SetCtlColors $install_title_label 0xFFFFFF  transparent
	SendMessage $install_title_label ${WM_SETFONT} $common_title_font 0

    nsiskin::CreateButton /NOUNLOAD $install_page_dialog 530 16 12 12 ""
    Pop $install_page_close_button

    ${NSD_CreateLabel} 20 230 520 4 "Progress"
    Pop $install_page_progress_bar
    ${NSD_AddStyle} $install_page_progress_bar ${SS_OWNERDRAW}

    ${NSD_CreateLabel} 20 245 100% 20% "正在安装,请耐心等待..."
    Pop $install_page_progress_label
	${NSD_AddStyle} $install_page_progress_label ${SS_LEFT}
	SetCtlColors $install_page_progress_label ""  transparent
	ShowWindow $install_page_progress_label ${SW_SHOW}
	
	${NSD_CreateLabel} 20 245 100% 20% "安装完成！"
    Pop $install_page_finish
	${NSD_AddStyle} $install_page_finish ${SS_LEFT}
	SetCtlColors $install_page_finish ""  transparent
	ShowWindow $install_page_finish ${SW_HIDE}

    ${NSD_CreateTimer} InstallBgTimer 50 

    StrCpy $InstallPercent 0
    StrCpy $MainInstallLogicComplete 0
    ${NSD_CreateTimer} InstallFgTimer 100 

    nsiskin::Attach /NOUNLOAD $install_page_dialog

    nsDialogs::Show

FunctionEnd

Function CommonMsgBox
    Pop $R0

    nsiskin::CreateWindow /NOUNLOAD $HWNDPARENT 0 0 400 180 "提示"
    pop $common_msgbox

    System::Call 'User32::CreateWindowEx(ii00000004, t"STATIC", t"安装提示", i0x50020000, i40, i30, i72, i20, i$common_msgbox, i1130, i0, i0) i.R1'
    SendMessage $R1 ${WM_SETFONT} $common_msgbox_caption_font 0

    System::Call 'User32::CreateWindowEx(i00000004, t"STATIC", t"$R0", i0x50020000, i40, i60, i320, i60, i$common_msgbox, i1130, i0, i0) i.R1'
    SendMessage $R1 ${WM_SETFONT} $common_msgbox_text_font 0
    
    nsiskin::CreateButton /NOUNLOAD $common_msgbox 366 20 12 12 "" 2
    Pop $common_msgbox_close_button

    nsiskin::CreateButton /NOUNLOAD $common_msgbox 304 130 76 30 "" 2
    Pop $common_msgbox_ok_button

    EnableWindow $HWNDPARENT 0
    ShowWindow $common_msgbox ${SW_SHOW}
FunctionEnd

;------------------------------------------------安装后台逻辑-----------------------------------------------------------

Function CheckVersion
  ;check old version
  ReadRegStr $0 HKCU "SOFTWARE\AllMobilize\Redcore" "pv"
  ${LogText} "old version:$0"
  ${VersionCompare} "${CHROME_VERSION}" $0 $R0
  IntCmp $R0 2 caseEqual caseLess
caseEqual:
  push $(NewerVersionInstalled)
  call CommonMsgBox
  goto CheckVersionEnd
caseLess:
  call onClickNext
CheckVersionEnd:
FunctionEnd

Function MainInstallLogic

  Sleep 1000

  ;do install
  SetOutPath "$INSTDIR"
  SetOverwrite try

  ClearErrors
  File nsis_src\install_redcore.exe
  IfErrors onError onOK
onError:  
  ${LogText} "copy install_redcore.exe faild"
  MessageBox MB_OK "Fail write data to path, please select another install directory."
  goto onEnd
onOK:
  SetShellVarContext current
  ;${If} $is_set_default == '1'
      ;nsYSP::ExecWait /NOUNLOAD "$INSTDIR\install_redcore.exe --make-chrome-default"
  ;${Else}
      ;nsYSP::ExecWait /NOUNLOAD "$INSTDIR\install_redcore.exe"
  ;${EndIf}
  
  ${if} ${SYSTEM_INSTALL_LEVEL} == "TRUE"
	ExecWait "$INSTDIR\install_redcore.exe --system-level"
  ${else}
	ExecWait "$INSTDIR\install_redcore.exe"
  ${endif}
  
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_96DPI_PIXEL" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_BEHAVIORS" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_BROWSER_EMULATION" "redcore.exe" 0x00001b58
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_DISABLE_MK_PROTOCOL" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_DISABLE_SQM_UPLOAD_FOR_APP" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_ENABLE_WEB_CONTROL_VISUALS" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_INTERNET_SHELL_FOLDERS" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_LEGACY_DLCONTROL_BEHAVIORS" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_LOCALMACHINE_LOCKDOWN" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_MIME_HANDLING" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_MIME_SNIFFING" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_NINPUT_LEGACYMODE" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_OBJECT_CACHING" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_PROTOCOL_LOCKDOWN" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_RESTRICT_ACTIVEXINSTALL" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_SAFE_BINDTOOBJECT" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_SCRIPTURL_MITIGATION" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_SECURITYBAND" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_SPELLCHECKING" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_TABBED_BROWSING" "redcore.exe" 0x00000000
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_WEBOC_DOCUMENT_ZOOM" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_WEBOC_POPUPMANAGEMENT" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_WINDOW_RESTRICTIONS" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_XSSFILTER" "redcore.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_ZONE_ELEVATION" "redcore.exe" 0x00000001
  
  ;自带IE用的Flash插件
  SetOutPath "$INSTDIR\Application\${CHROME_VERSION}\FlashAx"
  SetOverwrite try
  File /nonfatal nsis_src\FlashAx\*.ocx
  
  ;判断系统版本
  ReadRegStr $WINDOWS_Version HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "ProductName"
  ;${StrContains} $1 "Windows 10" $WINDOWS_Version
  ;StrCmp $1 "" InstallIE8
  ;Goto Installsangfor
;InstallIE8:
  ;SetOutPath "$INSTDIR\Application\ie8core"
  ;SetOverwrite try
  ;File nsis_src\ie8core\*.*
  ;ExecWait "regedit.exe /s $\"$INSTDIR\Application\ie8core\IE8Reg.reg$\""
;Installsangfor:
  ;ClearErrors
  ;SetOutPath "$INSTDIR\Application\sangfor"
  ;SetOverwrite try
  ;File nsis_src\sangfor\*.*
  ;IfErrors 0 +3
  ;${LogText} "Copy sangfor files faild"
InstallYSPGm:
  ClearErrors
  SetOutPath "$INSTDIR\Application\${CHROME_VERSION}\gm"
  SetOverwrite try
  File nsis_src\gm\gmcrypto.dll
  SetOutPath "$INSTDIR\Application\${CHROME_VERSION}\gm\gmcert"
  SetOverwrite try
  File nsis_src\gm\gmcert\*.*
  SetOutPath "$INSTDIR\Application\${CHROME_VERSION}\gm\gmcert-hd"
  SetOverwrite try
  File nsis_src\gm\gmcert-hd\*.*
  ${EnvVarUpdate} $0 "REDCORE_ENGINES" "A" "HKLM" "$INSTDIR\Application\${CHROME_VERSION}\gm"
  SetOutPath "$SYSDIR"
  SetOverwrite try
  File nsis_src\gm\KeyGDBApi.dll
  IfErrors 0 +3
  ${LogText} "Copy GM files faild"
CheckFlashAx:
  Call IsFlashInstalled
  Pop $R0
  ${If} $R0 == "0"
    ${LogText} "There is no flashplayer activex installed in the system"
    MessageBox MB_OK "$(NoFlashAxNotify)"
  ${EndIf}
  ;${If} $R0 == "1"
  ;  MessageBox MB_OK "$$R0 is not '0'"
  ;${EndIf}
onEnd:
  Delete "$INSTDIR\install_redcore.exe"
  ${LogSetOff}
  SetAutoClose true

FunctionEnd

Function CheckFlashAx
    CheckFlashAx:
    Call IsFlashInstalled
    Pop $R0
    ${If} $R0 == "0"
        ${LogText} "There is no flashplayer activex installed in the system"
        ;MessageBox MB_OK "$(NoFlashAxNotify)"
        push "$(NoFlashAxNotify)"
        call CommonMsgBox
    ${EndIf}
    ;${If} $R0 == "1"
    ;  MessageBox MB_OK "$$R0 is not '0'"
    ;${EndIf}
FunctionEnd

Function CreateShortCuts
  
  ;MessageBox MB_OK "$SMPROGRAMS\$(ProductName)\$(Shortcut_Unintall)"
  Delete "$SMPROGRAMS\$(ProductName).lnk"
  Delete "$SMPROGRAMS\redcore.lnk"
  Delete "$SMPROGRAMS\$(ProductName)\$(ProductName).lnk"
  Delete "$DESKTOP\redcore.lnk"
  CreateDirectory "$SMPROGRAMS\$(ProductName)"
  ;MessageBox MB_OK "$INSTDIR\Application\redcore.exe"
  CreateShortCut "$SMPROGRAMS\$(ProductName)\$(ShortcutName).lnk" \
    "$INSTDIR\Application\redcore.exe"
  CreateShortCut "$SMPROGRAMS\$(ProductName)\$(Shortcut_Unintall)" \
    "$INSTDIR\Application\${UNINSTALLER_NAME}"
  CreateShortCut "$DESKTOP\$(ShortcutName).lnk" "$INSTDIR\Application\redcore.exe" "" "" 0 SW_SHOWNORMAL "" "$(ShortcutDescription)"
  ;HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Redcore
  WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Redcore" \          
    "UninstallString" \
    "$INSTDIR\Application\${UNINSTALLER_NAME}"
  
FunctionEnd

Function WriteUninstaller
  WriteUninstaller "$INSTDIR\Application\${UNINSTALLER_NAME}"
FunctionEnd


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

;--------------------------------------------------------

Function un.onInit

    ;User data folder as default  
    Strcpy $INSTDIR '$LOCALAPPDATA\AllMobilize\Redcore'

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
    Pop $common_close_button_black_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_close_button_white.bmp"
    Pop $common_close_button_white_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_ok_button.bmp"
    Pop $common_ok_button_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_cancel_button_black.bmp"
    Pop $common_cancel_button_black_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_cancel_button_white.bmp"
    Pop $common_cancel_button_white_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_quit_button.bmp"
    Pop $common_quit_button_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_checkbox_unchecked.bmp"
    Pop $common_checkbox_unchecked_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_checkbox_checked.bmp"
    Pop $common_checkbox_checked_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_load_bg.bmp"
    Pop $common_load_bg_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\common_load_fg.bmp"
    Pop $common_load_fg_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\bg_common_msgbox.bmp"
    Pop $common_msgbox_bg_image
    
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\bg_uninstall_page.bmp"
    Pop $un_uninstall_page_bg_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\dont_uninstall.bmp"
    Pop $un_dont_uninstall_button_image
    nsiskin::LoadImg /NOUNLOAD "$PLUGINSDIR\confirm_uninstall.bmp"
    Pop $un_confirm_uninstall_button_image

FunctionEnd

Function un.onGUIInit	
    GetFunctionAddress $R0 un.onImageCallback
    GetFunctionAddress $R1 un.onButtonClick
    nsiskin::Init /NOUNLOAD $R0 $R1 280
    CreateFont $common_msgbox_caption_font "Microsoft YaHei" "12" "600"
    CreateFont $common_msgbox_text_font "Microsoft YaHei" "10" "400"
    ${SetWindowSize} $HWNDPARENT 560 280

    ;隐藏一些既有控件
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

    ${If} $0 = $common_msgbox
        push $common_msgbox_bg_image
    ${ElseIf} $0 = $common_msgbox_close_button
        push $common_close_button_black_image
    ${ElseIf} $0 = $common_msgbox_ok_button
        push $common_ok_button_image
    ${EndIf}

    ${If} $0 = $un_uninstall_page_dialog
        push $un_uninstall_page_bg_image
    ${ElseIf} $0 = $un_uninstall_page_close_button
        push $common_close_button_white_image
    ${ElseIf} $0 = $un_del_user_data_checkbox
        ${If} $un_is_del_user_data == '0'
            push $common_checkbox_unchecked_image
        ${Else}
            push $common_checkbox_checked_image
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
    nsYSP::FindProc "redcore.exe"
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
    pop $common_msgbox

    System::Call 'User32::CreateWindowEx(ii00000004, t"STATIC", t"卸载提示", i0x50020000, i40, i30, i72, i20, i$common_msgbox, i1130, i0, i0) i.R1'
    SendMessage $R1 ${WM_SETFONT} $common_msgbox_caption_font 0

    System::Call 'User32::CreateWindowEx(i00000004, t"STATIC", t"$R0", i0x50020000, i40, i60, i320, i60, i$common_msgbox, i1130, i0, i0) i.R1'
    SendMessage $R1 ${WM_SETFONT} $common_msgbox_text_font 0
    
    nsiskin::CreateButton /NOUNLOAD $common_msgbox 366 20 12 12 "" 2
    Pop $common_msgbox_close_button

    nsiskin::CreateButton /NOUNLOAD $common_msgbox 304 130 76 30 "" 2
    Pop $common_msgbox_ok_button

    EnableWindow $HWNDPARENT 0
    ShowWindow $common_msgbox ${SW_SHOW}
FunctionEnd

;------------------------------------------------卸载后台逻辑-----------------------------------------------------------

Function un.MainUninstallLogic
  ;MessageBox MB_OK "$INSTDIR\Application\${CHROME_VERSION}\Installer\setup.exe"
  ${If} $un_is_del_user_data == '1'
      ExecWait "$INSTDIR\Application\${CHROME_VERSION}\Installer\setup.exe --uninstall --force-uninstall --delete-profile" $0
  ${Else}
      ExecWait "$INSTDIR\Application\${CHROME_VERSION}\Installer\setup.exe --uninstall --force-uninstall" $0
  ${EndIf}
  ;MessageBox MB_OK $0
  IntCmp $0 19 caseEqual caseLess caseMore
  caseEqual:
  ;MessageBox MB_OK "$SMPROGRAMS\$(ProductName)\$(Shortcut_Unintall)"
  Delete "$SMPROGRAMS\$(ProductName)\$(ShortcutName).lnk"
  Delete "$SMPROGRAMS\$(ProductName)\$(Shortcut_Unintall)"
  RMDir /r "$SMPROGRAMS\$(ProductName)"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_96DPI_PIXEL" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_BEHAVIORS" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_BROWSER_EMULATION" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_DISABLE_MK_PROTOCOL" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_DISABLE_SQM_UPLOAD_FOR_APP" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_INTERNET_SHELL_FOLDERS" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_LEGACY_DLCONTROL_BEHAVIORS" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_LOCALMACHINE_LOCKDOWN" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_MIME_HANDLING" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_MIME_SNIFFING" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_NINPUT_LEGACYMODE" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_OBJECT_CACHING" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_PROTOCOL_LOCKDOWN" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_RESTRICT_ACTIVEXINSTALL" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_SAFE_BINDTOOBJECT" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_SCRIPTURL_MITIGATION" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_SECURITYBAND" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_SPELLCHECKING" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_TABBED_BROWSING" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_WEBOC_DOCUMENT_ZOOM" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_WEBOC_POPUPMANAGEMENT" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_WINDOW_RESTRICTIONS" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_XSSFILTER" "redcore.exe"
  DeleteRegValue HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_ZONE_ELEVATION" "redcore.exe"
  caseLess:
  caseMore:
  SetAutoClose true
FunctionEnd


;----------------------------------------------------------------------------------------------------------------------------------

Section MainSection
SectionEnd
