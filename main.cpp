#include <cstdlib>

#include "Memory.h"
#include "Loader.h"

int main(int argc,char *argv[]){

    // TODO: getopt
    
    // Main memory
    Memory memory;

    // Load program
    Loader::load_file(argv[1], memory);

    Memory::dump(memory);

    // TODO: Core

    return 0;
}