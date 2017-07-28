#include <Loader.h>

Loader::Loader(Store &store, std::vector<ModuleInst *> &moduleInsts):
    store(store), moduleInsts(moduleInsts){
}
void Loader::load(const char *fileName){
    // Open file
    FILE *fin = fopen(fileName, "rb");
    if(fin == NULL){
        throw LoaderException(std::string("Cannot open file ") + fileName + " : " + strerror(errno), false);
    }
    // Set buffer & position indicator
    if(fseek(fin, 0, SEEK_END)){
        throw LoaderException(std::string("Cannot seek file ") + fileName, false);
    }
    long int fileSize = ftell(fin);
    if(fileSize == -1){
        throw LoaderException(std::string("Cannot get file size of ") + fileName + " : " + strerror(errno), false);
    }
    rewind(fin);
    char fileBuf[fileSize];
    char *cur = fileBuf;
    // Read file & close
    if(fread(fileBuf, 1, fileSize, fin) != fileSize){
        throw LoaderException(std::string("Read file ") + fileName + " error", false);
    }
    if(fclose(fin)){
        throw LoaderException(std::string("Close file ") + fileName + " error", false);
    }
    // Check magic
    if(*(uint32_t *)cur != Util::toLittle32(WASM_MAGIC)){
        throw LoaderException(std::string(fileName) + " isn't a WebAssembly file.", false);
    }
    cur += 4;
    // Check version
    if(*(uint32_t *)cur != Util::toLittle32(WASM_VERSION)){
        throw LoaderException(std::string("Wrong version of ") + fileName, false);
    }
    cur += 4;
}
std::uint32_t Loader::allocFunc(){
}
std::uint32_t Loader::allocTable(){
}
std::uint32_t Loader::allocMem(){
}
std::uint32_t Loader::allocGlobal(){
}
ModuleInst * Loader::allocModule(){
}
void Loader::instantiate(){
}