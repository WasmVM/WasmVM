# Name section through the real CLI tools (issue #51). Run as
#   cmake -DCASE=<case> -DAS=<wasmvm-as> -DLD=<wasmvm-ld> -DREADWASM=<readwasm>
#         -DSRC=<this directory> -DBIN=<scratch directory> -P run_names.cmake
#
# as_readwasm  wasmvm-as -> readwasm prints the expected names, and its output
#              reassembles to the same text
# ld_readwasm  wasmvm-as -> wasmvm-ld -> readwasm, likewise
# as_strip     wasmvm-as --strip-names leaves no name for readwasm to print
# ld_strip     wasmvm-ld --strip-names, likewise
# dump_reassembles  wasmvm-as ${FIXTURE} -> readwasm --all, and that output
#              reassembles to the same text

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

# readwasm's output assembles again and prints the same text.
function(expect_reassembles text)
  file(WRITE ${BIN}/reassembled.wat "${text}")
  run(${AS} reassembled.wat -o reassembled.wasm)
  run(${READWASM} reassembled.wasm)
  if(NOT run_output STREQUAL text)
    message(FATAL_ERROR "reassembled output differs:\n${run_output}")
  endif()
endfunction()

function(expect_no_names text)
  string(FIND "${text}" "$" found)
  if(NOT found EQUAL -1)
    message(FATAL_ERROR "names left after --strip-names:\n${text}")
  endif()
endfunction()

function(link_inputs)
  run(${AS} ${SRC}/link_a.wat -o link_a.wasm)
  run(${AS} ${SRC}/link_b.wat -o link_b.wasm)
endfunction()

if(CASE STREQUAL "as_readwasm")
  run(${AS} ${SRC}/names.wat -o names.wasm)
  run(${READWASM} names.wasm)
  expect_file_text("${run_output}" ${SRC}/names.expected.wat)
  expect_reassembles("${run_output}")
elseif(CASE STREQUAL "ld_readwasm")
  link_inputs()
  # b is linked first: the import it names is defined by a, and the
  # function type and the env import they share take b's names.
  run(${LD} --start all linked.wasm link_b.wasm link_a.wasm)
  run(${READWASM} linked.wasm)
  expect_file_text("${run_output}" ${SRC}/link.expected.wat)
  expect_reassembles("${run_output}")
elseif(CASE STREQUAL "as_strip")
  run(${AS} --strip-names ${SRC}/names.wat -o stripped.wasm)
  run(${READWASM} stripped.wasm)
  expect_no_names("${run_output}")
elseif(CASE STREQUAL "ld_strip")
  link_inputs()
  run(${LD} --strip-names --start all stripped.wasm link_b.wasm link_a.wasm)
  run(${READWASM} stripped.wasm)
  expect_no_names("${run_output}")
elseif(CASE STREQUAL "dump_reassembles")
  run(${AS} ${SRC}/${FIXTURE} -o dump.wasm)
  run(${READWASM} --all dump.wasm)
  expect_reassembles("${run_output}")
else()
  message(FATAL_ERROR "unknown CASE '${CASE}'")
endif()
