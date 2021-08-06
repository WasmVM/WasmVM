include_directories(${PROJECT_ROOT}/src/include)
include_directories(${PROJECT_ROOT}/src/lib)

# Extract spec test cases
add_custom_target(spec_test_extract
  COMMAND python3 ${CMAKE_CURRENT_LIST_DIR}/extract_spec_tests.py ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR}/third_party/wabt/wast2json
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/
  DEPENDS wabt
)

# harness
include_directories(${CMAKE_CURRENT_LIST_DIR}/harness)
add_subdirectory(harness)

# Macro for adding spec tests
macro(add_spec_test test_name)
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/spec/${test_name}/${test_name}.c
    COMMAND test -f ${CMAKE_CURRENT_BINARY_DIR}/spec/${test_name}/${test_name}.c
    DEPENDS spec_test_extract
  )
  add_executable(spec_${test_name}
    ${CMAKE_CURRENT_BINARY_DIR}/spec/${test_name}/${test_name}.c
  )
  target_link_libraries(spec_${test_name}
    harness
    WasmVM
  )
  set_target_properties(spec_${test_name}
    PROPERTIES OUTPUT_NAME spec/${test_name}/${test_name}
  )
  add_test(NAME spec_${test_name}
    COMMAND ${test_name}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/spec/${test_name}
  )
endmacro(add_spec_test)

# Spec tests
add_spec_test(address)