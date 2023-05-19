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
    
protected:

    struct Context {
        Context(const std::vector<FuncType>& types, const std::vector<WasmElem>& elems, const std::vector<WasmData>& datas) : types(types), elems(elems), datas(datas) {}
        const std::vector<FuncType>& types;
        std::vector<std::reference_wrapper<const FuncType>> funcs;
        std::vector<std::reference_wrapper<const TableType>> tables;
        std::vector<std::reference_wrapper<const MemType>> mems;
        std::vector<std::reference_wrapper<const GlobalType>> globals;
        const std::vector<WasmElem>& elems;
        const std::vector<WasmData>& datas;
    };

    Context context;
};

}
}

#endif