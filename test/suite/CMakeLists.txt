macro(add_test_suite suite_name)
  add_executable(suite_${suite_name}_target
    ${suite_name}/${suite_name}.cpp
  )
  target_link_libraries(suite_${suite_name}_target
    harness_cpp
    WasmVM
  )
  set_target_properties(suite_${suite_name}_target
    PROPERTIES OUTPUT_NAME ${suite_name}/${suite_name}
  )
  add_test(NAME suite_${suite_name}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${suite_name}/${suite_name}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${suite_name}
  )
  set_property(TEST suite_${suite_name} PROPERTY 
    ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/lib"
  )
endmacro(add_test_suite)

# Add tests under here
add_test_suite(comments)
add_test_suite(token)
add_test_suite(module)
add_test_suite(type)
add_test_suite(import)