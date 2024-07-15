set(PARGEN_PATH "pargen" CACHE PATH "Path of pargen program")
add_custom_target(update_pxml
    ${PARGEN_PATH} ${CMAKE_CURRENT_LIST_DIR}/wasm.pxml -d
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)

add_executable(test_parse
    test.cpp
    Lexer.cpp
    Token.cpp
    Parser.cpp
)
target_link_libraries(test_parse WasmVM)