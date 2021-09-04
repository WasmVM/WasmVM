#ifndef WASMVM_DECODE_SECTIONS_DEF
#define WASMVM_DECODE_SECTIONS_DEF

#include <structures/WasmModule.h>
#include <Opcodes.h>

#define WASM_MAGIC      0x6D736100  // little endian
#define WASM_VERSION    0x01

int skip_custom_section(const byte_t **read_p, const byte_t * const end_p);
int parse_magic_version(const byte_t **read_p, const byte_t * const end_p);
int parse_type_section(WasmModule *module, const byte_t **read_p, const byte_t * const end_p);
int parse_import_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
int parse_func_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
int parse_table_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
int parse_memory_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
int parse_global_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
int parse_export_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
int parse_start_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
int parse_element_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
int parse_code_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
// int parse_data_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p);

// TODO: name section

#endif