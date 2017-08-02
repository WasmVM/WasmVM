#ifndef LOADERINSTS_DEF
#define LOADERINSTS_DEF

#include <cstdint>
#include <vector>
#include <string>
#include <Value.h>

class Limits {
public:
    char flag;
    std::uint32_t min;
    std::uint32_t max;
};

class Global {
public:
    bool mut;
    Value value;
};

class Elem {
public:
    // The only tableidx is 0 now, ignore
    std::uint32_t offset;
    std::vector<std::uint32_t> init;
};

class Data {
public:
    // The only tableidx is 0 now, ignore
    std::uint64_t offset;
    std::vector<char> init;
};

class Import {
public:
    std::string module;
    std::string name;
    ExternType kind;
    union {
        std::uint32_t *func;
        Limits *table;
        Limits *mem;
        Global *global;
    } desc;
};

class Export {
public:
    std::string name;
    ExternType kind;
    std::uint32_t desc;
};

class Module {
public:
    std::vector<FuncType> types;
    std::vector<Func> funcs;
    /* 1. ElemType is anyfunc currently, needn't to specify now.
     * 2. It only allow one table now.
     */
    Limits table; 
    /* 1. It only allow one memory now.
     */
    Limits mem;
    std::vector<Global> globals;
    std::vector<Elem> elem;
    std::vector<Data> data;
    std::uint32_t *start = nullptr; // Using nullptr to indicate no start
    std::vector<Import> imports;
    std::vector<Export> exports;
};

#endif