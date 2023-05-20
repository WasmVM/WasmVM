#ifndef WASMVM_PP_VALIDATE_DEF
#define WASMVM_PP_VALIDATE_DEF

#include <WasmVM.hpp>

#include <vector>
#include <functional>

namespace WasmVM {
namespace Validate {

struct Validator {

    Validator (const WasmModule& module);

    template<typename T>
    void operator()(const T&);
    void operator()(const WasmGlobal&, index_t);
    
    struct Context {
        Context(const std::vector<FuncType>& types, const std::vector<WasmElem>& elems, const std::vector<WasmData>& datas) : types(types), elems(elems), datas(datas) {}
        const std::vector<FuncType>& types;
        std::vector<FuncType> funcs;
        std::vector<TableType> tables;
        std::vector<MemType> mems;
        std::vector<GlobalType> globals;
        const std::vector<WasmElem>& elems;
        const std::vector<WasmData>& datas;
    };

    Context context;
};

}
}

#endif