# -*- coding: utf-8 -*-

import argparse
import codecs
import os
import shutil
import subprocess
# from auto_build_win import _MASTER_BRANCH_LIST

parser = argparse.ArgumentParser(description='manual to this script')
parser.add_argument('--working-dir', type=str, default = "")
parser.add_argument('--product-name', type=str, default = "")

args = parser.parse_args()

_WORKING_DIR = args.working_dir
_PRODUCT_NAME = args.product_name

# 不在这个列表中的product_name，最终生成的安装包会将redcore替换为对应的product-name
_MASTER_BRANCH_LIST = [
    "49_dev",
    "70_dev"
]

_IS_MASTER_BRANCH = _PRODUCT_NAME in _MASTER_BRANCH_LIST

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

_PDB_FILES = [
    "chrome.dll.pdb",
    "chrome_child.dll.pdb",
    "chrome_elf.dll.pdb",
    "chrome_watcher.dll.pdb",
    "initialexe/redcore.exe.pdb"
]


def execCmd(cmd):
  print cmd
  #universal_newlines=True, it means by text way to open stdout and stderr
  p = subprocess.Popen(cmd, shell=True, universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
  curline = p.stdout.readline()
  while(curline != ""):
    print(curline),
    curline = p.stdout.readline()
    # 这里无法用returncode来判断bat脚本执行错误，所以bat语句连接符需要设置成 &&
  p.wait()
  if p.returncode != 0:
    raise Exception("\nExit with code %s" %p.returncode)


def firstSignWithNewKey():
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
  shutil.copy(os.path.join(releaseDir, "mini_installer.exe"), os.path.join(nsisSrcDir, "install_redcore.exe"))
  print "copying gm folder"
  shutil.copytree(os.path.join(releaseDir, "gm"), os.path.join(nsisSrcDir, "gm"))
  # 更改版本号
  buffstr = "!define CHROME_VERSION \"" + getBuildVersion() + "\"\n"
  fileObject = open(os.path.join(nsisDir, "version.nsh"), "wb")
  fileObject.write(buffstr)
  fileObject.close()
  print "modify nsis version.nsh:", getBuildVersion()
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
  installNamePerfix = "install_redcore."
  updateNamePerfix = "update_redcore."
  if not _IS_MASTER_BRANCH:
    installNamePerfix = installNamePerfix.replace("redcore", _PRODUCT_NAME)
    updateNamePerfix = updateNamePerfix.replace("redcore", _PRODUCT_NAME)
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
  with codecs.open(os.path.join(_WORKING_DIR, "chrome", "VERSION"), "r", encoding='utf-8') as tempFile:
    for line in tempFile:
      if line.startswith('YSPMAJOR='):
        major = int(line[9:])
      elif line.startswith('YSPMINOR='):
        minor = int(line[9:])
      elif line.startswith('YSPBUILD='):
        build = int(line[9:])
  return "%s.%s.%s" % (major, minor, build)


firstSignWithNewKey()
# firstSignWithOldkey()
nsisPackage()
finalSignWithNewKey()
# finalSignWithOldKey()
renameExe()
