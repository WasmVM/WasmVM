add_library(WasmVM_host SHARED)

target_link_libraries(WasmVM_host
  WasmVM_core
)

# Macro for adding host modules
macro(add_host_module mod_name mod_desc)
  set(BUILD_HOST_${mod_name} ON CACHE BOOL ${mod_desc})
  if(BUILD_HOST_${mod_name})
    add_subdirectory(${mod_name})
  endif(BUILD_HOST_${mod_name})
endmacro(add_host_module)

# Add most modules here
add_host_module(spectest "Essential module for test")

install(TARGETS WasmVM_host
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)