#include <Loader.h>

Loader::Loader(Store &store, std::map<std::string, ModuleInst *> &moduleInsts):
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
            newModule->imports.push_back(newImport);
        }
    }
    /** Section 3: Function **/
    if(skipToSection(3, cur, endAddr) == 3){
        std::uint32_t funcNum = Util::get_uleb128_32(cur);
        while(funcNum-- > 0){
            Func newFunc;
            newFunc.typeidx = Util::get_uleb128_32(cur);
            newModule->funcs.push_back(newFunc);
        }
    }
    /** Section 4: Table **/
    if(skipToSection(4, cur, endAddr) == 4){
        cur += 2; // There's only an anyfunc table currently, skip 2 bytes
        newModule->table.flag = *(cur++);
        newModule->table.min = Util::get_uleb128_32(cur);
        if(newModule->table.flag){
            newModule->table.max = Util::get_uleb128_32(cur);
        }
    }
    /** Section 5: Memory **/
    if(skipToSection(5, cur, endAddr) == 5){
        cur += 1; // There's only a mempry currently, skip 1 byte
        newModule->mem.flag = *(cur++);
        newModule->mem.min = Util::get_uleb128_32(cur);
        if(newModule->mem.flag){
            newModule->mem.max = Util::get_uleb128_32(cur);
        }
    }
    /** Section 6: Global **/
    if(skipToSection(6, cur, endAddr) == 6){
        std::uint32_t globalNum = Util::get_uleb128_32(cur);
        while(globalNum-- > 0){
            Global newGlobal;
            // Set value type
            switch(*(cur++)){
                case 0x7f:
                    newGlobal.value.type = i32;
                break;
                case 0x7e:
                    newGlobal.value.type = i64;
                break;
                case 0x7d:
                    newGlobal.value.type = f32;
                break;
                case 0x7c:
                    newGlobal.value.type = f64;
                break;
                default:
                    throw LoaderException(std::string(fileName) + ": Unknown global value type.", true, cur - 1 - fileBuf);
            }
            // Set mut
            newGlobal.mut = *(cur++);
            // Set value
            switch(*(cur++)){
                case 0x41:
                    newGlobal.value.data.i32 = Util::get_leb128_32(cur);
                break;
                case 0x42:
                    newGlobal.value.data.i64 = Util::get_leb128_64(cur);
                break;
                case 0x43:
                    newGlobal.value.data.i32 = Util::toLittle32(*((uint32_t *)cur));
                    cur += 4;
                break;
                case 0x44:
                    newGlobal.value.data.i64 = Util::toLittle64(*((uint64_t *)cur));
                    cur += 8;
                break;
                default:
                    throw LoaderException(std::string(fileName) + ": Global must be initialized with a constant expression.", true, cur - 1 - fileBuf);
            }
            // Skip end
            cur++;
            // Push to module
            newModule->globals.push_back(newGlobal);
        }
    }
    /** Section 7: Export **/
    if(skipToSection(7, cur, endAddr) == 7){
        std::uint32_t exportNum = Util::get_uleb128_32(cur);
        while(exportNum-- > 0){
            Export newExport;
            // Get name length
            std::uint32_t nameLen = Util::get_uleb128_32(cur);
            // Get name
            char name[nameLen + 1];
            name[nameLen] = '\0';
            strncpy(name, cur, nameLen);
            cur += nameLen;
            newExport.name = name;
            // Export kind
            switch(*(cur++)){
                case 0x00:
                    newExport.kind = func;
                break;
                case 0x01:
                    newExport.kind = table;
                break;
                case 0x02:
                    newExport.kind = mem;
                break;
                case 0x03:
                    newExport.kind = global;
                break;
                default:
                    throw LoaderException(std::string(fileName) + ": Unknown export type.", true, cur - 1 - fileBuf);
            }
            newExport.desc = Util::get_uleb128_32(cur);
            newModule->exports.push_back(newExport);
        }
    }
    /** Section 8: Start **/
    if(skipToSection(8, cur, endAddr) == 8){
        newModule->start = new std::uint32_t;
        *(newModule->start) = Util::get_uleb128_32(cur);
    }
    /** Section 9: Element **/
    if(skipToSection(9, cur, endAddr) == 9){
        std::uint32_t elemNum = Util::get_uleb128_32(cur);
        while(elemNum-- > 0){
            Elem newElem;
            // Ignore table index
            Util::get_uleb128_32(cur);
            // Offset
            if((*(cur++)) == 0x41){
                newElem.offset = Util::get_leb128_32(cur);
            }else{
                throw LoaderException(std::string(fileName) + ": Element offset must be an i32.const expression.", true, cur - 1 - fileBuf);
            }
            cur++; // Skip end
            // Init
            std::uint32_t initNum = Util::get_uleb128_32(cur);
            while(initNum-- > 0){
                newElem.init.push_back(Util::get_leb128_32(cur));
            }
            newModule->elem.push_back(newElem);
        }
    }
    /** Section 10: Code **/
    if(skipToSection(10, cur, endAddr) == 10){
        std::uint32_t codeNum = Util::get_uleb128_32(cur);
        if(codeNum != newModule->funcs.size()){
            throw LoaderException(std::string(fileName) + ": Code count does not match function count.", true, cur - fileBuf);
        }
        for(std::uint32_t i = 0; i < codeNum; ++i){
            std::uint32_t codeSize = Util::get_leb128_32(cur);
            char *curAddr = cur;
            // Locals
            std::uint32_t localCount = Util::get_leb128_32(cur);
            while(localCount-- > 0){
                std::uint32_t typeCount = Util::get_leb128_32(cur);
                ValType newType;
                switch(*(cur++)){
                    case 0x7f:
                        newType = i32;
                    break;
                    case 0x7e:
                        newType = i64;
                    break;
                    case 0x7d:
                        newType = f32;
                    break;
                    case 0x7c:
                        newType = f64;
                    break;
                    default:
                        throw LoaderException(std::string(fileName) + ": Unknown local type.", true, cur - 1 - fileBuf);
                }
                while(typeCount-- > 0){
                    newModule->funcs[i].localTypes.push_back(newType);
                }
            }
            // Code
            codeSize -= cur - curAddr;
            newModule->funcs[i].body.assign(cur, cur + codeSize);
            cur += codeSize;
        }
    }
    /** Section 11: Data **/
    if(skipToSection(11, cur, endAddr) == 11){
        std::uint32_t dataNum = Util::get_uleb128_32(cur);
        while(dataNum-- > 0){
            Data newData;
            Util::get_uleb128_32(cur); // Ignore memory index
            // Offset
            if((*(cur++)) == 0x41){
                newData.offset = Util::get_leb128_32(cur);
            }else{
                throw LoaderException(std::string(fileName) + ": Data offset must be an i32.const expression.", true, cur - 1 - fileBuf);
            }
            cur++; // Skip end
            // Init data
            std::uint32_t dataSize = Util::get_uleb128_32(cur);
            newData.init.assign(cur, cur + dataSize);
            cur += dataSize;
            newModule->data.push_back(newData);
        }
    }
    // Get imports
    std::vector<ExternVal> importVals;
    getImportVals(newModule, importVals);
    // Alloc module instance
    moduleInsts[moduleName] = allocModule(*newModule, importVals);
}
std::uint32_t Loader::allocFunc(Func &func, ModuleInst *moduleInst){
    FuncInst *funcInst = new FuncInst;
    funcInst->type = moduleInst->types[func.typeidx];
    funcInst->module = moduleInst;
    funcInst->code = func;
    store.funcs.push_back(funcInst);
    return store.funcs.size() - 1;
}
std::uint32_t Loader::allocTable(Module &module){
    TableInst *tableInst = new TableInst;
    tableInst->elem.resize(module.table.min);
    if(module.table.flag){
        tableInst->max = module.table.max;
    }else{
        tableInst->max = 0;
    }
    store.tables.push_back(tableInst);
    return store.tables.size() - 1;
}
std::uint32_t Loader::allocMem(Module &module){
    MemInst *memInst = new MemInst;
    memInst->data.resize(module.mem.min * 65536);
    if(module.mem.flag){
        memInst->max = module.mem.max * 65536;
    }else{
        memInst->max = 0;
    }
    store.mems.push_back(memInst);
    return store.mems.size() - 1;
}
std::uint32_t Loader::allocGlobal(Global &global){
    GlobalInst *globalInst = new GlobalInst;
    globalInst->val = global.value;
    globalInst->mut = global.mut;
    store.globals.push_back(globalInst);
    return store.globals.size() - 1;
}
ModuleInst * Loader::allocModule(Module &module, std::vector<ExternVal> &importVals){
    ModuleInst *moduleInst = new ModuleInst;
    // Types
    moduleInst->types = module.types;
    // Imports
    for(std::vector<ExternVal>::iterator importIt = importVals.begin(); importIt != importVals.end(); ++importIt){
        switch(importIt->type){
            case func:
                moduleInst->funcaddrs.push_back(importIt->addr);
            break;
            case table:
                moduleInst->tableaddrs.push_back(importIt->addr);
            break;
            case mem:
                moduleInst->memaddrs.push_back(importIt->addr);
            break;
            case global:
                moduleInst->globaladdrs.push_back(importIt->addr);
            break;
            default:
                throw LoaderException("No such import type");
        }
    }
    // Functions
    for(std::vector<Func>::iterator funcIt = module.funcs.begin(); funcIt != module.funcs.end(); ++funcIt){
        moduleInst->funcaddrs.push_back(allocFunc(*funcIt, moduleInst));
    }
    // Tables
    moduleInst->tableaddrs.push_back(allocTable(module));
    // Memory
    moduleInst->memaddrs.push_back(allocMem(module));
    // Globals
    for(std::vector<Global>::iterator globIt = module.globals.begin(); globIt != module.globals.end(); ++globIt){
        moduleInst->globaladdrs.push_back(allocGlobal(*globIt));
    }
    // Exports
    for(std::vector<Export>::iterator expIt = module.exports.begin(); expIt != module.exports.end(); ++expIt){
        ExportInst exportInst;
        exportInst.externval.type = expIt->kind;
        exportInst.name = expIt->name;
        exportInst.externval.addr = expIt->desc;
        moduleInst->exports.push_back(exportInst);
    }
    return moduleInst;
}
void Loader::instantiate(){
}
void Loader::getImportVals(Module *module, std::vector<ExternVal> &externVals){
    for(std::vector<Import>::iterator it = module->imports.begin(); it != module->imports.end(); ++it){
        std::vector<ExportInst> &modExports = moduleInsts[it->module]->exports;
        bool found = false;
        for(std::vector<ExportInst>::iterator expIt = modExports.begin(); expIt != modExports.end(); ++expIt){
            ExternVal exp = expIt->externval;
            if(expIt->name == it->name){
                switch(exp.type){
                    case func:
                        exp.addr = moduleInsts[it->module]->funcaddrs[exp.addr];
                    break;
                    case table:
                        exp.addr = moduleInsts[it->module]->tableaddrs[exp.addr];
                    break;
                    case mem:
                        exp.addr = moduleInsts[it->module]->memaddrs[exp.addr];
                    break;
                    case global:
                        exp.addr = moduleInsts[it->module]->globaladdrs[exp.addr];
                    break;
                    default:
                        throw LoaderException("No such export type");
                }
                externVals.push_back(exp);
                found = true;
                break;
            }
        }
        if(!found){
            throw LoaderException("Can't find export name " + it->name + " in module " + it->module);
        }
    }
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
    if(ret == sectionNum){
        Util::get_uleb128_32(++cur);
    }
    return ret;
}