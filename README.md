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

1. Bump the version in `lib/zxing/library.json` and `lib/zxing/Version.h`.
2. Build: `platformio run`. Watch for:
   - new/changed API in `src/main.cpp` (e.g. `DecodeHints` → `ReaderOptions`,
     `Result` → `Barcode` on newer releases),
   - the C++ standard in `platformio.ini` (upstream ≥ v3.0 requires C++20),
   - new RTTI/`dynamic_cast` usage (enable with `-frtti` in `build_flags`).
3. The GitHub Action CI checks out submodules automatically (`submodules: 'true'`).

> Note: the pre-build script leaves `lib/zxing/src/core/src/Version.h` as an
> untracked file inside the submodule, so `git submodule status` may show the
> submodule as having untracked content. That is expected and harmless.
