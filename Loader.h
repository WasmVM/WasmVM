#ifndef LOADER
#define LOADER

#include <sys/stat.h>
#include <fstream>

#include "commonTypes.h"
#include "Memory.h"

using namespace std;

namespace Loader{

// get file size
uint32_t file_size(const char *filename);

// load file into memory
void load_file(const char *filename, Memory &memory);

};

#endif