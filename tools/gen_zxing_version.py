"""micro-zxing: make the Version.h shim visible to the zxing-cpp sources.

Upstream zxing-cpp generates `core/src/Version.h` from `core/Version.h.in` via
CMake, but PlatformIO does not run CMake. This pre-build hook adds the directory
holding this repo's shim (`lib/zxing/Version.h`) to the compiler's include path,
so the quoted `#include "Version.h"` statements inside the submodule resolve.

Nothing is written into `lib/zxing/src`, keeping the submodule pristine.

To bump the ZXing version: update `lib/zxing/Version.h` (and the submodule tag),
no other changes needed here.
"""
import os

Import("env")  # noqa: F821  (provided by SCons when loaded as extra_scripts)

PROJECT_DIR = env.subst("$PROJECT_DIR")
SHIM_DIR = os.path.join(PROJECT_DIR, "lib", "zxing")

if not os.path.isfile(os.path.join(SHIM_DIR, "Version.h")):
    raise SystemExit("micro-zxing: missing %s" % os.path.join(SHIM_DIR, "Version.h"))

env.Append(CPPPATH=[SHIM_DIR])
print("micro-zxing: added %s to the include path (Version.h shim)" % SHIM_DIR)

