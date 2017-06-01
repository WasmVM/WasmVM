#ifndef MODULE
#define MODULE

#include <iostream>
#include <vector>

class Module{
public:
    // constructor
    Module(int init_page_size = 1); 
    // memory operator
    int grow_memory(int page_size = 1); // grow memory (unit: page)
    int current_memory(); 
    // load & store => FIXME: support every instruction 
    int i32_load(int loc);      // fetch data which start from loc
    double i64_load(int loc);   
    int i32_store(int value);   // store value into linear memory
    int i64_store(double value);
private:
    std::vector<char> *linear_m; // Each module deploy one linear memory
    int page_counter;   // record how many page (64KB) usage 
    int current_loc;    // record latest(current) elements location (in this vector)
};

#endif