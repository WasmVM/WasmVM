# Tests for the CLI executables' shared helpers (compiled from src/exec).
# Extra sources from src/exec are passed per suite, since each suite needs a
# different helper compiled in.
macro(add_exec_test_suite suite_name)
  add_executable(exec_suite_${suite_name}_target
    ${suite_name}/${suite_name}.cpp
    ${ARGN}
  )
  target_include_directories(exec_suite_${suite_name}_target PRIVATE
    ${PROJECT_SOURCE_DIR}/src/exec
  )
  target_link_libraries(exec_suite_${suite_name}_target
    harness
    WasmVM
  )
  set_target_properties(exec_suite_${suite_name}_target
    PROPERTIES OUTPUT_NAME test_${suite_name}
  )
  add_test(NAME exec_suite_${suite_name}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_${suite_name}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${suite_name}
  )
  set_property(TEST exec_suite_${suite_name} PROPERTY
    ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/lib"
  )
  # On Windows, WasmVM.dll must sit next to the test binary (cmake/CopyDlls.cmake).
  wasmvm_copy_dlls(exec_suite_${suite_name}_target WasmVM)
endmacro(add_exec_test_suite)

add_exec_test_suite(commandparser ${PROJECT_SOURCE_DIR}/src/exec/CommandParser.cpp)
add_exec_test_suite(json)
add_exec_test_suite(linker ${PROJECT_SOURCE_DIR}/src/exec/Linker.cpp)

# Limits of memory and table imports, matched by the wasmvm CLI (issue #26).
# Each importer is run as the main module with the real wasmvm executable; the
# exporter sits next to it and is found by name. The .wat fixtures are
# assembled with wasmvm-as at build time.
set(IMPORT_LIMITS_SRC ${CMAKE_CURRENT_LIST_DIR}/import_limits)
set(IMPORT_LIMITS_BIN ${CMAKE_CURRENT_BINARY_DIR}/import_limits)
set(IMPORT_LIMITS_MODULES
  mem2 mem1to2 mem1 grown
  imp_mem_min1_from_mem2 imp_mem_min2_from_mem2 imp_mem_nomax_from_max2
  imp_mem_max2_from_nomax imp_table_min0_from_grown
)
set(IMPORT_LIMITS_WASM)
foreach(module ${IMPORT_LIMITS_MODULES})
  add_custom_command(
    OUTPUT ${IMPORT_LIMITS_BIN}/${module}.wasm
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMPORT_LIMITS_BIN}
    COMMAND $<TARGET_FILE:wasmvm-as> ${IMPORT_LIMITS_SRC}/${module}.wat
      -o ${IMPORT_LIMITS_BIN}/${module}.wasm
    DEPENDS wasmvm-as ${IMPORT_LIMITS_SRC}/${module}.wat
  )
  list(APPEND IMPORT_LIMITS_WASM ${IMPORT_LIMITS_BIN}/${module}.wasm)
endforeach()
add_custom_target(exec_import_limits_modules ALL DEPENDS ${IMPORT_LIMITS_WASM})

# An importer that matches must instantiate (exit 0); one that does not must be
# rejected by the limit check itself, not by some other failure, so its test
# passes only on the limit message.
macro(add_import_limits_test importer)
  add_test(NAME exec_import_limits_${importer}
    COMMAND $<TARGET_FILE:wasmvm> --no-system ${IMPORT_LIMITS_BIN}/${importer}.wasm
  )
  set_property(TEST exec_import_limits_${importer} PROPERTY
    ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/lib"
  )
  if(${ARGC} GREATER 1)
    set_property(TEST exec_import_limits_${importer} PROPERTY
      PASS_REGULAR_EXPRESSION "${ARGV1}"
    )
  endif()
endmacro(add_import_limits_test)

add_import_limits_test(imp_mem_min1_from_mem2)
add_import_limits_test(imp_mem_min2_from_mem2)
add_import_limits_test(imp_mem_nomax_from_max2)
add_import_limits_test(imp_mem_max2_from_nomax "import 'm' not match memory limit")
add_import_limits_test(imp_table_min0_from_grown)

# Cross-module type identity (issue #39), through the real wasmvm CLI loading
# two modules. The importer's start function casts the object it imports from
# `a` to its own type 0: a differently-shaped type 0 must trap (not be accepted
# because both modules numbered a struct 0), while an isomorphic type 0 must
# succeed. The .wat fixtures are assembled with wasmvm-as at build time, as the
# import-limits fixtures are.
set(CANON_ID_SRC ${CMAKE_CURRENT_LIST_DIR}/canon_identity)
set(CANON_ID_BIN ${CMAKE_CURRENT_BINARY_DIR}/canon_identity)
set(CANON_ID_MODULES a importer_bad importer_iso)
set(CANON_ID_WASM)
foreach(module ${CANON_ID_MODULES})
  add_custom_command(
    OUTPUT ${CANON_ID_BIN}/${module}.wasm
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CANON_ID_BIN}
    COMMAND $<TARGET_FILE:wasmvm-as> ${CANON_ID_SRC}/${module}.wat
      -o ${CANON_ID_BIN}/${module}.wasm
    DEPENDS wasmvm-as ${CANON_ID_SRC}/${module}.wat
  )
  list(APPEND CANON_ID_WASM ${CANON_ID_BIN}/${module}.wasm)
endforeach()
add_custom_target(exec_canon_identity_modules ALL DEPENDS ${CANON_ID_WASM})

# A mismatched cast across the module boundary must trap with "cast failure".
add_test(NAME exec_canon_identity_mismatch
  COMMAND $<TARGET_FILE:wasmvm> --no-system ${CANON_ID_BIN}/importer_bad.wasm
)
set_property(TEST exec_canon_identity_mismatch PROPERTY
  PASS_REGULAR_EXPRESSION "cast failure"
)
set_property(TEST exec_canon_identity_mismatch PROPERTY
  ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/lib"
)
# An isomorphic cast across the boundary must succeed (instantiation completes).
add_test(NAME exec_canon_identity_isomorphic
  COMMAND $<TARGET_FILE:wasmvm> --no-system ${CANON_ID_BIN}/importer_iso.wasm
)
set_property(TEST exec_canon_identity_isomorphic PROPERTY
  ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/lib"
)

# Import function-type matching (issue #39), through the real wasmvm CLI loading
# two modules. Module `a` exports "f" of type `$g` (result (ref null eq)).
# importer_ok declares the import as `$h` (result (ref null any)): `$g` does not
# declare `$h` as a supertype, but it structurally refines it (eq <: any), so the
# structural import rule (canon_matches) accepts it -- declared-only matching
# would not. importer_bad declares it as `$u` (result (ref null extern)), a
# different hierarchy that `$g` neither equals nor refines, so it is rejected.
set(IMPORT_MATCH_SRC ${CMAKE_CURRENT_LIST_DIR}/import_match)
set(IMPORT_MATCH_BIN ${CMAKE_CURRENT_BINARY_DIR}/import_match)
set(IMPORT_MATCH_MODULES a importer_ok importer_bad)
set(IMPORT_MATCH_WASM)
foreach(module ${IMPORT_MATCH_MODULES})
  add_custom_command(
    OUTPUT ${IMPORT_MATCH_BIN}/${module}.wasm
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMPORT_MATCH_BIN}
    COMMAND $<TARGET_FILE:wasmvm-as> ${IMPORT_MATCH_SRC}/${module}.wat
      -o ${IMPORT_MATCH_BIN}/${module}.wasm
    DEPENDS wasmvm-as ${IMPORT_MATCH_SRC}/${module}.wat
  )
  list(APPEND IMPORT_MATCH_WASM ${IMPORT_MATCH_BIN}/${module}.wasm)
endforeach()
add_custom_target(exec_import_match_modules ALL DEPENDS ${IMPORT_MATCH_WASM})

# The export structurally refines the (undeclared) import type; it is accepted
# and the module instantiates (exit 0).
add_test(NAME exec_import_match_accept
  COMMAND $<TARGET_FILE:wasmvm> --no-system ${IMPORT_MATCH_BIN}/importer_ok.wasm
)
set_property(TEST exec_import_match_accept PROPERTY
  ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/lib"
)
# A structurally incompatible import type is not satisfied; the load is rejected
# (the find_if filters it out, so it surfaces as "not found").
add_test(NAME exec_import_match_reject
  COMMAND $<TARGET_FILE:wasmvm> --no-system ${IMPORT_MATCH_BIN}/importer_bad.wasm
)
set_property(TEST exec_import_match_reject PROPERTY
  PASS_REGULAR_EXPRESSION "import 'f' not found in module"
)
set_property(TEST exec_import_match_reject PROPERTY
  ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/lib"
)

# Name section (issue #51): the library side by the harness suite, the CLI
# chains wasmvm-as / wasmvm-ld / readwasm by run_names.cmake.
add_exec_test_suite(names ${PROJECT_SOURCE_DIR}/src/exec/Linker.cpp)
foreach(case as_readwasm ld_readwasm as_strip ld_strip)
  add_test(NAME exec_names_${case}
    COMMAND ${CMAKE_COMMAND}
      -DCASE=${case}
      -DAS=$<TARGET_FILE:wasmvm-as>
      -DLD=$<TARGET_FILE:wasmvm-ld>
      -DREADWASM=$<TARGET_FILE:readwasm>
      -DSRC=${CMAKE_CURRENT_LIST_DIR}/names
      -DBIN=${CMAKE_CURRENT_BINARY_DIR}/names/${case}
      -P ${CMAKE_CURRENT_LIST_DIR}/names/run_names.cmake
  )
  set_property(TEST exec_names_${case} PROPERTY
    ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/lib"
  )
endforeach()

# The DAP server behind `wasmvm --dap` (#54), only in debugger builds. The
# suite compiles the server's sources for its direct tests and starts the real
# wasmvm for the session tests; the .wat fixtures are assembled with wasmvm-as
# at build time.
if(WASMVM_DEBUG)
  set(DAP_SRC ${CMAKE_CURRENT_LIST_DIR}/dap)
  set(DAP_BIN ${CMAKE_CURRENT_BINARY_DIR}/dap)
  set(DAP_WASM)
  foreach(module main trap)
    add_custom_command(
      OUTPUT ${DAP_BIN}/${module}.wasm
      COMMAND ${CMAKE_COMMAND} -E make_directory ${DAP_BIN}
      COMMAND $<TARGET_FILE:wasmvm-as> ${DAP_SRC}/${module}.wat
        -o ${DAP_BIN}/${module}.wasm
      DEPENDS wasmvm-as ${DAP_SRC}/${module}.wat
    )
    list(APPEND DAP_WASM ${DAP_BIN}/${module}.wasm)
  endforeach()
  add_custom_target(exec_dap_modules ALL DEPENDS ${DAP_WASM})
  find_package(Threads REQUIRED)
  add_exec_test_suite(dap
    ${PROJECT_SOURCE_DIR}/src/exec/dap/DapServer.cpp
    ${PROJECT_SOURCE_DIR}/src/exec/dap/Framing.cpp
    ${PROJECT_SOURCE_DIR}/src/exec/dap/Socket.cpp
  )
  target_include_directories(exec_suite_dap_target PRIVATE ${PROJECT_SOURCE_DIR}/src/lib)
  target_link_libraries(exec_suite_dap_target Threads::Threads)
  if(WIN32)
    target_link_libraries(exec_suite_dap_target ws2_32)
  endif(WIN32)
  target_compile_definitions(exec_suite_dap_target PRIVATE
    WASMVM_BIN="$<TARGET_FILE:wasmvm>"
    DAP_FIXTURES="${DAP_BIN}"
    DAP_SOURCE_DIR="${DAP_SRC}"
  )
  add_dependencies(exec_suite_dap_target wasmvm exec_dap_modules)
endif(WASMVM_DEBUG)

# `wasmvm --profile` (#61), only in profiler builds, through the real wasmvm by
# run_profile.cmake. The exit case needs the sys_proc host module.
if(WASMVM_PROFILE)
  set(PROFILE_CASES normal trap unwritable)
  if(BUILD_HOST_MODULES)
    list(APPEND PROFILE_CASES exit)
  endif()
  foreach(case ${PROFILE_CASES})
    add_test(NAME exec_profile_${case}
      COMMAND ${CMAKE_COMMAND}
        -DCASE=${case}
        -DAS=$<TARGET_FILE:wasmvm-as>
        -DWASMVM=$<TARGET_FILE:wasmvm>
        -DSRC=${CMAKE_CURRENT_LIST_DIR}/profile
        -DBIN=${CMAKE_CURRENT_BINARY_DIR}/profile/${case}
        -P ${CMAKE_CURRENT_LIST_DIR}/profile/run_profile.cmake
    )
    set_property(TEST exec_profile_${case} PROPERTY
      ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/lib"
    )
  endforeach()
endif(WASMVM_PROFILE)
