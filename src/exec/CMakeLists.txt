add_executable(wasmvm-as
    as.cpp
    CommandParser.cpp
)
target_link_libraries(wasmvm-as
    WasmVM
)

add_executable(readwasm
    readwasm.cpp
    CommandParser.cpp
)
target_link_libraries(readwasm
    WasmVM
)

if(INSTALL_CPP)
install(TARGETS wasmvm-as readwasm
    RUNTIME DESTINATION bin
)
endif(INSTALL_CPP)