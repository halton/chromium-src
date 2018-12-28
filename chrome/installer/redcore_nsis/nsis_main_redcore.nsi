!addplugindir Plugins

; 申请管理员权限
RequestExecutionLevel admin
SetCompress Auto
SetCompressor /FINAL /SOLID zlib

!include "MUI.nsh"
!include "nsDialogs.nsh"
!include "WordFunc.nsh"
!include "FileFunc.nsh"
!include "version.nsh"
!include "str_contains.nsh"
!include "is_flash_ax_Inst.nsh"
!include "text_log.nsh"
!include "enterplorer_config.nsh"
!include "EnvVarUpdate.nsh"
!include "common_var.nsh"

; 使用自定义欢迎页面，安装页面以及卸载页面
!define MUI_CUSTOMFUNCTION_GUIINIT onGUIInit
Page custom WelcomePage
Page custom InstallPage
UninstPage custom un.UninstallPage

; 语言设置必须放在Page调用下面，否则页面出不来，程序执行会异常
!include "language_var.nsh"

VIProductVersion "${CHROME_VERSION}"
Var WINDOWS_Version
Name "$(ProductName)"
OutFile "install_${CHROME_VERSION}.exe"
DirText "$(DirPageTop)"

;------------------------------------------------安装界面逻辑-----------------------------------------------------------

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
Var welcome_page_bg_image
Var install_page_bg_image
Var quick_install_button_image

;------------------------控件变量------------------------

Var common_msgbox
Var common_msgbox_close_button
Var common_msgbox_ok_button

Var quit_msgbox
Var quit_msgbox_close_button
Var quit_msgbox_cancel_button
Var quit_msgbox_quit_button

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

Var common_msgbox_caption_font
Var common_msgbox_text_font
Var common_title_font
Var is_set_default
Var InstallPercent
Var MainInstallLogicComplete
Var DelayTime

;--------------------------------------------------------

Function .onInit
  ; 控制客户端安装的目录，chromium目前只支持User目录和ProgramFiles，通过enterplorer_config.nsh中的变量控制
  ${if} ${SYSTEM_INSTALL_LEVEL} == "TRUE"
    Strcpy $INSTDIR '$PROGRAMFILES\AllMobilize\Enterplorer'
  ${else}
    Strcpy $INSTDIR '$LOCALAPPDATA\AllMobilize\Enterplorer'
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
  pop $quit_msgbox_close_button

  nsiskin::CreateButton /NOUNLOAD $quit_msgbox 208 130 76 30 "" 2
  pop $quit_msgbox_cancel_button

  nsiskin::CreateButton /NOUNLOAD $quit_msgbox 304 130 76 30 "" 1
  pop $quit_msgbox_quit_button

  EnableWindow $HWNDPARENT 0
  ShowWindow $quit_msgbox ${SW_SHOW}
FunctionEnd

Function InstallBgTimer
  ${NSD_KillTimer} InstallBgTimer

  GetFunctionAddress $0 MainInstallLogic
  BgWorker::CallAndWait

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
      ShowWindow $install_page_progress_label ${SW_HIDE}
      ShowWindow $install_page_finish ${SW_SHOW}
      IntOp $DelayTime $DelayTime + 1
    ${EndIf}
  CompareEnd:
    System::Call 'User32::InvalidateRect(i$install_page_progress_bar, i0, i1) i.R9'
    ${If} $DelayTime == 5
      nsYSP::EndApplication
    ${EndIf}
FunctionEnd

Function WelcomePage
  nsDialogs::Create 1044
  pop $welcome_page_dialog
  ${If} $welcome_page_dialog == error
    Abort
  ${EndIf}
  ${SetWindowSize} $welcome_page_dialog 560 280
	
  ${NSD_CreateLabel} 15 15 280 30 $(MUI_TEXT_DIRECTORY_SUBTITLE)
  pop $install_title_label
  SetCtlColors $install_title_label 0xFFFFFF  transparent
  SendMessage $install_title_label ${WM_SETFONT} $common_title_font 0
    
  nsiskin::CreateButton /NOUNLOAD $welcome_page_dialog 530 16 12 12 ""
  pop $welcome_page_close_button

  nsiskin::CreateButton /NOUNLOAD $welcome_page_dialog 199 70 162 162 "" 0
  pop $quick_install_button

  ; nsiskin::CreateButton /NOUNLOAD $welcome_page_dialog 30 275 150 15 ""
  ; pop $set_defalut_checkbox
  ; StrCpy $is_set_default 1
     
  nsiskin::Attach /NOUNLOAD $welcome_page_dialog
    
  nsDialogs::Show
  ${NSD_FreeImage} $1
FunctionEnd

Function InstallPage
  nsDialogs::Create 1044
  pop $install_page_dialog
  ${If} $install_page_dialog == error
    Abort
  ${EndIf}
  ${SetWindowSize} $install_page_dialog 560 280
	
  ${NSD_CreateLabel} 15 15 250 30 "$(ProductName)-安装"
  pop $install_title_label
  SetCtlColors $install_title_label 0xFFFFFF  transparent
  SendMessage $install_title_label ${WM_SETFONT} $common_title_font 0

  nsiskin::CreateButton /NOUNLOAD $install_page_dialog 530 16 12 12 ""
  pop $install_page_close_button

  ${NSD_CreateLabel} 20 230 520 4 "Progress"
  pop $install_page_progress_bar
  ${NSD_AddStyle} $install_page_progress_bar ${SS_OWNERDRAW}

  ${NSD_CreateLabel} 20 245 100% 20% "正在安装,请耐心等待..."
  pop $install_page_progress_label
  ${NSD_AddStyle} $install_page_progress_label ${SS_LEFT}
  SetCtlColors $install_page_progress_label ""  transparent
  ShowWindow $install_page_progress_label ${SW_SHOW}
	
  ${NSD_CreateLabel} 20 245 100% 20% "安装完成！"
  pop $install_page_finish
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
  pop $common_msgbox_close_button

  nsiskin::CreateButton /NOUNLOAD $common_msgbox 304 130 76 30 "" 2
  pop $common_msgbox_ok_button

  EnableWindow $HWNDPARENT 0
  ShowWindow $common_msgbox ${SW_SHOW}
FunctionEnd

;------------------------------------------------安装后台逻辑-----------------------------------------------------------

Function CheckVersion
  ReadRegStr $0 HKCU "SOFTWARE\AllMobilize\Enterplorer" "pv"
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

  SetOutPath "$INSTDIR"
  SetOverwrite try

  ClearErrors
  File nsis_src\mini_installer.exe
  IfErrors onError onOK
  onError:
    ${LogText} "copy mini_installer.exe faild"
    push "Fail write data to path, please select another install directory."
    call CommonMsgBox
    goto onEnd
  onOK:
    call ExecuteInstall
    ; 判断系统版本
    ReadRegStr $WINDOWS_Version HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "ProductName"

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
  onEnd:
    call InstallComplete
    ${LogSetOff}
    SetAutoClose true
FunctionEnd

Function WriteUninstaller
  WriteUninstaller "$INSTDIR\Application\${UNINSTALLER_NAME}"
FunctionEnd

!include "uninstall_page.nsh"

Section MainSection
SectionEnd
