# micro-zxing

[![Platform IO CI](https://github.com/rzeldent/micro-zxing/actions/workflows/main.yml/badge.svg)](https://github.com/rzeldent/micro-zxing/actions/workflows/main.yml)

ZXing Zebra crossing barcode library

20250628:
Updated ZXing by putting in sourcecode from: [https://github.com/zxing-cpp/zxing-cpp](https://github.com/zxing-cpp/zxing-cpp)

Additional steps to update the library:

- Updated version.h manually because there is no project preprocessing
- removed libzint directory (stibs) because is not used and caused compile error