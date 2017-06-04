#include "Module.h"

Module::Module(int init_page_size){
    // memory 
    this->linear_m = new std::vector<char>(init_page_size*64*1024);
    // attributes
    this->page_counter = 1;
    this->current_loc = 0;
}

int Module::grow_memory(int page_size){
    this->linear_m->resize((page_size+this->page_counter)*64*1024);
    this->page_counter += page_size;
    return 0;
}

int Module::current_memory(){
    return (this->page_counter-1)*64*1024 + current_loc;
}

int Module::i32_load(int loc){
    int return_v = linear_m->at(loc);
    return_v += ((int)linear_m->at(loc+1) << 8);
    return_v += ((int)linear_m->at(loc+1) << 16);
    return_v += ((int)linear_m->at(loc+1) << 24);
    // return the value
    return return_v;
}

uint64_t Module::i64_load(int loc){
    uint64_t return_v = linear_m->at(loc);
    return_v += ((uint64_t)linear_m->at(loc+1) << 8);
    return_v += ((uint64_t)linear_m->at(loc+2) << 16);
    return_v += ((uint64_t)linear_m->at(loc+3) << 24);
    return_v += ((uint64_t)linear_m->at(loc+4) << 32);
    return_v += ((uint64_t)linear_m->at(loc+5) << 40);
    return_v += ((uint64_t)linear_m->at(loc+6) << 48);
    return_v += ((uint64_t)linear_m->at(loc+7) << 54);
    // return the value
    return return_v;
}

int Module::i32_store(int value){
    try{
        if(this->current_memory() + 4 >= (this->page_counter*64*1024)){
            // Memory shortage
            throw -1;
        }
    }catch(const flag){
        // Notify the user, need to grow memory
        if(flag == -1){
            std::cout << "Linear Memory is starving... Using `grow_memory` to grow the size of memory." << std::endl;
        }
        return -1;
    }
    // if memory storage is enough, then we store
    linear_m->at(this->current_memory()) = value && 255; 
    linear_m->at(this->current_memory() + 1) = (value>>8) && 255;
    linear_m->at(this->current_memory() + 2) = (value>>16) && 255;
    linear_m->at(this->current_memory() + 3) = (value>>24) && 255;
    this->current_loc += 4;
    return 0;
}

int Module::i64_store(uint64_t value){
    try{
        if(this->current_memory() + 8 >= (this->page_counter*64*1024)){
            // Memory shortage
            throw -1;
        }
    }catch(const flag){
        // Notify the user, need to grow memory
        if(flag == -1){
            std::cout << "Linear Memory is starving... Using `grow_memory` to grow the size of memory." << std::endl;
        }
        return -1;
    }
    linear_m->at(this->current_memory()) = value && 255; 
    linear_m->at(this->current_memory() + 1) = (value>>8) && 255;
    linear_m->at(this->current_memory() + 2) = (value>>16) && 255;
    linear_m->at(this->current_memory() + 3) = (value>>24) && 255;
    linear_m->at(this->current_memory() + 4) = (value>>32) && 255;
    linear_m->at(this->current_memory() + 5) = (value>>40) && 255;
    linear_m->at(this->current_memory() + 6) = (value>>48) && 255;
    linear_m->at(this->current_memory() + 7) = (value>>54) && 255;
    this->current_loc += 8;
    return 0;
}