include_directories(${PROJECT_ROOT}/src/include)
include_directories(${PROJECT_ROOT}/src/lib)
include_directories(${PROJECT_ROOT}/host)

# harness
include_directories(${CMAKE_CURRENT_LIST_DIR}/harness)
add_subdirectory(harness)

# testsuite
add_subdirectory(parse)