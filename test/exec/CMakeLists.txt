# Tests for the CLI executables' shared helpers (compiled from src/exec).
macro(add_exec_test_suite suite_name)
  add_executable(exec_suite_${suite_name}_target
    ${suite_name}/${suite_name}.cpp
    ${PROJECT_SOURCE_DIR}/src/exec/CommandParser.cpp
  )
  target_include_directories(exec_suite_${suite_name}_target PRIVATE
    ${PROJECT_SOURCE_DIR}/src/exec
  )
  target_link_libraries(exec_suite_${suite_name}_target
    harness
    WasmVM
  )
  set_target_properties(exec_suite_${suite_name}_target
    PROPERTIES OUTPUT_NAME test_${suite_name}
  )
  add_test(NAME exec_suite_${suite_name}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_${suite_name}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${suite_name}
  )
  set_property(TEST exec_suite_${suite_name} PROPERTY
    ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/lib"
  )
endmacro(add_exec_test_suite)

add_exec_test_suite(commandparser)
