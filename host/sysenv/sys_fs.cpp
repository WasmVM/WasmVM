// Copyright 2025 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Module: sys_fs
// Provides POSIX-style filesystem syscalls to WebAssembly modules.
// String / buffer arguments are passed as (ptr, len) i64 pairs into wasm linear
// memory (wasm64 only).  On error, functions return -errno (negative i32).

#include "sysenv.hpp"
#include "sys_fs.hpp"
#include "compat.hpp"

#include <cstring>
#include <cstdint>
#include <stdexcept>
#include <system_error>
#include <filesystem>

using WasmVM::sysenv_compat::sys_open;
using WasmVM::sysenv_compat::sys_close;
using WasmVM::sysenv_compat::sys_read;
using WasmVM::sysenv_compat::sys_write;
using WasmVM::sysenv_compat::sys_lseek;
using WasmVM::sysenv_compat::sys_unlink;
using WasmVM::sysenv_compat::sys_rmdir;
using WasmVM::sysenv_compat::sys_mkdir;
using WasmVM::sysenv_compat::sys_rename;
using WasmVM::sysenv_compat::sys_getcwd;
using WasmVM::sysenv_compat::sys_stat;
using WasmVM::sysenv_compat::sys_fstat;
using WasmVM::sysenv_compat::stat_t;
using WasmVM::sysenv_compat::stat_atime_ns;
using WasmVM::sysenv_compat::stat_mtime_ns;
using WasmVM::sysenv_compat::stat_ctime_ns;

namespace fs = std::filesystem;

// dirent.d_type constants the wasm side expects (mirrors POSIX).
static constexpr uint8_t WASM_DT_UNKNOWN = 0;
static constexpr uint8_t WASM_DT_DIR     = 4;
static constexpr uint8_t WASM_DT_REG     = 8;
static constexpr uint8_t WASM_DT_LNK     = 10;

static uint8_t dt_from_entry(const fs::directory_entry& e) {
    std::error_code ec;
    if(e.is_symlink(ec)) return WASM_DT_LNK;
    if(e.is_directory(ec)) return WASM_DT_DIR;
    if(e.is_regular_file(ec)) return WASM_DT_REG;
    return WASM_DT_UNKNOWN;
}

// ---- Helper: write a struct stat into wasm memory --------------------------

static void write_wasm_stat(Stack& stack, i64_t stat_buf, const stat_t& st) {
    uint8_t buf[WASM_STAT_SIZE] = {};
    uint32_t dev   = (uint32_t)st.st_dev;
    uint32_t ino   = (uint32_t)st.st_ino;
    uint32_t mode  = (uint32_t)st.st_mode;
    uint32_t nlink = (uint32_t)st.st_nlink;
    uint32_t uid   = (uint32_t)st.st_uid;
    uint32_t gid   = (uint32_t)st.st_gid;
    uint64_t size  = (uint64_t)st.st_size;
    uint64_t atime_ns = stat_atime_ns(st);
    uint64_t mtime_ns = stat_mtime_ns(st);
    uint64_t ctime_ns = stat_ctime_ns(st);

    std::memcpy(buf +  0, &dev,      4);
    std::memcpy(buf +  4, &ino,      4);
    std::memcpy(buf +  8, &mode,     4);
    std::memcpy(buf + 12, &nlink,    4);
    std::memcpy(buf + 16, &uid,      4);
    std::memcpy(buf + 20, &gid,      4);
    std::memcpy(buf + 24, &size,     8);
    std::memcpy(buf + 32, &atime_ns, 8);
    std::memcpy(buf + 40, &mtime_ns, 8);
    std::memcpy(buf + 48, &ctime_ns, 8);

    mem_write(stack, stat_buf, buf, WASM_STAT_SIZE);
}

// ---- sys_fs host functions --------------------------------------------------
// Each function uses get_ptr() to extract pointer/length locals (wasm64: i64).

// open(path_ptr, path_len, flags:i32, mode:i32) -> i32
static std::vector<Value> fs_open(Stack& stack) {
    Frame& frame = stack.frames.top();
    i64_t path_ptr = get_ptr(frame.locals[0]);
    i64_t path_len = get_ptr(frame.locals[1]);
    i32_t flags    = std::get<i32_t>(frame.locals[2]);
    i32_t mode     = std::get<i32_t>(frame.locals[3]);
    try {
        std::string path = mem_string(stack, path_ptr, path_len);
        int fd = sys_open(path.c_str(), flags, (int)mode);
        if(fd < 0) return {Value(err_code())};
        return {Value(fd_table.alloc(fd))};
    } catch(...) {
        return {Value(i32_t(-EFAULT))};
    }
}

// close(fd:i32) -> i32
static std::vector<Value> fs_close(Stack& stack) {
    Frame& frame = stack.frames.top();
    i32_t fd = std::get<i32_t>(frame.locals[0]);
    FdEntry* e = fd_table.get(fd);
    if(!e) return {Value(i32_t(-EBADF))};
    fd_table.release(fd);
    return {Value(i32_t(0))};
}

// read(fd:i32, buf_ptr, buf_len) -> i32
static std::vector<Value> fs_read(Stack& stack) {
    Frame& frame = stack.frames.top();
    i32_t fd      = std::get<i32_t>(frame.locals[0]);
    i64_t buf_ptr = get_ptr(frame.locals[1]);
    i64_t buf_len = get_ptr(frame.locals[2]);
    FdEntry* e = fd_table.get(fd);
    if(!e || e->is_dir) return {Value(i32_t(-EBADF))};
    try {
        auto sp = mem_span(stack, buf_ptr, buf_len);
        auto n = sys_read(e->os_fd, sp.data(), sp.size());
        if(n < 0) return {Value(err_code())};
        return {Value(i32_t(n))};
    } catch(...) {
        return {Value(i32_t(-EFAULT))};
    }
}

// write(fd:i32, buf_ptr, buf_len) -> i32
static std::vector<Value> fs_write(Stack& stack) {
    Frame& frame = stack.frames.top();
    i32_t fd      = std::get<i32_t>(frame.locals[0]);
    i64_t buf_ptr = get_ptr(frame.locals[1]);
    i64_t buf_len = get_ptr(frame.locals[2]);
    FdEntry* e = fd_table.get(fd);
    if(!e || e->is_dir) return {Value(i32_t(-EBADF))};
    try {
        auto sp = mem_span(stack, buf_ptr, buf_len);
        auto n = sys_write(e->os_fd, sp.data(), sp.size());
        if(n < 0) return {Value(err_code())};
        return {Value(i32_t(n))};
    } catch(...) {
        return {Value(i32_t(-EFAULT))};
    }
}

// lseek(fd:i32, offset:i64, whence:i32) -> i64
static std::vector<Value> fs_lseek(Stack& stack) {
    Frame& frame = stack.frames.top();
    i32_t fd     = std::get<i32_t>(frame.locals[0]);
    i64_t offset = std::get<i64_t>(frame.locals[1]);
    i32_t whence = std::get<i32_t>(frame.locals[2]);
    FdEntry* e = fd_table.get(fd);
    if(!e || e->is_dir) return {Value(i64_t(-EBADF))};
    int64_t result = sys_lseek(e->os_fd, offset, (int)whence);
    if(result < 0) return {Value(i64_t(-errno))};
    return {Value(i64_t(result))};
}

// stat(path_ptr, path_len, stat_buf) -> i32
static std::vector<Value> fs_stat(Stack& stack) {
    Frame& frame = stack.frames.top();
    i64_t path_ptr = get_ptr(frame.locals[0]);
    i64_t path_len = get_ptr(frame.locals[1]);
    i64_t stat_buf = get_ptr(frame.locals[2]);
    try {
        std::string path = mem_string(stack, path_ptr, path_len);
        stat_t st;
        if(sys_stat(path.c_str(), &st) != 0) return {Value(err_code())};
        write_wasm_stat(stack, stat_buf, st);
        return {Value(i32_t(0))};
    } catch(...) {
        return {Value(i32_t(-EFAULT))};
    }
}

// fstat(fd:i32, stat_buf) -> i32
static std::vector<Value> fs_fstat(Stack& stack) {
    Frame& frame = stack.frames.top();
    i32_t fd       = std::get<i32_t>(frame.locals[0]);
    i64_t stat_buf = get_ptr(frame.locals[1]);
    FdEntry* e = fd_table.get(fd);
    if(!e || e->is_dir) return {Value(i32_t(-EBADF))};
    try {
        stat_t st;
        if(sys_fstat(e->os_fd, &st) != 0) return {Value(err_code())};
        write_wasm_stat(stack, stat_buf, st);
        return {Value(i32_t(0))};
    } catch(...) {
        return {Value(i32_t(-EFAULT))};
    }
}

// unlink(path_ptr, path_len) -> i32
static std::vector<Value> fs_unlink(Stack& stack) {
    Frame& frame = stack.frames.top();
    i64_t path_ptr = get_ptr(frame.locals[0]);
    i64_t path_len = get_ptr(frame.locals[1]);
    try {
        std::string path = mem_string(stack, path_ptr, path_len);
        if(sys_unlink(path.c_str()) != 0) return {Value(err_code())};
        return {Value(i32_t(0))};
    } catch(...) {
        return {Value(i32_t(-EFAULT))};
    }
}

// rename(old_ptr, old_len, new_ptr, new_len) -> i32
static std::vector<Value> fs_rename(Stack& stack) {
    Frame& frame = stack.frames.top();
    i64_t old_ptr = get_ptr(frame.locals[0]);
    i64_t old_len = get_ptr(frame.locals[1]);
    i64_t new_ptr = get_ptr(frame.locals[2]);
    i64_t new_len = get_ptr(frame.locals[3]);
    try {
        std::string old_path = mem_string(stack, old_ptr, old_len);
        std::string new_path = mem_string(stack, new_ptr, new_len);
        if(sys_rename(old_path.c_str(), new_path.c_str()) != 0) return {Value(err_code())};
        return {Value(i32_t(0))};
    } catch(...) {
        return {Value(i32_t(-EFAULT))};
    }
}

// mkdir(path_ptr, path_len, mode:i32) -> i32
static std::vector<Value> fs_mkdir(Stack& stack) {
    Frame& frame = stack.frames.top();
    i64_t path_ptr = get_ptr(frame.locals[0]);
    i64_t path_len = get_ptr(frame.locals[1]);
    i32_t mode     = std::get<i32_t>(frame.locals[2]);
    try {
        std::string path = mem_string(stack, path_ptr, path_len);
        if(sys_mkdir(path.c_str(), (int)mode) != 0) return {Value(err_code())};
        return {Value(i32_t(0))};
    } catch(...) {
        return {Value(i32_t(-EFAULT))};
    }
}

// rmdir(path_ptr, path_len) -> i32
static std::vector<Value> fs_rmdir(Stack& stack) {
    Frame& frame = stack.frames.top();
    i64_t path_ptr = get_ptr(frame.locals[0]);
    i64_t path_len = get_ptr(frame.locals[1]);
    try {
        std::string path = mem_string(stack, path_ptr, path_len);
        if(sys_rmdir(path.c_str()) != 0) return {Value(err_code())};
        return {Value(i32_t(0))};
    } catch(...) {
        return {Value(i32_t(-EFAULT))};
    }
}

// getcwd(buf_ptr, buf_len) -> i32
static std::vector<Value> fs_getcwd(Stack& stack) {
    Frame& frame = stack.frames.top();
    i64_t buf_ptr = get_ptr(frame.locals[0]);
    i64_t buf_len = get_ptr(frame.locals[1]);
    try {
        auto sp = mem_span(stack, buf_ptr, buf_len);
        if(sys_getcwd(reinterpret_cast<char*>(sp.data()), sp.size()) == nullptr) {
            return {Value(err_code())};
        }
        i32_t written = (i32_t)std::strlen(reinterpret_cast<char*>(sp.data())) + 1;
        return {Value(written)};
    } catch(...) {
        return {Value(i32_t(-EFAULT))};
    }
}

// opendir(path_ptr, path_len) -> i32
static std::vector<Value> fs_opendir(Stack& stack) {
    Frame& frame = stack.frames.top();
    i64_t path_ptr = get_ptr(frame.locals[0]);
    i64_t path_len = get_ptr(frame.locals[1]);
    try {
        std::string path = mem_string(stack, path_ptr, path_len);
        std::error_code ec;
        fs::directory_iterator it(fs::path(path), ec);
        if(ec) {
            errno = ec.value();
            return {Value(err_code())};
        }
        auto handle = std::make_shared<DirIter>();
        handle->it = std::move(it);
        handle->path = fs::path(path);
        return {Value(fd_table.alloc_dir(std::move(handle)))};
    } catch(...) {
        return {Value(i32_t(-EFAULT))};
    }
}

// readdir(dir_fd:i32, entry_ptr) -> i32
// Returns: 0 = entry written, 1 = end of directory, -errno = error.
static std::vector<Value> fs_readdir(Stack& stack) {
    Frame& frame = stack.frames.top();
    i32_t dir_fd    = std::get<i32_t>(frame.locals[0]);
    i64_t entry_ptr = get_ptr(frame.locals[1]);
    FdEntry* e = fd_table.get(dir_fd);
    if(!e || !e->is_dir || !e->dir) return {Value(i32_t(-EBADF))};
    DirIter& di = *e->dir;
    if(di.it == di.end) return {Value(i32_t(1))};

    try {
        const fs::directory_entry& entry = *di.it;
        uint8_t buf[WASM_DIRENT_SIZE] = {};
        // std::filesystem doesn't expose a stable inode number cross-platform;
        // hash the path so each entry has a distinct, deterministic id.
        uint32_t ino  = (uint32_t)fs::hash_value(entry.path());
        uint8_t  type = dt_from_entry(entry);
        std::string name = entry.path().filename().string();
        std::memcpy(buf + 0, &ino,  4);
        std::memcpy(buf + 4, &type, 1);
        std::strncpy(reinterpret_cast<char*>(buf + 5), name.c_str(), 254);
        buf[5 + 254] = '\0';
        mem_write(stack, entry_ptr, buf, WASM_DIRENT_SIZE);

        std::error_code ec;
        di.it.increment(ec);
        if(ec) {
            errno = ec.value();
            return {Value(err_code())};
        }
        return {Value(i32_t(0))};
    } catch(...) {
        return {Value(i32_t(-EFAULT))};
    }
}

// closedir(dir_fd:i32) -> i32
static std::vector<Value> fs_closedir(Stack& stack) {
    Frame& frame = stack.frames.top();
    i32_t dir_fd = std::get<i32_t>(frame.locals[0]);
    FdEntry* e = fd_table.get(dir_fd);
    if(!e || !e->is_dir) return {Value(i32_t(-EBADF))};
    fd_table.release(dir_fd);
    return {Value(i32_t(0))};
}

// ---- Registration -----------------------------------------------------------

void sys_fs_instanciate(
    std::map<std::filesystem::path, ModuleInst>& moduleinsts,
    Store& store)
{
    if(moduleinsts.contains("sys_fs")) return;

    moduleinsts.emplace("sys_fs", ModuleInst());
    ModuleInst& m = moduleinsts["sys_fs"];

    // open(path_ptr:i64, path_len:i64, flags:i32, mode:i32) -> i32
    reg_func(m, store, "open",
        {ValueType::i64, ValueType::i64, ValueType::i32, ValueType::i32},
        {ValueType::i32}, fs_open);

    // close(fd:i32) -> i32
    reg_func(m, store, "close",
        {ValueType::i32}, {ValueType::i32}, fs_close);

    // read(fd:i32, buf_ptr:i64, buf_len:i64) -> i32
    reg_func(m, store, "read",
        {ValueType::i32, ValueType::i64, ValueType::i64},
        {ValueType::i32}, fs_read);

    // write(fd:i32, buf_ptr:i64, buf_len:i64) -> i32
    reg_func(m, store, "write",
        {ValueType::i32, ValueType::i64, ValueType::i64},
        {ValueType::i32}, fs_write);

    // lseek(fd:i32, offset:i64, whence:i32) -> i64
    reg_func(m, store, "lseek",
        {ValueType::i32, ValueType::i64, ValueType::i32},
        {ValueType::i64}, fs_lseek);

    // stat(path_ptr:i64, path_len:i64, stat_buf:i64) -> i32
    reg_func(m, store, "stat",
        {ValueType::i64, ValueType::i64, ValueType::i64},
        {ValueType::i32}, fs_stat);

    // fstat(fd:i32, stat_buf:i64) -> i32
    reg_func(m, store, "fstat",
        {ValueType::i32, ValueType::i64},
        {ValueType::i32}, fs_fstat);

    // unlink(path_ptr:i64, path_len:i64) -> i32
    reg_func(m, store, "unlink",
        {ValueType::i64, ValueType::i64},
        {ValueType::i32}, fs_unlink);

    // rename(old_ptr:i64, old_len:i64, new_ptr:i64, new_len:i64) -> i32
    reg_func(m, store, "rename",
        {ValueType::i64, ValueType::i64, ValueType::i64, ValueType::i64},
        {ValueType::i32}, fs_rename);

    // mkdir(path_ptr:i64, path_len:i64, mode:i32) -> i32
    reg_func(m, store, "mkdir",
        {ValueType::i64, ValueType::i64, ValueType::i32},
        {ValueType::i32}, fs_mkdir);

    // rmdir(path_ptr:i64, path_len:i64) -> i32
    reg_func(m, store, "rmdir",
        {ValueType::i64, ValueType::i64},
        {ValueType::i32}, fs_rmdir);

    // getcwd(buf_ptr:i64, buf_len:i64) -> i32
    reg_func(m, store, "getcwd",
        {ValueType::i64, ValueType::i64},
        {ValueType::i32}, fs_getcwd);

    // opendir(path_ptr:i64, path_len:i64) -> i32
    reg_func(m, store, "opendir",
        {ValueType::i64, ValueType::i64},
        {ValueType::i32}, fs_opendir);

    // readdir(dir_fd:i32, entry_ptr:i64) -> i32
    reg_func(m, store, "readdir",
        {ValueType::i32, ValueType::i64},
        {ValueType::i32}, fs_readdir);

    // closedir(dir_fd:i32) -> i32
    reg_func(m, store, "closedir",
        {ValueType::i32},
        {ValueType::i32}, fs_closedir);
}
