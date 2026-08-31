"""micro-zxing: copy the Version.h shim into the zxing-cpp submodule.

Upstream zxing-cpp generates core/src/Version.h from core/Version.h.in via CMake,
but PlatformIO does not run CMake. This pre-build hook copies the shim kept in
this repo (lib/zxing/Version.h) into the submodule (lib/zxing/src/core/src/),
where the quoted `#include "Version.h"` statements resolve.

To bump the ZXing version: update lib/zxing/Version.h (and the submodule tag),
no other changes needed here.
"""
import os
import shutil

Import("env")  # noqa: F821  (provided by SCons when loaded as extra_scripts)

PROJECT_DIR = env.subst("$PROJECT_DIR")
SRC = os.path.join(PROJECT_DIR, "lib", "zxing", "Version.h")
DST = os.path.join(PROJECT_DIR, "lib", "zxing", "src", "core", "src", "Version.h")

if not os.path.isfile(SRC):
    raise SystemExit("micro-zxing: missing %s" % SRC)

os.makedirs(os.path.dirname(DST), exist_ok=True)
shutil.copyfile(SRC, DST)
print("micro-zxing: copied %s -> %s" % (SRC, DST))
