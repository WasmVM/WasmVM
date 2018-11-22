include_directories(${PROJECT_ROOT}/src/lib/validator)

macro(add_core_unittest unittest)
    add_unittest(${unittest} lib/validator/)
endmacro(add_core_unittest unittest)

add_core_unittest(validates_unittest)