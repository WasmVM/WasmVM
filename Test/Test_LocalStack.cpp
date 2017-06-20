#include <cstdio>
#define TEST

#include "../LocalStack.h"

int main(int argc, char const *argv[])
{
  LocalStack my_stack;
  printf("LocalStack:\n");

  // Append value
  {
    // Instrction
    my_stack.append_Values();
    // Check
    if(my_stack._localsValues.size() == 1){
      printf("[OK] append_values()\n");
    }else{
      printf("[Error] append_values()\n");
    }
  }

  // Append indices
  {
    // Instrction
    my_stack.append_Indices();
    // Check
    if(my_stack._localIndices.size() == 1){
      printf("[OK] append_Indices()\n");
    }else{
      printf("[Error] append_Indices()\n");
    }
  }

  // Set local
  {
    // Instruction
    Value val;
    val.data.i32 = 9487;
    val.type = i32;
    my_stack.set_local(0, val);
    // Check
    Value ret = my_stack._localsValues.top().at(0);
    if(ret.data.i32 == 9487){
      if(ret.type == i32){
        printf("[OK] set_local()\n");
      }else{
        printf("[Error] set_local(): Wrong type\n");
      }
    }else{
      printf("[Error] set_local(): Wrong value\n");
    }
  }

  // Tee local
  {
    // Instruction
    Value val;
    val.data.i32 = 6767;
    val.type = i32;
    Value ret = my_stack.tee_local(1, val);
    // Check
    Value inStack = my_stack._localsValues.top().at(1);
    if(inStack.data.i32 == 6767){
      if(inStack.type == i32){
        if(ret.data.i32 == 6767){
          if(ret.type == i32){
            printf("[OK] tee_local()\n");
          }else{
            printf("[Error] tee_local(): Wrong type of return\n");
          }
        }else{
          printf("[Error] tee_local(): Wrong value of return\n");
        }
      }else{
        printf("[Error] tee_local(): Wrong type in stack\n");
      }
    }else{
      printf("[Error] tee_local(): Wrong value in stack\n");
    }
  }

  // Get local
  {
    // Instruction
    Value val1 = my_stack.get_local(0);
    Value val2 = my_stack.get_local(1);
    // Check
    if(val1.data.i32 == 9487){
      if(val1.type == i32){
        if(val2.data.i32 == 6767){
          if(val2.type == i32){
            printf("[OK] get_local()\n");
          }else{
            printf("[Error] get_local(): Wrong type of val2\n");
          }
        }else{
          printf("[Error] get_local(): Wrong value of val2\n");
        }
      }else{
        printf("[Error] get_local(): Wrong type of val1\n");
      }
    }else{
      printf("[Error] get_local(): Wrong value of val1\n");
    }
  }

  // Shrink values
  {
    // Instrction
    my_stack.shrink_Values();
    // Check
    if(my_stack._localsValues.size() == 0){
      printf("[OK] shrink_Values()\n");
    }else{
      printf("[Error] shrink_Values()\n");
    }
  }

  // Shrink indices
  {
    // Instrction
    my_stack.shrink_Indices();
    // Check
    if(my_stack._localIndices.size() == 0){
      printf("[OK] shrink_Indices()\n");
    }else{
      printf("[Error] shrink_Indices()\n");
    }
  }
  return 0;
}
