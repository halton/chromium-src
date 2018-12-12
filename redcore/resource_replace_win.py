# -*- coding: utf-8 -*-

import argparse
import codecs
import json
import os
import shutil
import re
from build_utils import execCmd
from build_utils import getDiskString

parser = argparse.ArgumentParser(description='manual to this script')
parser.add_argument('--product-name', type=str, default = "")
parser.add_argument('--working-dir', type=str, default = "")
args = parser.parse_args()

_PRODUCT_NAME = args.product_name

_WORKING_DIR = args.working_dir

_PRODUCT_DIR = os.path.join(_WORKING_DIR, "redcore", "browser-customize",  _PRODUCT_NAME)

_PRODUCT_NAME_NEEDED_REPLACE_FILES = [
    "chrome/installer/redcore_nsis/nsis_main_redcore.nsi",
    "chrome/installer/redcore_nsis/nsis_update_redcore.nsi",
    "tools/grit/grit/format/yspbrowser/zh_cn.py",
    "tools/grit/grit/format/yspbrowser/en.py",
    "chrome/app/resources/generated_resources_zh-CN.xtb",
    "chrome/installer/util/browser_distribution.cc",
    "chrome/browser/resources/help/help_content.html",
    "chrome/browser/ui/webui/uber/uber_ui.cc"
]


# apply git patch
def applyGitPatch():
  applyCmdLine = "%s \
    cd %s &&;\
    " % (getDiskString(_WORKING_DIR), _WORKING_DIR)
  patchDir = os.path.join(_PRODUCT_DIR, "patches")
  if not os.path.exists(patchDir):
    return
  filesList = os.listdir(patchDir)
  if len(filesList) == 0:
    return
  filesList.sort()
  for filename in filesList:
    applyCmdLine += "git apply %s &&;" % (os.path.join(patchDir, filename))
  applyCmdLine = applyCmdLine[:-4]
  execCmd(applyCmdLine)


# replace grd and cpp file content
def replaceNewtab():
  ntpDir = os.path.join(_PRODUCT_DIR, "new_tab_page")
  cppFileString = ""
  grdElementString = ""
  for (filePath, dirs ,files) in os.walk(ntpDir):
    for file in files:
      relativePath = os.path.join(filePath,file).replace(ntpDir + os.path.sep, "")
      uniqueName = "IDR_REDCORE_NEWTAB_" + replaceSpecialString(relativePath.upper())     
      grdElementString += createGrdElementString(uniqueName, relativePath)
      cppFileString += createCppFileString(uniqueName, relativePath)
  if len(grdElementString) != 0:
    replaceFileContentUseRegular(os.path.join(_WORKING_DIR, "chrome", "browser",
                                    "ui", "webui", "ysp_ntp", "ep_newtab_ui.cc"),
                       r"\/\/ redcore_start_tag([\s\S]+)\/\/ redcore_stop_tag", cppFileString)
    print "replace ep_newtab_ui.cc success"
  if len(cppFileString) != 0:
    replaceFileContentUseRegular(os.path.join(_WORKING_DIR, "chrome", "browser", "browser_resources.grd"),
                       "<!-- redcore_start_tag -->([\\s\\S]+)<!-- redcore_stop_tag -->", grdElementString)
    print "replace browser_resources.grd success"
  targetNtpDir = os.path.join(_WORKING_DIR, "chrome", "browser", "resources", "redcore_ntp")
  shutil.rmtree(targetNtpDir)
  shutil.copytree(ntpDir, targetNtpDir)


def replaceSpecialString(path):
  specialString = "\\/!();@&.=-+$,%#[]"
  string = ""
  for singleStr in path:
    if singleStr in specialString:
      string += "_"
    else:
      string += singleStr
  return string


def createGrdElementString(fileName, filePath):
  elementString = ""
  if filePath.lower().endswith(".html"):
    elementString = "<include name=\"" + fileName + "\" file=\"resources\\redcore_ntp\\" + filePath + "\" flattenhtml=\"true\"\
     allowexternalscript=\"true\" type=\"BINDATA\" />\n      "
  elif filePath.lower().endswith(".js"):
    elementString = "<include name=\"" + fileName + "\" file=\"resources\\redcore_ntp\\" + filePath + "\" flattenhtml=\"true\" \
    type=\"BINDATA\" />\n      "
  else:
    elementString = "<include name=\"" + fileName + "\" file=\"resources\\redcore_ntp\\" + filePath + "\" type=\"BINDATA\" />\n      "
  return elementString


def createCppFileString(fileName, filePath):
  return "html_source->AddResourcePath(\"" + filePath.replace("\\", "/") + "\", " + fileName + ");\n        "


def replaceFileContentUseRegular(filePath, regStr, newStr):
  fileData = ""
  with codecs.open(filePath, "r", encoding='utf-8') as tempFile:
    for line in tempFile:
      fileData += line
    needReplaceStr = "redcore_need_replace_placeholder"
    fileData = re.sub(regStr, needReplaceStr, fileData)
    fileData = fileData.replace(needReplaceStr, newStr)
  with codecs.open(filePath, "w", encoding='utf-8') as tempFile:
    tempFile.write(fileData)


def replaceFileContent(filePath, oldStr, newStr):
  fileData = ""
  with codecs.open(filePath, "r", encoding='utf-8') as tempFile:
    for line in tempFile:
      if oldStr in line:
        line = line.replace(oldStr, newStr)
      fileData += line
  with codecs.open(filePath, "w", encoding='utf-8') as tempFile:
    tempFile.write(fileData)


# replace product name
def replaceIcon():
  iconDir = os.path.join(_PRODUCT_DIR, "icons")
  for (filePath, dirs ,files) in os.walk(iconDir):
    for file in files:
      sourcePath = os.path.join(filePath, file);
      targetPath = os.path.join(_WORKING_DIR, sourcePath.replace(iconDir + os.path.sep, ""))
      shutil.copy(sourcePath, targetPath)
  print "replace icons success"


def updateProductInfo():
  productInfoPath = os.path.join(_PRODUCT_DIR, "product_info.json")
  with codecs.open(productInfoPath, "r", encoding='utf-8') as tempFile:
    productJson = json.load(tempFile)
    for filepath in _PRODUCT_NAME_NEEDED_REPLACE_FILES:
      for key in productJson["replace"].keys():
        replaceFileContent(os.path.join(_WORKING_DIR, os.path.normcase(filepath)), key, productJson["replace"][key])
  print "update product info success"


applyGitPatch()
replaceNewtab()
replaceIcon()
updateProductInfo()
