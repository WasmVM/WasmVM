set(PARGEN_PATH "pargen" CACHE PATH "Path of pargen program")
add_custom_target(update_pxml
    ${PARGEN_PATH} ${CMAKE_CURRENT_LIST_DIR}/wasm.pxml
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)