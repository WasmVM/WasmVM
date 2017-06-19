#include <cstdio>
#define TEST

#include "../OperandStack.h"
#include "../NumericInst.h"

int main(int argc, char const *argv[])
{
  printf("Numeric Instruction:\n");
  
  // inn_const
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    // Instrction 32
    Instruction::i32_const(my_stack, 65);
    // Check
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 65){
      if(val32.type == i32){
        printf("[OK] i32_const()\n");
      }else{
        printf("[Error] i32_const(): Wrong type\n");
      }
    }else{
      printf("[Error] i32_const(): Wrong value\n");
    }
    // Instrction 64
    Instruction::i64_const(my_stack, 65);
    // Check
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 65){
      if(val64.type == i64){
        printf("[OK] i64_const()\n");
      }else{
        printf("[Error] i64_const(): Wrong type\n");
      }
    }else{
      printf("[Error] i64_const(): Wrong value\n");
    }
  }

  // inn_eqz
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value zero;
    zero.data.i32 = 0;
    zero.type = i32;
    my_stack.push(zero);
    // Instrction 32
    Instruction::i32_eqz(my_stack);
    // Check
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 1){
      if(val32.type == i32){
        printf("[OK] i32_eqz(0)\n");
      }else{
        printf("[Error] i32_eqz(0): Wrong type\n");
      }
    }else{
      printf("[Error] i32_eqz(0): Wrong value\n");
    }
    Instruction::i32_eqz(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 0){
      if(val32.type == i32){
        printf("[OK] i32_eqz(1)\n");
      }else{
        printf("[Error] i32_eqz(1): Wrong type\n");
      }
    }else{
      printf("[Error] i32_eqz(1): Wrong value\n");
    }
    // Instrction 64
    my_stack.pop();
    zero.data.i64 = 0;
    zero.type = i64;
    my_stack.push(zero);
    Instruction::i64_eqz(my_stack);
    // Check
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 1){
      if(val64.type == i64){
        printf("[OK] i64_eqz(0)\n");
      }else{
        printf("[Error] i64_eqz(0): Wrong type\n");
      }
    }else{
      printf("[Error] i64_eqz(0): Wrong value\n");
    }
    Instruction::i64_eqz(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 0){
      if(val64.type == i64){
        printf("[OK] i64_eqz(1)\n");
      }else{
        printf("[Error] i64_eqz(1): Wrong type\n");
      }
    }else{
      printf("[Error] i64_eqz(1): Wrong value\n");
    }
  }

  // inn_eq
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    val1.data.i32 = 23;
    val1.type = i32;
    val2.data.i32 = 23;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    // Check
    Instruction::i32_eq(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 1){
      if(val32.type == i32){
        printf("[OK] i32_eq(23,23)\n");
      }else{
        printf("[Error] i32_eq(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_eq(23,23): Wrong value\n");
    }
    my_stack.push(val1);
    Instruction::i32_eq(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 0){
      if(val32.type == i32){
        printf("[OK] i32_eq(1,23)\n");
      }else{
        printf("[Error] i32_eq(1,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_eq(1,23): Wrong value\n");
    }
    val1.data.i64 = 23;
    val1.type = i64;
    val2.data.i64 = 23;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_eq(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 1){
      if(val64.type == i64){
        printf("[OK] i64_eq(23,23)\n");
      }else{
        printf("[Error] i64_eq(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_eq(23,23): Wrong value\n");
    }
    my_stack.push(val1);
    Instruction::i64_eq(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 0){
      if(val64.type == i64){
        printf("[OK] i64_eq(1,23)\n");
      }else{
        printf("[Error] i64_eq(1,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_eq(1,23): Wrong value\n");
    }
  }

  // inn_ne
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    val1.data.i32 = 23;
    val1.type = i32;
    val2.data.i32 = 23;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    // Check
    Instruction::i32_ne(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 != 1){
      if(val32.type == i32){
        printf("[OK] i32_ne(23,23)\n");
      }else{
        printf("[Error] i32_ne(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_ne(23,23): Wrong value\n");
    }
    my_stack.push(val1);
    Instruction::i32_ne(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.i32 != 0){
      if(val32.type == i32){
        printf("[OK] i32_ne(1,23)\n");
      }else{
        printf("[Error] i32_ne(1,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_ne(1,23): Wrong value\n");
    }
    val1.data.i64 = 23;
    val1.type = i64;
    val2.data.i64 = 23;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_ne(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 != 1){
      if(val64.type == i64){
        printf("[OK] i64_ne(23,23)\n");
      }else{
        printf("[Error] i64_ne(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_ne(23,23): Wrong value\n");
    }
    my_stack.push(val1);
    Instruction::i64_ne(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.i64 != 0){
      if(val64.type == i64){
        printf("[OK] i64_ne(1,23)\n");
      }else{
        printf("[Error] i64_ne(1,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_ne(1,23): Wrong value\n");
    }
  }

  // inn_lt_s
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.i32 = 24;
    val1.type = i32;
    val2.data.i32 = 23;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_lt_s(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 0){
      if(val32.type == i32){
        printf("[OK] i32_lt_s(24,23)\n");
      }else{
        printf("[Error] i32_lt_s(24,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_lt_s(24,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.i32 = 22;
    val1.type = i32;
    val2.data.i32 = 23;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_lt_s(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 1){
      if(val32.type == i32){
        printf("[OK] i32_lt_s(22,23)\n");
      }else{
        printf("[Error] i32_lt_s(22,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_lt_s(22,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.i32 = 23;
    val1.type = i32;
    val2.data.i32 = 23;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_lt_s(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 0){
      if(val32.type == i32){
        printf("[OK] i32_lt_s(23,23)\n");
      }else{
        printf("[Error] i32_lt_s(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_lt_s(23,23): Wrong value\n");
    }
    // 64
    val1.data.i64 = 24;
    val1.type = i64;
    val2.data.i64 = 23;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_lt_s(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 0){
      if(val64.type == i64){
        printf("[OK] i64_lt_s(24,23)\n");
      }else{
        printf("[Error] i64_lt_s(24,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_lt_s(24,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.i64 = 22;
    val1.type = i64;
    val2.data.i64 = 23;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_lt_s(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 1){
      if(val64.type == i64){
        printf("[OK] i64_lt_s(22,23)\n");
      }else{
        printf("[Error] i64_lt_s(22,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_lt_s(22,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.i64 = 23;
    val1.type = i64;
    val2.data.i64 = 23;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_lt_s(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 0){
      if(val64.type == i64){
        printf("[OK] i64_lt_s(23,23)\n");
      }else{
        printf("[Error] i64_lt_s(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_lt_s(23,23): Wrong value\n");
    }
  }

  // inn_gt_s
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.i32 = 24;
    val1.type = i32;
    val2.data.i32 = 23;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_gt_s(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 1){
      if(val32.type == i32){
        printf("[OK] i32_gt_s(24,23)\n");
      }else{
        printf("[Error] i32_gt_s(24,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_gt_s(24,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.i32 = 22;
    val1.type = i32;
    val2.data.i32 = 23;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_gt_s(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 0){
      if(val32.type == i32){
        printf("[OK] i32_gt_s(22,23)\n");
      }else{
        printf("[Error] i32_gt_s(22,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_gt_s(22,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.i32 = 23;
    val1.type = i32;
    val2.data.i32 = 23;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_gt_s(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 0){
      if(val32.type == i32){
        printf("[OK] i32_gt_s(23,23)\n");
      }else{
        printf("[Error] i32_gt_s(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_gt_s(23,23): Wrong value\n");
    }
    // 64
    val1.data.i64 = 24;
    val1.type = i64;
    val2.data.i64 = 23;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_gt_s(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 1){
      if(val64.type == i64){
        printf("[OK] i64_gt_s(24,23)\n");
      }else{
        printf("[Error] i64_gt_s(24,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_gt_s(24,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.i64 = 22;
    val1.type = i64;
    val2.data.i64 = 23;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_gt_s(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 0){
      if(val64.type == i64){
        printf("[OK] i64_gt_s(22,23)\n");
      }else{
        printf("[Error] i64_gt_s(22,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_gt_s(22,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.i64 = 23;
    val1.type = i64;
    val2.data.i64 = 23;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_gt_s(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 0){
      if(val64.type == i64){
        printf("[OK] i64_gt_s(23,23)\n");
      }else{
        printf("[Error] i64_gt_s(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_lt_s(23,23): Wrong value\n");
    }
  }

  // inn_le_s
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.i32 = 24;
    val1.type = i32;
    val2.data.i32 = 23;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_le_s(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 0){
      if(val32.type == i32){
        printf("[OK] i32_le_s(24,23)\n");
      }else{
        printf("[Error] i32_le_s(24,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_le_s(24,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.i32 = 22;
    val1.type = i32;
    val2.data.i32 = 23;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_le_s(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 1){
      if(val32.type == i32){
        printf("[OK] i32_le_s(22,23)\n");
      }else{
        printf("[Error] i32_le_s(22,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_le_s(22,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.i32 = 23;
    val1.type = i32;
    val2.data.i32 = 23;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_le_s(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 1){
      if(val32.type == i32){
        printf("[OK] i32_le_s(23,23)\n");
      }else{
        printf("[Error] i32_le_s(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_le_s(23,23): Wrong value\n");
    }
    // 64
    val1.data.i64 = 24;
    val1.type = i64;
    val2.data.i64 = 23;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_le_s(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 0){
      if(val64.type == i64){
        printf("[OK] i64_le_s(24,23)\n");
      }else{
        printf("[Error] i64_le_s(24,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_le_s(24,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.i64 = 22;
    val1.type = i64;
    val2.data.i64 = 23;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_le_s(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 1){
      if(val64.type == i64){
        printf("[OK] i64_le_s(22,23)\n");
      }else{
        printf("[Error] i64_le_s(22,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_le_s(22,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.i64 = 23;
    val1.type = i64;
    val2.data.i64 = 23;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_le_s(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 1){
      if(val64.type == i64){
        printf("[OK] i64_le_s(23,23)\n");
      }else{
        printf("[Error] i64_le_s(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_le_s(23,23): Wrong value\n");
    }
  }

  // inn_ge_s
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.i32 = 24;
    val1.type = i32;
    val2.data.i32 = 23;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_ge_s(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 1){
      if(val32.type == i32){
        printf("[OK] i32_ge_s(24,23)\n");
      }else{
        printf("[Error] i32_ge_s(24,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_ge_s(24,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.i32 = 22;
    val1.type = i32;
    val2.data.i32 = 23;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_ge_s(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 0){
      if(val32.type == i32){
        printf("[OK] i32_ge_s(22,23)\n");
      }else{
        printf("[Error] i32_ge_s(22,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_ge_s(22,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.i32 = 23;
    val1.type = i32;
    val2.data.i32 = 23;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_ge_s(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 1){
      if(val32.type == i32){
        printf("[OK] i32_ge_s(23,23)\n");
      }else{
        printf("[Error] i32_ge_s(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_ge_s(23,23): Wrong value\n");
    }
    // 64
    val1.data.i64 = 24;
    val1.type = i64;
    val2.data.i64 = 23;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_ge_s(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 1){
      if(val64.type == i64){
        printf("[OK] i64_ge_s(24,23)\n");
      }else{
        printf("[Error] i64_ge_s(24,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_ge_s(24,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.i64 = 22;
    val1.type = i64;
    val2.data.i64 = 23;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_ge_s(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 0){
      if(val64.type == i64){
        printf("[OK] i64_ge_s(22,23)\n");
      }else{
        printf("[Error] i64_ge_s(22,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_ge_s(22,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.i64 = 23;
    val1.type = i64;
    val2.data.i64 = 23;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_ge_s(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 1){
      if(val64.type == i64){
        printf("[OK] i64_ge_s(23,23)\n");
      }else{
        printf("[Error] i64_ge_s(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_ge_s(23,23): Wrong value\n");
    }
  }
  
  // inn_lt_u
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.u32 = 24;
    val1.type = u32;
    val2.data.u32 = 23;
    val2.type = u32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_lt_u(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.u32 == 0){
      if(val32.type == u32){
        printf("[OK] i32_lt_u(24,23)\n");
      }else{
        printf("[Error] i32_lt_u(24,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_lt_u(24,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.u32 = 22;
    val1.type = u32;
    val2.data.u32 = 23;
    val2.type = u32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_lt_u(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.u32 == 1){
      if(val32.type == u32){
        printf("[OK] i32_lt_u(22,23)\n");
      }else{
        printf("[Error] i32_lt_u(22,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_lt_u(22,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.u32 = 23;
    val1.type = u32;
    val2.data.u32 = 23;
    val2.type = u32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_lt_u(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.u32 == 0){
      if(val32.type == u32){
        printf("[OK] i32_lt_u(23,23)\n");
      }else{
        printf("[Error] i32_lt_u(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_lt_u(23,23): Wrong value\n");
    }
    // 64
    val1.data.u64 = 24;
    val1.type = u64;
    val2.data.u64 = 23;
    val2.type = u64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_lt_u(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.u64 == 0){
      if(val64.type == u64){
        printf("[OK] i64_lt_u(24,23)\n");
      }else{
        printf("[Error] i64_lt_u(24,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_lt_u(24,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.u64 = 22;
    val1.type = u64;
    val2.data.u64 = 23;
    val2.type = u64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_lt_u(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.u64 == 1){
      if(val64.type == u64){
        printf("[OK] i64_lt_u(22,23)\n");
      }else{
        printf("[Error] i64_lt_u(22,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_lt_u(22,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.u64 = 23;
    val1.type = u64;
    val2.data.u64 = 23;
    val2.type = u64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_lt_u(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.u64 == 0){
      if(val64.type == u64){
        printf("[OK] i64_lt_u(23,23)\n");
      }else{
        printf("[Error] i64_lt_u(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_lt_u(23,23): Wrong value\n");
    }
  }

  // inn_gt_u
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.u32 = 24;
    val1.type = u32;
    val2.data.u32 = 23;
    val2.type = u32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_gt_u(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.u32 == 1){
      if(val32.type == u32){
        printf("[OK] i32_gt_u(24,23)\n");
      }else{
        printf("[Error] i32_gt_u(24,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_gt_u(24,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.u32 = 22;
    val1.type = u32;
    val2.data.u32 = 23;
    val2.type = u32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_gt_u(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.u32 == 0){
      if(val32.type == u32){
        printf("[OK] i32_gt_u(22,23)\n");
      }else{
        printf("[Error] i32_gt_u(22,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_gt_u(22,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.u32 = 23;
    val1.type = u32;
    val2.data.u32 = 23;
    val2.type = u32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_gt_u(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.u32 == 0){
      if(val32.type == u32){
        printf("[OK] i32_gt_u(23,23)\n");
      }else{
        printf("[Error] i32_gt_u(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_gt_u(23,23): Wrong value\n");
    }
    // 64
    val1.data.u64 = 24;
    val1.type = u64;
    val2.data.u64 = 23;
    val2.type = u64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_gt_u(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.u64 == 1){
      if(val64.type == u64){
        printf("[OK] i64_gt_u(24,23)\n");
      }else{
        printf("[Error] i64_gt_u(24,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_gt_u(24,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.u64 = 22;
    val1.type = u64;
    val2.data.u64 = 23;
    val2.type = u64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_gt_u(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.u64 == 0){
      if(val64.type == u64){
        printf("[OK] i64_gt_u(22,23)\n");
      }else{
        printf("[Error] i64_gt_u(22,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_gt_u(22,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.u64 = 23;
    val1.type = u64;
    val2.data.u64 = 23;
    val2.type = u64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_gt_u(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.u64 == 0){
      if(val64.type == u64){
        printf("[OK] i64_gt_u(23,23)\n");
      }else{
        printf("[Error] i64_gt_u(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_lt_u(23,23): Wrong value\n");
    }
  }

  // inn_le_u
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.u32 = 24;
    val1.type = u32;
    val2.data.u32 = 23;
    val2.type = u32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_le_u(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.u32 == 0){
      if(val32.type == u32){
        printf("[OK] i32_le_u(24,23)\n");
      }else{
        printf("[Error] i32_le_u(24,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_le_u(24,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.u32 = 22;
    val1.type = u32;
    val2.data.u32 = 23;
    val2.type = u32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_le_u(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.u32 == 1){
      if(val32.type == u32){
        printf("[OK] i32_le_u(22,23)\n");
      }else{
        printf("[Error] i32_le_u(22,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_le_u(22,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.u32 = 23;
    val1.type = u32;
    val2.data.u32 = 23;
    val2.type = u32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_le_u(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.u32 == 1){
      if(val32.type == u32){
        printf("[OK] i32_le_u(23,23)\n");
      }else{
        printf("[Error] i32_le_u(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_le_u(23,23): Wrong value\n");
    }
    // 64
    val1.data.u64 = 24;
    val1.type = u64;
    val2.data.u64 = 23;
    val2.type = u64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_le_u(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.u64 == 0){
      if(val64.type == u64){
        printf("[OK] i64_le_u(24,23)\n");
      }else{
        printf("[Error] i64_le_u(24,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_le_u(24,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.u64 = 22;
    val1.type = u64;
    val2.data.u64 = 23;
    val2.type = u64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_le_u(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.u64 == 1){
      if(val64.type == u64){
        printf("[OK] i64_le_u(22,23)\n");
      }else{
        printf("[Error] i64_le_u(22,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_le_u(22,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.u64 = 23;
    val1.type = u64;
    val2.data.u64 = 23;
    val2.type = u64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_le_u(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.u64 == 1){
      if(val64.type == u64){
        printf("[OK] i64_le_u(23,23)\n");
      }else{
        printf("[Error] i64_le_u(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_le_u(23,23): Wrong value\n");
    }
  }

  // inn_ge_u
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.u32 = 24;
    val1.type = u32;
    val2.data.u32 = 23;
    val2.type = u32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_ge_u(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.u32 == 1){
      if(val32.type == u32){
        printf("[OK] i32_ge_u(24,23)\n");
      }else{
        printf("[Error] i32_ge_u(24,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_ge_u(24,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.u32 = 22;
    val1.type = u32;
    val2.data.u32 = 23;
    val2.type = u32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_ge_u(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.u32 == 0){
      if(val32.type == u32){
        printf("[OK] i32_ge_u(22,23)\n");
      }else{
        printf("[Error] i32_ge_u(22,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_ge_u(22,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.u32 = 23;
    val1.type = u32;
    val2.data.u32 = 23;
    val2.type = u32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_ge_u(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.u32 == 1){
      if(val32.type == u32){
        printf("[OK] i32_ge_u(23,23)\n");
      }else{
        printf("[Error] i32_ge_u(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i32_ge_u(23,23): Wrong value\n");
    }
    // 64
    val1.data.u64 = 24;
    val1.type = u64;
    val2.data.u64 = 23;
    val2.type = u64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_ge_u(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.u64 == 1){
      if(val64.type == u64){
        printf("[OK] i64_ge_u(24,23)\n");
      }else{
        printf("[Error] i64_ge_u(24,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_ge_u(24,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.u64 = 22;
    val1.type = u64;
    val2.data.u64 = 23;
    val2.type = u64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_ge_u(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.u64 == 0){
      if(val64.type == u64){
        printf("[OK] i64_ge_u(22,23)\n");
      }else{
        printf("[Error] i64_ge_u(22,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_ge_u(22,23): Wrong value\n");
    }
    my_stack.pop();
    val1.data.u64 = 23;
    val1.type = u64;
    val2.data.u64 = 23;
    val2.type = u64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_ge_u(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.u64 == 1){
      if(val64.type == u64){
        printf("[OK] i64_ge_u(23,23)\n");
      }else{
        printf("[Error] i64_ge_u(23,23): Wrong type\n");
      }
    }else{
      printf("[Error] i64_ge_u(23,23): Wrong value\n");
    }
  }

  // inn_add
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.i32 = 58;
    val1.type = i32;
    val2.data.i32 = 22;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_add(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 80){
      if(val32.type == i32){
        printf("[OK] i32_add(58,22)\n");
      }else{
        printf("[Error] i32_add(58,22): Wrong type\n");
      }
    }else{
      printf("[Error] i32_add(58,22): Wrong value\n");
    }
    // 64
    val1.data.u64 = 58;
    val1.type = i64;
    val2.data.u64 = 22;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_add(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 80){
      if(val64.type == i64){
        printf("[OK] i64_add(58,22)\n");
      }else{
        printf("[Error] i64_add(58,22): Wrong type\n");
      }
    }else{
      printf("[Error] i64_add(58,22): Wrong value\n");
    }
  }

  // inn_sub
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.i32 = 58;
    val1.type = i32;
    val2.data.i32 = 22;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_sub(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 36){
      if(val32.type == i32){
        printf("[OK] i32_sub(58,22)\n");
      }else{
        printf("[Error] i32_sub(58,22): Wrong type\n");
      }
    }else{
      printf("[Error] i32_sub(58,22): Wrong value\n");
    }
    // 64
    val1.data.u64 = 58;
    val1.type = i64;
    val2.data.u64 = 22;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_sub(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 36){
      if(val64.type == i64){
        printf("[OK] i64_sub(58,22)\n");
      }else{
        printf("[Error] i64_sub(58,22): Wrong type\n");
      }
    }else{
      printf("[Error] i64_sub(58,22): Wrong value\n");
    }
  }

  // inn_mul
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.i32 = 50;
    val1.type = i32;
    val2.data.i32 = 22;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_mul(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 1100){
      if(val32.type == i32){
        printf("[OK] i32_mul(50,22)\n");
      }else{
        printf("[Error] i32_mul(50,22): Wrong type\n");
      }
    }else{
      printf("[Error] i32_mul(58,22): Wrong value\n");
    }
    // 64
    val1.data.u64 = 50;
    val1.type = i64;
    val2.data.u64 = 22;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_mul(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 1100){
      if(val64.type == i64){
        printf("[OK] i64_mul(50,22)\n");
      }else{
        printf("[Error] i64_mul(50,22): Wrong type\n");
      }
    }else{
      printf("[Error] i64_mul(50,22): Wrong value\n");
    }
  }

  // inn_div_u
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.u32 = 50;
    val1.type = u32;
    val2.data.u32 = 25;
    val2.type = u32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_div_u(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.u32 == 2){
      if(val32.type == u32){
        printf("[OK] i32_div_u(50,25)\n");
      }else{
        printf("[Error] i32_div_u(50,25): Wrong type\n");
      }
    }else{
      printf("[Error] i32_div_u(50,25): Wrong value\n");
    }
    // 64
    val1.data.u64 = 50;
    val1.type = u64;
    val2.data.u64 = 22;
    val2.type = u64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_div_u(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.u64 == 2){
      if(val64.type == u64){
        printf("[OK] i64_div_u(50,22)\n");
      }else{
        printf("[Error] i64_div_u(50,22): Wrong type\n");
      }
    }else{
      printf("[Error] i64_div_u(50,22): Wrong value\n");
    }
  }

  // inn_rem_u
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.u32 = 50;
    val1.type = u32;
    val2.data.u32 = 22;
    val2.type = u32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_rem_u(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 6){
      if(val32.type == u32){
        printf("[OK] i32_rem_u(50,22)\n");
      }else{
        printf("[Error] i32_rem_u(50,22): Wrong type\n");
      }
    }else{
      printf("[Error] i32_rem_u(58,22): Wrong value\n");
    }
    // 64
    val1.data.u64 = 50;
    val1.type = u64;
    val2.data.u64 = 22;
    val2.type = u64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_rem_u(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.u64 == 6){
      if(val64.type == u64){
        printf("[OK] i64_rem_u(50,22)\n");
      }else{
        printf("[Error] i64_rem_u(50,22): Wrong type\n");
      }
    }else{
      printf("[Error] i64_rem_u(50,22): Wrong value\n");
    }
  }

  // inn_div_s
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.i32 = -50;
    val1.type = i32;
    val2.data.i32 = 25;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_div_s(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == -2){
      if(val32.type == i32){
        printf("[OK] i32_div_s(-50,25)\n");
      }else{
        printf("[Error] i32_div_s(-50,25): Wrong type\n");
      }
    }else{
      printf("[Error] i32_div_s(-50,25): Wrong value\n");
    }
    // 64
    val1.data.i64 = -50;
    val1.type = i64;
    val2.data.i64 = 22;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_div_s(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == -2){
      if(val64.type == i64){
        printf("[OK] i64_div_s(-50,22)\n");
      }else{
        printf("[Error] i64_div_s(-50,22): Wrong type\n");
      }
    }else{
      printf("[Error] i64_div_s(-50,22): Wrong value\n");
    }
  }

  // inn_rem_s
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.i32 = -52;
    val1.type = i32;
    val2.data.i32 = 25;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_rem_s(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == -2){
      if(val32.type == i32){
        printf("[OK] i32_rem_s(-50,25)\n");
      }else{
        printf("[Error] i32_rem_s(-50,25): Wrong type\n");
      }
    }else{
      printf("[Error] i32_rem_s(-50,25): Wrong value\n");
    }
    // 64
    val1.data.i64 = -52;
    val1.type = i64;
    val2.data.i64 = 25;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_rem_s(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == -2){
      if(val64.type == i64){
        printf("[OK] i64_rem_s(-50,22)\n");
      }else{
        printf("[Error] i64_rem_s(-50,22): Wrong type\n");
      }
    }else{
      printf("[Error] i64_rem_s(-50,22): Wrong value\n");
    }
  }

  // inn_and
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.i32 = -50;
    val1.type = i32;
    val2.data.i32 = 25;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_and(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 8){
      if(val32.type == i32){
        printf("[OK] i32_and(-50,25)\n");
      }else{
        printf("[Error] i32_and(-50,25): Wrong type\n");
      }
    }else{
      printf("[Error] i32_and(-50,25): Wrong value\n");
    }
    // 64
    val1.data.i64 = -50;
    val1.type = i64;
    val2.data.i64 = 25;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_and(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 8){
      if(val64.type == i64){
        printf("[OK] i64_and(-50,22)\n");
      }else{
        printf("[Error] i64_and(-50,22): Wrong type\n");
      }
    }else{
      printf("[Error] i64_and(-50,22): Wrong value\n");
    }
  }

  // inn_or
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.i32 = -50;
    val1.type = i32;
    val2.data.i32 = 25;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_or(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == -33){
      if(val32.type == i32){
        printf("[OK] i32_or(-50,25)\n");
      }else{
        printf("[Error] i32_or(-50,25): Wrong type\n");
      }
    }else{
      printf("[Error] i32_or(-50,25): Wrong value\n");
    }
    // 64
    val1.data.i64 = -50;
    val1.type = i64;
    val2.data.i64 = 25;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_or(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == -33){
      if(val64.type == i64){
        printf("[OK] i64_or(-50,22)\n");
      }else{
        printf("[Error] i64_or(-50,22): Wrong type\n");
      }
    }else{
      printf("[Error] i64_or(-50,22): Wrong value\n");
    }
  }

  // inn_xor
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.i32 = -50;
    val1.type = i32;
    val2.data.i32 = 25;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_xor(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == -41){
      if(val32.type == i32){
        printf("[OK] i32_xor(-50,25)\n");
      }else{
        printf("[Error] i32_xor(-50,25): Wrong type\n");
      }
    }else{
      printf("[Error] i32_xor(-50,25): Wrong value\n");
    }
    // 64
    val1.data.i64 = -50;
    val1.type = i64;
    val2.data.i64 = 25;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_xor(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == -41){
      if(val64.type == i64){
        printf("[OK] i64_xor(-50,22)\n");
      }else{
        printf("[Error] i64_xor(-50,22): Wrong type\n");
      }
    }else{
      printf("[Error] i64_xor(-50,22): Wrong value\n");
    }
  }

  // inn_shl
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.i32 = 18;
    val1.type = i32;
    val2.data.i32 = 4;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_shl(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 288){
      if(val32.type == i32){
        printf("[OK] i32_shl(18,4)\n");
      }else{
        printf("[Error] i32_shl(18,4): Wrong type\n");
      }
    }else{
      printf("[Error] i32_shl(18,4): Wrong value\n");
    }
    // 64
    val1.data.i64 = 18;
    val1.type = i64;
    val2.data.i64 = 4;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_shl(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 288){
      if(val64.type == i64){
        printf("[OK] i64_shl(18,4)\n");
      }else{
        printf("[Error] i64_shl(18,4): Wrong type\n");
      }
    }else{
      printf("[Error] i64_shl(18,4): Wrong value\n");
    }
  }

  // inn_shr_s
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.i32 = -96;
    val1.type = i32;
    val2.data.i32 = 4;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_shr_s(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == -6){
      if(val32.type == i32){
        printf("[OK] i32_shr_s(-96,4)\n");
      }else{
        printf("[Error] i32_shr_s(-96,4): Wrong type\n");
      }
    }else{
      printf("[Error] i32_shr_s(-96,4): Wrong value\n");
    }
    // 64
    val1.data.i64 = -96;
    val1.type = i64;
    val2.data.i64 = 4;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_shr_s(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == -6){
      if(val64.type == i64){
        printf("[OK] i64_shr_s(-96,4)\n");
      }else{
        printf("[Error] i64_shr_s(-96,4): Wrong type\n");
      }
    }else{
      printf("[Error] i64_shr_s(-96,4): Wrong value\n");
    }
  }

  // inn_shr_u
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.u32 = 0xFFFFFFA0;
    val1.type = u32;
    val2.data.u32 = 4;
    val2.type = u32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_shr_u(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.u32 == 0x0FFFFFFA){
      if(val32.type == u32){
        printf("[OK] i32_shr_u(0xFFFFFFA0,4)\n");
      }else{
        printf("[Error] i32_shr_u(0xFFFFFFA0,4): Wrong type\n");
      }
    }else{
      printf("[Error] i32_shr_u(0xFFFFFFA0,4): Wrong value\n");
    }
    // 64
    val1.data.u64 = 0xFFFFFFFFFFFFFFA0;
    val1.type = u64;
    val2.data.u64 = 4;
    val2.type = u64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_shr_u(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.u64 == 0x0FFFFFFFFFFFFFFA){
      if(val64.type == u64){
        printf("[OK] i64_shr_u(0xFFFFFFFFFFFFFFA0,4)\n");
      }else{
        printf("[Error] i64_shr_u(0xFFFFFFFFFFFFFFA0,4): Wrong type\n");
      }
    }else{
      printf("[Error] i64_shr_u(0xFFFFFFFFFFFFFFA0,4): Wrong value\n");
    }
  }

  // inn_clz
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1;
    // Check
    val1.data.i32 = 0x04FFFA0C;
    val1.type = i32;
    my_stack.push(val1);
    Instruction::i32_clz(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 5){
      if(val32.type == i32){
        printf("[OK] i32_clz(0x0FFFFA0C)\n");
      }else{
        printf("[Error] i32_clz(0x0FFFFA0C): Wrong type\n");
      }
    }else{
      printf("[Error] i32_clz(0x0FFFFA0C): Wrong value\n");
    }
    // 64
    val1.data.i64 = 0x04FFFFFF04FFFA0C;
    val1.type = i64;
    my_stack.push(val1);
    Instruction::i64_clz(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.i64 == 5){
      if(val32.type == i64){
        printf("[OK] i64_clz(0x4FFFFFFF04FFFA0C)\n");
      }else{
        printf("[Error] i64_clz(0x4FFFFFFF04FFFA0C): Wrong type\n");
      }
    }else{
      printf("[Error] i64_clz(0x4FFFFFFF04FFFA0C): Wrong value\n");
    }
  }

  // inn_clt
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1;
    // Check
    val1.data.i32 = 0x04FFFA0C;
    val1.type = i32;
    my_stack.push(val1);
    Instruction::i32_ctz(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 2){
      if(val32.type == i32){
        printf("[OK] i32_clt(0x0FFFFA0C)\n");
      }else{
        printf("[Error] i32_clt(0x0FFFFA0C): Wrong type\n");
      }
    }else{
      printf("[Error] i32_clt(0x0FFFFA0C): Wrong value\n");
    }
    // 64
    val1.data.i64 = 0x04FFFFFF04FFFA00;
    val1.type = i64;
    my_stack.push(val1);
    Instruction::i64_ctz(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.i64 == 9){
      if(val32.type == i64){
        printf("[OK] i64_clt(0x4FFFFFFF04FFFA0C)\n");
      }else{
        printf("[Error] i64_clt(0x4FFFFFFF04FFFA0C): Wrong type\n");
      }
    }else{
      printf("[Error] i64_clt(0x4FFFFFFF04FFFA0C): Wrong value\n");
    }
  }

  // inn_popcnt
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1;
    // Check
    val1.data.i32 = 0x04FFFA0C;
    val1.type = i32;
    my_stack.push(val1);
    Instruction::i32_popcnt(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 17){
      if(val32.type == i32){
        printf("[OK] i32_popcnt(0x04FFFA0C)\n");
      }else{
        printf("[Error] i32_popcnt(0x04FFFA0C): Wrong type\n");
      }
    }else{
      printf("[Error] i32_popcnt(0x04FFFA0C): Wrong value\n");
    }
    // 64
    val1.data.i64 = 0x04F000FF04FFFA00;
    val1.type = i64;
    my_stack.push(val1);
    Instruction::i64_popcnt(my_stack);
    val32 = my_stack._stacks.top().top();
    if(val32.data.i64 == 28){
      if(val32.type == i64){
        printf("[OK] i64_popcnt(0x04F000FF04FFFA00)\n");
      }else{
        printf("[Error] i64_popcnt(0x04F000FF04FFFA00): Wrong type\n");
      }
    }else{
      printf("[Error] i64_popcnt(0x04F000FF04FFFA00): Wrong value\n");
    }
  }

  // inn_rotl
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.i32 = 0x04FFFA0C;
    val1.type = i32;
    val2.data.i32 = 12;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_rotl(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 0xFFA0C04F){
      if(val32.type == i32){
        printf("[OK] i32_rotl(0x04FFFA0C)\n");
      }else{
        printf("[Error] i32_rotl(0x04FFFA0C): Wrong type\n");
      }
    }else{
      printf("[Error] i32_rotl(0x04FFFA0C): Wrong value\n");
    }
    // 64
    val1.data.i64 = 0x04FAB0FF04FFFA00;
    val1.type = i64;
    val2.data.i64 = 20;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_rotl(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 0x0FF04FFFA0004FAB){
      if(val64.type == i64){
        printf("[OK] i64_rotl(0x04FAB0FF04FFFA00)\n");
      }else{
        printf("[Error] i64_rotl(0x04FAB0FF04FFFA00): Wrong type\n");
      }
    }else{
      printf("[Error] i64_rotl(0x04FAB0FF04FFFA00): Wrong value\n");
    }
  }

  // inn_rotr
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1, val2;
    // Check
    val1.data.i32 = 0x04FFFA0C;
    val1.type = i32;
    val2.data.i32 = 12;
    val2.type = i32;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i32_rotr(my_stack);
    Value val32 = my_stack._stacks.top().top();
    if(val32.data.i32 == 0xA0C04FFF){
      if(val32.type == i32){
        printf("[OK] i32_rotr(0x04FFFA0C)\n");
      }else{
        printf("[Error] i32_rotr(0x04FFFA0C): Wrong type\n");
      }
    }else{
      printf("[Error] i32_rotr(0x04FFFA0C): Wrong value\n");
    }
    // 64
    val1.data.i64 = 0x04FAB0FF04FFFA00;
    val1.type = i64;
    val2.data.i64 = 20;
    val2.type = i64;
    my_stack.push(val1);
    my_stack.push(val2);
    Instruction::i64_rotr(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 0xFFA0004FAB0FF04F){
      if(val64.type == i64){
        printf("[OK] i64_rotr(0x04FAB0FF04FFFA00)\n");
      }else{
        printf("[Error] i64_rotr(0x04FAB0FF04FFFA00): Wrong type\n");
      }
    }else{
      printf("[Error] i64_rotr(0x04FAB0FF04FFFA00): Wrong value\n");
    }
  }

  // i32_wrap_i64
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1;
    // Check
    val1.data.i64 = 0x04FAB0FF04FFFA00;
    val1.type = i64;
    my_stack.push(val1);
    Instruction::i32_wrap_i64(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i32 == 0x04FFFA00){
      if(val64.type == i32){
        printf("[OK] i32_wrap_i64(0x04FAB0FF04FFFA00)\n");
      }else{
        printf("[Error] i32_wrap_i64(0x04FAB0FF04FFFA00): Wrong type\n");
      }
    }else{
      printf("[Error] i32_wrap_i64(0x04FAB0FF04FFFA00): Wrong value\n");
    }
  }

  // i64_extend_s_i32
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1;
    // Check
    val1.data.i32 = 0xF4FAB0FF;
    val1.type = i32;
    my_stack.push(val1);
    Instruction::i64_extend_s_i32(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 0xFFFFFFFFF4FAB0FF){
      if(val64.type == i64){
        printf("[OK] i64_extend_s_i32(0xF4FAB0FF)\n");
      }else{
        printf("[Error] i64_extend_s_i32(0xF4FAB0FF): Wrong type\n");
      }
    }else{
      printf("[Error] i64_extend_s_i32(0xF4FAB0FF): Wrong value\n");
    }
    val1.data.i32 = 0x74FAB0FF;
    val1.type = i32;
    my_stack.push(val1);
    Instruction::i64_extend_s_i32(my_stack);
    val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 0x0000000074FAB0FF){
      if(val64.type == i64){
        printf("[OK] i64_extend_s_i32(0x74FAB0FF)\n");
      }else{
        printf("[Error] i64_extend_s_i32(0x74FAB0FF): Wrong type\n");
      }
    }else{
      printf("[Error] i64_extend_s_i32(0x74FAB0FF): Wrong value\n");
    }
  }

  // i64_extend_u_i32
  {
    OperandStack my_stack;
    my_stack.append_Stack();
    Value val1;
    // Check
    val1.data.i32 = 0xF4FAB0FF;
    val1.type = i32;
    my_stack.push(val1);
    Instruction::i64_extend_u_i32(my_stack);
    Value val64 = my_stack._stacks.top().top();
    if(val64.data.i64 == 0x00000000F4FAB0FF){
      if(val64.type == i64){
        printf("[OK] i64_extend_u_i32(0xF4FAB0FF)\n");
      }else{
        printf("[Error] i64_extend_u_i32(0xF4FAB0FF): Wrong type\n");
      }
    }else{
      printf("[Error] i64_extend_u_i32(0xF4FAB0FF): Wrong value\n");
    }
  }

  return 0;
}
