# micro-zxing

[![Platform IO CI](https://github.com/rzeldent/micro-zxing/actions/workflows/main.yml/badge.svg)](https://github.com/rzeldent/micro-zxing/actions/workflows/main.yml)

ZXing Zebra crossing barcode library for the ESP32, built from the original
[ZXing-C++](https://github.com/zxing-cpp/zxing-cpp) library.

## Layout

* [`lib/zxing`](lib/zxing) — git **submodule** pointing at the original
  [zxing-cpp/zxing-cpp](https://github.com/zxing-cpp/zxing-cpp) repository.
  The library sources live in `lib/zxing/core/src`.
* [`lib/zxing_platformio`](lib/zxing_platformio) — PlatformIO project library that compiles the
  submodule sources (readers-only, i.e. the upstream default `ZXING_READERS`/`ZXING_WRITERS=OFF`
  configuration). It also contains the [`Version.h`](lib/zxing_platformio/Version.h) that upstream
  normally generates via CMake, plus the list of upstream sources excluded from the build.
* [`lib/zxing_patches`](lib/zxing_patches) — small ESP32 (xtensa/newlib) **portability overlay**.
  Files here share the relative path of the upstream file they replace and are compiled *instead*
  of the pristine upstream original (which is excluded from the `lib/zxing_platformio` build).
  The submodule itself is never modified.
* [`CMakeLists.txt`](CMakeLists.txt) — ESP-IDF component wrapper (same readers-only config and
  patch overlay as PlatformIO).

## Getting the source

[`lib/zxing`](lib/zxing) is a git submodule (and itself pulls in the nested
`zint` submodule), so a fresh clone must fetch those submodules too. Either
clone recursively:

    git clone --recurse-submodules https://github.com/rzeldent/micro-zxing.git

or initialize the submodules after an existing (plain) clone:

    git clone https://github.com/rzeldent/micro-zxing.git
    cd micro-zxing
    git submodule update --init --recursive

If `lib/zxing` is left empty, the PlatformIO and ESP-IDF builds will fail because
the ZXing-C++ sources live inside it.

## Usage with ESP-IDF

Simply add this project as a dependency:

    idf.py add-dependency --git https://github.com/rzeldent/micro-zxing.git micro-zxing

The component requires a C++20 capable toolchain (ESP-IDF v5.1 or newer).

## PlatformIO (development / CI)

The `esp32cam` environment in [`platformio.ini`](platformio.ini) builds the firmware in
`src/main.cpp` against the library. Current ZXing-C++ requires C++20, which the stock ESP32
Arduino toolchain (gcc 8.4) does not support, so `platformio.ini` selects the newer xtensa gcc
12.2 via `platform_packages`.

## Updating ZXing-C++

The submodule tracks the upstream `master` branch and is pinned to a specific commit:

    git -C lib/zxing fetch origin
    git -C lib/zxing checkout master
    git -C lib/zxing pull origin master
    git add lib/zxing
    # update lib/zxing_platformio/Version.h:
    #   - bump ZXING_VERSION_* to the value in lib/zxing/core/CMakeLists.txt
    #   - review the ZXING_ENABLE_* macros against lib/zxing/core/CMakeLists.txt
    # review the writer-only exclusion lists (CMakeLists.txt /
    #   lib/zxing_platformio/library.json) against the ZXING_WRITERS_OLD sections of
    #   lib/zxing/core/CMakeLists.txt
    # re-check that lib/zxing_patches still matches the upstream sources (diff against
    #   lib/zxing) and update if upstream changed

