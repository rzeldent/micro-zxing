"""Update the zxing-cpp version metadata after bumping the submodule tag.

Usage:
    python tools/bump_zxing_version.py 3.1.1

Takes the new version (with or without a leading "v"). It updates:

  - lib/zxing/library.json        version field
  - lib/zxing/Version.h           ZXING_VERSION_MAJOR/MINOR/PATCH/STR macros
  - lib/library.json              version field
  - platformio.ini                -std=gnu++17 -> -std=gnu++20 when major >= 3
                                  (upstream v3+ core requires C++20)

Run this after:  cd lib/zxing/src && git fetch --tags && git checkout <tag>
"""
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def update_json(path: Path, version: str) -> None:
    data = json.loads(path.read_text())
    data["version"] = version
    path.write_text(json.dumps(data, indent=4) + "\n")


def update_version_h(path: Path, version: str) -> None:
    major, minor, patch = (version.split(".") + ["0", "0"])[:3]
    text = path.read_text()
    text = re.sub(
        r"#define ZXING_VERSION_MAJOR \d+", f"#define ZXING_VERSION_MAJOR {major}", text
    )
    text = re.sub(
        r"#define ZXING_VERSION_MINOR \d+", f"#define ZXING_VERSION_MINOR {minor}", text
    )
    text = re.sub(
        r"#define ZXING_VERSION_PATCH \d+", f"#define ZXING_VERSION_PATCH {patch}", text
    )
    text = re.sub(
        r'#define ZXING_VERSION_STR "[^"]*"',
        f'#define ZXING_VERSION_STR "{version}"',
        text,
    )
    path.write_text(text)


def update_platformio_ini(path: Path, version: str) -> None:
    major = int(version.split(".")[0])
    if major < 3:
        return  # keep -std=gnu++17 for v2.x
    text = path.read_text()
    text = text.replace("-std=gnu++17", "-std=gnu++20")
    path.write_text(text)


def main() -> None:
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    version = sys.argv[1].lstrip("v")

    update_json(ROOT / "lib" / "zxing" / "library.json", version)
    update_version_h(ROOT / "lib" / "zxing" / "Version.h", version)
    update_json(ROOT / "lib" / "library.json", version)
    update_platformio_ini(ROOT / "platformio.ini", version)

    print(f"micro-zxing: bumped zxing-cpp metadata to {version}")


if __name__ == "__main__":
    main()
