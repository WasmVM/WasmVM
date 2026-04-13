// Copyright 2025 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef WASMVM_HOST_SYSENV_DEF
#define WASMVM_HOST_SYSENV_DEF

#include <Types.hpp>
#include <instances/Stack.hpp>
#include <instances/ModuleInst.hpp>
#include <instances/Store.hpp>

#include <map>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <span>
#include <cerrno>
#include <dirent.h>

using namespace WasmVM;

// ---- FD Table ---------------------------------------------------------------

struct FdEntry {
    int  os_fd;   // underlying OS file descriptor (-1 if dir-only)
    bool is_dir;
    DIR* dir;     // non-null when entry represents an open directory
};

class FdTable {
public:
    FdTable();

    // Allocate a new wasm fd wrapping an OS file descriptor.
    i32_t alloc(int os_fd);

    // Allocate a new wasm fd wrapping an open directory handle.
    i32_t alloc_dir(DIR* dir);

    // Look up a wasm fd. Returns nullptr if not found.
    FdEntry* get(i32_t fd);

    // Close the underlying OS resource and remove the entry.
    void release(i32_t fd);

private:
    std::unordered_map<i32_t, FdEntry> table_;
    i32_t next_fd_ = 3;
};

extern FdTable fd_table;

// ---- Linear-memory helpers --------------------------------------------------

// Return a span over [ptr, ptr+len) in the wasm module's memory 0.
// Throws std::out_of_range if the range is outside the current memory.
std::span<byte_t> mem_span(Stack& stack, i32_t ptr, i32_t len);

// Copy [ptr, ptr+len) from wasm memory into a std::string.
std::string mem_string(Stack& stack, i32_t ptr, i32_t len);

// Write 'len' bytes from 'src' into wasm memory starting at 'ptr'.
// Throws std::out_of_range if the range is outside the current memory.
void mem_write(Stack& stack, i32_t ptr, const void* src, size_t len);

// Map the current OS errno to a negative i32 result code.
inline i32_t err_code() { return -(i32_t)errno; }

// ---- Sub-module registration ------------------------------------------------

void sys_fs_instanciate(std::map<std::filesystem::path, ModuleInst>& moduleinsts, Store& store);
void sys_proc_instanciate(std::map<std::filesystem::path, ModuleInst>& moduleinsts, Store& store);

// ---- Helper: register one host function into a module ----------------------

// Appends a FuncInst to the store, adds its address to moduleinst.funcaddrs,
// and registers an ExportInst.  'params' and 'results' list ValueTypes.
void reg_func(
    ModuleInst& moduleinst,
    Store& store,
    const std::string& name,
    std::initializer_list<ValueType> params,
    std::initializer_list<ValueType> results,
    hostfunc_t fn
);

#endif // WASMVM_HOST_SYSENV_DEF
