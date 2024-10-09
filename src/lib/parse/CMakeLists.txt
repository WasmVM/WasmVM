# add_executable(test_parse
#     test.cpp
#     Lexer.cpp
#     Token.cpp
#     Parser.cpp
# )
# target_link_libraries(test_parse WasmVM)

cmake_policy(SET CMP0135 NEW)

set(ANTLR4_ZIP_REPOSITORY ${PROJECT_SOURCE_DIR}/external/antlr4-4.13.2.zip)
include(ExternalAntlr4Cpp)
include_directories(${ANTLR4_INCLUDE_DIRS})

add_executable(test_parse
    test.cpp
)
target_link_libraries(test_parse WasmVM antlr4_static)