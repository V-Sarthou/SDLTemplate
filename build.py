#!/usr/bin/env python

import os
import subprocess

source_dir = os.path.dirname(os.path.realpath(__file__))
build_dir = os.path.join(source_dir, "build")

if not os.path.isdir(build_dir):
  os.makedirs(build_dir)

subprocess.call(["cmake", "-S", source_dir, "-B", build_dir, "-DCMAKE_BUILD_TYPE=Release"])
subprocess.call(["cmake", "--build", build_dir, "--config", "Release"])
