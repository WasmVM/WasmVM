add_executable(test_parse
    test.cpp
)
target_link_libraries(test_parse WasmVM)
target_include_directories(test_parse 
    PUBLIC ${PROJECT_BINARY_DIR}/external/antlr4_runtime/src/antlr4_runtime-install/include/antlr4-runtime
    PUBLIC ${PROJECT_BINARY_DIR}/src/lib
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)