add_executable(wasmvm-as
    as.cpp
    CommandParser.cpp
)
target_link_libraries(wasmvm-as
    WasmVM
)
if(INSTALL_CPP)
install(TARGETS wasmvm-as
    RUNTIME DESTINATION bin
)
endif(INSTALL_CPP)