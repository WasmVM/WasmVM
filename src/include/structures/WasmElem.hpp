#ifndef WASMVM_PP_STRUCTURE_ELEM
#define WASMVM_PP_STRUCTURE_ELEM

#include <Types.hpp>
#include <structures/WasmInstr.hpp>

#include <vector>
#include <optional>

namespace WasmVM {

struct WasmElem {
    
    struct ElemMode {
        enum class Mode { passive, active, declarative } type;
        std::optional<index_t> tableidx;
        std::optional<ConstInstr> offset;
    };
    
    RefType type;
    std::vector<ConstInstr> elemlist;
    ElemMode mode;
};

}

#endif