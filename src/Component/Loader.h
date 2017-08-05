#ifndef LOADER_DEF
#define LOADER_DEF

#include <map>
#include <vector>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <cstdint>
#include <string>
#include <Instance.h>
#include <LoaderInsts.h>
#include <LoaderException.h>
#include <Store.h>
#include <Util.h>

#define WASM_MAGIC 0x6D736100 // Little endian
#define WASM_VERSION 0x01

class Loader {
public:
    Loader(Store &store, std::map<std::string, ModuleInst *> &moduleInsts);
    void load(const char *fileName);
private:
    Store &store;
    std::map<std::string, ModuleInst *> &moduleInsts;
    std::map<std::string, Module *> modules;

    std::uint32_t allocFunc(Func &func, ModuleInst *moduleInst);
    std::uint32_t allocTable(Module &module);
    std::uint32_t allocMem(Module &module);
    std::uint32_t allocGlobal(Global &global);
    ModuleInst *allocModule(Module &module, std::vector<ExternVal> &importVals);

    void getImportVals(Module *module, std::vector<ExternVal> &externVals);
    void instantiate();
    char skipToSection(char sectionNum, char* &cur, const char *endAddr); // return section size and go to content
};

#endif