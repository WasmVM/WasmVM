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
        section_detail();
        return 1;
    }
}

int Memory::section_detail(){
    // Type 
    if(get_section_loc(1) != 0){
        int offset = 2;
        num_types = i32_load8_u(get_section_loc(1)+(offset++));
        type_elements.resize(num_types);
        if(num_types >= 1){
            for(int i = 0;i<num_types;i++){
                type_elements[i].function_form = i32_load8_u(get_section_loc(1)+(offset++));
                type_elements[i].num_param = i32_load8_u(get_section_loc(1)+(offset++));
                type_elements[i].param_type.resize(type_elements[i].num_param);
                // store param type into vector!
                for(int j=0;j<type_elements[i].num_param;j++){
                    type_elements[i].param_type[j] = i32_load8_u(get_section_loc(1)+(offset++));
                }
                // get result number 
                type_elements[i].num_result = i32_load8_u(get_section_loc(1)+(offset++));
                type_elements[i].result_type.resize(type_elements[i].num_result);
                // store result type into vector!
                for(int j=0;j<type_elements[i].num_result;j++){
                    type_elements[i].result_type[j] = i32_load8_u(get_section_loc(1)+(offset++));
                }
            }
        }
    }
    // Import 
    if(get_section_loc(2) != 0){
        int offset = 2;
        num_imports = i32_load8_u(get_section_loc(2)+(offset++));
        import_elements.resize(num_imports);
        if(num_imports >= 1){
            for(int i=0;i<num_imports;i++){
                import_elements[i].module_len = i32_load8_u(get_section_loc(2)+(offset++));
                import_elements[i].module_name = string("");
                // get the module name
                for(int j=0;j<import_elements[i].module_len;j++){
                    import_elements[i].module_name += string(1,(char)i32_load8_u(get_section_loc(2)+(offset++)));
                }
                import_elements[i].function_len = i32_load8_u(get_section_loc(2)+(offset++));
                import_elements[i].function_name = string("");
                // get the function name
                for(int j=0;j<import_elements[i].function_len;j++){
                    import_elements[i].function_name += string(1,(char)i32_load8_u(get_section_loc(2)+(offset++)));
                }
                import_elements[i].kind = i32_load8_u(get_section_loc(2)+(offset++));
                import_elements[i].signature_index = i32_load8_u(get_section_loc(2)+(offset++));
            }
        }
    }
    // Function
    if(get_section_loc(3) != 0){
        int offset = 2;
        num_funcs = i32_load8_u(get_section_loc(3)+(offset++));
        funcs_elements.resize(num_funcs);
        if(num_funcs >= 1){
            for(int i=0;i<num_funcs;i++){
                funcs_elements[i].func_signature_index = i32_load8_u(get_section_loc(3)+(offset++));
            }
        }
    }
    // Table 
    if(get_section_loc(4) != 0){
        int offset = 2;
        num_tables = i32_load8_u(get_section_loc(4)+(offset++));
        table_elements.resize(num_tables);
        if(num_tables >= 1){
            for(int i=0;i<num_tables;i++){
                table_elements[i].table_type = i32_load8_u(get_section_loc(4)+(offset++));
                table_elements[i].table_limit_flag = i32_load8_u(get_section_loc(4)+(offset++));
                table_elements[i].table_limit_init = i32_load8_u(get_section_loc(4)+(offset++));
                table_elements[i].table_limit_max = i32_load8_u(get_section_loc(4)+(offset++));
            }
        }
    }
    // Memory
    if(get_section_loc(5) != 0){
        int offset = 2;
        num_memories = i32_load8_u(get_section_loc(5)+(offset++));
        memory_elements.resize(num_memories);
        if(num_memories >= 1){
            for(int i=0;i<num_memories;i++){
                memory_elements[i].mem_limit_flag = i32_load8_u(get_section_loc(5)+(offset++));
                memory_elements[i].mem_limit_init = i32_load8_u(get_section_loc(5)+(offset++));
                memory_elements[i].mem_limit_max = i32_load8_u(get_section_loc(5)+(offset++));
            }
        }
    }
    // Global 
    if(get_section_loc(6) != 0){
        // TODO
    }
    // Export 
    if(get_section_loc(7) != 0){
        int offset = 2;
        num_exports = i32_load8_u(get_section_loc(7)+(offset++));
        export_elements.resize(num_exports);
        if(num_exports >= 1){
            for(int i=0;i<num_exports;i++){
                export_elements[i].str_len = i32_load8_u(get_section_loc(7)+(offset++));
                for(int j=0;j<export_elements[i].str_len;j++){
                    export_elements[i].export_name += string(1,(char)i32_load8_u(get_section_loc(7)+(offset++)));
                }
                export_elements[i].kind = i32_load8_u(get_section_loc(7)+(offset++));
                export_elements[i].func_index = i32_load8_u(get_section_loc(7)+(offset++));
            }
        }
        
    }
    // Start
    if(get_section_loc(8) != 0){
        int offset = 2;
        start_entry = i32_load8_u(get_section_loc(8)+(offset++));
    }
    // Element 
    if(get_section_loc(9) != 0){
        // TODO
    }
    // Code 
    if(get_section_loc(10) != 0){
        int offset = 2;
        num_codes = i32_load8_u(get_section_loc(10)+(offset++));
        code_elements.resize(num_codes);
        if(num_codes >= 1){
            for(int i=0;i<num_codes;i++){
                code_elements[i].func_mem_loc = get_section_loc(10)+(offset++);
                code_elements[i].func_body_size = i32_load8_u(code_elements[i].func_mem_loc);
                offset += code_elements[i].func_body_size;
            }
        }
    }
    // Data
    if(get_section_loc(11) != 0){
        int offset = 2;
        num_datas = i32_load8_u(get_section_loc(11)+(offset++));
        data_elements.resize(num_datas);
        if(num_datas >= 1){
            for(int i=0;i<num_datas;i++){
                data_elements[i].memory_index = i32_load8_u(get_section_loc(11)+(offset++));
                data_elements[i].data_instr_type = i32_load8_u(get_section_loc(11)+(offset++));
                data_elements[i].data_instr_init = i32_load8_u(get_section_loc(11)+(offset++));
                offset++; // Skip end instruction
                data_elements[i].data_segment_size = i32_load8_u(get_section_loc(11)+(offset++));
                data_elements[i].data.resize(data_elements[i].data_segment_size);
                for(int j=0;j<data_elements[i].data_segment_size;j++){
                    data_elements[i].data[j] = (char)i32_load8_u(get_section_loc(11)+(offset++));
                }
            }
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
            switch(i){
                case 1:
                    // Print out the type section
                    cout << left << setw(6) << " " << "Type Section Detail:" << endl;
                    cout << left << setw(6) << " " << "Number of Types: " << setw(6) << memory.num_types << endl;
                    for(int j=0;j<memory.num_types;j++){
                        cout << left << setw(10) << " " << "Function Form: " << setw(6) << memory.type_elements[j].function_form << endl;
                        cout << left << setw(10) << " " << "Number of Parameters: " << setw(6) << memory.type_elements[j].num_param << endl;
                        for(int k=0;k<memory.type_elements[j].num_param;k++){
                            cout << left << setw(14) << " " << "Type of Parameter: " << setw(6) << memory.type_elements[j].param_type[k] << endl;
                        }
                        cout << left << setw(10) << " " << "Number of Results: " << setw(6) << memory.type_elements[j].num_result << endl;
                        for(int k=0;k<memory.type_elements[j].num_result;k++){
                            cout << left << setw(14) << " " << "Type of Result: " << setw(6) << memory.type_elements[j].result_type[k] << endl;
                        }
                    }
                    cout << setfill('-') << setw(36) << "-" << setfill(' ') << endl;
                    break;
                case 2:
                    // Print out the import section
                    cout << left << setw(6) << " " << "Import Section Detail:" << endl;
                    cout << left << setw(6) << " " << "Number of Import Modules: " << setw(6) << memory.num_imports << endl;
                    for(int j=0;j<memory.num_imports;j++){
                        cout << left << setw(10) << " " << "Imported Module Name: " << setw(6) << memory.import_elements[j].module_name << endl;
                        cout << left << setw(14) << " " << "Imported Function Name: " << setw(6) << memory.import_elements[j].function_name << endl;
                        cout << left << setw(14) << " " << "Imported Function Kind: " << setw(6) << memory.import_elements[j].kind << endl;
                        cout << left << setw(14) << " " << "Imported Function Signature Index: " << setw(6) << memory.import_elements[j].signature_index << endl;
                    }
                    cout << setfill('-') << setw(36) << "-" << setfill(' ') << endl;
                    break;
                case 3:
                    // Print out the function section
                    cout << left << setw(6) << " " << "Function Section Detail:" << endl;
                    cout << left << setw(6) << " " << "Number of Function Signature index: " << setw(6) << memory.num_funcs << endl;
                    for(int j=0;j<memory.num_funcs;j++){
                        cout << left << setw(14) << " " << "Function Signature Index: " << setw(6) << memory.funcs_elements[j].func_signature_index << endl;
                    }
                    cout << setfill('-') << setw(36) << "-" << setfill(' ') << endl;
                    break;
                case 4:
                    // Print out the table section 
                    cout << left << setw(6) << " " << "Table Section Detail:" << endl;
                    cout << left << setw(6) << " " << "Number of Table Type: " << setw(6) << memory.num_tables << endl;
                    for(int j=0;j<memory.num_tables;j++){
                        cout << left << setw(14) << " " << "Type: " << setw(6) << memory.table_elements[j].table_type << endl;
                        cout << left << setw(14) << " " << "Limit Flag: " << setw(6) << memory.table_elements[j].table_limit_flag << endl;
                        cout << left << setw(14) << " " << "Limit Initial: " << setw(6) << memory.table_elements[j].table_limit_init << endl;
                        cout << left << setw(14) << " " << "Limit Maximum: " << setw(6) << memory.table_elements[j].table_limit_max << endl;
                    }
                    cout << setfill('-') << setw(36) << "-" << setfill(' ') << endl;
                    break;
                case 5:
                    // Print out the memory section
                    cout << left << setw(6) << " " << "Memory Section Detail:" << endl;
                    cout << left << setw(6) << " " << "Number of Memory: " << setw(6) << memory.num_memories << endl;
                    for(int j=0;j<memory.num_memories;j++){
                        cout << left << setw(14) << " " << "Limit Flag: " << setw(6) << memory.memory_elements[j].mem_limit_flag << endl;
                        cout << left << setw(14) << " " << "Limit Initial: " << setw(6) << memory.memory_elements[j].mem_limit_init << endl;
                        cout << left << setw(14) << " " << "Limit Maximum: " << setw(6) << memory.memory_elements[j].mem_limit_max << endl;
                    }
                    cout << setfill('-') << setw(36) << "-" << setfill(' ') << endl;
                    break;
                case 6:
                    // Print out the global section 
                    cout << left << setw(6) << " " << "Not Support yet!" << endl;
                    cout << setfill('-') << setw(36) << "-" << setfill(' ') << endl;
                    break;
                case 7: 
                    // Print out the export section 
                    cout << left << setw(6) << " " << "Export Section Detail:" << endl;
                    cout << left << setw(6) << " " << "Number of Exports: " << setw(6) << memory.num_exports << endl;
                    for(int j=0;j<memory.num_exports;j++){
                        cout << left << setw(14) << " " << "Exported Name: " << setw(6) << memory.export_elements[j].export_name << endl;
                        cout << left << setw(14) << " " << "Exported func kind: " << setw(6) << memory.export_elements[j].kind << endl;
                        cout << left << setw(14) << " " << "Exported func index: " << setw(6) << memory.export_elements[j].func_index << endl;
                    }
                    cout << setfill('-') << setw(36) << "-" << setfill(' ') << endl;
                    break;
                case 8: 
                    // Print out the start section
                    cout << left << setw(6) << " " << "Start Section Detail:" << endl;
                    cout << left << setw(6) << " " << "Entry of start: " << setw(6) << memory.start_entry << endl;
                    cout << setfill('-') << setw(36) << "-" << setfill(' ') << endl;
                    break;
                case 9:
                    // Print out the element section 
                    cout << left << setw(6) << " " << "Not Support yet!" << endl;
                    cout << setfill('-') << setw(36) << "-" << setfill(' ') << endl;
                    break;
                case 10:
                    // Print out the code section
                    cout << left << setw(6) << " " << "Code Section Detail:" << endl;
                    cout << left << setw(6) << " " << "Number of Code: " << setw(6) << memory.num_codes << endl;
                    for(int j=0;j<memory.num_codes;j++){
                        cout << left << setw(14) << " " << "Function memory location: " << setw(6) << memory.code_elements[j].func_mem_loc << endl;
                        cout << left << setw(14) << " " << "Function body size: " << setw(6) << memory.code_elements[j].func_body_size << endl;
                    }
                    cout << setfill('-') << setw(36) << "-" << setfill(' ') << endl;
                    break;
                case 11:
                    // Print out the data section
                    cout << left << setw(6) << " " << "Data Section Detail:" << endl;
                    cout << left << setw(6) << " " << "Number of Data: " << setw(6) << memory.num_datas << endl;
                    for(int j=0;j<memory.num_datas;j++){
                        cout << left << setw(14) << " " << "Memory Index: " << setw(6) << memory.data_elements[j].memory_index << endl;
                        cout << left << setw(14) << " " << "Data Segment Size: " << setw(6) << memory.data_elements[j].data_segment_size << endl;
                        for(int k=0;k<memory.data_elements[j].data_segment_size;k++){
                            cout << left << setw(14) << " " << "Initialized Data: " << setw(6) << memory.data_elements[j].data[k] << endl;
                        }
                    }
                    cout << setfill('-') << setw(36) << "-" << setfill(' ') << endl;
                    break;
                default:
                    break;
            }
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