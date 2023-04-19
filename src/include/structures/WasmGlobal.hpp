#ifndef WASMVM_PP_STRUCTURE_GLOBAL
#define WASMVM_PP_STRUCTURE_GLOBAL

#include <structures/WasmInstr.hpp>

namespace WasmVM {

struct WasmGlobal {
    GlobalType type;
    ConstInstr init;
};

}
#endif