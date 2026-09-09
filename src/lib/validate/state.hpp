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

    // A reference to a concrete type -- (ref null? $t) -- as struct.new and
    // array.new produce it and struct.get and the like consume it. It lives
    // only inside the validator: ValueType has no such member yet, so no
    // local, global, parameter or result can hold one, and pop(ValueType)
    // rejects it wherever one of those is expected.
    struct TypedRef {
        bool nullable;
        index_t typeidx;
    };
    using StackValue = std::variant<std::monostate, ValueType, TypedRef>;

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
    // A reference to `expect.typeidx`; a non-null one satisfies a nullable
    // expectation, not the other way round.
    StackValue pop(TypedRef expect);

    void unreachable();

};

}
}

#endif
