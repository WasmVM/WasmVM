// Copyright 2025 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "sysenv.hpp"
#include "compat.hpp"

#include <instances/FuncInst.hpp>
#include <stdexcept>
#include <cstring>

// ---- FdTable ----------------------------------------------------------------

FdTable fd_table;

FdTable::FdTable() {
    // Pre-open stdin / stdout / stderr so wasm fd 0/1/2 map to the host's.
    table_[0] = FdEntry{0, false, nullptr};
    table_[1] = FdEntry{1, false, nullptr};
    table_[2] = FdEntry{2, false, nullptr};
}

i32_t FdTable::alloc(int os_fd) {
    i32_t wfd = next_fd_++;
    table_[wfd] = FdEntry{os_fd, false, nullptr};
    return wfd;
}

i32_t FdTable::alloc_dir(std::shared_ptr<DirIter> dir) {
    i32_t wfd = next_fd_++;
    table_[wfd] = FdEntry{-1, true, std::move(dir)};
    return wfd;
}

FdEntry* FdTable::get(i32_t fd) {
    auto it = table_.find(fd);
    if(it == table_.end()) return nullptr;
    return &it->second;
}

void FdTable::release(i32_t fd) {
    auto it = table_.find(fd);
    if(it == table_.end()) return;
    FdEntry& e = it->second;
    if(!e.is_dir && e.os_fd >= 3) {
        // Never close the host's own stdin/stdout/stderr.
        WasmVM::sysenv_compat::sys_close(e.os_fd);
    }
    // Directory iterator handles release automatically via shared_ptr.
    table_.erase(it);
}

// ---- Linear-memory helpers --------------------------------------------------
//
// Tagged pointers: a guest pointer carries its memory index in the high nibble
// (bits [60:63]) and the offset within that memory in the low 60 bits. memidx 0
// is the heap/static memory, memidx 1 the shadow stack; the guest dispatches on
// the same tag on every load/store. Syscalls must honor it too — otherwise a
// pointer into the shadow stack (e.g. write(fd, &local, n)) would be read from
// the heap memory. A plain (untagged) pointer has memidx 0, so this is backward
// compatible with hosts that never tag.
static constexpr uint64_t PTR_TAG_SHIFT = 60;
static constexpr uint64_t PTR_OFF_MASK  = (uint64_t(1) << PTR_TAG_SHIFT) - 1;

// Resolve the memory a tagged pointer refers to, in the *caller's* module.
static MemInst& get_mem(Stack& stack, i64_t ptr) {
    Frame& frame = stack.frames.top();
    uint64_t memidx = (uint64_t)ptr >> PTR_TAG_SHIFT;
    if(memidx >= frame.module.memaddrs.size()) {
        throw std::runtime_error("sysenv: pointer references an absent memory");
    }
    return stack.store.mems[frame.module.memaddrs[memidx]];
}

std::span<byte_t> mem_span(Stack& stack, i64_t ptr, i64_t len) {
    MemInst& mem = get_mem(stack, ptr);
    size_t off = (size_t)((uint64_t)ptr & PTR_OFF_MASK);
    size_t end = off + (size_t)len;
    if(end > mem.data.size()) {
        throw std::out_of_range("sysenv: memory access out of bounds");
    }
    return std::span<byte_t>(mem.data.data() + off, (size_t)len);
}

std::string mem_string(Stack& stack, i64_t ptr, i64_t len) {
    auto sp = mem_span(stack, ptr, len);
    return std::string(reinterpret_cast<const char*>(sp.data()), sp.size());
}

void mem_write(Stack& stack, i64_t ptr, const void* src, size_t len) {
    MemInst& mem = get_mem(stack, ptr);
    size_t off = (size_t)((uint64_t)ptr & PTR_OFF_MASK);
    size_t end = off + len;
    if(end > mem.data.size()) {
        throw std::out_of_range("sysenv: memory write out of bounds");
    }
    std::memcpy(mem.data.data() + off, src, len);
}

// ---- reg_func helper --------------------------------------------------------

void reg_func(
    ModuleInst& moduleinst,
    Store& store,
    const std::string& name,
    std::initializer_list<ValueType> params,
    std::initializer_list<ValueType> results,
    hostfunc_t fn)
{
    index_t addr = store.funcs.size();
    moduleinst.funcaddrs.emplace_back(addr);

    FuncInst& fi = store.funcs.emplace_back(moduleinst);
    fi.module = moduleinst;
    for(ValueType vt : params)   fi.type.params.emplace_back(vt);
    for(ValueType vt : results)  fi.type.results.emplace_back(vt);
    fi.body.emplace<hostfunc_t>(fn);

    ExportInst& ex = moduleinst.exports.emplace_back();
    ex.name = name;
    ex.value.type = ExternVal::ExternType::Func;
    ex.value.addr = addr;
}

// ---- Top-level entry point --------------------------------------------------

void sysenv_instanciate(
    std::map<std::filesystem::path, ModuleInst>& moduleinsts,
    Store& store)
{
    sys_fs_instanciate(moduleinsts, store);
    sys_proc_instanciate(moduleinsts, store);
}
