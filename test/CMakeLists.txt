include_directories(${PROJECT_ROOT}/src/include)
include_directories(${PROJECT_ROOT}/src/lib)

add_custom_target(spec_test_extract
  COMMAND python3 ${CMAKE_CURRENT_LIST_DIR}/extract_spec_tests.py ${CMAKE_CURRENT_BINARY_DIR}
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/
)

include_directories(${CMAKE_CURRENT_LIST_DIR}/harness)
add_subdirectory(harness)

macro(add_spec_test test_name)
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/spec/${test_name}/${test_name}.c
    COMMAND test -f ${CMAKE_CURRENT_BINARY_DIR}/spec/${test_name}/${test_name}.c
    DEPENDS spec_test_extract
  )
  add_executable(${test_name}
    ${CMAKE_CURRENT_BINARY_DIR}/spec/${test_name}/${test_name}.c
  )
  target_link_libraries(${test_name}
    harness
  )
endmacro(add_spec_test)

add_spec_test(address)