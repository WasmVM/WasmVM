include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    ${PROJECT_SOURCE_DIR}/include
)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/test/unittests/lib)

macro(add_lib_unittest unittest)
    add_unittest(${unittest} lib/)
endmacro(add_lib_unittest unittest)

add_lib_unittest(runtime/i32_add_unittest)
add_lib_unittest(runtime/i32_and_unittest)
add_lib_unittest(runtime/i32_sub_unittest)
add_lib_unittest(runtime/i32_clz_unittest)
add_lib_unittest(runtime/i32_ctz_unittest)
add_lib_unittest(runtime/i32_const_unittest)