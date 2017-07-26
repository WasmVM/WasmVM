#ifndef STORE_DEF
#define STORE_DEF

#include <vector>
#include <Instance.h>

class Store{
public:
    std::vector<FuncInst> funcs;
    std::vector<TableInst> tables;
    std::vector<MemInst> mems;
    std::vector<GlobalInst> globals;
};

#endif