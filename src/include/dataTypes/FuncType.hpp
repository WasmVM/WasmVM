#ifndef WASMVM_PP_DATATYPES_FUNCTYPE_DEF
#define WASMVM_PP_DATATYPES_FUNCTYPE_DEF

#include <vector>
#include <Value.hpp>

struct FuncType{
    std::vector<ValueType> params;
};


#endif