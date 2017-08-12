#ifndef BINCODE_DEF
#define BINCODE_DEF

#define WASM_MAGIC 0x6D736100 // Little endian
#define WASM_VERSION 0x01

#define TYPE_i32 0x7F
#define TYPE_i64 0x7E
#define TYPE_f32 0x7D
#define TYPE_f64 0x7C
#define TYPE_Func 0x60
#define TYPE_Table_anyfunc 0x70

#define IMPORT_Func 0x00
#define IMPORT_Table 0x01
#define IMPORT_Mem 0x02
#define IMPORT_Global 0x03

#define OP_CTRL_END 0x0B
#define OP_i32_const 0x41
#define OP_i64_const 0x42
#define OP_f32_const 0x43
#define OP_f64_const 0x44

#endif