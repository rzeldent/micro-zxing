# ZXing-C++ is pulled in as a git submodule (see lib/zxing). Its sources live in
# lib/zxing/core/src (the repository root only contains build scaffolding).
# lib/zxing_platformio/ provides the generated `Version.h` (readers-only config) that
# upstream normally produces via CMake preprocessing; it must stay in the include path
# for both this component and its consumers (several public headers include it).
set(SRCS_DIR "lib/zxing/core/src")
set(ZXING_GLUE_DIR "lib/zxing_platformio")

file(GLOB_RECURSE CPP_SRCS "${SRCS_DIR}/*.cpp")
file(GLOB_RECURSE C_SRCS "${SRCS_DIR}/*.c")

# Exclude the bundled libzint C backend (only needed for the zint-based "new" writers).
list(FILTER CPP_SRCS EXCLUDE REGEX "/libzint/")
list(FILTER C_SRCS EXCLUDE REGEX "/libzint/")

# Exclude writer-only modules (mirrors the upstream default ZXING_WRITERS=OFF).
# Keep this list in sync with the ZXING_WRITERS_OLD/* sections of
# lib/zxing/core/CMakeLists.txt when the submodule is updated.
set(ZXING_WRITER_MODULES
    TextEncoder MultiFormatWriter
    AZEncoder AZHighLevelEncoder AZToken AZWriter
    DMECEncoder DMHighLevelEncoder DMSymbolInfo DMWriter
    ODCodabarWriter ODCode39Writer ODCode93Writer ODCode128Writer
    ODEAN8Writer ODEAN13Writer ODITFWriter ODUPCEWriter ODUPCAWriter ODWriterHelper
    PDFEncoder PDFHighLevelEncoder PDFWriter
    QREncoder QRMaskUtil QRMatrixUtil QRWriter
)
foreach(_module IN LISTS ZXING_WRITER_MODULES)
    list(FILTER CPP_SRCS EXCLUDE REGEX "/${_module}\\.cpp$")
endforeach()

# ESP32 (xtensa/newlib) portability overlay: any .cpp with the same name under
# lib/zxing_patches is compiled instead of the pristine upstream original.
set(PATCH_SRCS_DIR "lib/zxing_patches")
file(GLOB_RECURSE PATCH_SRCS "${PATCH_SRCS_DIR}/*.cpp")
foreach(_patch IN LISTS PATCH_SRCS)
    get_filename_component(_patch_name "${_patch}" NAME)
    list(FILTER CPP_SRCS EXCLUDE REGEX "/${_patch_name}$")
endforeach()

idf_component_register(
    SRCS ${C_SRCS} ${CPP_SRCS} ${PATCH_SRCS}
    INCLUDE_DIRS ${SRCS_DIR} ${ZXING_GLUE_DIR}
)

target_compile_definitions(${COMPONENT_LIB}
    PRIVATE
        ZXING_INTERNAL
        ZUECI_EMBED_NO_TO_ECI
)

target_compile_options(${COMPONENT_LIB}
    PUBLIC
        -Wno-missing-field-initializers
    PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++20>
)
