#include "Fetcher.h"

Fetcher::Fetcher(Memory *wa)
    :parsing_wa(wa)
{
    // init elements
    section_loc[0] = -1;
    section_size[0] = -1;
    for(int i=1;i<=11;i++){
        section_loc[i] = 0;
        section_size[i] = 0;
    }
    // start Parsing (except the previous 8 elements)
    for(int i=8;i<=parsing_wa->current_memory();){
        // parsing start!
        if(parsing_wa->i32_load8_u(i) > 11 || parsing_wa->i32_load8_u(i) <= 0){
            // error occur!
            return;
        }
        // Find location
        section_loc[parsing_wa->i32_load8_u(i)] = i;
        // Find the size of this section
        section_size[parsing_wa->i32_load8_u(i)] = parsing_wa->i32_load8_u(i+1);
        // Skip 2 (section tag & section size) and sizeof section 
        i+=(section_size[parsing_wa->i32_load8_u(i)]+2);    
    }
}

void Fetcher::show_section(){
    // Output the fetching result of current Module.
    std::cout << std::left << std::setw(14) << "Section Name" << std::setw(2) << "|" << std::setw(10) << "Location" << std::setw(4) << "|" << std::setw(10) << "Size" << std::endl; 
    std::cout << std::setfill('-') << std::setw(40) << "-" << std::setfill(' ') << std::endl; // setfill to - and then set back to blank    
    for(int i=1;i<=11;i++){
        if(section_loc[i] != -1 && section_loc[i] != 0){
            std::cout << std::left << std::setw(14) << get_section_name(i) << std::setw(2) << "|" << std::setw(10) << std::setbase(16) << section_loc[i] << std::setw(4) << "|" << std::setw(10) << std::setbase(10) << section_size[i] << std::endl;
        }
    }
    std::cout << std::setfill('-') << std::setw(40) << "-" << std::setfill(' ') << std::endl; // setfill to - and then set back to blank    
    std::cout << std::endl;
}

std::string Fetcher::get_section_name(int section_id){
    switch(section_id){
        case 1:
            return std::string("Type");
            break;
        case 2:
            return std::string("Import");
            break;
        case 3:
            return std::string("Function");
            break;
        case 4:
            return std::string("Table");
            break;
        case 5:
            return std::string("Memory");
            break;
        case 6:
            return std::string("Global");
            break;
        case 7:
            return std::string("Export");
            break;
        case 8:
            return std::string("Start");
            break;
        case 9:
            return std::string("Element");
            break;
        case 10:
            return std::string("Code");
            break;
        case 11:
            return std::string("Data");
            break;
        default:
            return std::string("Not found");
            break;
    }
}