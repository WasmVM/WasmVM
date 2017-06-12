#include "Memory.h"

Memory::Memory(int init_page_size){
    // memory 
    linear_m = new vector<char>(init_page_size*64*1024);
    // attributes
    page_counter = 1;
    current_loc = 0;
}

Memory::Memory(Memory *memory){
    // Copy another memory
    linear_m = memory->linear_m;
    page_counter = memory->page_counter;
    current_loc = memory->current_loc;
}

int Memory::grow_memory(int page_size){
    linear_m->resize((page_size + page_counter)*64*1024);
    page_counter += page_size;
    return 0;
}

int Memory::current_memory(){
    return (page_counter-1)*64*1024 + current_loc;
}

uint32_t Memory::i32_load(int loc){
    uint32_t return_v = (uint8_t)linear_m->at(loc);
    return_v += (uint32_t)((uint8_t)linear_m->at(loc+1) << 8);
    return_v += (uint32_t)((uint8_t)linear_m->at(loc+2) << 16);
    return_v += (uint32_t)((uint8_t)linear_m->at(loc+3) << 24);

    // return the value
    return return_v;
}

uint32_t Memory::i32_load8_u(int loc){
    // load 1 byte and set 0 to other
    uint32_t return_v = (uint8_t)linear_m->at(loc);
    // return the value
    return return_v;
}

uint32_t Memory::i32_load16_u(int loc){
    // load 2 byte and set 0 to other
    uint32_t return_v = (uint8_t)linear_m->at(loc);
    return_v += (uint32_t)((uint8_t)linear_m->at(loc+1) << 8);
    // return the value
    return return_v;
}

uint64_t Memory::i64_load(int loc){
    uint64_t return_v = (uint8_t)linear_m->at(loc);
    return_v += (uint64_t)((uint8_t)linear_m->at(loc+1) << 8);
    return_v += (uint64_t)((uint8_t)linear_m->at(loc+2) << 16);
    return_v += (uint64_t)((uint8_t)linear_m->at(loc+3) << 24);
    return_v += ((uint64_t)(uint8_t)linear_m->at(loc+4) << 32);
    return_v += ((uint64_t)(uint8_t)linear_m->at(loc+5) << 40);
    return_v += ((uint64_t)(uint8_t)linear_m->at(loc+6) << 48);
    return_v += ((uint64_t)(uint8_t)linear_m->at(loc+7) << 54);
    // return the value
    return return_v;
}

uint64_t Memory::i64_load8_u(int loc){
    // load 1 byte and set 0 to other
    uint64_t return_v = (uint8_t)linear_m->at(loc);
    // return the value
    return return_v;
}

uint64_t Memory::i64_load16_u(int loc){
    // load 1 byte and set 0 to other
    uint64_t return_v = (uint8_t)linear_m->at(loc);
    return_v += (uint64_t)((uint8_t)linear_m->at(loc+1) << 8);
    // return the value
    return return_v;
}

uint64_t Memory::i64_load32_u(int loc){
    // load 1 byte and set 0 to other
    uint64_t return_v = (uint8_t)linear_m->at(loc);
    return_v += (uint64_t)((uint8_t)linear_m->at(loc+1) << 8);
    return_v += (uint64_t)((uint8_t)linear_m->at(loc+2) << 16);
    return_v += ((uint64_t)(uint8_t)linear_m->at(loc+3) << 32);
    // return the value
    return return_v;
}

int Memory::i32_store(uint32_t value){
    try{
        if(current_memory() + 4 >= (page_counter*64*1024)){
            // Memory shortage
            throw -1;
        }
    }catch(const uint8_t flag){
        // Notify the user, need to grow memory
        if(flag == -1){
            cout << "Linear Memory is starving... Using `grow_memory` to grow the size of memory." << endl;
        }
        return -1;
    }
    // if memory storage is enough, then we store
    linear_m->at(current_memory()) = (uint8_t)value & 255; 
    linear_m->at(current_memory() + 1) = (uint8_t)(value>>8) & 255;
    linear_m->at(current_memory() + 2) = (uint8_t)(value>>16) & 255;
    linear_m->at(current_memory() + 3) = (uint8_t)(value>>24) & 255;
    current_loc += 4;
    return 0;
}

int Memory::i32_store8(uint8_t value){
    try{
        if(current_memory() + 1 >= (page_counter*64*1024)){
            // Memory shortage
            throw -1;
        }
    }catch(const uint8_t flag){
        // Notify the user, need to grow memory
        if(flag == -1){
            cout << "Linear Memory is starving... Using `grow_memory` to grow the size of memory." << endl;
        }
        return -1;
    }
    // if memory storage is enough, then we store
    linear_m->at(current_memory()) = value & 255; 
    current_loc++;
    return 0;
}

int Memory::i32_store16(uint16_t value){
    try{
        if(current_memory() + 2 >= (page_counter*64*1024)){
            // Memory shortage
            throw -1;
        }
    }catch(const uint8_t flag){
        // Notify the user, need to grow memory
        if(flag == -1){
            cout << "Linear Memory is starving... Using `grow_memory` to grow the size of memory." << endl;
        }
        return -1;
    }
    // if memory storage is enough, then we store
    linear_m->at(current_memory()) = value & 255; 
    linear_m->at(current_memory() + 1) = (value >> 8) & 255; 
    current_loc += 2;
    return 0;
}

int Memory::i64_store(uint64_t value){
    try{
        if(current_memory() + 8 >= (page_counter*64*1024)){
            // Memory shortage
            throw -1;
        }
    }catch(const uint8_t flag){
        // Notify the user, need to grow memory
        if(flag == -1){
            cout << "Linear Memory is starving... Using `grow_memory` to grow the size of memory." << endl;
        }
        return -1;
    }
    linear_m->at(current_memory()) = value & 255; 
    linear_m->at(current_memory() + 1) = (value>>8) & 255;
    linear_m->at(current_memory() + 2) = (value>>16) & 255;
    linear_m->at(current_memory() + 3) = (value>>24) & 255;
    linear_m->at(current_memory() + 4) = (value>>32) & 255;
    linear_m->at(current_memory() + 5) = (value>>40) & 255;
    linear_m->at(current_memory() + 6) = (value>>48) & 255;
    linear_m->at(current_memory() + 7) = (value>>54) & 255;
    current_loc += 8;
    return 0;
}

int Memory::section_init(){
    if(current_memory() < 8){
        // has no memory
        return -1;
    }
    else{
        section_loc[0] = -1;
        section_size[0] = -1;
        for(int i=1;i<=11;i++){
            section_loc[i] = 0;
            section_size[i] = 0;
        }
        // start Parsing (except the previous 8 elements)
        for(int i=8;i<current_memory();){
            // parsing start!
            if(i32_load8_u(i) > 11 || i32_load8_u(i) <= 0){
                // error occur!
                return -1;
            }
            // Find location
            section_loc[i32_load8_u(i)] = i;
            // Find the size of this section
            section_size[i32_load8_u(i)] = i32_load8_u(i+1);
            // Skip 2 (section tag & section size) and sizeof section 
            i+=(section_size[i32_load8_u(i)]+2);    
        }
        // FIXME: Add each section denote

        return 1;
    }
}

int Memory::section_detail(){
    // Type 
    num_types = i32_load8_u(get_section_loc(1)+2);
    
    if(num_types >= 1){
        for(int i = 0;i<num_types;i++){

        }
    }
}

void Memory::dump(Memory &memory){
    cout << left << setw(36) << "Linear Memory: " << endl;
    cout << left << setw(16) << "Index" << setw(4) << "|" << setw(16) << "Value" << endl;
    cout << setfill('-') << setw(36) << "-" << setfill(' ') << endl; // setfill to - and then set back to blank
    for(int index = 0; index < memory.current_memory(); index++){
        cout << left << setw(16) << setbase(16) << index << setw(4) << "|" << setw(16) << setbase(10) << memory.i32_load8_u(index) << endl;
    }
    cout << endl;
}

void Memory::show_section(Memory &memory){
    // section init 
    if(memory.section_init() == -1){
        return;
    }
    // Output the fetching result of current Module.
    cout << left << setw(14) << "Section Name" << setw(2) << "|" << setw(10) << "Location" << setw(4) << "|" << setw(10) << "Size" << endl; 
    cout << setfill('-') << setw(40) << "-" << setfill(' ') << endl; // setfill to - and then set back to blank    
    for(int i=1;i<=11;i++){
        if(memory.get_section_loc(i) != -1){
            cout << left << setw(14) << memory.get_section_name(i) << setw(2) << "|" << setw(10) << setbase(16) << memory.get_section_loc(i) << setw(4) << "|" << setw(10) << setbase(10) << memory.get_section_size(i) << endl;
        }
    }
    cout << setfill('-') << setw(40) << "-" << setfill(' ') << endl; // setfill to - and then set back to blank    
    cout << endl;
}

string Memory::get_section_name(int section_id){
    switch(section_id){
        case 1:
            return string("Type");
            break;
        case 2:
            return string("Import");
            break;
        case 3:
            return string("Function");
            break;
        case 4:
            return string("Table");
            break;
        case 5:
            return string("Memory");
            break;
        case 6:
            return string("Global");
            break;
        case 7:
            return string("Export");
            break;
        case 8:
            return string("Start");
            break;
        case 9:
            return string("Element");
            break;
        case 10:
            return string("Code");
            break;
        case 11:
            return string("Data");
            break;
        default:
            return string("Not found");
            break;
    }
}

int Memory::get_section_loc(int section_index){
    if(section_index >= 1 && section_index <= 11){
        return section_loc[section_index];
    }
    else{
        return -1;
    }
}

int Memory::get_section_size(int section_index){
    if(section_index >= 1 && section_index <= 11){
        return section_size[section_index];
    }
    else{
        return -1;
    }
}