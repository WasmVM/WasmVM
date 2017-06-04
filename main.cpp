#include <cstdlib>
#include "Module.h"
#include "commonTypes.h"
//#include "OperandStack.h"

int main(int argc,char *argv[]){
    Module *wa = new Module();
    // store value
    if(wa->i32_store(atoi(argv[1]))){
        return 0;
    }
    // Test the current_memory
    std::cout << wa->current_memory() << std::endl;
    // Test the value by location
    std::cout << wa->i32_load8_u(0) << std::endl;
    std::cout << wa->i32_load8_u(1) << std::endl;
    std::cout << wa->i32_load8_u(2) << std::endl;
    std::cout << wa->i32_load8_u(3) << std::endl;
    // Test the value by location (4 bytes)
    std::cout << wa->i32_load(0) << std::endl;
    return 0;
}