; 该脚本使用 HM VNISEdit 脚本编辑器向导产生

Unicode true
!include "LogicLib.nsh"

; 安装程序初始定义常量
!define PRODUCT_NAME "Dmall In-store Server"
!define PRODUCT_VERSION ${PROGRAM_VERSION}
!define PRODUCT_PUBLISHER "Dmall Inc."
!define LEGAL_COPYRIGHT "Copyright © 2024 Dmall Inc. All rights reserved."
!define PRODUCT_WEB_SITE "http://www.dmall.com"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; 命令行传递参数
; PROGRAM_ROOTDIR [打包文件目录]
; PROGRAM_OUTFILE [输出文件名]
; PROGRAM_VERSION [程序版本号]

SetCompressor lzma

; ------ MUI 现代界面定义 (1.67 版本以上兼容) ------
!include "MUI.nsh"
!include "WordFunc.nsh"
!include "logiclib.nsh"
!include "FileFunc.nsh"

;Adds the Product Version on top of the Version Tab in the Properties of the file.
VIProductVersion "${PRODUCT_VERSION}"
;Adds a field in the Version Tab of the File Properties.
VIAddVersionKey "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey "LegalCopyright" "${LEGAL_COPYRIGHT}"
VIAddVersionKey "FileDescription" "${PRODUCT_NAME} Application"
VIAddVersionKey "ProductVersion" "${PRODUCT_VERSION}"
VIAddVersionKey "FileVersion" "${PRODUCT_VERSION}"
VIAddVersionKey "Package Build Date" "${__DATE__}"

; MUI 预定义常量
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install-nsis.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"

; 欢迎页面
!insertmacro MUI_PAGE_WELCOME
; 安装目录选择页面
!insertmacro MUI_PAGE_DIRECTORY
; 安装过程页面
!insertmacro MUI_PAGE_INSTFILES
; 安装完成页面
!define MUI_FINISHPAGE_RUN "$INSTDIR\Launcher.exe"
!insertmacro MUI_PAGE_FINISH

; 安装卸载过程页面
!insertmacro MUI_UNPAGE_INSTFILES

; 安装界面包含的语言设置
!insertmacro MUI_LANGUAGE "English"

; 安装预释放文件
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
; ------ MUI 现代界面定义结束 ------


Name "${PRODUCT_NAME}"
OutFile "${PROGRAM_OUTFILE}"
InstallDir "D:\InstoreServer"

InstallDirRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString"
ShowInstDetails show
ShowUnInstDetails show
RequestExecutionLevel admin

Function .onInit
    ;MessageBox MB_OK  $INSTDIR 
    ${WordFind} $INSTDIR "." "#" $R0 ;查找有不有版本号的目录，wordfind按“.”分段，如果没有"."  r0=$INSTDIR
    ${If} $R0 != $INSTDIR
        ${WordFind} $INSTDIR "." +1 $R0  ;取"."前段
        ${GetParent} $R0 $R1    ; 获取上一级目录
        StrCpy $INSTDIR $R1     ; 修改安装目录路径
        ;MessageBox MB_OK $INSTDIR 
    ${EndIf}

FunctionEnd

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite try
  File /r "${PROGRAM_ROOTDIR}\*.*"
  CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\Launcher.exe"
  CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\Launcher.exe"
  CreateShortCut "$SMSTARTUP\${PRODUCT_NAME}.lnk" "$INSTDIR\Launcher.exe"
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninst.lnk" "$INSTDIR\${PRODUCT_VERSION}\Uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\${PRODUCT_VERSION}\Uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "${PRODUCT_NAME}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\${PRODUCT_VERSION}\Uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "InstallPath" "$INSTDIR"
SectionEnd

/******************************
 *  以下是安装程序的卸载部分  *
 ******************************/

Section Uninstall
  ;Delete "$INSTDIR\${PRODUCT_NAME}.url"
	
  Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
  Delete "$SMSTARTUP\${PRODUCT_NAME}.lnk"
  
  ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "InstallPath"
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  
  RMDir /r "$SMPROGRAMS\${PRODUCT_NAME}"
  RMDir /r $0
  
  SetAutoClose true
SectionEnd

#-- 根据 NSIS 脚本编辑规则，所有 Function 区段必须放置在 Section 区段之后编写，以避免安装程序出现未可预知的问题。--#

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all its components?"IDYES +2
  Abort
FunctionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) has been successfully removed from your computer."
FunctionEnd
