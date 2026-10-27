// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// What table.grow answers, and where an access is allowed to reach.
//
// The spec has it return the table's length *before* the growth, or -1 if it
// could not grow. It returned the new length, so a guest using the result to
// find where its new slots begin was handed the end instead of the start, and
// on a table that had just doubled it was off by the whole original length.
// memory.grow next door gets this right and says so in a comment; nothing
// covered table.grow's result at all.
//
// The second half is the bounds checks, which every access in memory.cpp and
// table.cpp shared and every one of which could wrap. An address or a count
// arrived as an i32 and was widened with its sign, so -1 became the top of
// the 64-bit address space rather than 4294967295; the checks then read
// `(start + count) > size`, which comes back in range once the addition
// wraps. A load at -1 passed the check and read outside the memory, a store
// at -1 with offset 1 wrapped to zero and wrote at the start of it, and grow
// with -1 shrank instead of refusing.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>
#include <instances/Stack.hpp>

#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <atomic>
#include <cstdlib>
#include <new>

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

i32_t i32_at(const std::vector<Value>& values, size_t index) {
    return std::get<i32_t>(values.at(index));
}

// Whether running the module's "f" traps, rather than answering or escaping
// as something else.
bool traps(const std::string& wat) {
    try {
        run(wat);
    } catch (Exception::Trap&) {
        return true;
    } catch (Exception::Exception& e) {
        std::cout << "      not a trap: " << e.what() << std::endl;
        return false;
    }
    std::cout << "      did not trap" << std::endl;
    return false;
}

} // namespace

// #36: a non-null table.grow failure can only be std::bad_alloc -- the element
// bound (2^32-1) is far below vector<Value>::max_size() (~7.69e17, sizeof(Value)
// ==24), so no size request can make resize throw std::length_error, and a real
// bad_alloc would need tens of GiB. To reach the new catch without that, this
// test executable replaces the global operator new: when armed it throws
// std::bad_alloc for the one large allocation table.grow's resize makes. The
// positive-control test proves the replacement really intercepts an allocation
// made inside libWasmVM.dylib (where Table_grow runs). Small allocations, and
// everything while disarmed, go straight to malloc, so the rest of the harness
// is untouched. Both flags are constant-initialized (false/0), safe even if
// operator new runs before dynamic init.
namespace {
    constexpr std::size_t grow_alloc_threshold = 16u << 20; // 16 MiB
    std::atomic<bool>        fail_big_alloc{false};
    std::atomic<std::size_t> biggest_alloc{0};
}
void* operator new(std::size_t size) {
    if (size >= grow_alloc_threshold) {
        std::size_t prev = biggest_alloc.load();
        while (size > prev && !biggest_alloc.compare_exchange_weak(prev, size)) {}
        if (fail_big_alloc.load()) {
            throw std::bad_alloc();
        }
    }
    void* p = std::malloc(size ? size : 1);
    if (p == nullptr) {
        throw std::bad_alloc();
    }
    return p;
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

Suite invoke_table {
    Category("table.grow", {
        Test("it answers with the length before the growth", {
            std::vector<Value> results = run(
                "(module (table 1 funcref) (func (export \"f\") (result i32 i32)"
                " ref.null func i32.const 2 table.grow 0 table.size 0))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), 1); // what it was
            ExpectEq(i32_at(results, 1), 3); // what it is
        })
        Test("growing by nothing still answers with the length", {
            std::vector<Value> results = run(
                "(module (table 3 funcref) (func (export \"f\") (result i32)"
                " ref.null func i32.const 0 table.grow 0))");
            AssertEq(results.size(), 1U);
            ExpectEq(i32_at(results, 0), 3);
        })
        Test("the answer is the start of the new slots", {
            // The whole use of the result: index it and reach the first slot
            // that was just added.
            std::vector<Value> results = run(
                "(module (table 1 externref) (func (export \"f\") (result i32)"
                " (local $at i32)"
                " ref.null extern i32.const 2 table.grow 0 local.set $at"
                " local.get $at table.get 0 ref.is_null))");
            AssertEq(results.size(), 1U);
            ExpectEq(i32_at(results, 0), 1);
        })
        Test("growing past the maximum answers -1 and changes nothing", {
            std::vector<Value> results = run(
                "(module (table 1 2 funcref) (func (export \"f\") (result i32 i32)"
                " ref.null func i32.const 5 table.grow 0 table.size 0))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), -1);
            ExpectEq(i32_at(results, 1), 1);
        })
        Test("the given reference fills the new slots", {
            std::vector<Value> results = run(
                "(module (table 1 externref) (func (export \"f\") (result i32 i32)"
                " ref.null extern i32.const 2 table.grow 0 drop"
                " i32.const 1 table.get 0 ref.is_null"
                " i32.const 2 table.get 0 ref.is_null))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), 1);
            ExpectEq(i32_at(results, 1), 1);
        })
        Test("it grows a table of concrete references too", {
            std::vector<Value> results = run(
                "(module (type $s (sub (struct (field i32)))) (table 1 (ref null $s))"
                " (func (export \"f\") (result i32 i32)"
                " ref.null $s i32.const 3 table.grow 0 table.size 0))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), 1);
            ExpectEq(i32_at(results, 1), 4);
        })
        // memory.grow is the instruction table.grow was out of step with.
        Test("memory.grow answers the same way", {
            std::vector<Value> results = run(
                "(module (memory 1) (func (export \"f\") (result i32 i32)"
                " i32.const 2 memory.grow 0 memory.size 0))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), 1);
            ExpectEq(i32_at(results, 1), 3);
        })
    })
    // An address or a count arriving as -1 means 4294967295, which is out of
    // range for any of these -- not a step backwards, and never a way to
    // reach outside.
    Category("an access near the top of the address space", {
        Test("a load at -1 traps instead of reading outside", {
            Expect(traps("(module (memory 1) (func (export \"f\") (result i32)"
                         " i32.const -1 i32.load))"));
            Expect(traps("(module (memory 1) (func (export \"f\") (result i32)"
                         " i32.const -1 i32.load offset=1))"));
            Expect(traps("(module (memory 1) (func (export \"f\") (result i32)"
                         " i32.const -4 i32.load offset=4))"));
        })
        Test("a store at -1 traps instead of writing at zero", {
            // It used to wrap to address 0 and write there, which a read of
            // address 0 could then see.
            Expect(traps("(module (memory 1) (func (export \"f\")"
                         " i32.const -1 i32.const 9 i32.store offset=1))"));
        })
        Test("fill, copy and init refuse a wrapped range", {
            Expect(traps("(module (memory 1) (func (export \"f\")"
                         " i32.const -1 i32.const 0 i32.const 2 memory.fill 0))"));
            Expect(traps("(module (memory 1) (func (export \"f\")"
                         " i32.const 0 i32.const 0 i32.const -1 memory.copy 0 0))"));
            Expect(traps("(module (memory 1) (data \"ab\") (func (export \"f\")"
                         " i32.const -1 i32.const 0 i32.const 1 memory.init 0 0))"));
            Expect(traps("(module (table 2 funcref) (func (export \"f\")"
                         " i32.const -1 ref.null func i32.const 2 table.fill 0))"));
        })
        Test("an access that does fit is still allowed", {
            std::vector<Value> results = run(
                "(module (memory 1) (func (export \"f\") (result i32 i32)"
                " i32.const 0 i32.const 305419896 i32.store"
                " i32.const 0 i32.load"
                " i32.const 65532 i32.load))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), 305419896);
            ExpectEq(i32_at(results, 1), 0);
        })
        Test("one byte past the end still traps", {
            Expect(traps("(module (memory 1) (func (export \"f\") (result i32)"
                         " i32.const 65533 i32.load))"));
        })
    })
    Category("growing by a count with its top bit set", {
        Test("table.grow refuses instead of shrinking", {
            // It used to answer 1 and leave a two-slot table with one slot.
            std::vector<Value> results = run(
                "(module (table 2 funcref) (func (export \"f\") (result i32 i32)"
                " ref.null func i32.const -1 table.grow 0 table.size 0))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), -1);
            ExpectEq(i32_at(results, 1), 2);
        })
        Test("memory.grow refuses instead of shrinking", {
            // It used to answer 1 and leave a one-page memory with no pages.
            std::vector<Value> results = run(
                "(module (memory 1) (func (export \"f\") (result i32 i32)"
                " i32.const -1 memory.grow 0 memory.size 0))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), -1);
            ExpectEq(i32_at(results, 1), 1);
        })
        Test("a 32-bit memory is bounded at 2^16 pages", {
            // The bound applied to both widths was 2^48, which is a 64-bit
            // memory's. A 32-bit one could be asked for 2^32 pages and went
            // to the allocator for 256 TiB, which came back as a bad_alloc
            // rather than as -1.
            std::vector<Value> results = run(
                "(module (memory 1) (func (export \"f\") (result i32 i32)"
                " i32.const 65536 memory.grow 0 memory.size 0))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), -1);
            ExpectEq(i32_at(results, 1), 1);
            // One page short of the bound (65536 pages exactly) passes the bound
            // check and reaches the 4 GiB allocation. Committing it took ~84 s
            // (measured; ~99.6% of invoke_table), so instead arm the operator-new
            // override (added for #36) to throw std::bad_alloc on that allocation.
            // memory.grow then answers -1, but the override having SEEN a 4 GiB
            // request proves the bound let 65536 pages through: a `>=` bound would
            // return -1 before allocating and biggest_alloc would stay 0. The
            // trade-off is deliberate (#37): that 4 GiB can really be allocated
            // and zero-filled is no longer tested; the reason is time.
            fail_big_alloc.store(true);
            biggest_alloc.store(0);
            std::vector<Value> ok;
            try {
                ok = run(
                    "(module (memory 1) (func (export \"f\") (result i32 i32)"
                    " i32.const 65535 memory.grow 0 memory.size 0))");
            } catch (...) {
                fail_big_alloc.store(false); // disarm even if it escaped
                throw;
            }
            fail_big_alloc.store(false);
            AssertEq(ok.size(), 2U);
            ExpectEq(i32_at(ok, 0), -1); // forced allocation failure -> -1
            ExpectEq(i32_at(ok, 1), 1);  // memory unchanged at its one page
            // The bound let 65536 pages reach the 4 GiB (65536 * 65536 B)
            // allocation; a `>=` bound would never have tried it.
            Assert(biggest_alloc.load() >= (std::size_t)65536 * 65536);
        })
    })
    Category("growing past what the host can allocate", {
        Test("memory.grow answers -1 when the allocation fails", {
            // 2^40 pages is under a 64-bit memory's 2^48-page bound, but is
            // 2^56 bytes, which the allocator refuses outright rather than
            // handing out pages. The failure came out as a std::bad_alloc
            // escaping Stack::run() instead of as -1, and the memory must be
            // left as it was.
            std::vector<Value> results = run(
                "(module (memory i64 1) (func (export \"f\") (result i64 i64)"
                " i64.const 1099511627776 memory.grow 0 memory.size 0))");
            AssertEq(results.size(), 2U);
            ExpectEq(std::get<i64_t>(results.at(0)), (i64_t)-1);
            ExpectEq(std::get<i64_t>(results.at(1)), (i64_t)1);
        })
    })
    Category("growing to a page count whose byte size overflows", {
        // 2^48-1 pages added to the one page is 2^48 pages, whose byte count
        // 2^48 * 65536 is 2^64: it wrapped to 0 and resized the memory to
        // nothing while answering success. memory.grow must answer -1 and
        // leave the page, and its contents, untouched.
        Test("memory.grow refuses and keeps the memory", {
            std::vector<Value> r = run(
                "(module (memory i64 1) (func (export \"f\") (result i64 i64 i64)"
                " i64.const 0 i64.const 1234 i64.store"
                " i64.const 281474976710655 memory.grow 0"
                " memory.size 0"
                " i64.const 0 i64.load))");
            AssertEq(r.size(), 3U);
            ExpectEq(std::get<i64_t>(r.at(0)), (i64_t)-1);
            ExpectEq(std::get<i64_t>(r.at(1)), (i64_t)1);
            ExpectEq(std::get<i64_t>(r.at(2)), (i64_t)1234);
        })
        // The module validates (min <= 2^48 is allowed), but instantiating it
        // would resize to 2^64 bytes, which wrapped to an empty memory
        // reported as success. It must fail instantiation instead.
        Test("instantiating a memory whose byte size overflows fails", {
            ExpectThrows(Exception::Exception, run(
                "(module (memory i64 281474976710656)"
                " (func (export \"f\") (result i64) memory.size 0))"));
        })
        // A min that fits size_t but the host cannot allocate: 2^46 pages is
        // 2^62 bytes, which the allocator refuses up front. The resize threw a
        // std::bad_alloc that escaped module_instanciate; it must fail
        // instantiation with Exception::Exception instead.
        Test("instantiating a memory the host cannot allocate fails", {
            ExpectThrows(Exception::Exception, run(
                "(module (memory i64 70368744177664)"
                " (func (export \"f\") (result i64) memory.size 0))"));
        })
    })
    Category("growing past what the backing vector can hold (#44)", {
        // 2^47 pages is 2^63 bytes, just over a byte vector's max_size()
        // (PTRDIFF_MAX): below the size_t guard (2^48) but past max_size, so
        // resize throws std::length_error -- not std::bad_alloc -- which used to
        // escape and abort the host. memory.grow must answer -1 and leave the
        // one page as it was.
        Test("memory.grow refuses a page count past vector max_size", {
            std::vector<Value> r = run(
                "(module (memory i64 1) (func (export \"f\") (result i64 i64)"
                " i64.const 140737488355327 memory.grow 0 memory.size 0))");
            AssertEq(r.size(), 2U);
            ExpectEq(std::get<i64_t>(r.at(0)), (i64_t)-1);
            ExpectEq(std::get<i64_t>(r.at(1)), (i64_t)1);
        })
        // The same size as a declared min: instantiation must fail with an
        // Exception, not abort via an uncaught std::length_error.
        Test("instantiating a memory past vector max_size fails", {
            ExpectThrows(Exception::Exception, run(
                "(module (memory i64 140737488355328)"
                " (func (export \"f\") (result i64) memory.size 0))"));
        })
    })
    // #36: table.grow's resize can throw std::bad_alloc (the only failure a
    // table can hit; see the operator-new note above). It used to escape and
    // abort; it must answer -1 and leave the table unchanged.
    Category("table.grow when the host cannot allocate (#36)", {
        // Positive control: with the override disarmed, a real grow of
        // 2,000,000 reference slots makes one vector resize of
        // 2,000,000 * sizeof(Value) bytes. sizeof(Value) depends on the standard
        // library's std::variant layout -- 16 on libstdc++, 24 on libc++ -- so
        // the count is chosen so count * sizeof(Value) clears the 16 MiB override
        // threshold on either; a smaller count
        // whose product fell under the threshold would leave the override blind
        // to this allocation. The grow succeeds and biggest_alloc shows the
        // replaced operator new saw that request -- it is the vector resize inside
        // libWasmVM that makes it, so the override is in force there. Nothing else
        // in this run allocates that much, so an armed throw below hits the
        // resize, not some other allocation.
        Test("the replaced operator new intercepts table.grow's allocation", {
            fail_big_alloc.store(false);
            biggest_alloc.store(0);
            std::vector<Value> r = run(
                "(module (table 0 funcref) (func (export \"f\") (result i32 i32)"
                " ref.null func i32.const 2000000 table.grow 0 table.size 0))");
            AssertEq(r.size(), 2U);
            ExpectEq(i32_at(r, 0), 0);        // old length
            ExpectEq(i32_at(r, 1), 2000000);  // grown
            Assert(biggest_alloc.load() >= 2000000u * sizeof(Value));
        })
        // The fix itself: arm the override so the resize throws std::bad_alloc.
        // table.grow must answer -1 and leave the table at its old length.
        Test("table.grow answers -1 when the allocation throws bad_alloc", {
            fail_big_alloc.store(true);
            std::vector<Value> r;
            try {
                r = run(
                    "(module (table 0 funcref) (func (export \"f\") (result i32 i32)"
                    " ref.null func i32.const 2000000 table.grow 0 table.size 0))");
            } catch (...) {
                fail_big_alloc.store(false); // disarm even if it escaped (no catch)
                throw;
            }
            fail_big_alloc.store(false);
            AssertEq(r.size(), 2U);
            ExpectEq(i32_at(r, 0), -1); // allocation failed
            ExpectEq(i32_at(r, 1), 0);  // table unchanged
        })
    })
};
