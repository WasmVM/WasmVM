// Copyright 2025 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Module: sys_fs
// Provides POSIX-style filesystem syscalls to WebAssembly modules.
// String / buffer arguments are passed as (ptr, len) pairs into wasm linear
// memory.  ptr/len may be i32 (wasm32) or i64 (wasm64) — get_ptr() handles
// both.  On error, functions return -errno (negative i32 or i64).

#include "sysenv.hpp"
#include "sys_fs.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>
#include <cstdint>
#include <stdexcept>

// ---- Helper: write a struct stat into wasm memory --------------------------

static void write_wasm_stat(Stack& stack, i64_t stat_buf, const struct stat& st) {
    uint8_t buf[WASM_STAT_SIZE] = {};
    uint32_t dev   = (uint32_t)st.st_dev;
    uint32_t ino   = (uint32_t)st.st_ino;
    uint32_t mode  = (uint32_t)st.st_mode;
    uint32_t nlink = (uint32_t)st.st_nlink;
    uint32_t uid   = (uint32_t)st.st_uid;
    uint32_t gid   = (uint32_t)st.st_gid;
    uint64_t size  = (uint64_t)st.st_size;

#if defined(__APPLE__)
    uint64_t atime_ns = (uint64_t)st.st_atimespec.tv_sec * 1000000000ULL + (uint64_t)st.st_atimespec.tv_nsec;
    uint64_t mtime_ns = (uint64_t)st.st_mtimespec.tv_sec * 1000000000ULL + (uint64_t)st.st_mtimespec.tv_nsec;
    uint64_t ctime_ns = (uint64_t)st.st_ctimespec.tv_sec * 1000000000ULL + (uint64_t)st.st_ctimespec.tv_nsec;
#else
    uint64_t atime_ns = (uint64_t)st.st_atim.tv_sec * 1000000000ULL + (uint64_t)st.st_atim.tv_nsec;
    uint64_t mtime_ns = (uint64_t)st.st_mtim.tv_sec * 1000000000ULL + (uint64_t)st.st_mtim.tv_nsec;
    uint64_t ctime_ns = (uint64_t)st.st_ctim.tv_sec * 1000000000ULL + (uint64_t)st.st_ctim.tv_nsec;
#endif

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
// Each function uses get_ptr() to extract pointer/length locals so the same
// body works for both wasm32 (i32 locals) and wasm64 (i64 locals).

// open(path_ptr, path_len, flags:i32, mode:i32) -> i32
static std::vector<Value> fs_open(Stack& stack) {
    Frame& frame = stack.frames.top();
    i64_t path_ptr = get_ptr(frame.locals[0]);
    i64_t path_len = get_ptr(frame.locals[1]);
    i32_t flags    = std::get<i32_t>(frame.locals[2]);
    i32_t mode     = std::get<i32_t>(frame.locals[3]);
    try {
        std::string path = mem_string(stack, path_ptr, path_len);
        int fd = ::open(path.c_str(), flags, (mode_t)mode);
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
        ssize_t n = ::read(e->os_fd, sp.data(), sp.size());
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
        ssize_t n = ::write(e->os_fd, sp.data(), sp.size());
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
    off_t result = ::lseek(e->os_fd, (off_t)offset, (int)whence);
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
        struct stat st;
        if(::stat(path.c_str(), &st) != 0) return {Value(err_code())};
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
        struct stat st;
        if(::fstat(e->os_fd, &st) != 0) return {Value(err_code())};
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
        if(::unlink(path.c_str()) != 0) return {Value(err_code())};
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
        if(::rename(old_path.c_str(), new_path.c_str()) != 0) return {Value(err_code())};
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
        if(::mkdir(path.c_str(), (mode_t)mode) != 0) return {Value(err_code())};
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
        if(::rmdir(path.c_str()) != 0) return {Value(err_code())};
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
        if(::getcwd(reinterpret_cast<char*>(sp.data()), sp.size()) == nullptr) {
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
        DIR* dir = ::opendir(path.c_str());
        if(dir == nullptr) return {Value(err_code())};
        return {Value(fd_table.alloc_dir(dir))};
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
    if(!e || !e->is_dir) return {Value(i32_t(-EBADF))};
    errno = 0;
    struct dirent* de = ::readdir(e->dir);
    if(de == nullptr) {
        if(errno != 0) return {Value(err_code())};
        return {Value(i32_t(1))};
    }
    try {
        uint8_t buf[WASM_DIRENT_SIZE] = {};
        uint32_t ino  = (uint32_t)de->d_ino;
        uint8_t  type = (uint8_t)de->d_type;
        std::memcpy(buf + 0, &ino,  4);
        std::memcpy(buf + 4, &type, 1);
        std::strncpy(reinterpret_cast<char*>(buf + 5), de->d_name, 254);
        buf[5 + 254] = '\0';
        mem_write(stack, entry_ptr, buf, WASM_DIRENT_SIZE);
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
// Each pointer-taking function is registered twice under the same name:
//   - i32 variant for wasm32 callers
//   - i64 variant for wasm64 callers
// The type-aware import resolution in wasmvm.cpp picks the right one.

void sys_fs_instanciate(
    std::map<std::filesystem::path, ModuleInst>& moduleinsts,
    Store& store)
{
    if(moduleinsts.contains("sys_fs")) return;

    moduleinsts.emplace("sys_fs", ModuleInst());
    ModuleInst& m = moduleinsts["sys_fs"];

    // open(path_ptr, path_len, flags:i32, mode:i32) -> i32
    reg_func(m, store, "open",
        {ValueType::i32, ValueType::i32, ValueType::i32, ValueType::i32},
        {ValueType::i32}, fs_open);
    reg_func(m, store, "open",
        {ValueType::i64, ValueType::i64, ValueType::i32, ValueType::i32},
        {ValueType::i32}, fs_open);

    // close(fd:i32) -> i32  — fd is always i32; no i64 variant needed
    reg_func(m, store, "close",
        {ValueType::i32}, {ValueType::i32}, fs_close);

    // read(fd:i32, buf_ptr, buf_len) -> i32
    reg_func(m, store, "read",
        {ValueType::i32, ValueType::i32, ValueType::i32},
        {ValueType::i32}, fs_read);
    reg_func(m, store, "read",
        {ValueType::i32, ValueType::i64, ValueType::i64},
        {ValueType::i32}, fs_read);

    // write(fd:i32, buf_ptr, buf_len) -> i32
    reg_func(m, store, "write",
        {ValueType::i32, ValueType::i32, ValueType::i32},
        {ValueType::i32}, fs_write);
    reg_func(m, store, "write",
        {ValueType::i32, ValueType::i64, ValueType::i64},
        {ValueType::i32}, fs_write);

    // lseek(fd:i32, offset:i64, whence:i32) -> i64  — no ptr args; one variant
    reg_func(m, store, "lseek",
        {ValueType::i32, ValueType::i64, ValueType::i32},
        {ValueType::i64}, fs_lseek);

    // stat(path_ptr, path_len, stat_buf) -> i32
    reg_func(m, store, "stat",
        {ValueType::i32, ValueType::i32, ValueType::i32},
        {ValueType::i32}, fs_stat);
    reg_func(m, store, "stat",
        {ValueType::i64, ValueType::i64, ValueType::i64},
        {ValueType::i32}, fs_stat);

    // fstat(fd:i32, stat_buf) -> i32
    reg_func(m, store, "fstat",
        {ValueType::i32, ValueType::i32},
        {ValueType::i32}, fs_fstat);
    reg_func(m, store, "fstat",
        {ValueType::i32, ValueType::i64},
        {ValueType::i32}, fs_fstat);

    // unlink(path_ptr, path_len) -> i32
    reg_func(m, store, "unlink",
        {ValueType::i32, ValueType::i32},
        {ValueType::i32}, fs_unlink);
    reg_func(m, store, "unlink",
        {ValueType::i64, ValueType::i64},
        {ValueType::i32}, fs_unlink);

    // rename(old_ptr, old_len, new_ptr, new_len) -> i32
    reg_func(m, store, "rename",
        {ValueType::i32, ValueType::i32, ValueType::i32, ValueType::i32},
        {ValueType::i32}, fs_rename);
    reg_func(m, store, "rename",
        {ValueType::i64, ValueType::i64, ValueType::i64, ValueType::i64},
        {ValueType::i32}, fs_rename);

    // mkdir(path_ptr, path_len, mode:i32) -> i32
    reg_func(m, store, "mkdir",
        {ValueType::i32, ValueType::i32, ValueType::i32},
        {ValueType::i32}, fs_mkdir);
    reg_func(m, store, "mkdir",
        {ValueType::i64, ValueType::i64, ValueType::i32},
        {ValueType::i32}, fs_mkdir);

    // rmdir(path_ptr, path_len) -> i32
    reg_func(m, store, "rmdir",
        {ValueType::i32, ValueType::i32},
        {ValueType::i32}, fs_rmdir);
    reg_func(m, store, "rmdir",
        {ValueType::i64, ValueType::i64},
        {ValueType::i32}, fs_rmdir);

    // getcwd(buf_ptr, buf_len) -> i32
    reg_func(m, store, "getcwd",
        {ValueType::i32, ValueType::i32},
        {ValueType::i32}, fs_getcwd);
    reg_func(m, store, "getcwd",
        {ValueType::i64, ValueType::i64},
        {ValueType::i32}, fs_getcwd);

    // opendir(path_ptr, path_len) -> i32
    reg_func(m, store, "opendir",
        {ValueType::i32, ValueType::i32},
        {ValueType::i32}, fs_opendir);
    reg_func(m, store, "opendir",
        {ValueType::i64, ValueType::i64},
        {ValueType::i32}, fs_opendir);

    // readdir(dir_fd:i32, entry_ptr) -> i32
    reg_func(m, store, "readdir",
        {ValueType::i32, ValueType::i32},
        {ValueType::i32}, fs_readdir);
    reg_func(m, store, "readdir",
        {ValueType::i32, ValueType::i64},
        {ValueType::i32}, fs_readdir);

    // closedir(dir_fd:i32) -> i32  — fd is always i32; no i64 variant needed
    reg_func(m, store, "closedir",
        {ValueType::i32},
        {ValueType::i32}, fs_closedir);
}
