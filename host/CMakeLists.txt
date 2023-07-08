set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/host.cpp PROPERTIES GENERATED 1)
add_library(WasmVM_host SHARED
  ${CMAKE_CURRENT_BINARY_DIR}/host.cpp
)

# Macro for adding host modules
macro(add_host_module mod_name mod_desc)
  set(BUILD_HOST_${mod_name} ON CACHE BOOL ${mod_desc})
  if(BUILD_HOST_${mod_name})
    add_subdirectory(${mod_name})
    list(APPEND HOST_Modules ${mod_name})
  endif(BUILD_HOST_${mod_name})
endmacro(add_host_module)

# --- Begin ---

add_host_module(testhost "Test host module")

# ---  End  ---

list(TRANSFORM HOST_Modules PREPEND "void " OUTPUT_VARIABLE HOST_defs)
list(TRANSFORM HOST_defs APPEND "_instanciate(std::map<std::filesystem::path, ModuleInst>& moduleinsts, Store& store)\;")
list(JOIN HOST_defs "\n" HOST_defs)

list(TRANSFORM HOST_Modules PREPEND "\tMOD_instanciate(" OUTPUT_VARIABLE HOST_calls)
list(TRANSFORM HOST_calls APPEND ")\;")
list(JOIN HOST_calls "\n" HOST_calls)

configure_file("host.cpp.in" ${CMAKE_CURRENT_BINARY_DIR}/host.cpp)

install(TARGETS WasmVM_host
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)