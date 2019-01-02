!addplugindir Plugins

SetCompress Auto
SetCompressor /FINAL /SOLID zlib

!include "MUI.nsh"
!include "WordFunc.nsh"
!include "FileFunc.nsh"
!include "enterplorer_config.nsh"
!include "str_contains.nsh"
!include "is_flash_ax_inst.nsh"
!include "text_log.nsh"
!include "version.nsh"
!include "nsDialogs.nsh"
!include "EnvVarUpdate.nsh"
!include "common_var.nsh"

; 静默安装，没有安装界面，使用自定义卸载界面
SilentInstall silent
UninstPage custom un.UninstallPage

; 语言设置必须放在Page调用下面，否则页面出不来，程序执行会异常
!include "language_var.nsh"

VIProductVersion "${CHROME_VERSION}"
Var WINDOWS_Version
Name "$(ProductName)"
OutFile "update_${CHROME_VERSION}.exe"

Function .onInit
  ${if} ${SYSTEM_INSTALL_LEVEL} == "TRUE"
	Strcpy $INSTDIR '$PROGRAMFILES\AllMobilize\Enterplorer'
  ${else}
	Strcpy $INSTDIR '$LOCALAPPDATA\AllMobilize\Enterplorer'
  ${endif}

  Delete "$INSTDIR\InstallLog.log"
  ${LogSetFileName} "$INSTDIR\InstallLog.log"
  ${LogSetOn}
FunctionEnd

Section "!$(ProductName)" SEC01
  ReadRegStr $0 HKCU "SOFTWARE\AllMobilize\Enterplorer" "pv"
  ${LogText} "old version:$0"
  ${VersionCompare} "${CHROME_VERSION}" $0 $R0
  IntCmp $R0 2 caseEqual caseLess
caseEqual:
  goto onEnd
caseLess:

  ;do install
  SetOutPath "$INSTDIR"
  SetOverwrite try

  ClearErrors
  File nsis_src\mini_installer.exe
  IfErrors onError onOK
onError:  
  ${LogText} "copy mini_installer.exe faild"
  goto onEnd
onOK:
  call ExecuteInstall
onEnd:
  Delete "$INSTDIR\mini_installer.exe"
  call InstallComplete
  ${LogSetOff}
  SetAutoClose true
SectionEnd

Section -SecLast
  WriteUninstaller "$INSTDIR\Application\${UNINSTALLER_NAME}"
SectionEnd

!include "uninstall_page.nsh"