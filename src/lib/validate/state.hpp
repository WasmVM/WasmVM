#ifndef WASMVM_PP_VALIDATE_STATE_DEF
#define WASMVM_PP_VALIDATE_STATE_DEF

#include <WasmVM.hpp>

#include <vector>
#include <deque>
#include <variant>
#include <algorithm>

namespace WasmVM {
namespace Validate {

struct State {

    struct CtrlFrame {
        CtrlFrame(Opcode::opcode_t opcode, const FuncType& type, size_t height = 0)
            : opcode(opcode), type(type), height(height) {}
        Opcode::opcode_t opcode;
        const FuncType type;
        size_t height;
        bool unreachable = false;
        const std::vector<ValueType>& types();
    };

    std::vector<const ValueType> locals;
    using StackValue = std::variant<std::monostate, ValueType>;
    std::deque<StackValue> vals;
    std::deque<CtrlFrame> ctrls;

    void push(StackValue val);
    void push(std::vector<StackValue> vlist);
    void push(Opcode::opcode_t opcode, FuncType type);

    template<typename T>
    T pop();

    template<typename T>
    T pop(T);

    void unreachable();

};

}
}

#endif
