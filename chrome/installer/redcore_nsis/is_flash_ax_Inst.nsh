; 返回值到堆栈顶
; 0 (Flash 未安装)
; 或
; 1 (Flash 已安装)
;
; 用法:
;   Call IsFlashInstalled
;   Pop $R0
;   ; $R0 在这里为 "1" 或 "0"

Function IsFlashInstalled
  Push $R0
  ClearErrors
  ReadRegStr $R0 HKCR "CLSID\{D27CDB6E-AE6D-11cf-96B8-444553540000}" ""
  IfErrors lbl_na
    StrCpy $R0 1
  Goto lbl_end
  lbl_na:
    StrCpy $R0 0
  lbl_end:
  Exch $R0
FunctionEnd