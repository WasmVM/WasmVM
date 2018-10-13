enable_testing()
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/test/unittests)
function(add_unittest filepath prefix)
    get_filename_component(unittest ${filepath} NAME_WE)
    set(unittest_src "${PROJECT_BINARY_DIR}/test/unittests/${prefix}${filepath}.cpp")
    configure_file(${PROJECT_TEST_DIR}/unittests/unittest.cpp.in ${unittest_src})
    add_executable(${unittest}
        ${unittest_src}
        ${ARGN}
        ${filepath}.hpp
    )
    target_link_libraries(${unittest}
        wasmvm_core
        skypat
    )
    add_test(${unittest} ${PROJECT_BINARY_DIR}/test/unittests/${prefix}${unittest})
endfunction(add_unittest filepath prefix)

add_subdirectory(lib)