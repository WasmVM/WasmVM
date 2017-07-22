#include "Decode.h"

void Decode::decode(Memory &memory, OperandStack &opStack, LocalStack &locals, bool &halted){
  // Get opcode
  uint64_t &pc = locals.get_PC();
  uint32_t opCode = memory.i32_load8_u(pc++);
  // Instructions
  switch (opCode){
  case 0x00:
    Instruction::ctrl_unreachable(opStack, locals, memory);
    break;
  case 0x01:
    Instruction::ctrl_nop();
    break;
  case 0x02:
    Instruction::ctrl_block(opStack, locals, memory.i32_load8_u(pc++));
    break;
  case 0x03:
    Instruction::ctrl_loop(opStack, locals, memory.i32_load8_u(pc++));
    break;
  case 0x04:
    Instruction::ctrl_if(opStack, locals, memory, memory.i32_load8_u(pc++));
    break;
  case 0x05:
    Instruction::ctrl_else(locals);
    break;
  case 0x0B:
    Instruction::ctrl_end(opStack, locals, halted);
    break;
  case 0x0C:
    Instruction::ctrl_br(opStack, locals, memory, Common::get_uleb128_32(memory, pc), halted);
    break;
  case 0x0D:
    Instruction::ctrl_br_if(opStack, locals, memory, Common::get_uleb128_32(memory, pc), halted);
    break;
  case 0x0E:
    Instruction::ctrl_br_table(opStack, locals, memory, halted);
    break;
  case 0x0F:
    Instruction::ctrl_return(opStack, locals, halted);
    break;
  case 0x10:
    Instruction::ctrl_call(Common::get_uleb128_32(memory, pc), opStack, locals, memory);
    break;
  case 0x11:
    Instruction::ctrl_call_indirect(Common::get_uleb128_32(memory, pc), opStack, locals, memory);
    break;
  case 0x1A:
    Instruction::para_drop(opStack);
    break;
  case 0x1B:
    Instruction::para_select(opStack);
    break;
  case 0x20:
    Instruction::get_local(opStack, locals, memory);
    break;
  case 0x21:
    Instruction::set_local(opStack, locals, memory);
    break;
  case 0x22:
    Instruction::tee_local(opStack, locals, memory);
    break;
  case 0x41:
    Instruction::i32_const(opStack, Common::get_leb128_32(memory, pc));
    break;
  case 0x42:
    Instruction::i64_const(opStack, Common::get_leb128_64(memory, pc));
    break;
  case 0x45:
    Instruction::i32_eqz(opStack);
    break;
  case 0x46:
    Instruction::i32_eq(opStack);
    break;
  case 0x47:
    Instruction::i32_ne(opStack);
    break;
  case 0x48:
    Instruction::i32_lt_s(opStack);
    break;
  case 0x49:
    Instruction::i32_lt_u(opStack);
    break;
  case 0x4A:
    Instruction::i32_gt_s(opStack);
    break;
  case 0x4B:
    Instruction::i32_gt_u(opStack);
    break;
  case 0x4C:
    Instruction::i32_le_s(opStack);
    break;
  case 0x4D:
    Instruction::i32_le_u(opStack);
    break;
  case 0x4E:
    Instruction::i32_ge_s(opStack);
    break;
  case 0x4F:
    Instruction::i32_ge_u(opStack);
    break;
  case 0x50:
    Instruction::i32_eqz(opStack);
    break;
  case 0x51:
    Instruction::i32_eq(opStack);
    break;
  case 0x52:
    Instruction::i32_ne(opStack);
    break;
  case 0x53:
    Instruction::i32_lt_s(opStack);
    break;
  case 0x54:
    Instruction::i32_lt_u(opStack);
    break;
  case 0x55:
    Instruction::i32_gt_s(opStack);
    break;
  case 0x56:
    Instruction::i32_gt_u(opStack);
    break;
  case 0x57:
    Instruction::i32_le_s(opStack);
    break;
  case 0x58:
    Instruction::i32_le_u(opStack);
    break;
  case 0x59:
    Instruction::i32_ge_s(opStack);
    break;
  case 0x5A:
    Instruction::i32_ge_u(opStack);
    break;
  case 0x67:
    Instruction::i32_clz(opStack);
    break;
  case 0x68:
    Instruction::i32_ctz(opStack);
    break;
  case 0x69:
    Instruction::i32_popcnt(opStack);
    break;
  case 0x6A:
    Instruction::i32_add(opStack);
    break;
  case 0x6B:
    Instruction::i32_sub(opStack);
    break;
  case 0x6C:
    Instruction::i32_mul(opStack);
    break;
  case 0x6D:
    Instruction::i32_div_s(opStack);
    break;
  case 0x6E:
    Instruction::i32_div_u(opStack);
    break;
  case 0x6F:
    Instruction::i32_rem_s(opStack);
    break;
  case 0x70:
    Instruction::i32_rem_u(opStack);
    break;
  case 0x71:
    Instruction::i32_and(opStack);
    break;
  case 0x72:
    Instruction::i32_or(opStack);
    break;
  case 0x73:
    Instruction::i32_xor(opStack);
    break;
  case 0x74:
    Instruction::i32_shl(opStack);
    break;
  case 0x75:
    Instruction::i32_shr_s(opStack);
    break;
  case 0x76:
    Instruction::i32_shr_u(opStack);
    break;
  case 0x77:
    Instruction::i32_rotl(opStack);
    break;
  case 0x78:
    Instruction::i32_rotr(opStack);
    break;
  case 0x79:
    Instruction::i64_clz(opStack);
    break;
  case 0x7A:
    Instruction::i64_ctz(opStack);
    break;
  case 0x7B:
    Instruction::i64_popcnt(opStack);
    break;
  case 0x7C:
    Instruction::i64_add(opStack);
    break;
  case 0x7D:
    Instruction::i64_sub(opStack);
    break;
  case 0x7E:
    Instruction::i64_mul(opStack);
    break;
  case 0x7F:
    Instruction::i64_div_s(opStack);
    break;
  case 0x80:
    Instruction::i64_div_u(opStack);
    break;
  case 0x81:
    Instruction::i64_rem_s(opStack);
    break;
  case 0x82:
    Instruction::i64_rem_u(opStack);
    break;
  case 0x83:
    Instruction::i64_and(opStack);
    break;
  case 0x84:
    Instruction::i64_or(opStack);
    break;
  case 0x85:
    Instruction::i64_xor(opStack);
    break;
  case 0x86:
    Instruction::i64_shl(opStack);
    break;
  case 0x87:
    Instruction::i64_shr_s(opStack);
    break;
  case 0x88:
    Instruction::i64_shr_u(opStack);
    break;
  case 0x89:
    Instruction::i64_rotl(opStack);
    break;
  case 0x8A:
    Instruction::i64_rotr(opStack);
    break;
  default:
    printf("0x%02X ",opCode);
    throw "Not implement yet";
    break;
  }
}