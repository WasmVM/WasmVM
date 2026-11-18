# Pass STATIC_ON_WIN32 after the name for a suite that must see the library's
# heap allocations from the test executable (see the table suite below).
macro(add_invoke_suite suite_name)
  add_executable(invoke_${suite_name}_target
    ${suite_name}/${suite_name}.cpp
  )
  if(WIN32 AND "${ARGN}" STREQUAL "STATIC_ON_WIN32")
    set(invoke_suite_lib WasmVM_static)
  else()
    set(invoke_suite_lib WasmVM)
  endif()
  target_link_libraries(invoke_${suite_name}_target
    harness
    ${invoke_suite_lib}
  )
  set_target_properties(invoke_${suite_name}_target
    PROPERTIES OUTPUT_NAME test_invoke_${suite_name}
  )
  add_test(NAME invoke_${suite_name}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_invoke_${suite_name}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${suite_name}
  )
  set_property(TEST invoke_${suite_name} PROPERTY
    ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/lib"
  )
  # On Windows, WasmVM.dll must sit next to the test binary (cmake/CopyDlls.cmake).
  wasmvm_copy_dlls(invoke_${suite_name}_target WasmVM)
endmacro(add_invoke_suite)

# The private invoke headers (RunVisitor, the trap exception types).
include_directories(
  ${PROJECT_SOURCE_DIR}/src/lib/invoke
)

# Add tests under here
add_invoke_suite(numeric)
add_invoke_suite(interrupt)
add_invoke_suite(exception)
add_invoke_suite(traps)
add_invoke_suite(branch)
add_invoke_suite(gc)
add_invoke_suite(gc_collect)
add_invoke_suite(canon)
# The #36 table.grow tests replace the global operator new in the test
# executable. On Windows that replacement does not reach allocations made inside
# WasmVM.dll, which has its own, so link the library statically there.
add_invoke_suite(table STATIC_ON_WIN32)
add_invoke_suite(global_init)
add_invoke_suite(force_bounds)
add_invoke_suite(segment_index)
# Outer backstop for the interrupt suite's infinite-loop fixtures: a broken
# interrupt gate must read as a red test, not hang the whole test run.
set_property(TEST invoke_interrupt PROPERTY TIMEOUT 20)
