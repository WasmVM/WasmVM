#include "Module.h"

Module::Module(int init_page_size){
    // memory 
    this->linear_m = new vector<char>(init_page_size*64*1024);
    // attributes
    this.page_counter = 1;
    this.current_loc = 0;
}

int Module::grow_memory(int page_size){
    this->linear_m->resize((page_size+this.page_counter)*64*1024);
    return 0;
}

int Module::current_memory(){
    return (this.page_counter-1)*64*1024 + current_loc;
}

int Module::i32_load(int loc){
    
}

double Module::i64_load(int loc){
    
}

int Module::i32_store(int value){
    
}

int Module::i64_store(double value){
    
}