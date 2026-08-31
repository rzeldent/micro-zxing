# micro-zxing

[![Platform IO CI](https://github.com/rzeldent/micro-zxing/actions/workflows/main.yml/badge.svg)](https://github.com/rzeldent/micro-zxing/actions/workflows/main.yml)

ZXing Zebra crossing barcode library

## How the library is kept up to date

The upstream library [zxing-cpp/zxing-cpp](https://github.com/zxing-cpp/zxing-cpp) is
included as a **git submodule** at `lib/zxing/src`, pinned to a release tag.

- `lib/zxing/src` – the pristine upstream repo (git submodule, tag `v2.3.0`).
- `lib/zxing` – a thin PlatformIO wrapper that compiles only the submodule's
  `core/src` (via `library.json`: `srcDir`, `includeDir` and a `srcFilter` that
  excludes the unused `libzint` C sources).
- `lib/zxing/Version.h` – a shim replacing the `core/src/Version.h` that upstream
  normally generates via CMake. A pre-build script (`tools/gen_zxing_version.py`,
  registered in `platformio.ini` as `extra_scripts`) copies it into the submodule
  before compiling.

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
   `lib/library.json` and bumps the C++ standard to C++20 for v3+).
2. Build: `platformio run`. Watch for:
   - new/changed API in `src/main.cpp` (e.g. `DecodeHints` → `ReaderOptions`,
     `Result` → `Barcode` on newer releases),
   - the C++ standard in `platformio.ini` (upstream ≥ v3.0 requires C++20),
   - new RTTI/`dynamic_cast` usage (enable with `-frtti` in `build_flags`).
3. The GitHub Action CI checks out submodules automatically (`submodules: 'true'`).

### Automated updates

The workflow `.github/workflows/update-zxing-cpp.yml` runs weekly (and can be run
manually from the Actions tab). When upstream publishes a new tag it updates the
submodule, bumps the version metadata via `tools/bump_zxing_version.py`, runs a
build check and opens a pull request with the build status in the description.

> The `Version.h` shim is provided via the include path (`tools/gen_zxing_version.py`
> adds `lib/zxing` to `CPPPATH`); nothing is ever written into `lib/zxing/src`, so
> the submodule stays pristine. Don't commit files inside the submodule — CI only
> fetches the commit pinned in the superproject, so local-only commits break it.
