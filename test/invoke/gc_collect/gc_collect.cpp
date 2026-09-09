// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Reclaiming what a guest allocates.
//
// Reference counting frees an object the moment the last handle goes away,
// which covers everything except a cycle -- and a guest builds one as soon as
// it stores a reference back into its own graph, which is what a doubly-linked
// list or a tree with parent pointers is. Those used to be kept for the life
// of the process: destroying the Store, the ModuleInst and the Stack reclaimed
// none of it, because the objects were never owned by any of them.
//
// So the two things this has to show are opposite, and the second matters
// more: a cycle that has become garbage IS freed, and a cycle that is still
// reachable is NOT. A collector that frees a live object is a use-after-free,
// which is worse than the leak it replaces, so the reachable cases here read
// the data back after collecting rather than only counting.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>
#include <instances/Stack.hpp>

#include <list>
#include <sstream>
#include <string>
#include <vector>

using namespace WasmVM;
using namespace Testing;

namespace {

std::vector<Value> run(const std::string& wat) {
    std::istringstream source(wat);
    WasmModule module = module_parse(source);
    if (std::optional<Exception::Exception> error = module_validate(module)) {
        throw *error;
    }
    static std::list<ModuleInst> instances; // stable addresses; FuncInst holds a reference
    static std::list<Store>      stores;
    Store&      store    = stores.emplace_back();
    ModuleInst& instance = instances.emplace_back();
    module_instanciate(instance, store, module, {});
    for (ExportInst& exported : instance.exports) {
        if (exported.name == "f" && exported.value.type == ExternVal::ExternType::Func) {
            Stack stack(store);
            stack.invoke(exported.value.addr, {});
            return stack.run();
        }
    }
    throw Exception::Exception("test module has no exported function 'f'");
}

// $n is a node with a mutable link, so a guest can tie it into a cycle.
const std::string nodes =
    "(module (type $n (sub (struct (field $next (mut (ref null $n))) (field $tag i32)))) ";

// How many objects are alive, once everything already unreachable is gone.
std::size_t settled_live() {
    gc_collect();
    return gc_live_count();
}

} // namespace

Suite invoke_gc_collect {
    Category("garbage goes away", {
        Test("plain objects are all reclaimed", {
            // Nothing here is cyclic. An object that passed through the
            // candidate buffer waits for a collection rather than going the
            // instant its count reaches zero, so what is asserted is that
            // everything is back, not when.
            const std::size_t before = settled_live();
            run(nodes + "(func (export \"f\") (local $o (ref $n))"
                " ref.null $n i32.const 1 struct.new $n local.set $o"
                " ref.null $n i32.const 2 struct.new $n local.set $o))");
            gc_collect();
            ExpectEq(gc_live_count(), before);
        })
        Test("a self-referencing object is collected", {
            const std::size_t before = settled_live();
            run(nodes + "(func (export \"f\") (local $o (ref $n))"
                " ref.null $n i32.const 1 struct.new $n local.set $o"
                " local.get $o local.get $o struct.set $n $next))");
            // It is still there: its own link keeps the count above zero.
            ExpectEq(gc_live_count(), before + 1);
            gc_collect();
            ExpectEq(gc_live_count(), before);
        })
        Test("two objects pointing at each other are collected", {
            const std::size_t before = settled_live();
            run(nodes + "(func (export \"f\") (local $a (ref $n)) (local $b (ref $n))"
                " ref.null $n i32.const 1 struct.new $n local.set $a"
                " ref.null $n i32.const 2 struct.new $n local.set $b"
                " local.get $a local.get $b struct.set $n $next"
                " local.get $b local.get $a struct.set $n $next))");
            ExpectEq(gc_live_count(), before + 2);
            gc_collect();
            ExpectEq(gc_live_count(), before);
        })
        Test("a longer ring is collected whole", {
            const std::size_t before = settled_live();
            run(nodes + "(func (export \"f\")"
                " (local $a (ref $n)) (local $b (ref $n)) (local $c (ref $n))"
                " ref.null $n i32.const 1 struct.new $n local.set $a"
                " ref.null $n i32.const 2 struct.new $n local.set $b"
                " ref.null $n i32.const 3 struct.new $n local.set $c"
                " local.get $a local.get $b struct.set $n $next"
                " local.get $b local.get $c struct.set $n $next"
                " local.get $c local.get $a struct.set $n $next))");
            ExpectEq(gc_live_count(), before + 3);
            gc_collect();
            ExpectEq(gc_live_count(), before);
        })
        Test("an array holding itself is collected", {
            const std::size_t before = settled_live();
            run("(module (type $a (array (mut (ref null $a))))"
                " (func (export \"f\") (local $o (ref $a))"
                " i32.const 2 array.new_default $a local.set $o"
                " local.get $o i32.const 0 local.get $o array.set $a))");
            ExpectEq(gc_live_count(), before + 1);
            gc_collect();
            ExpectEq(gc_live_count(), before);
        })
    })
    // The half that must not go wrong. Freeing one of these would be a
    // use-after-free, so each reads its data back after collecting.
    Category("what is still reachable stays", {
        Test("a cycle the embedder still holds is not collected", {
            const std::size_t before = settled_live();
            // Returns one member of a two-object ring; the returned Value is
            // the only thing outside the ring that refers to it.
            std::vector<Value> held = run(nodes +
                "(func (export \"f\") (result (ref $n))"
                " (local $a (ref $n)) (local $b (ref $n))"
                " ref.null $n i32.const 111 struct.new $n local.set $a"
                " ref.null $n i32.const 222 struct.new $n local.set $b"
                " local.get $a local.get $b struct.set $n $next"
                " local.get $b local.get $a struct.set $n $next"
                " local.get $a))");
            AssertEq(held.size(), 1U);
            ExpectEq(gc_collect(), (std::size_t)0);
            ExpectEq(gc_live_count(), before + 2);

            // Still readable, and the ring is intact in both directions.
            Assert(std::holds_alternative<structref_t>(held[0]));
            const structref_t& a = std::get<structref_t>(held[0]);
            Assert(a != nullptr);
            AssertEq(a->fields.size(), 2U);
            ExpectEq(std::get<i32_t>(a->fields[1]), 111);
            const structref_t& b = std::get<structref_t>(a->fields[0]);
            Assert(b != nullptr);
            ExpectEq(std::get<i32_t>(b->fields[1]), 222);
            Expect(std::get<structref_t>(b->fields[0]) == a);

            // Letting go is what makes it garbage.
            held.clear();
            gc_collect();
            ExpectEq(gc_live_count(), before);
        })
        Test("collecting twice over does not disturb what is held", {
            const std::size_t before = settled_live();
            std::vector<Value> held = run(nodes +
                "(func (export \"f\") (result (ref $n))"
                " (local $a (ref $n))"
                " ref.null $n i32.const 7 struct.new $n local.set $a"
                " local.get $a local.get $a struct.set $n $next"
                " local.get $a))");
            AssertEq(held.size(), 1U);
            for (int round = 0; round < 5; ++round) {
                ExpectEq(gc_collect(), (std::size_t)0);
            }
            const structref_t& a = std::get<structref_t>(held[0]);
            Assert(a != nullptr);
            ExpectEq(std::get<i32_t>(a->fields[1]), 7);
            Expect(std::get<structref_t>(a->fields[0]) == a);
            ExpectEq(gc_live_count(), before + 1);
            held.clear();
            ExpectEq(gc_collect(), (std::size_t)1);
        })
        Test("a live object hanging off a collected ring survives", {
            // $b is in the ring; $c hangs off it and is also held from
            // outside, so the ring goes and $c stays.
            const std::size_t before = settled_live();
            std::vector<Value> held = run(nodes +
                "(func (export \"f\") (result (ref $n))"
                " (local $a (ref $n)) (local $b (ref $n)) (local $c (ref $n))"
                " ref.null $n i32.const 1 struct.new $n local.set $a"
                " ref.null $n i32.const 2 struct.new $n local.set $b"
                " ref.null $n i32.const 333 struct.new $n local.set $c"
                " local.get $a local.get $b struct.set $n $next"
                " local.get $b local.get $a struct.set $n $next"
                " local.get $c))");
            AssertEq(held.size(), 1U);
            ExpectEq(gc_live_count(), before + 3);
            ExpectEq(gc_collect(), (std::size_t)2); // the ring only
            const structref_t& c = std::get<structref_t>(held[0]);
            Assert(c != nullptr);
            ExpectEq(std::get<i32_t>(c->fields[1]), 333);
            ExpectEq(gc_live_count(), before + 1);
        })
    })
    // An exception payload is on the same heap and holds Values, so a cycle
    // can run through one. The text format has no way to catch a reference to
    // an exception yet, so this builds the shape directly.
    Category("an exception payload is on the heap too", {
        Test("a cycle through one is collected", {
            const std::size_t before = settled_live();
            {
                structref_t node = gc_new<StructObj>();
                node->typeidx = 0;
                node->fields.emplace_back(Value(exnref_t()));
                GcPtr<WasmException> payload =
                    gc_new<WasmException>(0, std::vector<Value>{Value(node)});
                node->fields[0] = Value(exnref_t(payload));
            }
            ExpectEq(gc_live_count(), before + 2);
            gc_collect();
            ExpectEq(gc_live_count(), before);
        })
        Test("one that is still held is not", {
            const std::size_t before = settled_live();
            structref_t node = gc_new<StructObj>();
            node->typeidx = 0;
            node->fields.emplace_back(Value(exnref_t()));
            {
                GcPtr<WasmException> payload =
                    gc_new<WasmException>(0, std::vector<Value>{Value(node)});
                node->fields[0] = Value(exnref_t(payload));
            }
            ExpectEq(gc_collect(), (std::size_t)0);
            ExpectEq(gc_live_count(), before + 2);
            // Readable through the cycle and back.
            const exnref_t& held = std::get<exnref_t>(node->fields[0]);
            Assert(held.has_value());
            AssertEq(held.value->values.size(), 1U);
            Expect(std::get<structref_t>(held.value->values[0]) == node);
            node = structref_t();
            gc_collect();
            ExpectEq(gc_live_count(), before);
        })
    })
};
