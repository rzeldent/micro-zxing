set(SRCS_DIR "lib/zxing")

file(GLOB_RECURSE CPP_SRCS "${SRCS_DIR}/*.cpp")
file(GLOB_RECURSE C_SRCS "${SRCS_DIR}/*.c")

idf_component_register(
    SRCS ${C_SRCS} ${CPP_SRCS}
    INCLUDE_DIRS ${SRCS_DIR}
)

target_compile_options(${COMPONENT_LIB}
    PUBLIC
        -Wno-missing-field-initializers
    PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++20>
)
