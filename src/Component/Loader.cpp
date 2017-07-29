#include <Loader.h>

Loader::Loader(Store &store, std::vector<ModuleInst *> &moduleInsts):
    store(store), moduleInsts(moduleInsts){
}
void Loader::load(const char *fileName){
    std::string moduleName(fileName);
    std::string pathStr = moduleName.substr(0, moduleName.rfind("/") + 1);
    moduleName = moduleName.substr(moduleName.rfind("/") + 1);
    // Never load exist module
    if(modules.find(moduleName) != modules.end()){
        return;
    }
    // Open file
    FILE *fin = fopen(fileName, "rb");
    if(fin == NULL){
        throw LoaderException(std::string("Cannot open file ") + fileName + " : " + strerror(errno));
    }
    // Set buffer & position indicator
    if(fseek(fin, 0, SEEK_END)){
        throw LoaderException(std::string("Cannot seek file ") + fileName);
    }
    long int fileSize = ftell(fin);
    if(fileSize == -1){
        throw LoaderException(std::string("Cannot get file size of ") + fileName + " : " + strerror(errno));
    }
    rewind(fin);
    char fileBuf[fileSize];
    char *cur = fileBuf;
    char *endAddr = &fileBuf[fileSize - 1];
    // Read file & close
    if(fread(fileBuf, 1, fileSize, fin) != fileSize){
        throw LoaderException(std::string("Read file ") + fileName + " error");
    }
    if(fclose(fin)){
        throw LoaderException(std::string("Close file ") + fileName + " error");
    }
    // Check magic
    if(*(uint32_t *)cur != Util::toLittle32(WASM_MAGIC)){
        throw LoaderException(std::string(fileName) + " isn't a WebAssembly file.", true, cur - fileBuf);
    }
    cur += 4;
    // Check version
    if(*(uint32_t *)cur != Util::toLittle32(WASM_VERSION)){
        throw LoaderException(std::string("Wrong version of ") + fileName, true, cur - fileBuf);
    }
    cur += 4;
    // Create Module
    Module *newModule = new Module();
    modules[moduleName] = newModule;
    /** Section 1: Type **/
    if(skipToSection(1, cur, endAddr) == 1){
        std::uint32_t typeNum = Util::get_uleb128_32(cur);
        while(typeNum-- > 0){
            FuncType newType;
            if(*cur != 0x60){
                throw LoaderException(std::string(fileName) + ": First byte of type is not 0x60.", true, cur - fileBuf);
            }
            // Param
            std::uint32_t paramNum = Util::get_uleb128_32(++cur);
            while(paramNum-- > 0){            
                switch(*cur){
                    case 0x7F:
                        newType.paramTypes.push_back(i32);
                    break;
                    case 0x7E:
                        newType.paramTypes.push_back(i64);
                    break;
                    case 0x7D:
                        newType.paramTypes.push_back(f32);
                    break;
                    case 0x7C:
                        newType.paramTypes.push_back(f64);
                    break;
                    default:
                        throw LoaderException(std::string(fileName) + ": No such parameter type.", true, cur - fileBuf);
                }
                cur++;
            }
            // Result
            std::uint32_t resultNum = Util::get_uleb128_32(cur);
            while(resultNum-- > 0){
                switch(*cur){
                    case 0x7F:
                        newType.resultTypes.push_back(i32);
                    break;
                    case 0x7E:
                        newType.resultTypes.push_back(i64);
                    break;
                    case 0x7D:
                        newType.resultTypes.push_back(f32);
                    break;
                    case 0x7C:
                        newType.resultTypes.push_back(f64);
                    break;
                    default:
                        throw LoaderException(std::string(fileName) + ": No such result type.", true, cur - fileBuf);
                }
                cur++;
            }
            newModule->types.push_back(newType);
        }
    }
    /** Section 2: Import **/
    if(skipToSection(2, cur, endAddr) == 2){
        std::uint32_t importNum = Util::get_uleb128_32(cur);
        while(importNum-- > 0){
            Import newImport;
            // Get module name length
            std::uint32_t depNameLen = Util::get_uleb128_32(cur);
            // Get module name
            char depName[depNameLen + 1];
            depName[depNameLen] = '\0';
            strncpy(depName, cur, depNameLen);
            cur += depNameLen;
            newImport.module = depName;
            // Get name length
            std::uint32_t nameLen = Util::get_uleb128_32(cur);
            // Get name
            char name[nameLen + 1];
            name[nameLen] = '\0';
            strncpy(name, cur, nameLen);
            cur += nameLen;
            newImport.name = name;
            // Load dependency
            if(modules.find(depName) == modules.end()){
                load((pathStr + depName).c_str());
            }
            // import kind
            switch(*(cur++)){
                case 0x00:
                    newImport.kind = func;
                    newImport.desc.func = new std::uint32_t;
                    *(newImport.desc.func) = Util::get_uleb128_32(cur);
                break;
                case 0x01:
                    newImport.kind = table;
                    newImport.desc.table = new Limits;
                    cur++; // skip anyfunc
                    newImport.desc.table->flag = *(cur++);
                    newImport.desc.table->min = Util::get_uleb128_32(cur);
                    if(newImport.desc.table->flag){
                        newImport.desc.table->max = Util::get_uleb128_32(cur);
                    }
                break;
                case 0x02:
                    newImport.kind = mem;
                    newImport.desc.mem = new Limits;
                    newImport.desc.mem->flag = *(cur++);
                    newImport.desc.mem->min = Util::get_uleb128_32(cur);
                    if(newImport.desc.mem->flag){
                        newImport.desc.mem->max = Util::get_uleb128_32(cur);
                    }
                break;
                case 0x03:
                    newImport.kind = global;
                    newImport.desc.global = new Global;
                    switch(*(cur++)){
                        case 0x7f:
                            newImport.desc.global->value.type = i32;
                        break;
                        case 0x7e:
                            newImport.desc.global->value.type = i64;
                        break;
                        case 0x7d:
                            newImport.desc.global->value.type = f32;
                        break;
                        case 0x7c:
                            newImport.desc.global->value.type = f64;
                        break;
                        default:
                            throw LoaderException(std::string(fileName) + ": Unknown import global value type.", true, cur - 1 - fileBuf);
                    }
                    newImport.desc.global->mut = *(cur++);
                break;
                default:
                    throw LoaderException(std::string(fileName) + ": Unknown import type.", true, cur - 1 - fileBuf);
            }
        }
    }
    /** Section 3: Function **/
    if(skipToSection(3, cur, endAddr) == 3){
        
    }
    /** Section 4: Table **/
    if(skipToSection(4, cur, endAddr) == 4){
        
    }
    /** Section 5: Memory **/
    if(skipToSection(5, cur, endAddr) == 5){
        
    }
    /** Section 6: Global **/
    if(skipToSection(6, cur, endAddr) == 6){
        
    }
    /** Section 7: Export **/
    if(skipToSection(7, cur, endAddr) == 7){
        
    }
    /** Section 8: Start **/
    if(skipToSection(8, cur, endAddr) == 8){
        
    }
    /** Section 9: Element **/
    if(skipToSection(9, cur, endAddr) == 9){
        
    }
    /** Section 10: Code **/
    if(skipToSection(10, cur, endAddr) == 10){
        
    }
    /** Section 11: Data **/
    if(skipToSection(11, cur, endAddr) == 11){
        
    }
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
char Loader::skipToSection(char sectionNum, char* &cur, const char *endAddr){
    if(cur > endAddr){
        return -1;
    }
    // Check section code
    while(*cur < sectionNum){
        cur++;
        // Get section size
        std::uint32_t size = Util::get_uleb128_32(cur);
        cur += size;
    }
    char ret = *cur;
    Util::get_uleb128_32(++cur);
    return ret;
}