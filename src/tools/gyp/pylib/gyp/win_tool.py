#!/usr/bin/env python

# Copyright (c) 2012 Google Inc. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Utility functions for Windows builds.

These functions are executed via gyp-win-tool when using the ninja generator.
"""

import os
import shutil
import subprocess
import sys
import platform

BASE_DIR = os.path.dirname(os.path.abspath(__file__))


def main(args):
  executor = WinTool()
  exit_code = executor.Dispatch(args)
  if exit_code is not None:
    sys.exit(exit_code)


class WinTool(object):
  """This class performs all the Windows tooling steps. The methods can either
  be executed directly, or dispatched from an argument list."""

  def Dispatch(self, args):
    """Dispatches a string command to a method."""
    if len(args) < 1:
      raise Exception("Not enough arguments")

    method = "Exec%s" % self._CommandifyName(args[0])
    return getattr(self, method)(*args[1:])

  def _CommandifyName(self, name_string):
    """Transforms a tool name like recursive-mirror to RecursiveMirror."""
    return name_string.title().replace('-', '')

  def _GetEnv(self, arch):
    """Gets the saved environment from a file for a given architecture."""
    # The environment is saved as an "environment block" (see CreateProcess
    # and msvs_emulation for details). We convert to a dict here.
    # Drop last 2 NULs, one for list terminator, one for trailing vs. separator.
    pairs = open(arch).read()[:-2].split('\0')
    kvs = [item.split('=', 1) for item in pairs]
    return dict(kvs)

  def ExecStamp(self, path):
    """Simple stamp command."""
    open(path, 'w').close()

  def ExecRecursiveMirror(self, source, dest):
    """Emulation of rm -rf out && cp -af in out."""
    if os.path.exists(dest):
      if os.path.isdir(dest):
        shutil.rmtree(dest)
      else:
        os.unlink(dest)
    if os.path.isdir(source):
      # Ignore .git directory.
      shutil.copytree(source, dest, ignore=shutil.ignore_patterns(r'.git'))
    else:
      shutil.copy2(source, dest)

  if platform.system() == 'Windows':
    def ExecLinkWrapper(self, arch, *args):
      """Filter diagnostic output from link that looks like:
      '   Creating library ui.dll.lib and object ui.dll.exp'
      This happens when there are exports from the dll or exe.
      """
      env = self._GetEnv(arch)
      popen = subprocess.Popen(args, shell=True, env=env,
                               stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
      out, _ = popen.communicate()
      for line in out.splitlines():
        if not line.startswith('   Creating library '):
          print line
      return popen.returncode

  def ExecManifestWrapper(self, arch, *args):
    """Run manifest tool with environment set. Strip out undesirable warning
    (some XML blocks are recognized by the OS loader, but not the manifest
    tool)."""
    env = self._GetEnv(arch)
    popen = subprocess.Popen(args, shell=True, env=env,
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    out, _ = popen.communicate()
    for line in out.splitlines():
      if line and 'manifest authoring warning 81010002' not in line:
        print line
    return popen.returncode

  def ExecMidlWrapper(self, arch, outdir, tlb, h, dlldata, iid, proxy, idl,
                      *flags):
    """Filter noisy filenames output from MIDL compile step that isn't
    quietable via command line flags.
    """
    env = self._GetEnv(arch)
    args = ['midl', '/nologo'] + list(flags) + [
          '/out', outdir,
          '/h', h,
          idl]
    popen = subprocess.Popen(args, shell=True, env=env,
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    out, _ = popen.communicate()
    # Filter junk out of stdout, and write filtered versions. Output we want
    # to filter is pairs of lines that look like this:
    # Processing C:\Program Files (x86)\Microsoft SDKs\...\include\objidl.idl
    # objidl.idl
    lines = out.splitlines()
    prefixes = ['Processing ', '64 bit Processing ', '64 bit MIDLRT Processing ']
    processing = set()
    for prefix in prefixes:
      processing = processing.union(set(os.path.basename(x) for x in lines if x.startswith(prefix)))
    for line in lines:
      if line in processing:
        continue
      if reduce(lambda x, y : x or y,
                [line.startswith(prefix) for prefix in prefixes]):
        continue
      print line
    return popen.returncode

  def ExecAsmWrapper(self, arch, *args):
    """Filter logo banner from invocations of asm.exe."""
    env = self._GetEnv(arch)
    # MSVS doesn't assemble x64 asm files.
    if arch == 'environment.x64':
      return 0
    popen = subprocess.Popen(args, shell=True, env=env,
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    out, _ = popen.communicate()
    for line in out.splitlines():
      if (not line.startswith('Copyright (C) Microsoft Corporation') and
          not line.startswith('Microsoft (R) Macro Assembler') and
          not line.startswith(' Assembling: ') and
          line):
        print line
    return popen.returncode

  def ExecRcWrapper(self, arch, *args):
    """Filter logo banner from invocations of rc.exe. Older versions of RC
    don't support the /nologo flag."""
    env = self._GetEnv(arch)
    popen = subprocess.Popen(args, shell=True, env=env,
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    out, _ = popen.communicate()
    for line in out.splitlines():
      if (not line.startswith('Microsoft (R) Windows (R) Resource Compiler') and
          not line.startswith('Copyright (C) Microsoft Corporation') and
          line):
        print line
    return popen.returncode

  def ExecActionWrapper(self, arch, rspfile, *dir):
    """Runs an action command line from a response file using the environment
    for |arch|. If |dir| is supplied, use that as the working directory."""
    env = self._GetEnv(arch)
    args = open(rspfile).read()
    dir = dir[0] if dir else None
    popen = subprocess.Popen(args, shell=True, env=env, cwd=dir)
    popen.wait()
    return popen.returncode

if __name__ == '__main__':
  sys.exit(main(sys.argv[1:]))
