#!/usr/bin/env python
# Copyright 2018 Redcore All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from shutil import copy2
import os
import sys

outdir = sys.argv[1]
files = [
	"chrome",
    "chrome_sandbox",
	"chrome_100_percent.pak",
	"chrome_200_percent.pak"
]

for f in files:
  newfile = f.replace('chrome', 'enterplorer')
  copy2(os.path.join(outdir, f), os.path.join(outdir, newfile))
