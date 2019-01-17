# -*- coding: utf-8 -*-

import argparse
import codecs
import os
import shutil
import platform
from build_utils import execCmd
from build_utils import getBuildType

parser = argparse.ArgumentParser(description='manual to this script')
parser.add_argument('--working-dir', type=str, default = "..")
parser.add_argument('--product-name', type=str, default = "70_dev")

args = parser.parse_args()

_WORKING_DIR = args.working_dir
_PRODUCT_NAME = args.product_name

# 不在这个列表中的product_name，最终生成的安装包会将redcore替换为对应的product-name
_MASTER_BRANCH_LIST = [
    "49_dev",
    "70_dev"
]

_IS_MASTER_BRANCH = _PRODUCT_NAME in _MASTER_BRANCH_LIST

if getBuildType(_WORKING_DIR) == "build_type_win_49":
  _FIRST_SIGN_FILES = [
      "chrome.dll",
      "chrome_child.dll",
      "chrome_elf.dll",
      "chrome_watcher.dll",
      "delegate_execute.exe",
      "libEGL.dll",
      "libexif.dll",
      "libGLESv2.dll",
      "metro_driver.dll",
      "mini_installer.exe",
      "nacl64.exe",
      "setup.exe",
      "initialexe/redcore.exe"
  ]
elif getBuildType(_WORKING_DIR) == "build_type_win_70":
  _FIRST_SIGN_FILES = [
      "chrome.dll",
      "chrome_watcher.dll",
      "libegl.dll",
      "sgx_urtsd.dll",
      "chrome_child.dll",
      "d3dcompiler_47.dll",
      "libglesv2.dll",
      "chrome_elf.dll",
      "enclave.signed.dll",
      "sgx_uae_service.dll",
      "libegl.dll",
      "libglesv2.dll",
      "ucrtbase.dll",
      "notification_helper.exe",
      "setup.exe",
      "mini_installer.exe",
      "enterplorer.exe",
      "initialexe/enterplorer.exe"
  ]

_PDB_FILES = [
    "chrome.dll.pdb",
    "chrome_child.dll.pdb",
    "chrome_elf.dll.pdb",
    "chrome_watcher.dll.pdb", 
    "initialexe/enterplorer.exe.pdb"
]


def firstSignWithNewKey():
  print _FIRST_SIGN_FILES
  globalSignDir = os.path.join(os.path.expanduser("~"), "Desktop", "GlobalSign")
  signToolPath = os.path.join(globalSignDir, "signtool.exe").replace("\\", "/")
  # TODO:签名证书路径处理
  signString = "%s sign /tr http://rfc3161timestamp.globalsign.com/advanced /td sha256 " % (signToolPath)
  for file in _FIRST_SIGN_FILES:
    signString += os.path.join(_WORKING_DIR, "out", "Release", os.path.normcase(file)).replace("\\", "/") + " "
  execCmd(signString)


def firstSignWithOldkey():
  signToolPath = os.path.join(os.path.expanduser("~"), "Desktop", "wosigncodecmd.exe")
  signString = signToolPath + " sign /default /file"
  cmdLine = ""
  for file in _FIRST_SIGN_FILES:
    cmdLine += signString + " \"" + os.path.join(_WORKING_DIR, "out", "Release", os.path.normcase(file)) + "\" &&; "
  # 去除最后一个&&;
  cmdLine = cmdLine[:-4]
  print "sign files with old key"
  execCmd(cmdLine)


# TODO：不知道需要不
def reLink():
  execCmd("%s &&; cd %s &&; ninja -C out/Release mini_installer" % (_WORKING_DIR[0:2], _WORKING_DIR))


def nsisPackage():
  releaseDir = os.path.join(_WORKING_DIR, "out", "Release")
  redcoreOutDir = os.path.join(releaseDir, "redcore_out")
  nsisDir = os.path.join(_WORKING_DIR, "chrome", "installer", "redcore_nsis") 
  nsisSrcDir = os.path.join(nsisDir, "nsis_src")
  if os.path.exists(redcoreOutDir):
    shutil.rmtree(redcoreOutDir)
  if os.path.exists(nsisSrcDir):
    print nsisSrcDir
    shutil.rmtree(nsisSrcDir)
  os.makedirs(redcoreOutDir)
  os.makedirs(nsisSrcDir)
  # 准备文件
  print "copying mini_installer.exe"
  shutil.copy(os.path.join(releaseDir, "mini_installer.exe"), 
    os.path.join(nsisSrcDir, "mini_installer.exe"))
  print "copying gm folder"
  shutil.copytree(os.path.join(releaseDir, "gm"), os.path.join(nsisSrcDir, "gm"))
  # 更改版本号
  buffstr = "!define CHROME_VERSION \"" + getBuildVersion() + "\"\n"
  fileObject = open(os.path.join(nsisDir, "version.nsh"), "wb")
  fileObject.write(buffstr)
  fileObject.close()
  print "modify nsis version.nsh:", getBuildVersion()
  # 移除原有.exe文件
  for file in os.listdir(nsisDir):
    if file.endswith(".exe"):
      os.remove(os.path.join(nsisDir, file))
  # 打包
  execCmd("\"C:\\Program Files (x86)\\NSIS\\Unicode\\makensis.exe\" /V2 \"" + os.path.join(nsisDir, "nsis_main_redcore.nsi") + "\"")
  execCmd("\"C:\\Program Files (x86)\\NSIS\\Unicode\\makensis.exe\" /V2 \"" + os.path.join(nsisDir, "nsis_update_redcore.nsi") + "\"")
  # 移动.exe
  for file in os.listdir(nsisDir):
    if file.endswith(".exe"):
      shutil.copy(os.path.join(nsisDir, file), redcoreOutDir)
  # 打包pdb
  redcorePdbDir = os.path.join(redcoreOutDir, "pdb")
  if not os.path.exists(redcorePdbDir):
    os.mkdir(redcorePdbDir)
  for pdb in _PDB_FILES:
    shutil.copy(os.path.join(releaseDir, os.path.normcase(pdb)), redcorePdbDir)
  print "compressing pdbs, please wait for a while!"
  execCmd("\"" +_WORKING_DIR + "\\third_party\\lzma_sdk\\Executable\\7za.exe\" \
    a -t7z -m0=BCJ -m1=LZMA:d=21 -ms -mmt " + "\"" + os.path.join(redcoreOutDir, "pdb.7z") + 
    "\" \"" + os.path.join(redcorePdbDir, "*.pdb") + "\"")
  shutil.rmtree(os.path.join(redcoreOutDir, "pdb"))


# 部分项目需要Flash插件
def addFlashAx(releaseDir, nsisSrcDir):
  flashDir = path.join(nsisSrcDir, "FlashAx")
  if os.exists(flashDir):
    shutil.rmtree(flashDir)
  shutil.copytree(os.path.join(releaseDir, "FlashAx"), flashDir)


def finalSignWithNewKey():
  globalSignDir = os.path.join(os.path.expanduser("~"), "Desktop", "GlobalSign")
  signToolPath = os.path.join(globalSignDir, "signtool.exe").replace("\\", "/")
  execCmd("%s sign /tr http://rfc3161timestamp.globalsign.com/advanced /td sha256 "
    % (signToolPath) + os.path.join(_WORKING_DIR, "out", "Release", "redcore_out", "*.exe").replace("\\", "/"))


def finalSignWithOldKey():
  signToolPath = os.path.join(os.path.expanduser("~"), "Desktop", "wosigncodecmd.exe")
  signString = signToolPath + " sign /default /file"
  execCmd(signString + " \"" + os.path.join(_WORKING_DIR, "out", "Release", "redcore_out", "*.exe") + "\"")


def renameExe():
  installNamePerfix = "install_enterplorer."
  updateNamePerfix = "update_enterplorer."
  if not _IS_MASTER_BRANCH:
    installNamePerfix = installNamePerfix.replace("enterplorer", _PRODUCT_NAME)
    updateNamePerfix = updateNamePerfix.replace("enterplorer", _PRODUCT_NAME)
  redcoreOutPath = os.path.join(_WORKING_DIR, "out", "Release", "redcore_out")
  installExePath = os.path.join(redcoreOutPath, "install_" + getBuildVersion() + ".exe")
  renameInstallExePath = os.path.join(redcoreOutPath, installNamePerfix + getRedcoreVersion() + ".exe")
  os.rename(installExePath, renameInstallExePath)
  updateExePath = os.path.join(redcoreOutPath, "update_" + getBuildVersion() + ".exe")
  renameUpdateExePath = os.path.join(redcoreOutPath, updateNamePerfix + getRedcoreVersion() + ".exe")
  os.rename(updateExePath, renameUpdateExePath)


def getBuildVersion():
  major = 0
  minor = 0
  build = 0
  patch = 0
  with codecs.open(os.path.join(_WORKING_DIR, "chrome", "VERSION"), "r", encoding='utf-8') as tempFile:
    for line in tempFile:
      if line.startswith('MAJOR='):
        major = int(line[6:])
      elif line.startswith('MINOR='):
        minor = int(line[6:])
      elif line.startswith('BUILD='):
        build = int(line[6:])
      elif line.startswith('PATCH='):
        patch = int(line[6:])
  return "%s.%s.%s.%s" % (major, minor, build, patch)


def getRedcoreVersion():
  major = 0
  minor = 0
  build = 0
  codeVersion = 0
  with codecs.open(os.path.join(_WORKING_DIR, "chrome", "VERSION"), "r", encoding='utf-8') as tempFile:
    for line in tempFile:
      if line.startswith('YSPMAJOR='):
        major = int(line[9:])
      elif line.startswith('MAJOR='):
        codeVersion = int(line[6:])
      elif line.startswith('YSPMINOR='):
        minor = int(line[9:])
      elif line.startswith('YSPBUILD='):
        build = int(line[9:])
  return "%s.%s.%s.%s" % (major, codeVersion, minor, build)

class MacOperation:
  def __init__(self, workingDir):
    self._workingDir = workingDir       
    self._appDir = os.path.join(self._workingDir, "out", "Release", "Enterplorer.app")
  def signMacApp(self):
    if not os.path.exists(self._appDir):
      self._appDir = os.path.join(_workingDir, "out", "Release", "Redcore.app")
      if not os.path.exists(self._appDir):
        print "No known app directory exist, please check the building process!"
        os._exit(-1)
    
    print "app dir is: %s" % self._appDir   
    codeSignatureDir = os.path.join(self._appDir, "Contents", "_CodeSignature") 
    signMacLine = "codesign -f -s \"Developer ID Application: Allmobilize Inc. (FD9594D6YV)\" %s\
      " % self._appDir
    print "execute signcode: %s" % signMacLine
    execCmd(signMacLine)

  def dmgPackage(self):
    releaseDir = os.path.join(_WORKING_DIR, "out", "Release")
    redcoreOutDir = os.path.join(releaseDir, "redcore_out")
    if os.path.exists(redcoreOutDir):
      shutil.rmtree(redcoreOutDir)
    os.makedirs(redcoreOutDir)
    shutil.copytree(os.path.join(releaseDir, "Enterplorer.app"), 
    os.path.join(redcoreOutDir, "Enterplorer.app"), True)
  # 创建软链接
    execCmd("ln -s /Applications %s" % (redcoreOutDir))
  # 使用系统工具打dmg
    installNamePerfix = "install_enterplorer."
    if not _IS_MASTER_BRANCH:
      installNamePerfix = installNamePerfix.replace("enterplorer", _PRODUCT_NAME)
    packageLine = "printf \"enterplorer\" | hdiutil create -srcfolder %s -stdinpass %s\
      " % (redcoreOutDir,os.path.join(redcoreOutDir, installNamePerfix + getRedcoreVersion() + ".dmg"))
    execCmd(packageLine)

def signAndPackage():
  if platform.system() == "Windows":
    firstSignWithNewKey()
    reLink()
    nsisPackage()
    finalSignWithNewKey()
    renameExe()
  elif platform.system() == "Darwin":
    mac = MacOperation(_WORKING_DIR)    
    mac.signMacApp()
    mac.dmgPackage()

if __name__ == '__main__':
  signAndPackage()
