enable_testing()
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/test/unittests)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    ${PROJECT_SOURCE_DIR}/include
)

add_library(wasmvm_unittest_util
    util/ExecutorHelper.c
    util/StackHelper.c
)

function(add_unittest filepath prefix)
    get_filename_component(unittest ${filepath} NAME_WE)
    set(unittest_src "${PROJECT_BINARY_DIR}/test/unittests/${prefix}${filepath}.cpp")
    set(unittest_header "${CMAKE_CURRENT_LIST_DIR}/${filepath}.hpp")
    configure_file(${PROJECT_TEST_DIR}/unittests/unittest.cpp.in ${unittest_src})
    add_executable(${unittest}
        ${unittest_src}
        ${ARGN}
        ${unittest_header}
    )
    if(TARGET SkyPat)
        add_dependencies(${unittest}
            SkyPat
        )
    endif(TARGET SkyPat)
    target_link_libraries(${unittest}
        wasmvm_core
        skypat
        wasmvm_unittest_util
    )
    add_test(${unittest} ${PROJECT_BINARY_DIR}/test/unittests/${prefix}${unittest})
endfunction(add_unittest filepath prefix)

add_subdirectory(lib)
