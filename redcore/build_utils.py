# -*- coding: utf-8 -*-
import os
import subprocess
import codecs
import platform


def execCmd(cmd):
  if platform.system() == "Darwin":
    cmd = "source ~/.bashrc &&" + cmd.replace("&&;", "&&")

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


def getBuildType(workingDir):
  osName = platform.system()
  if (osName == "Windows"):
    if (getMajorVersion(workingDir) == 49):
      return "build_type_win_49"
    else:
      return "build_type_win_70"
  elif (osName == "Darwin"):
    if (getMajorVersion(workingDir) == 49):
      return "build_type_mac_49"
    else:
      return "build_type_mac_70"


def getMajorVersion(workingDir):
  versionPath = os.path.join(workingDir, "chrome", "VERSION")
  with codecs.open(versionPath, "r", encoding="utf-8") as tempFile:
    for line in tempFile:
      if line.startswith("MAJOR="):
        return int(line[6:])
  raise Exception("\nCan not find the version file, please check the working-dir")


def getDiskString(workingDir):
  osName = platform.system()
  if (osName == "Windows"):
    return workingDir[0:2] + " &&;"
  elif (osName == "Darwin"):
    return ""
