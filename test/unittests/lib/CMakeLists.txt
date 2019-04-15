file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/test/unittests/lib)

macro(add_lib_unittest unittest)
    add_unittest(${unittest} lib/)
endmacro(add_lib_unittest unittest)

add_lib_unittest(Loader_unittest)
add_lib_unittest(LoaderRequest_unittest)

add_subdirectory(core)
add_subdirectory(instance)
add_subdirectory(dataTypes)
add_subdirectory(decoder)
add_subdirectory(validator)
add_subdirectory(instanciator)