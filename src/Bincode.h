#ifndef BINCODE_DEF
#define BINCODE_DEF

#define WASM_Magic 0x6D736100 // Little endian
#define WASM_Version 0x01

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

#define OP_Ctrl_unreachable 0x00
#define OP_Ctrl_nop 0x01
#define OP_Ctrl_block 0x02
#define OP_Ctrl_loop 0x03
#define OP_Ctrl_if 0x04
#define OP_Ctrl_else 0x05
#define OP_Ctrl_end 0x0B
#define OP_Ctrl_br 0x0C
#define OP_Ctrl_br_if 0x0D
#define OP_Ctrl_br_table 0x0E
#define OP_Ctrl_return 0x0F
#define OP_Ctrl_call 0x10
#define OP_Ctrl_call_indirect 0x11
#define OP_Drop 0x1A
#define OP_Select 0x1B
#define OP_Get_local 0x20
#define OP_Set_local 0x21
#define OP_Tee_local 0x22
#define OP_Get_global 0x23
#define OP_Set_global 0x24
#define OP_i32_load 0x28
#define OP_i64_load 0x29
#define OP_f32_load 0x2A
#define OP_f64_load 0x2B
#define OP_i32_load8_s 0x2C
#define OP_i32_load8_u 0x2D
#define OP_i32_load16_s 0x2E
#define OP_i32_load16_u 0x2F
#define OP_i64_load8_s 0x30
#define OP_i64_load8_u 0x31
#define OP_i64_load16_s 0x32
#define OP_i64_load16_u 0x33
#define OP_i64_load32_s 0x34
#define OP_i64_load32_u 0x35
#define OP_i32_store 0x36
#define OP_i64_store 0x37
#define OP_f32_store 0x38
#define OP_f64_store 0x39
#define OP_i32_store8 0x3A
#define OP_i32_store16 0x3B
#define OP_i64_store8 0x3C
#define OP_i64_store16 0x3D
#define OP_i64_store32 0x3E
#define OP_Current_memory 0x3F
#define OP_Grow_memory 0x40
#define OP_i32_const 0x41
#define OP_i64_const 0x42
#define OP_f32_const 0x43
#define OP_f64_const 0x44

#endif