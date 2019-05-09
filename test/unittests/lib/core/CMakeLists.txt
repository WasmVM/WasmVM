macro(add_core_unittest unittest)
    add_unittest(${unittest} lib/core/)
endmacro(add_core_unittest unittest)

add_core_unittest(stack_unittest)
add_core_unittest(store_unittest)
add_core_unittest(core_unittest)

add_subdirectory(runtime)