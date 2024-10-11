add_subdirectory(parse)

find_package(Java 1.11 REQUIRED)
add_custom_command(OUTPUT WasmLexer.cpp
    COMMAND ${Java_JAVA_EXECUTABLE} -jar ${PROJECT_ROOT}/external/antlr4-4.13.3-dev-complete.jar -package WasmVM -Xexact-output-dir -o ${CMAKE_CURRENT_BINARY_DIR} -no-listener ${CMAKE_CURRENT_LIST_DIR}/parse/token.g4 ${CMAKE_CURRENT_LIST_DIR}/parse/Wasm.g4
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/parse/Wasm.g4 ${CMAKE_CURRENT_LIST_DIR}/parse/token.g4
)
add_library(WasmVM_textfmt OBJECT
    # parse
    parse/parse.cpp
    WasmLexer.cpp
    # parse/WasmLexer.cpp
    # parse/WasmParser.cpp
    # dump
    dump/dump.cpp
    dump/type.cpp
    dump/import.cpp
    dump/func.cpp
    dump/instr.cpp
    dump/export.cpp
    dump/elem.cpp
    dump/data.cpp
    dump/global.cpp
)
add_dependencies(WasmVM_textfmt antlr4_runtime)
target_include_directories(WasmVM_textfmt 
    PUBLIC ${PROJECT_BINARY_DIR}/external/antlr4_runtime/src/antlr4_runtime-install/include/antlr4-runtime
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)
set_property(TARGET WasmVM_textfmt PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(WasmVM_binfmt OBJECT
    # encode
    encode/encode.cpp
    encode/type.cpp
    encode/import.cpp
    encode/table.cpp
    encode/global.cpp
    encode/func.cpp
    encode/memory.cpp
    encode/instr.cpp
    encode/export.cpp
    encode/elem.cpp
    encode/data.cpp
    # decode
    decode/decode.cpp
    decode/type.cpp
    decode/import.cpp
    decode/table.cpp
    decode/global.cpp
    decode/func.cpp
    decode/memory.cpp
    decode/instr.cpp
    decode/export.cpp
    decode/start.cpp
    decode/elem.cpp
    decode/data.cpp
)
set_property(TARGET WasmVM_binfmt PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(WasmVM_src OBJECT
    exception.cpp
    functype.cpp
)
set_property(TARGET WasmVM_src PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(WasmVM_validate OBJECT
    validate/validate.cpp
    validate/func.cpp
    validate/state.cpp
    validate/table.cpp
    validate/mem.cpp
    validate/global.cpp
    validate/elem.cpp
    validate/data.cpp
    validate/export.cpp
    validate/import.cpp
)
set_property(TARGET WasmVM_validate PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(WasmVM_exec OBJECT
    instanciate.cpp
    invoke/invoke.cpp
    invoke/control.cpp
    invoke/reference.cpp
    invoke/parametric.cpp
    invoke/variable.cpp
    invoke/table.cpp
    invoke/memory.cpp
    invoke/i32.cpp
    invoke/i64.cpp
    invoke/f32.cpp
    invoke/f64.cpp
    invoke/numeric.cpp
)
set_property(TARGET WasmVM_exec PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(WasmVM_json OBJECT
    json/json.cpp
)
set_property(TARGET WasmVM_json PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(WasmVM SHARED
    $<TARGET_OBJECTS:WasmVM_textfmt>
    $<TARGET_OBJECTS:WasmVM_binfmt>
    $<TARGET_OBJECTS:WasmVM_src>
    $<TARGET_OBJECTS:WasmVM_validate>
    $<TARGET_OBJECTS:WasmVM_exec>
    $<TARGET_OBJECTS:WasmVM_json>
)
target_link_libraries(WasmVM antlr4-runtime)
set_property(TARGET WasmVM PROPERTY POSITION_INDEPENDENT_CODE 1)

if(DEV_BUILD)
add_library(WasmVM_static STATIC
    $<TARGET_OBJECTS:WasmVM_textfmt>
    $<TARGET_OBJECTS:WasmVM_binfmt>
    $<TARGET_OBJECTS:WasmVM_src>
    $<TARGET_OBJECTS:WasmVM_validate>
    $<TARGET_OBJECTS:WasmVM_exec>
    $<TARGET_OBJECTS:WasmVM_json>
)
target_link_libraries(WasmVM_static antlr4-runtime)
set_property(TARGET WasmVM_static PROPERTY OUTPUT_NAME WasmVM)

install(TARGETS WasmVM_static 
    COMPONENT library
    ARCHIVE DESTINATION lib
)
endif(DEV_BUILD)

install(TARGETS WasmVM
    COMPONENT dll
    LIBRARY DESTINATION lib
)