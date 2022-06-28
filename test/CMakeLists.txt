include_directories(${PROJECT_ROOT}/src/include)
include_directories(${PROJECT_ROOT}/src/lib)
include_directories(${PROJECT_ROOT}/host)

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
set(BUILD_MEMCHECK_TESTS ON CACHE BOOL "Build tests with Valgrind memcheck")
macro(add_spec_test test_name)
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/spec/${test_name}/${test_name}.c
    COMMAND test -f ${CMAKE_CURRENT_BINARY_DIR}/spec/${test_name}/${test_name}.c
    DEPENDS spec_test_extract
  )
  add_executable(spec_${test_name}
    ${CMAKE_CURRENT_BINARY_DIR}/spec/${test_name}/${test_name}.c
  )
  add_dependencies(spec_${test_name}
    spec_test_extract
  )
  target_link_libraries(spec_${test_name}
    harness
    WasmVM
    wasmvm_spectest
  )
  set_target_properties(spec_${test_name}
    PROPERTIES OUTPUT_NAME spec/${test_name}/${test_name}
  )
  add_test(NAME spec_${test_name}_run
    COMMAND ${test_name}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/spec/${test_name}
  )
  if(BUILD_MEMCHECK_TESTS)
    add_test(NAME spec_${test_name}_memcheck
      COMMAND valgrind "--leak-check=yes" "--error-exitcode=255" "--track-origins=yes" ${CMAKE_CURRENT_BINARY_DIR}/spec/${test_name}/${test_name}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/spec/${test_name}
    )
  endif(BUILD_MEMCHECK_TESTS)
endmacro(add_spec_test)

# Spec test
add_spec_test(address)
add_spec_test(align)
add_spec_test(binary)
add_spec_test(binary-leb128)
add_spec_test(block)
add_spec_test(br)
add_spec_test(br_if)
add_spec_test(br_table)
add_spec_test(call)
add_spec_test(call_indirect)
add_spec_test(comments)
add_spec_test(const)
add_spec_test(conversions)
# add_spec_test(custom) # Custom section is skipped
add_spec_test(data)
add_spec_test(elem)
add_spec_test(endianness)
add_spec_test(exports)
add_spec_test(f32)
add_spec_test(f32_bitwise)
add_spec_test(f32_cmp)
add_spec_test(f64)
add_spec_test(f64_bitwise)
add_spec_test(f64_cmp)
add_spec_test(fac)
add_spec_test(float_exprs)
add_spec_test(float_literals)
add_spec_test(float_memory)
add_spec_test(float_misc)
add_spec_test(forward)
add_spec_test(func)
add_spec_test(func_ptrs)
add_spec_test(global)
add_spec_test(i32)
add_spec_test(i64)
add_spec_test(if)
add_spec_test(imports)
add_spec_test(inline-module)
add_spec_test(int_exprs)
add_spec_test(int_literals)
add_spec_test(labels)
add_spec_test(left-to-right)
add_spec_test(linking)
add_spec_test(load)
add_spec_test(local_get)
add_spec_test(local_set)
add_spec_test(local_tee)
add_spec_test(loop)
add_spec_test(memory)
add_spec_test(memory_grow)
add_spec_test(memory_redundancy)
add_spec_test(memory_size)
add_spec_test(memory_trap)
add_spec_test(names)
add_spec_test(nop)
add_spec_test(return)
add_spec_test(select)
add_spec_test(skip-stack-guard-page)
add_spec_test(stack)
add_spec_test(start)
add_spec_test(store)
add_spec_test(switch)
add_spec_test(table)
add_spec_test(token)
add_spec_test(traps)
add_spec_test(type)
add_spec_test(unreachable)
add_spec_test(unreached-invalid)
add_spec_test(unwind)
# add_spec_test(utf8-custom-section-id) # Custom section is skipped
# add_spec_test(utf8-import-field)
# add_spec_test(utf8-import-module)
# add_spec_test(utf8-invalid-encoding)