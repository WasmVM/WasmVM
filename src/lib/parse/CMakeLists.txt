add_custom_target(update_wat_parser
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_LIST_DIR}/WatLexer.h
    ${CMAKE_CURRENT_LIST_DIR}/WatLexer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WatParser.h
    ${CMAKE_CURRENT_LIST_DIR}/WatParser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WatVisitor.h
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/WatLexer.h
    ${CMAKE_CURRENT_BINARY_DIR}/WatLexer.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/WatParser.h
    ${CMAKE_CURRENT_BINARY_DIR}/WatParser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/WatVisitor.h
    ${CMAKE_CURRENT_LIST_DIR}
)
