# On Windows the loader looks for a program's DLLs in the program's own
# directory (and PATH), never in the build tree's src/lib or host. A program
# that cannot find a DLL does not exit: it blocks on a "DLL not found" dialog
# until it is killed, so a test reads as a timeout instead of a failure.
#
# wasmvm_copy_dlls(<target> <dll_target>...) copies each DLL target's file next
# to <target> after it is built. It does nothing on other platforms.
function(wasmvm_copy_dlls target)
  if(NOT WIN32)
    return()
  endif()
  foreach(dll ${ARGN})
    add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${dll}> $<TARGET_FILE_DIR:${target}>
    )
  endforeach()
endfunction()
