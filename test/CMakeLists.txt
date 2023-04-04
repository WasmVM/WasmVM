include_directories(${PROJECT_ROOT}/src/include)
include_directories(${PROJECT_ROOT}/src/lib)
include_directories(${PROJECT_ROOT}/host)

# harness
set(CONSOLE_COLOR ON CACHE BOOL "Turn on color in console result")
if(CONSOLE_COLOR)
  add_compile_definitions(CONSOLE_COLOR)
endif(CONSOLE_COLOR)
include_directories(${CMAKE_CURRENT_LIST_DIR}/harness)
add_subdirectory(harness)
add_subdirectory(suite)