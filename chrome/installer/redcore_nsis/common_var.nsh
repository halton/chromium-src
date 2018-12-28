; 定义NSIS新版安装界面相关设置

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

!define MUI_ICON "install.ico"
!define MUI_UNICON "uninstall.ico"
!define UNINSTALLER_NAME  "uninstall.exe"

Function ExecuteInstall
  SetShellVarContext current
  ${if} ${SYSTEM_INSTALL_LEVEL} == "TRUE"
	  ExecWait "$INSTDIR\mini_installer.exe --system-level"
  ${else}
	  ExecWait "$INSTDIR\mini_installer.exe"
  ${endif}

  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_96DPI_PIXEL" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_BEHAVIORS" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_BROWSER_EMULATION" "enterplorer.exe" 0x00001b58
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_DISABLE_MK_PROTOCOL" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_DISABLE_SQM_UPLOAD_FOR_APP" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_ENABLE_WEB_CONTROL_VISUALS" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_INTERNET_SHELL_FOLDERS" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_LEGACY_DLCONTROL_BEHAVIORS" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_LOCALMACHINE_LOCKDOWN" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_MIME_HANDLING" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_MIME_SNIFFING" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_NINPUT_LEGACYMODE" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_OBJECT_CACHING" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_PROTOCOL_LOCKDOWN" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_RESTRICT_ACTIVEXINSTALL" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_SAFE_BINDTOOBJECT" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_SCRIPTURL_MITIGATION" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_SECURITYBAND" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_SPELLCHECKING" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_TABBED_BROWSING" "enterplorer.exe" 0x00000000
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_WEBOC_DOCUMENT_ZOOM" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_WEBOC_POPUPMANAGEMENT" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_WINDOW_RESTRICTIONS" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_XSSFILTER" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_ZONE_ELEVATION" "enterplorer.exe" 0x00000001
  WriteRegDWORD HKCU "Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_GPU_RENDERING" "enterplorer.exe" 0x00000001
FunctionEnd

Function InstallComplete
  Delete "$INSTDIR\mini_installer.exe"
  Delete "$SMPROGRAMS\$(ProductName).lnk"
  Delete "$SMPROGRAMS\enterplorer.lnk"
  Delete "$SMPROGRAMS\$(ProductName)\$(ProductName).lnk"
  Delete "$DESKTOP\enterplorer.lnk"
  CreateDirectory "$SMPROGRAMS\$(ProductName)"
  CreateShortCut "$SMPROGRAMS\$(ProductName)\$(ShortcutName).lnk" \
    "$INSTDIR\Application\enterplorer.exe"
  CreateShortCut "$SMPROGRAMS\$(ProductName)\$(Shortcut_Unintall)" \
    "$INSTDIR\Application\${UNINSTALLER_NAME}"
  CreateShortCut "$DESKTOP\$(ShortcutName).lnk" "$INSTDIR\Application\enterplorer.exe" "" "" 0 SW_SHOWNORMAL "" "$(ShortcutDescription)"
  WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Allmobilize Enterplorer" \
    "UninstallString" \
    "$INSTDIR\Application\${UNINSTALLER_NAME}"
FunctionEnd
