#include <Loader.h>

Loader::Loader(Store& store, std::map<std::string, ModuleInst*>& moduleInsts)
    : store(store), moduleInsts(moduleInsts) {}
Loader::~Loader() {
  for (std::map<std::string, Module*>::iterator modIt = modules.begin();
       modIt != modules.end(); ++modIt) {
    delete modIt->second;
  }
}
void Loader::load(const char* fileName) {
  std::string moduleName(fileName);
  std::string pathStr =
      moduleName.substr(0, moduleName.rfind(PATH_SEPARATOR) + 1);
  moduleName = moduleName.substr(moduleName.rfind(PATH_SEPARATOR) + 1);
  // Never load exist module
  if (modules.find(moduleName) != modules.end()) {
    return;
  }
  // Open file
  FILE* fin = fopen(fileName, "rb");
  if (fin == NULL) {
    throw LoaderException(std::string("Cannot open file ") + fileName + " : " +
                          strerror(errno));
  }
  // Set buffer & position indicator
  if (fseek(fin, 0, SEEK_END)) {
    throw LoaderException(std::string("Cannot seek file ") + fileName);
  }
  long int fileSize = ftell(fin);
  if (fileSize == -1) {
    throw LoaderException(std::string("Cannot get file size of ") + fileName +
                          " : " + strerror(errno));
  }
  rewind(fin);
  char fileBuf[fileSize];
  char* cur = fileBuf;
  char* endAddr = &fileBuf[fileSize - 1];
  // Read file & close
  if (fread(fileBuf, 1, fileSize, fin) != (size_t)fileSize) {
    throw LoaderException(std::string("Read file ") + fileName + " error");
  }
  if (fclose(fin)) {
    throw LoaderException(std::string("Close file ") + fileName + " error");
  }
  // Check magic
  if (*(uint32_t*)cur != Util::toLittle32(WASM_Magic)) {
    throw LoaderException(std::string(fileName) + " isn't a WebAssembly file.",
                          true, cur - fileBuf);
  }
  cur += 4;
  // Check version
  if (*(uint32_t*)cur != Util::toLittle32(WASM_Version)) {
    throw LoaderException(std::string("Wrong version of ") + fileName, true,
                          cur - fileBuf);
  }
  cur += 4;
  // Create Module
  Module* newModule = new Module();
  modules[moduleName] = newModule;
  try {
    std::uint32_t importedFuncCount = 0;
    /** Section 1: Type **/
    if (skipToSection(1, cur, endAddr) == 1) {
      std::uint32_t typeNum = Util::getLeb128_u32(cur, endAddr);
      while (typeNum-- > 0) {
        FuncType newType;
        if (*cur != TYPE_Func) {
          throw LoaderException(
              std::string(fileName) +
                  ": Function type must start with function type code.",
              true, cur - fileBuf);
        }
        // Param
        std::uint32_t paramNum = Util::getLeb128_u32(++cur, endAddr);
        while (paramNum-- > 0) {
          switch (*cur) {
            case TYPE_i32:
              newType.paramTypes.push_back(i32);
              break;
            case TYPE_i64:
              newType.paramTypes.push_back(i64);
              break;
            case TYPE_f32:
              newType.paramTypes.push_back(f32);
              break;
            case TYPE_f64:
              newType.paramTypes.push_back(f64);
              break;
            default:
              throw LoaderException(
                  std::string(fileName) + ": No such parameter type.", true,
                  cur - fileBuf);
          }
          cur++;
        }
        // Result
        std::uint32_t resultNum = Util::getLeb128_u32(cur, endAddr);
        if (resultNum > 1) {
          throw LoaderException("Only one or zero result is allowed currently.",
                                true, cur - fileBuf);
        }
        while (resultNum-- > 0) {
          switch (*cur) {
            case TYPE_i32:
              newType.resultTypes.push_back(i32);
              break;
            case TYPE_i64:
              newType.resultTypes.push_back(i64);
              break;
            case TYPE_f32:
              newType.resultTypes.push_back(f32);
              break;
            case TYPE_f64:
              newType.resultTypes.push_back(f64);
              break;
            default:
              throw LoaderException(
                  std::string(fileName) + ": No such result type.", true,
                  cur - fileBuf);
          }
          cur++;
        }
        newModule->types.push_back(newType);
      }
    }
    /** Section 2: Import **/
    if (skipToSection(2, cur, endAddr) == 2) {
      std::uint32_t importNum = Util::getLeb128_u32(cur, endAddr);
      while (importNum-- > 0) {
        Import newImport;
        // Get module name length
        std::uint32_t depNameLen = Util::getLeb128_u32(cur, endAddr);
        // Get module name
        char depName[depNameLen + 1];
        depName[depNameLen] = '\0';
        strncpy(depName, cur, depNameLen);
        cur += depNameLen;
        newImport.module = depName;
        // Get name length
        std::uint32_t nameLen = Util::getLeb128_u32(cur, endAddr);
        // Get name
        char name[nameLen + 1];
        name[nameLen] = '\0';
        strncpy(name, cur, nameLen);
        cur += nameLen;
        newImport.name = name;
        // Load dependency
        if (modules.find(depName) == modules.end()) {
          load((pathStr + depName).c_str());
        }
        // import kind
        switch (*(cur++)) {
          case IMPORT_Func:
            newImport.kind = func;
            newImport.desc.funcType = new std::uint32_t;
            *(newImport.desc.funcType) = Util::getLeb128_u32(cur, endAddr);
            if (*(newImport.desc.funcType) >= newModule->types.size()) {
              throw LoaderException(
                  std::string(fileName) +
                      ": Type index of imported function must be defined.",
                  true, cur - 1 - fileBuf);
            }
            importedFuncCount++;
            break;
          case IMPORT_Table:
            newImport.kind = table;
            newImport.desc.table = new Limits;
            if (*cur++ != 0x70) {
              throw LoaderException(
                  std::string(fileName) +
                      ": Only anyfunc is allowed in table currently.",
                  true, cur - 1 - fileBuf);
            }
            newImport.desc.table->flag = *(cur++);
            newImport.desc.table->min = Util::getLeb128_u32(cur, endAddr);
            if (newImport.desc.table->flag) {
              newImport.desc.table->max = Util::getLeb128_u32(cur, endAddr);
            }
            break;
          case IMPORT_Mem:
            newImport.kind = mem;
            newImport.desc.mem = new Limits;
            newImport.desc.mem->flag = *(cur++);
            newImport.desc.mem->min = Util::getLeb128_u32(cur, endAddr);
            if (newImport.desc.mem->flag) {
              newImport.desc.mem->max = Util::getLeb128_u32(cur, endAddr);
            }
            break;
          case IMPORT_Global:
            newImport.kind = global;
            newImport.desc.global = new Global;
            switch (*(cur++)) {
              case TYPE_i32:
                newImport.desc.global->value.type = i32;
                break;
              case TYPE_i64:
                newImport.desc.global->value.type = i64;
                break;
              case TYPE_f32:
                newImport.desc.global->value.type = f32;
                break;
              case TYPE_f64:
                newImport.desc.global->value.type = f64;
                break;
              default:
                throw LoaderException(std::string(fileName) +
                                          ": Unknown import global value type.",
                                      true, cur - 1 - fileBuf);
            }
            if (*(cur++)) {
              throw LoaderException(
                  std::string(fileName) +
                      ": Only immutable global can be imported.",
                  true, cur - 1 - fileBuf);
            }
            newImport.desc.global->mut = false;
            break;
          default:
            throw LoaderException(
                std::string(fileName) + ": Unknown import type.", true,
                cur - 1 - fileBuf);
        }
        newModule->imports.push_back(newImport);
      }
    }
    /** Section 3: Function **/
    if (skipToSection(3, cur, endAddr) == 3) {
      std::uint32_t funcNum = Util::getLeb128_u32(cur, endAddr);
      while (funcNum-- > 0) {
        Func newFunc;
        newFunc.typeidx = Util::getLeb128_u32(cur, endAddr);
        newModule->funcs.push_back(newFunc);
      }
    }
    /** Section 4: Table **/
    if (skipToSection(4, cur, endAddr) == 4) {
      if (*cur++ > 1) {
        throw LoaderException(std::string(fileName) +
                                  ": There's only one table allowed currently.",
                              true, cur - 1 - fileBuf);
      }
      if (*cur++ != TYPE_Table_anyfunc) {
        throw LoaderException(
            std::string(fileName) +
                ": Only anyfunc is allowed in table currently.",
            true, cur - 1 - fileBuf);
      }
      newModule->table.flag = *(cur++);
      newModule->table.min = Util::getLeb128_u32(cur, endAddr);
      if (newModule->table.flag) {
        newModule->table.max = Util::getLeb128_u32(cur, endAddr);
      }
    }
    /** Section 5: Memory **/
    if (skipToSection(5, cur, endAddr) == 5) {
      if (*cur++ > 1) {
        throw LoaderException(
            std::string(fileName) +
                ": There's only one memory allowed currently.",
            true, cur - 1 - fileBuf);
      }
      newModule->mem.flag = *(cur++);
      newModule->mem.min = Util::getLeb128_u32(cur, endAddr);
      if (newModule->mem.flag) {
        newModule->mem.max = Util::getLeb128_u32(cur, endAddr);
      }
    }
    /** Section 6: Global **/
    if (skipToSection(6, cur, endAddr) == 6) {
      std::uint32_t globalNum = Util::getLeb128_u32(cur, endAddr);
      while (globalNum-- > 0) {
        Global newGlobal;
        // Set value type
        switch (*(cur++)) {
          case TYPE_i32:
            newGlobal.value.type = i32;
            break;
          case TYPE_i64:
            newGlobal.value.type = i64;
            break;
          case TYPE_f32:
            newGlobal.value.type = f32;
            break;
          case TYPE_f64:
            newGlobal.value.type = f64;
            break;
          default:
            throw LoaderException(
                std::string(fileName) + ": Unknown global value type.", true,
                cur - 1 - fileBuf);
        }
        // Set mut
        newGlobal.mut = *(cur++);
        // Set value
        switch (*(cur++)) {
          case OP_i32_const:
            newGlobal.value.data.i32 = Util::getLeb128_i32(cur, endAddr);
            break;
          case OP_i64_const:
            newGlobal.value.data.i64 = Util::getLeb128_i64(cur, endAddr);
            break;
          case OP_f32_const:
            newGlobal.value.data.i32 = Util::toLittle32(*((uint32_t*)cur));
            cur += 4;
            break;
          case OP_f64_const:
            newGlobal.value.data.i64 = Util::toLittle64(*((uint64_t*)cur));
            cur += 8;
            break;
          default:
            throw LoaderException(
                std::string(fileName) +
                    ": Global must be initialized with a constant expression.",
                true, cur - 1 - fileBuf);
        }
        // Skip end
        cur++;
        // Push to module
        newModule->globals.push_back(newGlobal);
      }
    }
    /** Section 7: Export **/
    if (skipToSection(7, cur, endAddr) == 7) {
      std::uint32_t exportNum = Util::getLeb128_u32(cur, endAddr);
      while (exportNum-- > 0) {
        Export newExport;
        // Get name length
        std::uint32_t nameLen = Util::getLeb128_u32(cur, endAddr);
        // Get name
        char name[nameLen + 1];
        name[nameLen] = '\0';
        strncpy(name, cur, nameLen);
        cur += nameLen;
        newExport.name = name;
        for (std::vector<Export>::iterator expIt = newModule->exports.begin();
             expIt != newModule->exports.end(); ++expIt) {
          if (expIt->name == newExport.name) {
            throw LoaderException(
                std::string(fileName) + ": Export name must be unique.", true,
                cur - nameLen - fileBuf);
          }
        }
        // Export kind
        switch (*(cur++)) {
          case IMPORT_Func:
            newExport.kind = func;
            break;
          case IMPORT_Table:
            newExport.kind = table;
            break;
          case IMPORT_Mem:
            newExport.kind = mem;
            break;
          case IMPORT_Global:
            newExport.kind = global;
            break;
          default:
            throw LoaderException(
                std::string(fileName) + ": Unknown export type.", true,
                cur - 1 - fileBuf);
        }
        newExport.desc = Util::getLeb128_u32(cur, endAddr);
        newModule->exports.push_back(newExport);
      }
    }
    /** Section 8: Start **/
    if (skipToSection(8, cur, endAddr) == 8) {
      newModule->start = new std::uint32_t;
      *(newModule->start) = Util::getLeb128_u32(cur, endAddr);
      std::uint32_t indexInFuncs = *(newModule->start) - importedFuncCount;
      if (indexInFuncs >= newModule->funcs.size()) {
        throw LoaderException(std::string(fileName) +
                                  ": Index of start function must be defined.",
                              true, cur - fileBuf);
      }
      FuncType& startFuncType =
          newModule->types.at(newModule->funcs.at(indexInFuncs).typeidx);
      if (startFuncType.paramTypes.size() > 0 ||
          startFuncType.resultTypes.size() > 0) {
        throw LoaderException(
            std::string(fileName) +
                ": Start function must be a void function without parameters.",
            true, cur - fileBuf);
      }
    }
    /** Section 9: Element **/
    if (skipToSection(9, cur, endAddr) == 9) {
      std::uint32_t elemNum = Util::getLeb128_u32(cur, endAddr);
      while (elemNum-- > 0) {
        Elem newElem;
        // Index
        if (Util::getLeb128_u32(cur, endAddr)) {
          throw LoaderException(
              std::string(fileName) + ": Only table 0 is allowed currently.",
              true, cur - fileBuf);
        }
        // Offset
        if ((*(cur++)) == OP_i32_const) {
          newElem.offset = Util::getLeb128_i32(cur, endAddr);
        } else {
          throw LoaderException(
              std::string(fileName) +
                  ": Element offset must be an i32.const expression.",
              true, cur - 1 - fileBuf);
        }
        cur++;  // Skip end
        // Init
        std::uint32_t initNum = Util::getLeb128_u32(cur, endAddr);
        while (initNum-- > 0) {
          std::uint32_t initIndex = Util::getLeb128_u32(cur, endAddr);
          if (initIndex >= newModule->funcs.size()) {
            throw LoaderException(
                std::string(fileName) +
                    ": Index of element function must be defined.",
                true, cur - fileBuf);
          }
          newElem.init.push_back(initIndex);
        }
        newModule->elem.push_back(newElem);
      }
    }
    /** Section 10: Code **/
    if (skipToSection(10, cur, endAddr) == 10) {
      std::uint32_t codeNum = Util::getLeb128_u32(cur, endAddr);
      if (codeNum != newModule->funcs.size()) {
        throw LoaderException(std::string(fileName) +
                                  ": Code count does not match function count.",
                              true, cur - fileBuf);
      }
      for (std::uint32_t i = 0; i < codeNum; ++i) {
        std::uint32_t codeSize = Util::getLeb128_u32(cur, endAddr);
        char* curAddr = cur;
        // Locals
        std::uint32_t localCount = Util::getLeb128_i32(cur, endAddr);
        while (localCount-- > 0) {
          std::uint32_t typeCount = Util::getLeb128_i32(cur, endAddr);
          ValType newType;
          switch (*(cur++)) {
            case TYPE_i32:
              newType = i32;
              break;
            case TYPE_i64:
              newType = i64;
              break;
            case TYPE_f32:
              newType = f32;
              break;
            case TYPE_f64:
              newType = f64;
              break;
            default:
              throw LoaderException(
                  std::string(fileName) + ": Unknown local type.", true,
                  cur - 1 - fileBuf);
          }
          while (typeCount-- > 0) {
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
    if (skipToSection(11, cur, endAddr) == 11) {
      std::uint32_t dataNum = Util::getLeb128_u32(cur, endAddr);
      while (dataNum-- > 0) {
        Data newData;
        // Index
        if (Util::getLeb128_u32(cur, endAddr)) {
          throw LoaderException(
              std::string(fileName) + ": Only table 0 is allowed currently.",
              true, cur - fileBuf);
        }
        // Offset
        if ((*(cur++)) == OP_i32_const) {
          newData.offset = Util::getLeb128_i32(cur, endAddr);
        } else {
          throw LoaderException(
              std::string(fileName) +
                  ": Data offset must be an i32.const expression.",
              true, cur - 1 - fileBuf);
        }
        cur++;  // Skip end
        // Init data
        std::uint32_t dataSize = Util::getLeb128_u32(cur, endAddr);
        newData.init.assign(cur, cur + dataSize);
        cur += dataSize;
        newModule->data.push_back(newData);
      }
    }
  } catch (const char* lebDesc) {
    throw LoaderException(std::string(fileName) + ": " + lebDesc, true,
                          cur - fileBuf);
  }
  // Get imports
  std::vector<ExternVal> importVals;
  getImportVals(newModule, importVals);
  // Alloc module instance
  moduleInsts[moduleName] = allocModule(*newModule, importVals);
  // Instantiate
  instantiate(*(moduleInsts[moduleName]), *newModule, importVals);
}
std::uint32_t Loader::allocFunc(Func& func, ModuleInst* moduleInst) {
  FuncInst* funcInst = new FuncInst;
  funcInst->type = moduleInst->types[func.typeidx];
  funcInst->module = moduleInst;
  funcInst->code = func;
  store.funcs.push_back(funcInst);
  return store.funcs.size() - 1;
}
std::uint32_t Loader::allocTable(Module& module) {
  TableInst* tableInst = new TableInst;
  tableInst->elem.resize(module.table.min);
  if (module.table.flag) {
    tableInst->max = module.table.max;
  } else {
    tableInst->max = 0;
  }
  store.tables.push_back(tableInst);
  return store.tables.size() - 1;
}
std::uint32_t Loader::allocMem(Module& module) {
  MemInst* memInst = new MemInst;
  memInst->data.resize(module.mem.min * 65536);
  if (module.mem.flag) {
    memInst->max = module.mem.max * 65536;
  } else {
    memInst->max = 0;
  }
  store.mems.push_back(memInst);
  return store.mems.size() - 1;
}
std::uint32_t Loader::allocGlobal(Global& global) {
  GlobalInst* globalInst = new GlobalInst;
  globalInst->val = global.value;
  globalInst->mut = global.mut;
  store.globals.push_back(globalInst);
  return store.globals.size() - 1;
}
ModuleInst* Loader::allocModule(Module& module,
                                std::vector<ExternVal>& importVals) {
  ModuleInst* moduleInst = new ModuleInst;
  // Types
  moduleInst->types = module.types;
  // Imports
  for (std::vector<ExternVal>::iterator importIt = importVals.begin();
       importIt != importVals.end(); ++importIt) {
    switch (importIt->type) {
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
  for (std::vector<Func>::iterator funcIt = module.funcs.begin();
       funcIt != module.funcs.end(); ++funcIt) {
    moduleInst->funcaddrs.push_back(allocFunc(*funcIt, moduleInst));
  }
  // Tables
  moduleInst->tableaddrs.push_back(allocTable(module));
  // Memory
  moduleInst->memaddrs.push_back(allocMem(module));
  // Globals
  for (std::vector<Global>::iterator globIt = module.globals.begin();
       globIt != module.globals.end(); ++globIt) {
    moduleInst->globaladdrs.push_back(allocGlobal(*globIt));
  }
  // Exports
  for (std::vector<Export>::iterator expIt = module.exports.begin();
       expIt != module.exports.end(); ++expIt) {
    ExportInst exportInst;
    exportInst.externval.type = expIt->kind;
    exportInst.name = expIt->name;
    exportInst.externval.addr = expIt->desc;
    moduleInst->exports.push_back(exportInst);
  }
  return moduleInst;
}
void Loader::instantiate(ModuleInst& moduleInst,
                         Module& module,
                         std::vector<ExternVal>& importVals) {
  // Import
  if (importVals.size() != module.imports.size()) {
    throw LoaderException("Import size does not match provided import count.");
  }
  // Element
  for (std::vector<Elem>::iterator elemIt = module.elem.begin();
       elemIt != module.elem.end(); ++elemIt) {
    TableInst* tableRef = store.tables.at(moduleInst.tableaddrs.at(0));
    std::uint32_t totalSize = elemIt->offset + elemIt->init.size();
    if (totalSize <= tableRef->max) {
      if (totalSize > tableRef->elem.size()) {
        tableRef->elem.resize(totalSize);
      }
    } else {
      throw LoaderException("Element init exceed table max size.");
    }
    for (std::uint32_t i = 0; i < elemIt->init.size(); ++i) {
      if (elemIt->init.at(i) < moduleInst.funcaddrs.size()) {
        store.tables.at(moduleInst.tableaddrs.at(0))
            ->elem.at(elemIt->offset + i) =
            moduleInst.funcaddrs.at(elemIt->init.at(i));
      } else {
        throw LoaderException("Element init function doesn't exist.");
      }
    }
  }
  // Data
  for (std::vector<Data>::iterator dataIt = module.data.begin();
       dataIt != module.data.end(); ++dataIt) {
    MemInst* memRef = store.mems.at(moduleInst.memaddrs.at(0));
    std::uint32_t totalSize = dataIt->offset + dataIt->init.size();
    if ((memRef->max == 0) || (totalSize <= memRef->max)) {
      if (totalSize > memRef->data.size()) {
        memRef->data.resize(totalSize + (65536 - totalSize % 65536));
      }
    } else {
      throw LoaderException("Data init exceed memory max size.");
    }
    for (std::uint32_t i = 0; i < dataIt->init.size(); ++i) {
      store.mems.at(moduleInst.memaddrs.at(0))->data.at(dataIt->offset + i) =
          dataIt->init.at(i);
    }
  }
  // Start
  moduleInst.start = module.start;
  if (module.start != nullptr) {
    moduleInst.start = new std::uint32_t;
    *(moduleInst.start) = moduleInst.funcaddrs.at(*(module.start));
  }
}
void Loader::getImportVals(Module* module, std::vector<ExternVal>& externVals) {
  for (std::vector<Import>::iterator impIt = module->imports.begin();
       impIt != module->imports.end(); ++impIt) {
    std::vector<ExportInst>& modExports = moduleInsts[impIt->module]->exports;
    bool found = false;
    for (std::vector<ExportInst>::iterator expIt = modExports.begin();
         expIt != modExports.end(); ++expIt) {
      ExternVal exp = expIt->externval;
      if (expIt->name == impIt->name) {
        if (impIt->kind != exp.type) {
          throw LoaderException("External type not match import type.");
        }
        switch (exp.type) {
          case func:
            exp.addr = moduleInsts[impIt->module]->funcaddrs[exp.addr];
            if (store.funcs.at(exp.addr)->type !=
                module->types.at(*(impIt->desc.funcType))) {
              throw LoaderException(
                  "External function type not match import function type.");
            }
            break;
          case table:
            exp.addr = moduleInsts[impIt->module]->tableaddrs[exp.addr];
            if (store.tables.at(exp.addr)->elem.size() <
                impIt->desc.table->min) {
              throw LoaderException(
                  "External table size must be larger than or equal to import "
                  "minimum.");
            }
            if (impIt->desc.table->flag &&
                store.tables.at(exp.addr)->max > impIt->desc.table->max) {
              throw LoaderException(
                  "External table maximum must be smaller than or equal to "
                  "import maximum.");
            }
            break;
          case mem:
            exp.addr = moduleInsts[impIt->module]->memaddrs[exp.addr];
            if (store.mems.at(exp.addr)->data.size() < impIt->desc.mem->min) {
              throw LoaderException(
                  "External memory size must be larger than or equal to import "
                  "minimum.");
            }
            if (impIt->desc.mem->flag &&
                store.mems.at(exp.addr)->max > impIt->desc.mem->max) {
              throw LoaderException(
                  "External memory maximum must be smaller than or equal to "
                  "import maximum.");
            }
            break;
          case global:
            exp.addr = moduleInsts[impIt->module]->globaladdrs[exp.addr];
            if (store.globals.at(exp.addr)->val.type !=
                impIt->desc.global->value.type) {
              throw LoaderException(
                  "External memory size must be larger than or equal to import "
                  "minimum.");
            }
            break;
          default:
            throw LoaderException("No such export type");
        }
        externVals.push_back(exp);
        found = true;
        break;
      }
    }
    if (!found) {
      throw LoaderException("Can't find export name " + impIt->name +
                            " in module " + impIt->module);
    }
  }
}
char Loader::skipToSection(char sectionNum, char*& cur, const char* endAddr) {
  if (cur > endAddr) {
    return -1;
  }
  // Check section code
  while (*cur < sectionNum) {
    cur++;
    // Get section size
    std::uint32_t size = Util::getLeb128_u32(cur, endAddr);
    cur += size;
  }
  char ret = *cur;
  if (ret == sectionNum) {
    Util::getLeb128_u32(++cur, endAddr);
  }
  return ret;
}
