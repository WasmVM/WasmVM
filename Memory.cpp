#include "Memory.h"

Memory::Memory(int init_page_size){
    // memory 
    this->linear_m = new std::vector<char>(init_page_size*64*1024);
    // attributes
    this->page_counter = 1;
    this->current_loc = 0;
}

Memory::Memory(Memory *other_wa){
    // Copy another wa 
    this->linear_m = other_wa->linear_m;
    this->page_counter = other_wa->page_counter;
    this->current_loc = other_wa->current_loc;
}

int Memory::grow_memory(int page_size){
    this->linear_m->resize((page_size+this->page_counter)*64*1024);
    this->page_counter += page_size;
    return 0;
}

int Memory::current_memory(){
    return (this->page_counter-1)*64*1024 + this->current_loc;
}

uint32_t Memory::i32_load(int loc){
    uint32_t return_v = (uint8_t)this->linear_m->at(loc);
    return_v += (uint32_t)((uint8_t)this->linear_m->at(loc+1) << 8);
    return_v += (uint32_t)((uint8_t)this->linear_m->at(loc+2) << 16);
    return_v += (uint32_t)((uint8_t)this->linear_m->at(loc+3) << 24);

    // return the value
    return return_v;
}

uint32_t Memory::i32_load8_u(int loc){
    // load 1 byte and set 0 to other
    uint32_t return_v = (uint8_t)this->linear_m->at(loc);
    // return the value
    return return_v;
}

uint32_t Memory::i32_load16_u(int loc){
    // load 2 byte and set 0 to other
    uint32_t return_v = (uint8_t)this->linear_m->at(loc);
    return_v += (uint32_t)((uint8_t)this->linear_m->at(loc+1) << 8);
    // return the value
    return return_v;
}

uint64_t Memory::i64_load(int loc){
    uint64_t return_v = (uint8_t)this->linear_m->at(loc);
    return_v += (uint64_t)((uint8_t)this->linear_m->at(loc+1) << 8);
    return_v += (uint64_t)((uint8_t)this->linear_m->at(loc+2) << 16);
    return_v += (uint64_t)((uint8_t)this->linear_m->at(loc+3) << 24);
    return_v += ((uint64_t)(uint8_t)this->linear_m->at(loc+4) << 32);
    return_v += ((uint64_t)(uint8_t)this->linear_m->at(loc+5) << 40);
    return_v += ((uint64_t)(uint8_t)this->linear_m->at(loc+6) << 48);
    return_v += ((uint64_t)(uint8_t)this->linear_m->at(loc+7) << 54);
    // return the value
    return return_v;
}

uint64_t Memory::i64_load8_u(int loc){
    // load 1 byte and set 0 to other
    uint64_t return_v = (uint8_t)this->linear_m->at(loc);
    // return the value
    return return_v;
}

uint64_t Memory::i64_load16_u(int loc){
    // load 1 byte and set 0 to other
    uint64_t return_v = (uint8_t)this->linear_m->at(loc);
    return_v += (uint64_t)((uint8_t)this->linear_m->at(loc+1) << 8);
    // return the value
    return return_v;
}

uint64_t Memory::i64_load32_u(int loc){
    // load 1 byte and set 0 to other
    uint64_t return_v = (uint8_t)this->linear_m->at(loc);
    return_v += (uint64_t)((uint8_t)this->linear_m->at(loc+1) << 8);
    return_v += (uint64_t)((uint8_t)this->linear_m->at(loc+2) << 16);
    return_v += ((uint64_t)(uint8_t)this->linear_m->at(loc+3) << 32);
    // return the value
    return return_v;
}

int Memory::i32_store(uint32_t value){
    try{
        if(this->current_memory() + 4 >= (this->page_counter*64*1024)){
            // Memory shortage
            throw -1;
        }
    }catch(const uint8_t flag){
        // Notify the user, need to grow memory
        if(flag == -1){
            std::cout << "Linear Memory is starving... Using `grow_memory` to grow the size of memory." << std::endl;
        }
        return -1;
    }
    // if memory storage is enough, then we store
    this->linear_m->at(this->current_memory()) = (uint8_t)value & 255; 
    this->linear_m->at(this->current_memory() + 1) = (uint8_t)(value>>8) & 255;
    this->linear_m->at(this->current_memory() + 2) = (uint8_t)(value>>16) & 255;
    this->linear_m->at(this->current_memory() + 3) = (uint8_t)(value>>24) & 255;
    this->current_loc += 4;
    return 0;
}

int Memory::i32_store8(uint8_t value){
    try{
        if(this->current_memory() + 1 >= (this->page_counter*64*1024)){
            // Memory shortage
            throw -1;
        }
    }catch(const uint8_t flag){
        // Notify the user, need to grow memory
        if(flag == -1){
            std::cout << "Linear Memory is starving... Using `grow_memory` to grow the size of memory." << std::endl;
        }
        return -1;
    }
    // if memory storage is enough, then we store
    this->linear_m->at(this->current_memory()) = value & 255; 
    this->current_loc++;
    return 0;
}

int Memory::i32_store16(uint16_t value){
    try{
        if(this->current_memory() + 2 >= (this->page_counter*64*1024)){
            // Memory shortage
            throw -1;
        }
    }catch(const uint8_t flag){
        // Notify the user, need to grow memory
        if(flag == -1){
            std::cout << "Linear Memory is starving... Using `grow_memory` to grow the size of memory." << std::endl;
        }
        return -1;
    }
    // if memory storage is enough, then we store
    this->linear_m->at(this->current_memory()) = value & 255; 
    this->linear_m->at(this->current_memory() + 1) = (value >> 8) & 255; 
    this->current_loc += 2;
    return 0;
}

int Memory::i64_store(uint64_t value){
    try{
        if(this->current_memory() + 8 >= (this->page_counter*64*1024)){
            // Memory shortage
            throw -1;
        }
    }catch(const uint8_t flag){
        // Notify the user, need to grow memory
        if(flag == -1){
            std::cout << "Linear Memory is starving... Using `grow_memory` to grow the size of memory." << std::endl;
        }
        return -1;
    }
    this->linear_m->at(this->current_memory()) = value & 255; 
    this->linear_m->at(this->current_memory() + 1) = (value>>8) & 255;
    this->linear_m->at(this->current_memory() + 2) = (value>>16) & 255;
    this->linear_m->at(this->current_memory() + 3) = (value>>24) & 255;
    this->linear_m->at(this->current_memory() + 4) = (value>>32) & 255;
    this->linear_m->at(this->current_memory() + 5) = (value>>40) & 255;
    this->linear_m->at(this->current_memory() + 6) = (value>>48) & 255;
    this->linear_m->at(this->current_memory() + 7) = (value>>54) & 255;
    this->current_loc += 8;
    return 0;
}

#ifdef DEBUG
void Memory::mem_map(){
    std::cout << std::left << std::setw(36) << "Linear Memory: " << std::endl;
    std::cout << std::left << std::setw(16) << "Index" << std::setw(4) << "|" << std::setw(16) << "Value" << std::endl;
    std::cout << std::setfill('-') << std::setw(36) << "-" << std::setfill(' ') << std::endl; // setfill to - and then set back to blank
    for(int index = 0; index < this->current_memory(); index++){
        std::cout << std::left << std::setw(16) << std::setbase(16) << index << std::setw(4) << "|" << std::setw(16) << std::setbase(10) << this->i32_load8_u(index) << std::endl;
    }
    std::cout << std::endl;
}
#endif