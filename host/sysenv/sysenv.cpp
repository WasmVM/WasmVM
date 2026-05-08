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

static MemInst& get_mem0(Stack& stack) {
    Frame& frame = stack.frames.top();
    if(frame.module.memaddrs.empty()) {
        throw std::runtime_error("sysenv: module has no linear memory");
    }
    return stack.store.mems[frame.module.memaddrs[0]];
}

std::span<byte_t> mem_span(Stack& stack, i64_t ptr, i64_t len) {
    MemInst& mem = get_mem0(stack);
    size_t end = (size_t)ptr + (size_t)len;
    if(end > mem.data.size()) {
        throw std::out_of_range("sysenv: memory access out of bounds");
    }
    return std::span<byte_t>(mem.data.data() + (size_t)ptr, (size_t)len);
}

std::string mem_string(Stack& stack, i64_t ptr, i64_t len) {
    auto sp = mem_span(stack, ptr, len);
    return std::string(reinterpret_cast<const char*>(sp.data()), sp.size());
}

void mem_write(Stack& stack, i64_t ptr, const void* src, size_t len) {
    MemInst& mem = get_mem0(stack);
    size_t end = (size_t)ptr + len;
    if(end > mem.data.size()) {
        throw std::out_of_range("sysenv: memory write out of bounds");
    }
    std::memcpy(mem.data.data() + (size_t)ptr, src, len);
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
