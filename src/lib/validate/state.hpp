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

    using StackValue = std::variant<std::monostate, ValueType>;

    struct CtrlFrame {
        CtrlFrame(Opcode::opcode_t opcode, const FuncType& type, size_t height = 0)
            : opcode(opcode), type(type), height(height) {}
        Opcode::opcode_t opcode;
        const FuncType type;
        size_t height;
        bool unreachable = false;
        const std::vector<ValueType>& types();
    };

    std::vector<ValueType> locals;
    std::deque<StackValue> vals;
    std::deque<CtrlFrame> ctrls;

    void push(StackValue val);
    void push(std::vector<StackValue> vlist);
    void push(std::vector<ValueType> vlist);
    void push(Opcode::opcode_t opcode, FuncType type);

    template<typename T>
    T pop();

    std::vector<StackValue> pop(std::vector<ValueType> expects);

    StackValue pop(StackValue expect);
    StackValue pop(ValueType expect1, ValueType expect2);

    void unreachable();

};

}
}

#endif
