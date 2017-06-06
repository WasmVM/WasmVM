#ifndef MODULE
#define MODULE

#include <iostream>
#include <vector>       // linear memory usage
#include <string>       // for normal string usage
#include <cstdint>      // for specific type
#include <iomanip>      // std::setw

class Module{
public:
    // constructor
    Module(int init_page_size = 1); 
    Module(Module *other_wa);
    // memory operator
    int grow_memory(int page_size = 1); // grow memory (unit: page)
    int current_memory(); 
    // load & store => FIXME: support every instruction 
    uint32_t i32_load(int loc);      // fetch data which start from loc
    uint32_t i32_load8_u(int loc);   // unsigned (load 1 byte and extend) i8 to i32 
    uint32_t i32_load16_u(int loc);  // unsigned (load 2 byte and extend) i16 to i32
    uint64_t i64_load(int loc);
    uint64_t i64_load8_u(int loc);
    uint64_t i64_load16_u(int loc);
    uint64_t i64_load32_u(int loc);
    
    int i32_store(uint32_t value);  // store value(4 bytes) into linear memory
    int i32_store8(uint8_t value);  // store 1 byte
    int i32_store16(uint16_t value);// store 2 bytes

    int i64_store(uint64_t value);  
    // Print out the Memory Map
    void mem_map();
private:
    std::vector<char> *linear_m; // Each module deploy one linear memory
    int page_counter;   // record how many page (64KB) usage 
    int current_loc;    // record latest(current) elements location (in this vector)
};

#endif