// Copyright 2025 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef WASMVM_HOST_SYSENV_COMPAT_DEF
#define WASMVM_HOST_SYSENV_COMPAT_DEF

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <utility>

#ifdef _WIN32
  #include <io.h>
  #include <direct.h>
  #include <fcntl.h>
  #include <sys/stat.h>
  #include <stdio.h>

  using ssize_t = std::ptrdiff_t;
  using mode_t  = int;
#else
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/stat.h>
#endif

namespace WasmVM::sysenv_compat {

#ifdef _WIN32
using stat_t = struct ::_stat64;
inline int sys_open(const char* path, int flags, mode_t mode) { return ::_open(path, flags, mode); }
inline int sys_close(int fd) { return ::_close(fd); }
inline ssize_t sys_read(int fd, void* buf, std::size_t n) { return ::_read(fd, buf, (unsigned)n); }
inline ssize_t sys_write(int fd, const void* buf, std::size_t n) { return ::_write(fd, buf, (unsigned)n); }
inline std::int64_t sys_lseek(int fd, std::int64_t off, int whence) { return ::_lseeki64(fd, off, whence); }
inline int sys_unlink(const char* p) { return ::_unlink(p); }
inline int sys_rmdir(const char* p) { return ::_rmdir(p); }
inline int sys_mkdir(const char* p, mode_t /*mode*/) { return ::_mkdir(p); }
inline int sys_rename(const char* a, const char* b) { return ::rename(a, b); }
inline char* sys_getcwd(char* buf, std::size_t n) { return ::_getcwd(buf, (int)n); }
inline int sys_stat(const char* path, stat_t* st) { return ::_stat64(path, st); }
inline int sys_fstat(int fd, stat_t* st) { return ::_fstat64(fd, st); }
#else
using stat_t = struct ::stat;
inline int sys_open(const char* path, int flags, mode_t mode) { return ::open(path, flags, mode); }
inline int sys_close(int fd) { return ::close(fd); }
inline ssize_t sys_read(int fd, void* buf, std::size_t n) { return ::read(fd, buf, n); }
inline ssize_t sys_write(int fd, const void* buf, std::size_t n) { return ::write(fd, buf, n); }
inline std::int64_t sys_lseek(int fd, std::int64_t off, int whence) {
    return (std::int64_t)::lseek(fd, (off_t)off, whence);
}
inline int sys_unlink(const char* p) { return ::unlink(p); }
inline int sys_rmdir(const char* p) { return ::rmdir(p); }
inline int sys_mkdir(const char* p, mode_t mode) { return ::mkdir(p, mode); }
inline int sys_rename(const char* a, const char* b) { return ::rename(a, b); }
inline char* sys_getcwd(char* buf, std::size_t n) { return ::getcwd(buf, n); }
inline int sys_stat(const char* path, stat_t* st) { return ::stat(path, st); }
inline int sys_fstat(int fd, stat_t* st) { return ::fstat(fd, st); }
#endif

inline std::uint64_t stat_atime_ns(const stat_t& st) {
#if defined(_WIN32)
    return (std::uint64_t)st.st_atime * 1000000000ULL;
#elif defined(__APPLE__)
    return (std::uint64_t)st.st_atimespec.tv_sec * 1000000000ULL + (std::uint64_t)st.st_atimespec.tv_nsec;
#else
    return (std::uint64_t)st.st_atim.tv_sec * 1000000000ULL + (std::uint64_t)st.st_atim.tv_nsec;
#endif
}

inline std::uint64_t stat_mtime_ns(const stat_t& st) {
#if defined(_WIN32)
    return (std::uint64_t)st.st_mtime * 1000000000ULL;
#elif defined(__APPLE__)
    return (std::uint64_t)st.st_mtimespec.tv_sec * 1000000000ULL + (std::uint64_t)st.st_mtimespec.tv_nsec;
#else
    return (std::uint64_t)st.st_mtim.tv_sec * 1000000000ULL + (std::uint64_t)st.st_mtim.tv_nsec;
#endif
}

inline std::uint64_t stat_ctime_ns(const stat_t& st) {
#if defined(_WIN32)
    return (std::uint64_t)st.st_ctime * 1000000000ULL;
#elif defined(__APPLE__)
    return (std::uint64_t)st.st_ctimespec.tv_sec * 1000000000ULL + (std::uint64_t)st.st_ctimespec.tv_nsec;
#else
    return (std::uint64_t)st.st_ctim.tv_sec * 1000000000ULL + (std::uint64_t)st.st_ctim.tv_nsec;
#endif
}

// Returns (seconds, nanoseconds) since the relevant epoch.
// monotonic=false → wall clock (system_clock)
// monotonic=true  → monotonic clock (steady_clock)
inline std::pair<std::int64_t, std::int32_t> clock_now_ns(bool monotonic) {
    if(monotonic) {
        auto d = std::chrono::steady_clock::now().time_since_epoch();
        auto s = std::chrono::duration_cast<std::chrono::seconds>(d);
        auto n = std::chrono::duration_cast<std::chrono::nanoseconds>(d - s);
        return {(std::int64_t)s.count(), (std::int32_t)n.count()};
    }
    auto d = std::chrono::system_clock::now().time_since_epoch();
    auto s = std::chrono::duration_cast<std::chrono::seconds>(d);
    auto n = std::chrono::duration_cast<std::chrono::nanoseconds>(d - s);
    return {(std::int64_t)s.count(), (std::int32_t)n.count()};
}

} // namespace WasmVM::sysenv_compat

#endif // WASMVM_HOST_SYSENV_COMPAT_DEF
