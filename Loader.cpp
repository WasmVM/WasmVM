#include "Loader.h"

// get file size
uint32_t Loader::file_size(const char *filename){
    struct stat results;
    if(stat(filename,&results) == 0){
        return results.st_size;
    }
    else{
        cout << "Error when using clib to stat a file" << endl;
    }
}

// load this file into our current webassembly
void Loader::load_file(const char *filename, Memory &memory){
    // Get reading file size
    int filesize = file_size(filename);
    // Input file stream 
    ifstream infile;
    infile.open(filename, ios::in | ios::binary);
    // one byte read
    char byte;
    // reading 
    for(int32_t index = 0; index < filesize; ++index){
        infile.read(&byte,1);
        memory.i32_store8(byte, index);
    }
    // Initialize Memory
    memory.section_init();
}