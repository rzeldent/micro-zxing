"""Apply a small source patch to the zxing-cpp submodule before building.

zxing-cpp 3.x declares BitArray::appendBits as `(int, int)` and `(uint32_t, int)`.
On the ESP32 (newlib), `uint32_t` is `unsigned long`, which is distinct from
`unsigned int`, so calls like `appendBits(0xAAAAAAAA, n)` become ambiguous
between the two overloads. Adding an `(unsigned int, int)` overload makes
`int`, `unsigned int` and `uint32_t` (== unsigned long) all resolve exactly.
All overloads only extract low bits, so behaviour is identical.

The patch is applied to the submodule working tree at build time and is never
committed, so `git submodule status` may show lib/zxing/src as modified after a
build. It is regenerated (idempotently) on every build, including CI.
"""
import os

Import("env")  # noqa: F821  (provided by SCons when loaded as extra_scripts)

BITARRAY_H = os.path.join(
    env.subst("$PROJECT_DIR"), "lib", "zxing", "src", "core", "src", "BitArray.h"
)

MARKER = "void appendBits(unsigned int value, int numBits)"
ANCHOR = "\tvoid appendBits(uint32_t value, int numBits)"
OVERLOAD = (
    "\tvoid appendBits(unsigned int value, int numBits)\n"
    "\t{\n"
    "\t\tappendBits(static_cast<int>(value), numBits);\n"
    "\t}"
)


def main() -> None:
    with open(BITARRAY_H, "r") as f:
        lines = f.read().split("\n")

    if any(MARKER in line for line in lines):
        return  # already patched

    idx = next((i for i, line in enumerate(lines) if ANCHOR in line), None)
    if idx is None:
        raise SystemExit("micro-zxing: could not find appendBits(uint32_t, int) in BitArray.h")

    # method body is '{' ... '}'; find the closing brace
    close = idx + 1
    while close < len(lines) and lines[close].strip() != "}":
        close += 1
    if close >= len(lines):
        raise SystemExit("micro-zxing: unexpected BitArray.h layout")

    lines.insert(close + 1, OVERLOAD)

    with open(BITARRAY_H, "w") as f:
        f.write("\n".join(lines))
    print("micro-zxing: patched BitArray.h (added appendBits(unsigned int, int) overload)")


main()
