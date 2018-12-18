include_directories(${PROJECT_ROOT}/src/lib/validator)

macro(add_core_unittest unittest)
    add_unittest(${unittest} lib/validator/)
endmacro(add_core_unittest unittest)

add_core_unittest(validate_FuncType_unittest)
add_core_unittest(validate_Table_unittest)
add_core_unittest(validate_Memory_unittest)
add_core_unittest(validate_Global_unittest)
add_core_unittest(validate_Elem_unittest)
add_core_unittest(validate_Data_unittest)
add_core_unittest(validate_Import_unittest)
add_core_unittest(validate_Export_unittest)
