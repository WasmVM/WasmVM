`v1.4`
# v1.4 Correctness fixes, getopt-style CLI, wasm64-only, tooling cleanup

Highlights since v1.3:

- **Execution correctness**: f64 arithmetic now pushes a double-width
  NaN (not a float-width one); out-of-range float-to-int truncation
  traps; `return` arity and the `i64` comparison result type are fixed.
- **Host / sysenv fixes**: host functions now access the *caller's*
  linear memory, and syscall memory access honors tagged pointers.
- **wasm64-only host interface**: `sys_fs` / `sys_proc` dropped wasm32
  support and require `i64` pointers, matching the 64-bit memory model.
- **Decoder / loader fixes**: multi-field stream values are read in a
  defined order, and module files are opened in binary mode (no more
  `0x0d`/`0x1a` mangling on Windows).
- **Validator**: `call_ref` is now accepted by the function validator.
- **getopt-style CLI**: a reworked `CommandParser` adds the `--extra`/`-e`
  flag, matches multi-character short aliases such as `-ns` / `-np`, and
  repairs the broken `--no-system` and `readwasm --start` flags.
- **wasmvm-ar**: correct archive-header magic is written, and `--config`
  is optional in create mode.
- **MSVC / Windows**: Release builds and installs work under MSVC, and
  the test runner copies `WasmVM.dll` next to the test binaries.
- **CI & tooling cleanup**: job timeouts and run de-duplication, Windows
  validated on `master`, docs built on `ubuntu-latest` via apt Sphinx,
  and the unused `external/`, `.devcontainer`, `.vscode`, and `docker/`
  scaffolding retired.
- **Documentation**: the reStructuredText reference docs are realigned
  with the current codebase (types, instances, host interface, opcodes).

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
