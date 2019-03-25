include_directories(${PROJECT_ROOT}/src/lib/stages/decoder)

macro(add_decoder_unittest unittest)
    add_unittest(${unittest} lib/decoder/)
endmacro(add_decoder_unittest unittest)

add_decoder_unittest(decode_magic_version_unittest)
add_decoder_unittest(decode_type_unittest)