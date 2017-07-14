#ifndef MEMORY
#define MEMORY

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <iomanip>

#include "sectionTypes.h"

using namespace std;

class Memory{
public:

    friend class SystemCall;

    Memory(int init_page_size = 1); 
    Memory(Memory *memory);
    // memory operator
    int grow_memory(int page_size = 1); // grow memory (unit: page)
    int current_memory();
    // load & store => FIXME: support every instruction FIXME: alignment is wrong
    uint32_t i32_load       (int32_t loc, uint32_t offset = 0, uint32_t align = 0); // fetch data which start from loc
    uint32_t i32_load8_u    (int32_t loc, uint32_t offset = 0, uint32_t align = 0); // unsigned (load 1 byte and extend) i8 to i32 
    uint32_t i32_load16_u   (int32_t loc, uint32_t offset = 0, uint32_t align = 0); // unsigned (load 2 byte and extend) i16 to i32
    int32_t i32_load8_s    (int32_t loc, uint32_t offset = 0, uint32_t align = 0); // unsigned (load 1 byte and extend) i8 to i32 
    int32_t i32_load16_s   (int32_t loc, uint32_t offset = 0, uint32_t align = 0); // unsigned (load 2 byte and extend) i16 to i32
    //uint64_t i64_load       (int32_t loc, uint32_t offset = 0, uint32_t align = 0);
    uint64_t i64_load8_u    (int32_t loc, uint32_t offset = 0, uint32_t align = 0);
    //uint64_t i64_load16_u   (int32_t loc, uint32_t offset = 0, uint32_t align = 0);
    //uint64_t i64_load32_u   (int32_t loc, uint32_t offset = 0, uint32_t align = 0);
    //uint64_t i64_load8_s    (int32_t loc, uint32_t offset = 0, uint32_t align = 0);
    //uint64_t i64_load16_s   (int32_t loc, uint32_t offset = 0, uint32_t align = 0);
    //uint64_t i64_load32_s   (int32_t loc, uint32_t offset = 0, uint32_t align = 0);
    float f32_load (int32_t loc, uint32_t offset = 0, uint32_t align = 2);
    double f64_load (int32_t loc, uint32_t offset = 0, uint32_t align = 3);
    
    //int i32_store   (uint32_t value, int32_t loc, uint32_t offset = 0, uint32_t align = 0); // store value(4 bytes) into linear memory
    void i32_store8   (uint8_t  value, int32_t loc, uint32_t offset = 0, uint32_t align = 0); // store 1 byte
    //int i32_store16 (uint16_t value, int32_t loc, uint32_t offset = 0, uint32_t align = 0); // store 2 bytes
    //int i64_store   (uint64_t value, int32_t loc, uint32_t offset = 0, uint32_t align = 0);
    //int i64_store8  (uint8_t  value, int32_t loc, uint32_t offset = 0, uint32_t align = 0);
    //int i64_store16 (uint16_t value, int32_t loc, uint32_t offset = 0, uint32_t align = 0);
    //int i64_store32 (uint32_t value, int32_t loc, uint32_t offset = 0, uint32_t align = 0);

    void f32_store (float value, int32_t loc, uint32_t offset = 0, uint32_t align = 2);
    void f64_store (double value, int32_t loc, uint32_t offset = 0, uint32_t align = 3);
 
    // section 
    int section_init();         // Tag each index space
    int section_detail();       // Tag details loc in each index space (Must go after section_init)
    static string get_section_name(int section_id);
    int get_section_loc(int section_index);
    int get_section_size(int section_index);
    int section_loc[12];
    int section_size[12];

    // Print out the Memory Map
    static void dump(Memory &memory);
    static void show_section(Memory &memory);

    // type section 
    int num_types;
    int num_imports;
    int num_funcs;
    int num_tables;
    int num_memories;
    int num_globals;
    int num_exports;
    int start_entry;
    int num_elements;
    int num_codes;
    int num_datas;
    vector<types> type_elements;
    vector<imports> import_elements;
    vector<functions> funcs_elements;
    vector<tables> table_elements;
    vector<memories> memory_elements;
    vector<globals> global_elements;
    vector<exports> export_elements;
    vector<elements> element_elements;
    vector<codes> code_elements;
    vector<datas> data_elements;
    
private:
    vector<uint8_t> *linear_m; // Each Memory deploy one linear memory
    int page_counter;   // record how many page (64KB) usage 
    int32_t accessed_loc;    // record latest(current) elements location (in this vector)
};

#endif