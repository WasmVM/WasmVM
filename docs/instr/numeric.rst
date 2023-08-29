Numeric instructions
--------------------

.. cpp:struct:: WasmVM::Instr::I32_const : public Base

    .. cpp:function:: I32_const(i32_t value)

    .. cpp:member:: i32_t value = 0

.. cpp:struct:: WasmVM::Instr::I64_const : public Base

    .. cpp:function:: I64_const(i64_t value)

    .. cpp:member:: i64_t value = 0

.. cpp:struct:: WasmVM::Instr::F32_const : public Base

    .. cpp:function:: F32_const(f32_t value)

    .. cpp:member:: f32_t value = 0

.. cpp:struct:: WasmVM::Instr::F64_const : public Base

    .. cpp:function:: F64_const(f64_t value)

    .. cpp:member:: f64_t value = 0

.. cpp:type:: I32_const = WasmVM::Instr::Atomic<Opcode::I32_const>
.. cpp:enumerator:: Opcode::I32_const = 0x41

.. cpp:type:: I64_const = WasmVM::Instr::Atomic<Opcode::I64_const>
.. cpp:enumerator:: Opcode::I64_const = 0x42

.. cpp:type:: F32_const = WasmVM::Instr::Atomic<Opcode::F32_const>
.. cpp:enumerator:: Opcode::F32_const = 0x43

.. cpp:type:: F64_const = WasmVM::Instr::Atomic<Opcode::F64_const>
.. cpp:enumerator:: Opcode::F64_const = 0x44

.. cpp:type:: I32_eqz = WasmVM::Instr::Atomic<Opcode::I32_eqz>
.. cpp:enumerator:: Opcode::I32_eqz = 0x45

.. cpp:type:: I32_eq = WasmVM::Instr::Atomic<Opcode::I32_eq>
.. cpp:enumerator:: Opcode::I32_eq = 0x46

.. cpp:type:: I32_ne = WasmVM::Instr::Atomic<Opcode::I32_ne>
.. cpp:enumerator:: Opcode::I32_ne = 0x47

.. cpp:type:: I32_lt_s = WasmVM::Instr::Atomic<Opcode::I32_lt_s>
.. cpp:enumerator:: Opcode::I32_lt_s = 0x48

.. cpp:type:: I32_lt_u = WasmVM::Instr::Atomic<Opcode::I32_lt_u>
.. cpp:enumerator:: Opcode::I32_lt_u = 0x49

.. cpp:type:: I32_gt_s = WasmVM::Instr::Atomic<Opcode::I32_gt_s>
.. cpp:enumerator:: Opcode::I32_gt_s = 0x4A

.. cpp:type:: I32_gt_u = WasmVM::Instr::Atomic<Opcode::I32_gt_u>
.. cpp:enumerator:: Opcode::I32_gt_u = 0x4B

.. cpp:type:: I32_le_s = WasmVM::Instr::Atomic<Opcode::I32_le_s>
.. cpp:enumerator:: Opcode::I32_le_s = 0x4C

.. cpp:type:: I32_le_u = WasmVM::Instr::Atomic<Opcode::I32_le_u>
.. cpp:enumerator:: Opcode::I32_le_u = 0x4D

.. cpp:type:: I32_ge_s = WasmVM::Instr::Atomic<Opcode::I32_ge_s>
.. cpp:enumerator:: Opcode::I32_ge_s = 0x4E

.. cpp:type:: I32_ge_u = WasmVM::Instr::Atomic<Opcode::I32_ge_u>
.. cpp:enumerator:: Opcode::I32_ge_u = 0x4F

.. cpp:type:: I64_eqz = WasmVM::Instr::Atomic<Opcode::I64_eqz>
.. cpp:enumerator:: Opcode::I64_eqz = 0x50

.. cpp:type:: I64_eq = WasmVM::Instr::Atomic<Opcode::I64_eq>
.. cpp:enumerator:: Opcode::I64_eq = 0x51

.. cpp:type:: I64_ne = WasmVM::Instr::Atomic<Opcode::I64_ne>
.. cpp:enumerator:: Opcode::I64_ne = 0x52

.. cpp:type:: I64_lt_s = WasmVM::Instr::Atomic<Opcode::I64_lt_s>
.. cpp:enumerator:: Opcode::I64_lt_s = 0x53

.. cpp:type:: I64_lt_u = WasmVM::Instr::Atomic<Opcode::I64_lt_u>
.. cpp:enumerator:: Opcode::I64_lt_u = 0x54

.. cpp:type:: I64_gt_s = WasmVM::Instr::Atomic<Opcode::I64_gt_s>
.. cpp:enumerator:: Opcode::I64_gt_s = 0x55

.. cpp:type:: I64_gt_u = WasmVM::Instr::Atomic<Opcode::I64_gt_u>
.. cpp:enumerator:: Opcode::I64_gt_u = 0x56

.. cpp:type:: I64_le_s = WasmVM::Instr::Atomic<Opcode::I64_le_s>
.. cpp:enumerator:: Opcode::I64_le_s = 0x57

.. cpp:type:: I64_le_u = WasmVM::Instr::Atomic<Opcode::I64_le_u>
.. cpp:enumerator:: Opcode::I64_le_u = 0x58

.. cpp:type:: I64_ge_s = WasmVM::Instr::Atomic<Opcode::I64_ge_s>
.. cpp:enumerator:: Opcode::I64_ge_s = 0x59

.. cpp:type:: I64_ge_u = WasmVM::Instr::Atomic<Opcode::I64_ge_u>
.. cpp:enumerator:: Opcode::I64_ge_u = 0x5A

.. cpp:type:: F32_eq = WasmVM::Instr::Atomic<Opcode::F32_eq>
.. cpp:enumerator:: Opcode::F32_eq = 0x5B

.. cpp:type:: F32_ne = WasmVM::Instr::Atomic<Opcode::F32_ne>
.. cpp:enumerator:: Opcode::F32_ne = 0x5C

.. cpp:type:: F32_lt = WasmVM::Instr::Atomic<Opcode::F32_lt>
.. cpp:enumerator:: Opcode::F32_lt = 0x5D

.. cpp:type:: F32_gt = WasmVM::Instr::Atomic<Opcode::F32_gt>
.. cpp:enumerator:: Opcode::F32_gt = 0x5E

.. cpp:type:: F32_le = WasmVM::Instr::Atomic<Opcode::F32_le>
.. cpp:enumerator:: Opcode::F32_le = 0x5F

.. cpp:type:: F32_ge = WasmVM::Instr::Atomic<Opcode::F32_ge>
.. cpp:enumerator:: Opcode::F32_ge = 0x60

.. cpp:type:: F64_eq = WasmVM::Instr::Atomic<Opcode::F64_eq>
.. cpp:enumerator:: Opcode::F64_eq = 0x61

.. cpp:type:: F64_ne = WasmVM::Instr::Atomic<Opcode::F64_ne>
.. cpp:enumerator:: Opcode::F64_ne = 0x62

.. cpp:type:: F64_lt = WasmVM::Instr::Atomic<Opcode::F64_lt>
.. cpp:enumerator:: Opcode::F64_lt = 0x63

.. cpp:type:: F64_gt = WasmVM::Instr::Atomic<Opcode::F64_gt>
.. cpp:enumerator:: Opcode::F64_gt = 0x64

.. cpp:type:: F64_le = WasmVM::Instr::Atomic<Opcode::F64_le>
.. cpp:enumerator:: Opcode::F64_le = 0x65

.. cpp:type:: F64_ge = WasmVM::Instr::Atomic<Opcode::F64_ge>
.. cpp:enumerator:: Opcode::F64_ge = 0x66

.. cpp:type:: I32_clz = WasmVM::Instr::Atomic<Opcode::I32_clz>
.. cpp:enumerator:: Opcode::I32_clz = 0x67

.. cpp:type:: I32_ctz = WasmVM::Instr::Atomic<Opcode::I32_ctz>
.. cpp:enumerator:: Opcode::I32_ctz = 0x68

.. cpp:type:: I32_popcnt = WasmVM::Instr::Atomic<Opcode::I32_popcnt>
.. cpp:enumerator:: Opcode::I32_popcnt = 0x69

.. cpp:type:: I32_add = WasmVM::Instr::Atomic<Opcode::I32_add>
.. cpp:enumerator:: Opcode::I32_add = 0x6A

.. cpp:type:: I32_sub = WasmVM::Instr::Atomic<Opcode::I32_sub>
.. cpp:enumerator:: Opcode::I32_sub = 0x6B

.. cpp:type:: I32_mul = WasmVM::Instr::Atomic<Opcode::I32_mul>
.. cpp:enumerator:: Opcode::I32_mul = 0x6C

.. cpp:type:: I32_div_s = WasmVM::Instr::Atomic<Opcode::I32_div_s>
.. cpp:enumerator:: Opcode::I32_div_s = 0x6D

.. cpp:type:: I32_div_u = WasmVM::Instr::Atomic<Opcode::I32_div_u>
.. cpp:enumerator:: Opcode::I32_div_u = 0x6E

.. cpp:type:: I32_rem_s = WasmVM::Instr::Atomic<Opcode::I32_rem_s>
.. cpp:enumerator:: Opcode::I32_rem_s = 0x6F

.. cpp:type:: I32_rem_u = WasmVM::Instr::Atomic<Opcode::I32_rem_u>
.. cpp:enumerator:: Opcode::I32_rem_u = 0x70

.. cpp:type:: I32_and = WasmVM::Instr::Atomic<Opcode::I32_and>
.. cpp:enumerator:: Opcode::I32_and = 0x71

.. cpp:type:: I32_or = WasmVM::Instr::Atomic<Opcode::I32_or>
.. cpp:enumerator:: Opcode::I32_or = 0x72

.. cpp:type:: I32_xor = WasmVM::Instr::Atomic<Opcode::I32_xor>
.. cpp:enumerator:: Opcode::I32_xor = 0x73

.. cpp:type:: I32_shl = WasmVM::Instr::Atomic<Opcode::I32_shl>
.. cpp:enumerator:: Opcode::I32_shl = 0x74

.. cpp:type:: I32_shr_s = WasmVM::Instr::Atomic<Opcode::I32_shr_s>
.. cpp:enumerator:: Opcode::I32_shr_s = 0x75

.. cpp:type:: I32_shr_u = WasmVM::Instr::Atomic<Opcode::I32_shr_u>
.. cpp:enumerator:: Opcode::I32_shr_u = 0x76

.. cpp:type:: I32_rotl = WasmVM::Instr::Atomic<Opcode::I32_rotl>
.. cpp:enumerator:: Opcode::I32_rotl = 0x77

.. cpp:type:: I32_rotr = WasmVM::Instr::Atomic<Opcode::I32_rotr>
.. cpp:enumerator:: Opcode::I32_rotr = 0x78

.. cpp:type:: I64_clz = WasmVM::Instr::Atomic<Opcode::I64_clz>
.. cpp:enumerator:: Opcode::I64_clz = 0x79

.. cpp:type:: I64_ctz = WasmVM::Instr::Atomic<Opcode::I64_ctz>
.. cpp:enumerator:: Opcode::I64_ctz = 0x7A

.. cpp:type:: I64_popcnt = WasmVM::Instr::Atomic<Opcode::I64_popcnt>
.. cpp:enumerator:: Opcode::I64_popcnt = 0x7B

.. cpp:type:: I64_add = WasmVM::Instr::Atomic<Opcode::I64_add>
.. cpp:enumerator:: Opcode::I64_add = 0x7C

.. cpp:type:: I64_sub = WasmVM::Instr::Atomic<Opcode::I64_sub>
.. cpp:enumerator:: Opcode::I64_sub = 0x7D

.. cpp:type:: I64_mul = WasmVM::Instr::Atomic<Opcode::I64_mul>
.. cpp:enumerator:: Opcode::I64_mul = 0x7E

.. cpp:type:: I64_div_s = WasmVM::Instr::Atomic<Opcode::I64_div_s>
.. cpp:enumerator:: Opcode::I64_div_s = 0x7F

.. cpp:type:: I64_div_u = WasmVM::Instr::Atomic<Opcode::I64_div_u>
.. cpp:enumerator:: Opcode::I64_div_u = 0x80

.. cpp:type:: I64_rem_s = WasmVM::Instr::Atomic<Opcode::I64_rem_s>
.. cpp:enumerator:: Opcode::I64_rem_s = 0x81

.. cpp:type:: I64_rem_u = WasmVM::Instr::Atomic<Opcode::I64_rem_u>
.. cpp:enumerator:: Opcode::I64_rem_u = 0x82

.. cpp:type:: I64_and = WasmVM::Instr::Atomic<Opcode::I64_and>
.. cpp:enumerator:: Opcode::I64_and = 0x83

.. cpp:type:: I64_or = WasmVM::Instr::Atomic<Opcode::I64_or>
.. cpp:enumerator:: Opcode::I64_or = 0x84

.. cpp:type:: I64_xor = WasmVM::Instr::Atomic<Opcode::I64_xor>
.. cpp:enumerator:: Opcode::I64_xor = 0x85

.. cpp:type:: I64_shl = WasmVM::Instr::Atomic<Opcode::I64_shl>
.. cpp:enumerator:: Opcode::I64_shl = 0x86

.. cpp:type:: I64_shr_s = WasmVM::Instr::Atomic<Opcode::I64_shr_s>
.. cpp:enumerator:: Opcode::I64_shr_s = 0x87

.. cpp:type:: I64_shr_u = WasmVM::Instr::Atomic<Opcode::I64_shr_u>
.. cpp:enumerator:: Opcode::I64_shr_u = 0x88

.. cpp:type:: I64_rotl = WasmVM::Instr::Atomic<Opcode::I64_rotl>
.. cpp:enumerator:: Opcode::I64_rotl = 0x89

.. cpp:type:: I64_rotr = WasmVM::Instr::Atomic<Opcode::I64_rotr>
.. cpp:enumerator:: Opcode::I64_rotr = 0x8A

.. cpp:type:: F32_abs = WasmVM::Instr::Atomic<Opcode::F32_abs>
.. cpp:enumerator:: Opcode::F32_abs = 0x8B

.. cpp:type:: F32_neg = WasmVM::Instr::Atomic<Opcode::F32_neg>
.. cpp:enumerator:: Opcode::F32_neg = 0x8C

.. cpp:type:: F32_ceil = WasmVM::Instr::Atomic<Opcode::F32_ceil>
.. cpp:enumerator:: Opcode::F32_ceil = 0x8D

.. cpp:type:: F32_floor = WasmVM::Instr::Atomic<Opcode::F32_floor>
.. cpp:enumerator:: Opcode::F32_floor = 0x8E

.. cpp:type:: F32_trunc = WasmVM::Instr::Atomic<Opcode::F32_trunc>
.. cpp:enumerator:: Opcode::F32_trunc = 0x8F

.. cpp:type:: F32_nearest = WasmVM::Instr::Atomic<Opcode::F32_nearest>
.. cpp:enumerator:: Opcode::F32_nearest = 0x90

.. cpp:type:: F32_sqrt = WasmVM::Instr::Atomic<Opcode::F32_sqrt>
.. cpp:enumerator:: Opcode::F32_sqrt = 0x91

.. cpp:type:: F32_add = WasmVM::Instr::Atomic<Opcode::F32_add>
.. cpp:enumerator:: Opcode::F32_add = 0x92

.. cpp:type:: F32_sub = WasmVM::Instr::Atomic<Opcode::F32_sub>
.. cpp:enumerator:: Opcode::F32_sub = 0x93

.. cpp:type:: F32_mul = WasmVM::Instr::Atomic<Opcode::F32_mul>
.. cpp:enumerator:: Opcode::F32_mul = 0x94

.. cpp:type:: F32_div = WasmVM::Instr::Atomic<Opcode::F32_div>
.. cpp:enumerator:: Opcode::F32_div = 0x95

.. cpp:type:: F32_min = WasmVM::Instr::Atomic<Opcode::F32_min>
.. cpp:enumerator:: Opcode::F32_min = 0x96

.. cpp:type:: F32_max = WasmVM::Instr::Atomic<Opcode::F32_max>
.. cpp:enumerator:: Opcode::F32_max = 0x97

.. cpp:type:: F32_copysign = WasmVM::Instr::Atomic<Opcode::F32_copysign>
.. cpp:enumerator:: Opcode::F32_copysign = 0x98

.. cpp:type:: F64_abs = WasmVM::Instr::Atomic<Opcode::F64_abs>
.. cpp:enumerator:: Opcode::F64_abs = 0x99

.. cpp:type:: F64_neg = WasmVM::Instr::Atomic<Opcode::F64_neg>
.. cpp:enumerator:: Opcode::F64_neg = 0x9A

.. cpp:type:: F64_ceil = WasmVM::Instr::Atomic<Opcode::F64_ceil>
.. cpp:enumerator:: Opcode::F64_ceil = 0x9B

.. cpp:type:: F64_floor = WasmVM::Instr::Atomic<Opcode::F64_floor>
.. cpp:enumerator:: Opcode::F64_floor = 0x9C

.. cpp:type:: F64_trunc = WasmVM::Instr::Atomic<Opcode::F64_trunc>
.. cpp:enumerator:: Opcode::F64_trunc = 0x9D

.. cpp:type:: F64_nearest = WasmVM::Instr::Atomic<Opcode::F64_nearest>
.. cpp:enumerator:: Opcode::F64_nearest = 0x9E

.. cpp:type:: F64_sqrt = WasmVM::Instr::Atomic<Opcode::F64_sqrt>
.. cpp:enumerator:: Opcode::F64_sqrt = 0x9F

.. cpp:type:: F64_add = WasmVM::Instr::Atomic<Opcode::F64_add>
.. cpp:enumerator:: Opcode::F64_add = 0xA0

.. cpp:type:: F64_sub = WasmVM::Instr::Atomic<Opcode::F64_sub>
.. cpp:enumerator:: Opcode::F64_sub = 0xA1

.. cpp:type:: F64_mul = WasmVM::Instr::Atomic<Opcode::F64_mul>
.. cpp:enumerator:: Opcode::F64_mul = 0xA2

.. cpp:type:: F64_div = WasmVM::Instr::Atomic<Opcode::F64_div>
.. cpp:enumerator:: Opcode::F64_div = 0xA3

.. cpp:type:: F64_min = WasmVM::Instr::Atomic<Opcode::F64_min>
.. cpp:enumerator:: Opcode::F64_min = 0xA4

.. cpp:type:: F64_max = WasmVM::Instr::Atomic<Opcode::F64_max>
.. cpp:enumerator:: Opcode::F64_max = 0xA5

.. cpp:type:: F64_copysign = WasmVM::Instr::Atomic<Opcode::F64_copysign>
.. cpp:enumerator:: Opcode::F64_copysign = 0xA6

.. cpp:type:: I32_wrap_i64 = WasmVM::Instr::Atomic<Opcode::I32_wrap_i64>
.. cpp:enumerator:: Opcode::I32_wrap_i64 = 0xA7

.. cpp:type:: I32_trunc_s_f32 = WasmVM::Instr::Atomic<Opcode::I32_trunc_s_f32>
.. cpp:enumerator:: Opcode::I32_trunc_s_f32 = 0xA8

.. cpp:type:: I32_trunc_u_f32 = WasmVM::Instr::Atomic<Opcode::I32_trunc_u_f32>
.. cpp:enumerator:: Opcode::I32_trunc_u_f32 = 0xA9

.. cpp:type:: I32_trunc_s_f64 = WasmVM::Instr::Atomic<Opcode::I32_trunc_s_f64>
.. cpp:enumerator:: Opcode::I32_trunc_s_f64 = 0xAA

.. cpp:type:: I32_trunc_u_f64 = WasmVM::Instr::Atomic<Opcode::I32_trunc_u_f64>
.. cpp:enumerator:: Opcode::I32_trunc_u_f64 = 0xAB

.. cpp:type:: I64_extend_s_i32 = WasmVM::Instr::Atomic<Opcode::I64_extend_s_i32>
.. cpp:enumerator:: Opcode::I64_extend_s_i32 = 0xAC

.. cpp:type:: I64_extend_u_i32 = WasmVM::Instr::Atomic<Opcode::I64_extend_u_i32>
.. cpp:enumerator:: Opcode::I64_extend_u_i32 = 0xAD

.. cpp:type:: I64_trunc_s_f32 = WasmVM::Instr::Atomic<Opcode::I64_trunc_s_f32>
.. cpp:enumerator:: Opcode::I64_trunc_s_f32 = 0xAE

.. cpp:type:: I64_trunc_u_f32 = WasmVM::Instr::Atomic<Opcode::I64_trunc_u_f32>
.. cpp:enumerator:: Opcode::I64_trunc_u_f32 = 0xAF

.. cpp:type:: I64_trunc_s_f64 = WasmVM::Instr::Atomic<Opcode::I64_trunc_s_f64>
.. cpp:enumerator:: Opcode::I64_trunc_s_f64 = 0xB0

.. cpp:type:: I64_trunc_u_f64 = WasmVM::Instr::Atomic<Opcode::I64_trunc_u_f64>
.. cpp:enumerator:: Opcode::I64_trunc_u_f64 = 0xB1

.. cpp:type:: F32_convert_s_i32 = WasmVM::Instr::Atomic<Opcode::F32_convert_s_i32>
.. cpp:enumerator:: Opcode::F32_convert_s_i32 = 0xB2

.. cpp:type:: F32_convert_u_i32 = WasmVM::Instr::Atomic<Opcode::F32_convert_u_i32>
.. cpp:enumerator:: Opcode::F32_convert_u_i32 = 0xB3

.. cpp:type:: F32_convert_s_i64 = WasmVM::Instr::Atomic<Opcode::F32_convert_s_i64>
.. cpp:enumerator:: Opcode::F32_convert_s_i64 = 0xB4

.. cpp:type:: F32_convert_u_i64 = WasmVM::Instr::Atomic<Opcode::F32_convert_u_i64>
.. cpp:enumerator:: Opcode::F32_convert_u_i64 = 0xB5

.. cpp:type:: F32_demote_f64 = WasmVM::Instr::Atomic<Opcode::F32_demote_f64>
.. cpp:enumerator:: Opcode::F32_demote_f64 = 0xB6

.. cpp:type:: F64_convert_s_i32 = WasmVM::Instr::Atomic<Opcode::F64_convert_s_i32>
.. cpp:enumerator:: Opcode::F64_convert_s_i32 = 0xB7

.. cpp:type:: F64_convert_u_i32 = WasmVM::Instr::Atomic<Opcode::F64_convert_u_i32>
.. cpp:enumerator:: Opcode::F64_convert_u_i32 = 0xB8

.. cpp:type:: F64_convert_s_i64 = WasmVM::Instr::Atomic<Opcode::F64_convert_s_i64>
.. cpp:enumerator:: Opcode::F64_convert_s_i64 = 0xB9

.. cpp:type:: F64_convert_u_i64 = WasmVM::Instr::Atomic<Opcode::F64_convert_u_i64>
.. cpp:enumerator:: Opcode::F64_convert_u_i64 = 0xBA

.. cpp:type:: F64_promote_f32 = WasmVM::Instr::Atomic<Opcode::F64_promote_f32>
.. cpp:enumerator:: Opcode::F64_promote_f32 = 0xBB

.. cpp:type:: I32_reinterpret_f32 = WasmVM::Instr::Atomic<Opcode::I32_reinterpret_f32>
.. cpp:enumerator:: Opcode::I32_reinterpret_f32 = 0xBC

.. cpp:type:: I64_reinterpret_f64 = WasmVM::Instr::Atomic<Opcode::I64_reinterpret_f64>
.. cpp:enumerator:: Opcode::I64_reinterpret_f64 = 0xBD

.. cpp:type:: F32_reinterpret_i32 = WasmVM::Instr::Atomic<Opcode::F32_reinterpret_i32>
.. cpp:enumerator:: Opcode::F32_reinterpret_i32 = 0xBE

.. cpp:type:: F64_reinterpret_i64 = WasmVM::Instr::Atomic<Opcode::F64_reinterpret_i64>
.. cpp:enumerator:: Opcode::F64_reinterpret_i64 = 0xBF

.. cpp:type:: I32_extend8_s = WasmVM::Instr::Atomic<Opcode::I32_extend8_s>
.. cpp:enumerator:: Opcode::I32_extend8_s = 0xC0

.. cpp:type:: I32_extend16_s = WasmVM::Instr::Atomic<Opcode::I32_extend16_s>
.. cpp:enumerator:: Opcode::I32_extend16_s = 0xC1

.. cpp:type:: I64_extend8_s = WasmVM::Instr::Atomic<Opcode::I64_extend8_s>
.. cpp:enumerator:: Opcode::I64_extend8_s = 0xC2

.. cpp:type:: I64_extend16_s = WasmVM::Instr::Atomic<Opcode::I64_extend16_s>
.. cpp:enumerator:: Opcode::I64_extend16_s = 0xC3

.. cpp:type:: I64_extend32_s = WasmVM::Instr::Atomic<Opcode::I64_extend32_s>
.. cpp:enumerator:: Opcode::I64_extend32_s = 0xC4

.. cpp:type:: I32_trunc_sat_f32_s = WasmVM::Instr::Atomic<Opcode::I32_trunc_sat_f32_s>
.. cpp:enumerator:: Opcode::I32_trunc_sat_f32_s = 0xFC00

.. cpp:type:: I32_trunc_sat_f32_u = WasmVM::Instr::Atomic<Opcode::I32_trunc_sat_f32_u>
.. cpp:enumerator:: Opcode::I32_trunc_sat_f32_u = 0xFC01

.. cpp:type:: I32_trunc_sat_f64_s = WasmVM::Instr::Atomic<Opcode::I32_trunc_sat_f64_s>
.. cpp:enumerator:: Opcode::I32_trunc_sat_f64_s = 0xFC02

.. cpp:type:: I32_trunc_sat_f64_u = WasmVM::Instr::Atomic<Opcode::I32_trunc_sat_f64_u>
.. cpp:enumerator:: Opcode::I32_trunc_sat_f64_u = 0xFC03

.. cpp:type:: I64_trunc_sat_f32_s = WasmVM::Instr::Atomic<Opcode::I64_trunc_sat_f32_s>
.. cpp:enumerator:: Opcode::I64_trunc_sat_f32_s = 0xFC04

.. cpp:type:: I64_trunc_sat_f32_u = WasmVM::Instr::Atomic<Opcode::I64_trunc_sat_f32_u>
.. cpp:enumerator:: Opcode::I64_trunc_sat_f32_u = 0xFC05

.. cpp:type:: I64_trunc_sat_f64_s = WasmVM::Instr::Atomic<Opcode::I64_trunc_sat_f64_s>
.. cpp:enumerator:: Opcode::I64_trunc_sat_f64_s = 0xFC06

.. cpp:type:: I64_trunc_sat_f64_u = WasmVM::Instr::Atomic<Opcode::I64_trunc_sat_f64_u>
.. cpp:enumerator:: Opcode::I64_trunc_sat_f64_u = 0xFC07

