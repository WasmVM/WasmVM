include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    ${PROJECT_SOURCE_DIR}/include
)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/test/unittests/lib)

add_subdirectory(core)
add_subdirectory(instance)