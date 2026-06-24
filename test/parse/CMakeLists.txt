macro(add_test_suite suite_name)
  add_executable(suite_${suite_name}_target
    ${suite_name}/${suite_name}.cpp
  )
  target_link_libraries(suite_${suite_name}_target
    harness
    WasmVM
  )
  set_target_properties(suite_${suite_name}_target
    PROPERTIES OUTPUT_NAME test_${suite_name}
  )
  add_test(NAME suite_${suite_name}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_${suite_name}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${suite_name}
  )
  set_property(TEST suite_${suite_name} PROPERTY
    ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/lib"
  )
  # On Windows the loader searches the executable's own directory for
  # WasmVM.dll, not src/lib. Copy the DLL next to the test binary so the
  # suite can launch instead of blocking on a missing-DLL dialog.
  if(WIN32)
    add_custom_command(TARGET suite_${suite_name}_target POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:WasmVM> $<TARGET_FILE_DIR:suite_${suite_name}_target>
    )
  endif()
endmacro(add_test_suite)

include_directories(
  ${PROJECT_SOURCE_DIR}/src/lib/parse
  ${PROJECT_BINARY_DIR}/src/lib
)

# Add tests under here
add_test_suite(comments)
add_test_suite(module)
add_test_suite(type)
add_test_suite(import)
add_test_suite(table)
add_test_suite(memory)
add_test_suite(global)
add_test_suite(export)
add_test_suite(func)
add_test_suite(start)
add_test_suite(elem)
add_test_suite(data)
add_test_suite(instruction)
add_test_suite(roundtrip)