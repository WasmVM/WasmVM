`v1.3`
# v1.3 New parser, GC/exception handling, sysenv, cross-platform

Highlights since v1.2.2:

- **New WAT parser**: replaced the ANTLR4-based parser with a custom
  recursive-descent implementation — faster build, smaller binary.
- **Exception handling & GC instructions**: parser support for the
  reference-types / exception-handling / GC proposal opcodes.
- **POSIX-style host interface**: new `sys_fs` and `sys_proc` host
  modules expose filesystem and process syscalls to wasm.
- **memory64**: sys_fs and sys_proc accept i64 pointers for wasm64
  callers; parser/validator updated for 64-bit memory.
- **Argument passthrough**: `wasmvm --args ...` forwarded to host
  modules via `sys_proc.argv*`.
- **Windows support**: sysenv now compiles on MSVC; CI builds and
  tests on Linux, macOS, and Windows.
- **Validator / encoder fixes**: br_table relative-depth indexing,
  i64.store alignment bound, call_indirect / return_call_indirect
  encoder argument order, and clearer error messages.
- **Build improvements**: lower compile-time memory and smaller
  binary via a WasmInstr refactor.

## Install guides

### Ubuntu

下載 .deb 後使用 apt-get 指令安裝
Download the .deb file, then use `apt-get install` to install

Command:
```sh
sudo apt-get install <deb-file-path>
```

### MacOS

下載後點開 .pkg 檔，照著安裝程式的步驟安裝
Download and double click the .pkg file, then follow the steps in installer

### Windows

下載後點開 .exe 檔，照著安裝程式的步驟安裝
Download and double click the .exe file, then follow the steps in installer
