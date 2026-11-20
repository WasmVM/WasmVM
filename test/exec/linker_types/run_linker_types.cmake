# Type indices of functions, imported functions and imported tags through the
# real CLI tools (issue #59). Run as
#   cmake -DCASE=<case> -DAS=<wasmvm-as> -DLD=<wasmvm-ld> -DREADWASM=<readwasm>
#         -DSRC=<this directory> -DBIN=<scratch directory> -P run_linker_types.cmake
#
# rec_pending   a function, an imported function and an imported tag whose type
#               sits in a rec group keep that type; the imports stay pending
# rec_explicit  likewise, with both imports named by --imports
# plain_after   a plain function type identical to an earlier module's rec group
#               member is still a type of its own

file(MAKE_DIRECTORY ${BIN})

function(run)
  execute_process(COMMAND ${ARGN} WORKING_DIRECTORY ${BIN}
    RESULT_VARIABLE result OUTPUT_VARIABLE out ERROR_VARIABLE err)
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "failed (${result}): ${ARGN}\n${out}\n${err}")
  endif()
  set(run_output "${out}" PARENT_SCOPE)
endfunction()

function(expect_file_text actual expected_file)
  file(READ ${expected_file} expected)
  if(NOT actual STREQUAL expected)
    message(FATAL_ERROR "output differs from ${expected_file}:\n${actual}")
  endif()
endfunction()

run(${AS} ${SRC}/rec_typed.wat -o rec_typed.wasm)

if(CASE STREQUAL "rec_pending")
  run(${LD} linked.wasm rec_typed.wasm)
  run(${READWASM} linked.wasm)
  expect_file_text("${run_output}" ${SRC}/rec_typed.expected.wat)
elseif(CASE STREQUAL "rec_explicit")
  run(${LD} --imports env:g,env:t linked.wasm rec_typed.wasm)
  run(${READWASM} linked.wasm)
  expect_file_text("${run_output}" ${SRC}/rec_typed.expected.wat)
elseif(CASE STREQUAL "plain_after")
  run(${AS} ${SRC}/plain_typed.wat -o plain_typed.wasm)
  run(${LD} linked.wasm rec_typed.wasm plain_typed.wasm)
  run(${READWASM} linked.wasm)
  expect_file_text("${run_output}" ${SRC}/plain_after.expected.wat)
else()
  message(FATAL_ERROR "unknown CASE '${CASE}'")
endif()
