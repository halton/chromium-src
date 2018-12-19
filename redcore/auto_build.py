# -*- coding: utf-8 -*-

import argparse
import os
import codecs
import platform
from build_utils import execCmd
from build_utils import getBuildType
from build_utils import getDiskString


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

# 不在这个列表中的product_name，最终生成的安装包会将redcore替换为对应的product-name
_MASTER_BRANCH_LIST = [
    "49_dev",
    "70_dev"
]

_IS_MASTER_BRANCH = _PRODUCT_NAME in _MASTER_BRANCH_LIST

_INTERNAL_VERSION = ""
_WINDOW_VERSION = ""


def modifyBuildVersion():
  major = 0
  minor = 0
  build = 0
  patch = 0
  yspMajor = 0
  yspMinor = 0
  yspBuild = 0
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
      elif line.startswith("YSPMAJOR="):
        yspMajor = int(line[9:])
      elif line.startswith("YSPMINOR="):
        yspMinor = int(line[9:])
      elif line.startswith("YSPBUILD="):
        yspBuild = int(line[9:])
        yspBuild += 1
  versionFileContent = "MAJOR=%s\nMINOR=%s\nBUILD=%s\nPATCH=%s\nYSPMAJOR=%s\nYSPMINOR=%s\nYSPBUILD=%s\n" % (major, minor, build, patch, yspMajor, yspMinor, yspBuild)
  # 这里chrome下的VERSION每次都要更改，因为编译需要
  with codecs.open(os.path.join(_WORKING_DIR, "chrome", "VERSION"), "w", encoding="utf-8") as tempFile:
    tempFile.write(versionFileContent)
  # 如果不是项目主分支的编译，还需要增长customize下的version，然后push
  if not _IS_MASTER_BRANCH and needIncreaseCustomVersion():
    with codecs.open(versionPath, "w", encoding='utf-8') as tempFile:
      tempFile.write(versionFileContent)
  # 记录版本号，在编译成功以后，push代码
  global _INTERNAL_VERSION, _WINDOW_VERSION
  _INTERNAL_VERSION = "%s.%s.%s.%s" % (major, minor, build, patch)
  _WINDOW_VERSION = "%s.%s.%s.%s" % (yspMajor, major, yspMinor, yspBuild)


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
  pullCmdLine = "%s \
    cd %s &&;\
    git pull --rebase git@github.com:1redcore/chromium-src.git %s\
    " % (getDiskString(_WORKING_DIR), _WORKING_DIR, _BRANCH_NAME)
  execCmd(pullCmdLine)


def gitPushVersionChangeTo1redcore(chromeVersion, redcoreVersion):
  commitMessage = "%s the internal version is %s ; modify windows version to %s ;" % (_PRODUCT_NAME, chromeVersion, redcoreVersion)
  pushCmdLine = "%s \
    cd %s &&;\
    git add chrome/VERSION &&;\
    git commit -m \"%s\" &&;\
    git push git@github.com:1redcore/chromium-src.git %s\
    " % (getDiskString(_WORKING_DIR), _WORKING_DIR, commitMessage, _BRANCH_NAME)
  execCmd(pushCmdLine)


def gitPushVersionChangeToCustomize(chromeVersion, redcoreVersion):
  commitMessage = "%s the internal version is %s ; modify windows version to %s ;" % (_PRODUCT_NAME, chromeVersion, redcoreVersion)
  pushCmdLine = "%s \
    cd %s &&;\
    git add . &&;\
    git commit -m \"%s\" &&;\
    git push git@github.com:1redcore/browser-customize.git master:master\
    " % (getDiskString(_WORKING_DIR), os.path.join(_WORKING_DIR, "redcore", "browser-customize"), commitMessage)
  execCmd(pushCmdLine)


def gitPushVersionChange():
  if _PUSH_VERSION:
    if _IS_MASTER_BRANCH or not needIncreaseCustomVersion():
      # 由于win和mac编译需要相同的版本号，所以只有在编译windows的时候才会push
      if platform.system() == "Windows":
        gitPushVersionChangeTo1redcore(_INTERNAL_VERSION, _WINDOW_VERSION)
    else:
      gitPushVersionChangeToCustomize(_INTERNAL_VERSION, _WINDOW_VERSION)
  

def gitResetWorkSpace():
  resetCmdLine = "%s \
    cd %s &&;\
    git reset --hard HEAD && git clean -df \
    " % (getDiskString(_WORKING_DIR), _WORKING_DIR)
  execCmd(resetCmdLine)


def gclient():
  cleanBuildCmdLine = ""
  if _CLEAN_BUILD:
    cleanBuildCmdLine = "ninja -C out/Release mini_installer -t clean &&;"
  gclientCmdLine = "%s \
    cd %s &&;\
    %s\
    gclient sync \
    " % (getDiskString(_WORKING_DIR), _WORKING_DIR, cleanBuildCmdLine)
  execCmd(gclientCmdLine)


def resourceReplace():
  if not _IS_MASTER_BRANCH:
    replacePythonCmdLine = "%s \
      cd %s &&;\
      python redcore%sresource_replace.py --working-dir=%s --product-name=%s\
      " % (getDiskString(_WORKING_DIR), _WORKING_DIR, os.path.sep, _WORKING_DIR, _PRODUCT_NAME)
    execCmd(replacePythonCmdLine)


def buildRelease49Win():
  autoBuild = "%s \
    cd %s &&;\
    python build%sgyp_chromium.py -Dbranding=Chromium -Dbuildtype=Official -Dproprietary_codecs=1 -Dffmpeg_branding=Chrome &&;\
    ninja -C out/Release mini_installer \
    " % (getDiskString(_WORKING_DIR), _WORKING_DIR, os.path.sep)
  execCmd(autoBuild)


def buildRelease70Win():
  autoBuild = "%s \
    cd %s &&;\
    gn gen --ide=\"vs2017\" --winsdk=10.0.17134.1  --filters=//chrome out/Release --args=\"is_component_build=false is_debug=false is_official_build=false enable_nacl=false enable_precompiled_headers=false treat_warnings_as_errors=false\" &&;\
    ninja -C out/Release mini_installer \
    " % (getDiskString(_WORKING_DIR), _WORKING_DIR)
  execCmd(autoBuild)


def buildRelease49Mac():
  autoBuild = "cd %s &&;\
    python build%sgyp_chromium.py -Dbranding=Chromium -Dbuildtype=Official -Dproprietary_codecs=1 -Dffmpeg_branding=Chrome &&;\
    ninja -C out/Release chrome \
    " % (_WORKING_DIR, os.path.sep)
  execCmd(autoBuild)


def buildRelease70Mac():
  autoBuild = "cd %s &&;\
    gn gen --filters=//chrome out/Release --args=\"is_component_build=false is_debug=false is_official_build=false enable_nacl=false enable_precompiled_headers=false treat_warnings_as_errors=false\" &&;\
    ninja -C out/Release chrome \
    " % (_WORKING_DIR)
  execCmd(autoBuild)


def buildRelease():
  buildType = getBuildType(_WORKING_DIR)
  if buildType == "build_type_win_49":
    buildRelease49Win()
  elif buildType == "build_type_win_70":
    buildRelease70Win()
  elif buildType == "build_type_mac_49":
    buildRelease49Mac()
  elif buildType == "build_type_mac_70":
    buildRelease70Mac()


def sign():
  signCmdLine = "%s \
    cd %s &&;\
    python redcore%ssign.py --working-dir=%s --product-name=%s\
    " % (getDiskString(_WORKING_DIR), _WORKING_DIR, os.path.sep, _WORKING_DIR, _PRODUCT_NAME)
  execCmd(signCmdLine)


gitResetWorkSpace()
gitPull()
gclient()
resourceReplace()
modifyBuildVersion()
buildRelease()
gitPushVersionChange()
sign()
