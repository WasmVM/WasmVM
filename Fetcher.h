#ifndef FETCHER
#define FETCHER 

#include "Module.h"

class Fetcher{
public:
    Fetcher(Module *wa);
    void show_section();
private:
    std::string get_section_name(int section_id);
    Module *parsing_wa;
    // loc
    int section_loc[12];
    // size
    int section_size[12];
};

#endif