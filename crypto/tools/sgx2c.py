#!/usr/bin/env python
# Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import subprocess
import os
import sys

def main():
  target = os.path.join(sys.argv[1], './sgx_edger8r.exe');
  opt_type = ' --untrusted '
  source = sys.argv[2]
  opt_dir = ' --untrusted-dir ';
  dir_value = sys.argv[3];
  cmd = '%s%s%s%s%s' % (target, opt_type, source, opt_dir, dir_value)
  subprocess.call(cmd, shell=True)
	
	
if __name__ == "__main__":
  main()
