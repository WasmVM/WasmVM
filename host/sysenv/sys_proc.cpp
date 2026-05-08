// Copyright 2025 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Module: sys_proc
// Provides process control, environment access, and time syscalls.

#include "sysenv.hpp"
#include "sys_proc.hpp"
#include "compat.hpp"

#include <host.hpp>       // wasmvm_args

#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cstdint>

// ---- sys_proc host functions ------------------------------------------------

// exit(code:i32) -> (no return)
static std::vector<Value> proc_exit(Stack& stack) {
    Frame& frame = stack.frames.top();
    i32_t code = std::get<i32_t>(frame.locals[0]);
    std::exit((int)code);
}

// argc() -> i32
// Returns the number of arguments passed on the wasmvm command line.
static std::vector<Value> proc_argc(Stack& stack) {
    return {Value(i32_t(wasmvm_args.size()))};
}

// argv_len(idx:i32) -> i32
// Returns the byte length of argument[idx] (not including null terminator),
// or -EINVAL if idx is out of range.
static std::vector<Value> proc_argv_len(Stack& stack) {
    Frame& frame = stack.frames.top();
    i32_t idx = std::get<i32_t>(frame.locals[0]);
    if(idx < 0 || (size_t)idx >= wasmvm_args.size()) {
        return {Value(i32_t(-EINVAL))};
    }
    return {Value(i32_t(wasmvm_args[(size_t)idx].size()))};
}

// argv(idx:i32, buf_ptr, buf_len) -> i32
static std::vector<Value> proc_argv(Stack& stack) {
    Frame& frame = stack.frames.top();
    i32_t idx     = std::get<i32_t>(frame.locals[0]);
    i64_t buf_ptr = get_ptr(frame.locals[1]);
    i64_t buf_len = get_ptr(frame.locals[2]);
    if(idx < 0 || (size_t)idx >= wasmvm_args.size()) {
        return {Value(i32_t(-EINVAL))};
    }
    const std::string& arg = wasmvm_args[(size_t)idx];
    i64_t copy_len = std::min((i64_t)arg.size(), buf_len);
    try {
        mem_write(stack, buf_ptr, arg.data(), (size_t)copy_len);
        if(copy_len < buf_len) {
            byte_t nul = std::byte{0};
            mem_write(stack, buf_ptr + copy_len, &nul, 1);
        }
        return {Value(i32_t(copy_len))};
    } catch(...) {
        return {Value(i32_t(-EFAULT))};
    }
}

// getenv(name_ptr, name_len, buf_ptr, buf_len) -> i32
static std::vector<Value> proc_getenv(Stack& stack) {
    Frame& frame = stack.frames.top();
    i64_t name_ptr = get_ptr(frame.locals[0]);
    i64_t name_len = get_ptr(frame.locals[1]);
    i64_t buf_ptr  = get_ptr(frame.locals[2]);
    i64_t buf_len  = get_ptr(frame.locals[3]);
    try {
        std::string name = mem_string(stack, name_ptr, name_len);
        const char* val = std::getenv(name.c_str());
        if(val == nullptr) return {Value(i32_t(-ENOENT))};
        size_t val_len = std::strlen(val);
        if((size_t)buf_len < val_len + 1) return {Value(i32_t(-ERANGE))};
        mem_write(stack, buf_ptr, val, val_len + 1);
        return {Value(i32_t(val_len))};
    } catch(...) {
        return {Value(i32_t(-EFAULT))};
    }
}

// clock_gettime(clk_id:i32, ts_ptr) -> i32
static std::vector<Value> proc_clock_gettime(Stack& stack) {
    Frame& frame = stack.frames.top();
    i32_t clk_id = std::get<i32_t>(frame.locals[0]);
    i64_t ts_ptr = get_ptr(frame.locals[1]);

    bool monotonic;
    switch(clk_id) {
        case WASM_CLOCK_REALTIME:  monotonic = false; break;
        case WASM_CLOCK_MONOTONIC: monotonic = true;  break;
        default: return {Value(i32_t(-EINVAL))};
    }

    auto [sec, nsec] = WasmVM::sysenv_compat::clock_now_ns(monotonic);

    try {
        uint8_t buf[WASM_TIMESPEC_SIZE];
        std::memcpy(buf + 0, &sec,  8);
        std::memcpy(buf + 8, &nsec, 4);
        mem_write(stack, ts_ptr, buf, WASM_TIMESPEC_SIZE);
        return {Value(i32_t(0))};
    } catch(...) {
        return {Value(i32_t(-EFAULT))};
    }
}

// ---- Registration -----------------------------------------------------------

void sys_proc_instanciate(
    std::map<std::filesystem::path, ModuleInst>& moduleinsts,
    Store& store)
{
    if(moduleinsts.contains("sys_proc")) return;

    moduleinsts.emplace("sys_proc", ModuleInst());
    ModuleInst& m = moduleinsts["sys_proc"];

    // exit(code:i32) -> ()
    reg_func(m, store, "exit",
        {ValueType::i32}, {}, proc_exit);

    // argc() -> i32
    reg_func(m, store, "argc",
        {}, {ValueType::i32}, proc_argc);

    // argv_len(idx:i32) -> i32
    reg_func(m, store, "argv_len",
        {ValueType::i32}, {ValueType::i32}, proc_argv_len);

    // argv(idx:i32, buf_ptr, buf_len) -> i32
    reg_func(m, store, "argv",
        {ValueType::i32, ValueType::i32, ValueType::i32},
        {ValueType::i32}, proc_argv);
    reg_func(m, store, "argv",
        {ValueType::i32, ValueType::i64, ValueType::i64},
        {ValueType::i32}, proc_argv);

    // getenv(name_ptr, name_len, buf_ptr, buf_len) -> i32
    reg_func(m, store, "getenv",
        {ValueType::i32, ValueType::i32, ValueType::i32, ValueType::i32},
        {ValueType::i32}, proc_getenv);
    reg_func(m, store, "getenv",
        {ValueType::i64, ValueType::i64, ValueType::i64, ValueType::i64},
        {ValueType::i32}, proc_getenv);

    // clock_gettime(clk_id:i32, ts_ptr) -> i32
    reg_func(m, store, "clock_gettime",
        {ValueType::i32, ValueType::i32},
        {ValueType::i32}, proc_clock_gettime);
    reg_func(m, store, "clock_gettime",
        {ValueType::i32, ValueType::i64},
        {ValueType::i32}, proc_clock_gettime);
}
