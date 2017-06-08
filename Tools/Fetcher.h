#ifndef FETCHER
#define FETCHER 

#include "../Memory.h"

class Fetcher{
public:
    Fetcher(Memory *wa);
    void show_section();
private:
    std::string get_section_name(int section_id);
    Memory *parsing_wa;
    // loc
    int section_loc[12];
    // size
    int section_size[12];
};

#endif