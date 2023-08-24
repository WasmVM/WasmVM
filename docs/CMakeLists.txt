add_custom_target(docs 
    COMMAND sphinx-build -M html "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
)