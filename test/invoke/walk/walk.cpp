// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Read-only walk of Stack::frames, Frame::labels and Label::values (#52).
//
// The debugger has to see the whole call stack, each frame's locals and each
// label's operand values, not just the tops. The guest below builds a known
// stack -- two wasm frames, the inner one with a block label, operands left
// on every label -- and then calls an imported host function. The host
// function receives the live Stack& from the interpreter and records what the
// walk sees in both directions; the test compares it after run() returns.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>
#include <instances/Stack.hpp>

#include <list>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace WasmVM;
using namespace Testing;

// The walk hands out const elements even from a non-const stack.
static_assert(std::is_const_v<std::remove_reference_t<decltype(*std::declval<Stack::Frames&>().begin())>>);
static_assert(std::is_const_v<std::remove_reference_t<decltype(*std::declval<Stack::Frames&>().rbegin())>>);
static_assert(std::is_const_v<std::remove_reference_t<decltype(*std::declval<Frame::Labels&>().begin())>>);
static_assert(std::is_const_v<std::remove_reference_t<decltype(*std::declval<Frame::Labels&>().rbegin())>>);
static_assert(std::is_const_v<std::remove_reference_t<decltype(*std::declval<Label::Values&>().begin())>>);
static_assert(std::is_const_v<std::remove_reference_t<decltype(*std::declval<Label::Values&>().rbegin())>>);

namespace {

std::string show(const Value& value) {
    if (std::holds_alternative<i32_t>(value)) {
        return std::to_string(std::get<i32_t>(value));
    }
    if (std::holds_alternative<i64_t>(value)) {
        return std::to_string(std::get<i64_t>(value)) + "L";
    }
    return "?";
}

// One frame as "#funcaddr (locals) [label values][label values]...".
std::string show_locals(const Frame& frame) {
    std::string out = "#" + std::to_string(frame.funcaddr) + " (";
    for (size_t i = 0; i < frame.locals.size(); ++i) {
        out += (i ? "," : "") + show(frame.locals[i]);
    }
    return out + ") ";
}

std::string walk_bottom_to_top(const Stack& stack) {
    std::string out;
    for (const Frame& frame : stack.frames) {
        out += (out.empty() ? "" : " | ") + show_locals(frame);
        for (const Label& label : frame.labels) {
            out += "[";
            bool first = true;
            for (const Value& value : label.values) {
                out += (first ? "" : ",") + show(value);
                first = false;
            }
            out += "]";
        }
    }
    return out;
}

std::string walk_top_to_bottom(const Stack& stack) {
    std::string out;
    for (auto frame = stack.frames.rbegin(); frame != stack.frames.rend(); ++frame) {
        out += (out.empty() ? "" : " | ") + show_locals(*frame);
        for (auto label = frame->labels.rbegin(); label != frame->labels.rend(); ++label) {
            out += "[";
            for (auto value = label->values.rbegin(); value != label->values.rend(); ++value) {
                out += (value == label->values.rbegin() ? "" : ",") + show(*value);
            }
            out += "]";
        }
    }
    return out;
}

struct Seen {
    int calls = 0;
    size_t frame_count = 0;
    std::string bottom_to_top;
    std::string top_to_bottom;
};
Seen seen;

std::vector<Value> probe(Stack& stack) {
    seen.calls += 1;
    seen.frame_count = stack.frames.size();
    seen.bottom_to_top = walk_bottom_to_top(stack);
    seen.top_to_bottom = walk_top_to_bottom(stack);
    return {};
}

// Store function 0 is the host probe, imported as $probe; the module's own
// functions follow it: $inner at 1 and "f" at 2.
const std::string guest =
    "(module "
    "  (import \"env\" \"probe\" (func $probe (param i32))) "
    "  (func $inner (param i32) (local i64) "
    "    i64.const 7 "
    "    local.set 1 "
    "    i32.const 30 "
    "    block "
    "      i32.const 40 "
    "      i32.const 41 "
    "      local.get 0 "
    "      call $probe "
    "      drop "
    "      drop "
    "    end "
    "    drop) "
    "  (func (export \"f\") "
    "    i32.const 10 "
    "    i32.const 5 "
    "    call $inner "
    "    drop))";

void run_guest() {
    static std::list<ModuleInst> instances; // stable addresses; FuncInst holds a reference
    static std::list<Store>      stores;
    Store&      store = stores.emplace_back();
    ModuleInst& host  = instances.emplace_back();
    FuncInst& func = store.funcs.emplace_back(host);
    func.type.params.emplace_back(ValueType::i32);
    func.body = &probe;

    std::istringstream source(guest);
    WasmModule module = module_parse(source);
    if (std::optional<Exception::Exception> error = module_validate(module)) {
        throw *error;
    }
    ModuleInst& instance = instances.emplace_back();
    module_instanciate(instance, store, module, {ExternVal{ExternVal::ExternType::Func, 0}});
    for (ExportInst& exported : instance.exports) {
        if (exported.name == "f" && exported.value.type == ExternVal::ExternType::Func) {
            Stack stack(store);
            stack.invoke(exported.value.addr, {});
            stack.run();
            return;
        }
    }
    throw Exception::Exception("test module has no exported function 'f'");
}

} // namespace

Suite walk {
    Category("walk", {
        Test("frames, labels and operand values walk bottom to top and top to bottom", {
            seen = Seen();
            run_guest();
            // The probe must actually have run, or the strings below compare empty.
            AssertEq(seen.calls, 1);
            ExpectEq(seen.frame_count, (size_t)3);
            ExpectEq(seen.bottom_to_top,
                     std::string("#2 () [10] | #1 (5,7L) [30][40,41] | #0 (5) []"));
            ExpectEq(seen.top_to_bottom,
                     std::string("#0 (5) [] | #1 (5,7L) [41,40][30] | #2 () [10]"));
        })
    })
};
