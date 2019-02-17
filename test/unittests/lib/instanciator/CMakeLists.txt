include_directories(${PROJECT_ROOT}/src/lib/stages/instanciator)

macro(add_instanciator_unittest unittest)
    add_unittest(${unittest} lib/instanciator/)
endmacro(add_instanciator_unittest unittest)

add_instanciator_unittest(instanciator_unittest)
add_instanciator_unittest(allocate_function_unittest)
add_instanciator_unittest(allocate_instructions_unittest)