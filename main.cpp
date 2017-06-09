#include <cstdlib>

#include "Memory.h"
#include "Loader.h"
#include "Core.h"

int main(int argc,char *argv[]){

    // TODO: getopt
    
    // Main memory
    Memory memory;

    // Load program
    Loader::load_file(argv[1], memory);

    // Core
    Core core(memory);
    core.run();

    return 0;
}