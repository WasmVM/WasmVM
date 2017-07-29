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
    Loader(Store &store, std::vector<ModuleInst *> &moduleInsts);
    void load(const char *fileName);

private:
    Store &store;
    std::vector<ModuleInst *> &moduleInsts;
    std::map<std::string, Module *> modules;

    std::uint32_t allocFunc();
    std::uint32_t allocTable();
    std::uint32_t allocMem();
    std::uint32_t allocGlobal();
    ModuleInst *allocModule();
    void instantiate();
    char skipToSection(char sectionNum, char* &cur, const char *endAddr); // return section size and go to content
};

#endif