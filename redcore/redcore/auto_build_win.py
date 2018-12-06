# -*- coding: utf-8 -*-

import argparse
import os
import subprocess
import codecs

parser = argparse.ArgumentParser(description = 'manual to this script')
parser.add_argument("--product-name", type = str, default = "")
parser.add_argument("--working-dir", type = str, default = "")
parser.add_argument("--branch-name", type = str, default = "")
parser.add_argument("--clean-build", type = bool, default = False)
parser.add_argument("--push-version", type = bool, default = True)

args = parser.parse_args()
#print args.project_name

_PRODUCT_NAME = args.product_name
_WORKING_DIR = args.working_dir
_BRANCH_NAME = args.branch_name
_CLEAN_BUILD = args.clean_build
# 测试时将push-version参数设置为False，不会将测试版本号推到github上
_PUSH_VERSION = args.push_version

if len(_PRODUCT_NAME) == 0:
  raise Exception("--product-name could not be nil")

if len(_WORKING_DIR) == 0:
  raise Exception("--working-dir could not be nil")

_MASTER_BRANCH_LIST = [
    "49_dev",
    "70_dev"
]

_IS_MASTER_BRANCH = _PRODUCT_NAME in _MASTER_BRANCH_LIST

_INTERNAL_VERSION = ""
_WINDOW_VERSION = ""
_MAC_VERSION = ""


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


def modifyBuildVersion():
  major = 0
  minor = 0
  build = 0
  patch = 0
  yspMajor = 0
  yspMinor = 0
  yspBuild = 0
  yspMacMajor = 0
  yspMacMinor = 0
  yspMacBuild = 0
  versionPath = ""
  # 判断定制资源中如果没有VERSION文件，或者VERSION文件为空，则定制版的版本号跟随主版本号递增
  if _IS_MASTER_BRANCH or not needIncreaseCustomVersion():
    versionPath = os.path.join(_WORKING_DIR, "chrome", "VERSION")
  else:
    versionPath = os.path.join(_WORKING_DIR, "redcore", "browser-customize", _PRODUCT_NAME, "VERSION")
  with codecs.open(versionPath, "r", encoding="utf-8") as tempFile:
    for line in tempFile:
      if line.startswith("MAJOR="):
        major = int(line[6:])
      elif line.startswith("MINOR="):
        minor = int(line[6:])
      elif line.startswith("BUILD="):
        build = int(line[6:])
      elif line.startswith("PATCH="):
        patch = int(line[6:])
        patch += 1
      elif line.startswith("YSPMAJOR="):
        yspMajor = int(line[9:])
      elif line.startswith("YSPMINOR="):
        yspMinor = int(line[9:])
      elif line.startswith("YSPBUILD="):
        yspBuild = int(line[9:])
        yspBuild += 1
      elif line.startswith("YSP_MAC_MAJOR"):
        yspMacMajor = int(line[14:])
      elif line.startswith("YSP_MAC_MINOR"):
        yspMacMinor = int(line[14:])
      elif line.startswith("YSP_MAC_BUILD"):
        yspMacBuild = int(line[14:])
        yspMacBuild += 1
  versionFileContent = "MAJOR=%s\nMINOR=%s\nBUILD=%s\nPATCH=%s\nYSPMAJOR=%s\nYSPMINOR=%s\nYSPBUILD=%s\nYSP_MAC_MAJOR=%s\nYSP_MAC_MINOR=%s\nYSP_MAC_BUILD=%s\n" % (major, minor, build, patch, yspMajor, yspMinor, yspBuild, yspMacMajor, yspMacMinor, yspMacBuild)
  # 这里chrome下的VERSION每次都要更改，因为编译需要
  with codecs.open(os.path.join(_WORKING_DIR, "chrome", "VERSION"), "w", encoding="utf-8") as tempFile:
    tempFile.write(versionFileContent)
  # 如果不是项目主分支的编译，还需要增长customize下的version，然后push
  if not _IS_MASTER_BRANCH and needIncreaseCustomVersion():
    with codecs.open(versionPath, "w", encoding='utf-8') as tempFile:
      tempFile.write(versionFileContent)
  # 记录版本号，在编译成功以后，push代码
  global _INTERNAL_VERSION, _WINDOW_VERSION, _MAC_VERSION
  _INTERNAL_VERSION = "%s.%s.%s.%s" % (major, minor, build, patch)
  _WINDOW_VERSION = "%s.%s.%s" % (yspMajor, yspMinor, yspBuild)
  _MAC_VERSION = "%s.%s.%s" % (yspMacMajor, yspMacMinor, yspMacBuild)


def needIncreaseCustomVersion():
  versionPath = os.path.join(_WORKING_DIR, "redcore", "browser-customize", _PRODUCT_NAME, "VERSION")
  if not os.path.exists(versionPath):
    return False
  with codecs.open(versionPath, "r", encoding="utf-8") as tempFile:
    for line in tempFile:
      if "MAJOR=" in line:
        return True
  return False


def gitPull():
  pullCmdLine = "%s &&;\
    cd %s &&;\
    git pull --rebase git@github.com:1redcore/chromium-src.git %s\
    " % (_WORKING_DIR[0:2], _WORKING_DIR, _BRANCH_NAME)
  execCmd(pullCmdLine)


def gitPushVersionChangeTo1redcore(chromeVersion, redcoreVersion, macVersion):
  commitMessage = "%s modify the internal version to %s ; modify windows version to %s ; modify Mac version to %s" % (_PRODUCT_NAME, chromeVersion, redcoreVersion, macVersion)
  pushCmdLine = "%s &&;\
    cd %s &&;\
    git add chrome/VERSION &&;\
    git commit -m \"%s\" &&;\
    git push git@github.com:1redcore/chromium-src.git %s\
    " % (_WORKING_DIR[0:2], _WORKING_DIR, commitMessage, _BRANCH_NAME)
  execCmd(pushCmdLine)


def gitPushVersionChangeToCustomize(chromeVersion, redcoreVersion, macVersion):
  commitMessage = "%s modify the internal version to %s ; modify windows version to %s ; modify Mac version to:%s" % (_PRODUCT_NAME, chromeVersion, redcoreVersion, macVersion)
  pushCmdLine = "%s &&;\
    cd %s &&;\
    git add . &&;\
    git commit -m \"%s\" &&;\
    git push git@github.com:1redcore/browser-customize.git master:master\
    " % (_WORKING_DIR[0:2], os.path.join(_WORKING_DIR, "redcore", "browser-customize"), commitMessage)
  execCmd(pushCmdLine)


def gitPushVersionChange():
  if _PUSH_VERSION:
    if _IS_MASTER_BRANCH or not needIncreaseCustomVersion():
      gitPushVersionChangeTo1redcore(_INTERNAL_VERSION, _WINDOW_VERSION, _MAC_VERSION)
    else:
      gitPushVersionChangeToCustomize(_INTERNAL_VERSION, _WINDOW_VERSION, _MAC_VERSION)
  

def gitResetWorkSpace():
  resetCmdLine = "%s &&;\
    cd %s &&;\
    git reset --hard HEAD && git clean -df \
    " % (_WORKING_DIR[0:2], _WORKING_DIR)
  execCmd(resetCmdLine)


def gclient():
  cleanBuildCmdLine = ""
  if _CLEAN_BUILD:
    cleanBuildCmdLine = "ninja -C out/Release mini_installer -t clean &&;"
  gclientCmdLine = "%s &&;\
    cd %s &&;\
    %s\
    gclient sync \
    " % (_WORKING_DIR[0:2], _WORKING_DIR, cleanBuildCmdLine)
  execCmd(gclientCmdLine)


def resourceReplace():
  if not _IS_MASTER_BRANCH:
    replacePythonCmdLine = "%s &&;\
      cd %s &&;\
      python redcore\\resource_replace_win.py --working-dir=%s --product-name=%s" % (_WORKING_DIR[0:2], _WORKING_DIR, _WORKING_DIR, _PRODUCT_NAME)
    execCmd(replacePythonCmdLine)


def buildRelease():
  autoBuild = "%s &&;\
    cd %s &&;\
    python build\\gyp_chromium.py -Dbranding=Chromium -Dbuildtype=Official -Dproprietary_codecs=1 -Dffmpeg_branding=Chrome &&;\
    ninja -C out/Release mini_installer \
    " % (_WORKING_DIR[0:2], _WORKING_DIR)
  execCmd(autoBuild)


def sign():
  signCmdLine = "%s &&;\
    cd %s &&;\
    python redcore\\sign_win.py --working-dir=%s --product-name=%s\
    " % (_WORKING_DIR[0:2], _WORKING_DIR, _WORKING_DIR, _PRODUCT_NAME)
  execCmd(signCmdLine)


gitResetWorkSpace()
gitPull()
gclient()
modifyBuildVersion()
resourceReplace()
buildRelease()
gitPushVersionChange()
sign()
