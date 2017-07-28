#ifndef LOADER_DEF
#define LOADER_DEF

#include <stack>
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
    std::stack<Module> modules;

    std::uint32_t allocFunc();
    std::uint32_t allocTable();
    std::uint32_t allocMem();
    std::uint32_t allocGlobal();
    ModuleInst *allocModule();
    void instantiate();
    void skipToSection(char sectionNum);
};

#endif