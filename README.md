# micro-zxing

[![Platform IO CI](https://github.com/rzeldent/micro-zxing/actions/workflows/main.yml/badge.svg)](https://github.com/rzeldent/micro-zxing/actions/workflows/main.yml)

ZXing Zebra crossing barcode library

## How the library is kept up to date

The upstream library [zxing-cpp/zxing-cpp](https://github.com/zxing-cpp/zxing-cpp) is
included as a **git submodule** at `lib/zxing/src`, pinned to a release tag.

- `lib/zxing/src` – the pristine upstream repo (git submodule, tag `v3.1.1`).
- `lib/zxing` – a thin PlatformIO wrapper that compiles only the submodule's
  `core/src` (via `library.json`: `srcDir`, `includeDir`, a `srcFilter` that
  excludes the unused `libzint` C sources, and `-DZXING_INTERNAL`, a CMake
  internal macro that upstream sets for the core build).
- `lib/zxing/Version.h` – a shim replacing the `core/src/Version.h` that upstream
  normally generates via CMake. A pre-build script (`tools/gen_zxing_version.py`)
  exposes it on the include path via `CPPPATH`.
- `tools/patch_zxing_core.py` – a pre-build script that applies a small, never-
  committed patch to `core/src/BitArray.h` (adds an `appendBits(unsigned int, int)`
  overload) because on ESP32 newlib `uint32_t` is `unsigned long`, which makes
  upstream's `appendBits(int,int)`/`appendBits(uint32_t,int)` pair ambiguous.

### Build requirements (v3.1.1)

- **C++20**: upstream ≥ v3.0 core is C++20. The default espressif32 toolchain is
  GCC 8.4 (no C++20), so `platformio.ini` pins the platform and overrides the
  toolchain with GCC 12.2:
  `platform_packages = espressif/toolchain-xtensa-esp32 @ 12.2.0+20230208`.
- **API**: `DecodeHints` became `ReaderOptions` and `Result` became `Barcode`;
  `src/main.cpp` was migrated accordingly (see `BarcodeFormat::All`, `setFormats`,
  `ReaderOptions` properties).

### Updating to a new upstream release

```bash
cd lib/zxing/src
git fetch --tags
git checkout vX.Y.Z              # the new upstream tag
cd ../..
git add lib/zxing/src
git commit -m "Update zxing-cpp to vX.Y.Z"
```

Then:

1. Bump the version in `lib/zxing/library.json` and `lib/zxing/Version.h`
   (or run `python tools/bump_zxing_version.py X.Y.Z`, which also updates
   `lib/library.json` and the C++ standard in `platformio.ini` for v3+).
2. Build: `platformio run`. Watch for:
   - new/changed API in `src/main.cpp` (e.g. `DecodeHints` → `ReaderOptions`,
     `Result` → `Barcode`),
   - `ZXING_ENABLE_*` format flags and `ZXING_INTERNAL` in `lib/zxing/Version.h`
     (v3 enables formats per-flag; missing flags default to disabled),
   - whether `tools/patch_zxing_core.py` still matches the new `BitArray.h`
     (it fails loudly if upstream changes the layout).
3. The GitHub Action CI checks out submodules automatically (`submodules: 'true'`).

### Automated updates

The workflow `.github/workflows/update-zxing-cpp.yml` runs weekly (and can be run
manually from the Actions tab). When upstream publishes a new tag it updates the
submodule, bumps the version metadata via `tools/bump_zxing_version.py`, runs a
build check and opens a pull request with the build status in the description.

> The `Version.h` shim is provided via the include path, but `tools/patch_zxing_core.py`
> modifies `lib/zxing/src/core/src/BitArray.h` at build time, so `git submodule status`
> shows `lib/zxing/src` as modified after a build. That patch is regenerated on every
> build (including CI) and is never committed — don't commit it or any other file
> inside the submodule, since CI only fetches the pinned commit.
