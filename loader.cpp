#include <sys/stat.h>
#include <fstream>
#include <cstdlib>
#include "Module.h"
#include "commonTypes.h"

struct stat results;

int main(int argc,char *argv[]){
    Module *wa_bin = new Module();
    int filesize = 0;
    // 
    if(stat("Test/addTwo.wasm",&results) == 0){
        std::cout << "Binary file size: " << results.st_size << std::endl;
        filesize = results.st_size;
    }
    else{
        std::cout << "Error when using clib to stat a file" << std::endl;
    }
    // Input file stream 
    std::ifstream infile;
    infile.open("Test/addTwo.wasm", std::ios::in | std::ios::binary);
    char onebytes;
    // reading 
    for(int index = 0; index < filesize; index++){
        infile.read(&onebytes,1);
        wa_bin->i32_store8(onebytes);
    }
    // Print out the linear map
    wa_bin->mem_map();
}