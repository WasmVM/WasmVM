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
    int return_v = linear_m[loc];
    return_v += (linear_m[loc+1] << 8);
    return_v += (linear_m[loc+2] << 16);
    return_v += (linear_m[loc+3] << 24);
    // return the value
    return return_v;
}

double Module::i64_load(int loc){
    double return_v = linear_m[loc];
    return_v += (linear_m[loc+1] << 8);
    return_v += (linear_m[loc+2] << 16);
    return_v += (linear_m[loc+3] << 24);
    return_v += (linear_m[loc+4] << 32);
    return_v += (linear_m[loc+5] << 40);
    return_v += (linear_m[loc+6] << 48);
    return_v += (linear_m[loc+7] << 54);
    // return the value
    return return_v;
}

int Module::i32_store(int value){
    linear_m[this.current_memory()] = value && 255; 
    linear_m[this.current_memory() + 1] = (value>>8) && 255;
    linear_m[this.current_memory() + 2] = (value>>16) && 255;
    linear_m[this.current_memory() + 3] = (value>>24) && 255;
    this.current_loc += 4;
}

int Module::i64_store(double value){
    linear_m[this.current_memory()] = value && 255; 
    linear_m[this.current_memory() + 1] = (value>>8) && 255;
    linear_m[this.current_memory() + 2] = (value>>16) && 255;
    linear_m[this.current_memory() + 3] = (value>>24) && 255;
    linear_m[this.current_memory() + 4] = (value>>32) && 255; 
    linear_m[this.current_memory() + 5] = (value>>40) && 255;
    linear_m[this.current_memory() + 6] = (value>>48) && 255;
    linear_m[this.current_memory() + 7] = (value>>56) && 255;
    this.current_loc += 8;
}