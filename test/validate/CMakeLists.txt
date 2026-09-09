macro(add_validate_suite suite_name)
  add_executable(validate_${suite_name}_target
    ${suite_name}/${suite_name}.cpp
  )
  target_link_libraries(validate_${suite_name}_target
    harness
    WasmVM
  )
  set_target_properties(validate_${suite_name}_target
    PROPERTIES OUTPUT_NAME test_validate_${suite_name}
  )
  add_test(NAME validate_${suite_name}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_validate_${suite_name}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${suite_name}
  )
  set_property(TEST validate_${suite_name} PROPERTY
    ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/lib"
  )
endmacro(add_validate_suite)

# Add tests under here
add_validate_suite(exception)
add_validate_suite(gc_types)
