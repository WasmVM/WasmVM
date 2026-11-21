`v1.7`
# v1.7 Debugger, profiler and the name section

Highlights since v1.6:

- **Debugger** (CMake option `WASMVM_DEBUG`, off by default): `wasmvm --dap
  [address:]port` runs the program under a Debug Adapter Protocol client over
  TCP, with breakpoints, `next` / `stepIn` / `stepOut`, pause, stops on traps
  and uncaught exceptions, a virtual WAT source per function, and locals, the
  operand stack, globals and memory while stopped. Embedders get the
  `Debugger` class in `Debugger.hpp`.
- **Profiler** (CMake option `WASMVM_PROFILE`, off by default): `wasmvm
  --profile <prefix>` writes `<prefix>.json`, with instruction counts per
  function and opcode, call counts and inclusive / exclusive time per
  function, and `<prefix>.folded`, collapsed stacks for flame graphs. Both are
  written however the program ends. Embedders get the `Profiler` class in
  `Profiler.hpp`.
- **Name section**: decoded, kept on `WasmModule` and `ModuleInst`, and
  encoded again; `wasmvm-as` fills it from the text's ids, `wasmvm-ld`
  carries it through linking, `readwasm` prints the names, and `wasmvm-as` /
  `wasmvm-ld` take `--strip-names`.
- **Embedder interface**: `Stack` and `module_instanciate` take an
  `Observers` parameter whose members depend on `WASMVM_DEBUG` and
  `WASMVM_PROFILE`; code built against the installed headers must define them
  as the library was built. Stack frames, labels and operand values can be
  walked read-only.
- **JSON** (`wasmvm-ld --config`): negative numbers, 64-bit integers, `\u`
  escapes and empty arrays and objects are parsed correctly, and strings are
  escaped when written; input that ends inside a string throws instead of
  hanging, and a truncated `true` / `false` / `null` no longer reads
  uninitialized bytes.
- **Validator**: an instruction after a function's closing `end` is refused;
  before, a 25-byte module could crash `wasmvm` and `readwasm`.
- **`wasmvm-as`** (behaviour changes):
  - `(type x)` with inline params / results is `x` itself; mismatching
    inline params or results are a parse error. Before, a new type was made
    with them appended.
  - A `(type x)` and every type id may refer to a type defined later in the
    module; a numeric forward `(type N)` used to crash the parser.
  - Types are numbered as in the spec: definitions first, then the types
    inline typeuses insert.
  - A block label is bound only inside its own block; sibling blocks may
    reuse it, and a `br` to a closed block's label is an error instead of a
    branch to another block.
- **`wasmvm-ld`** keeps each function's and tag's own type; a function whose
  type was in a `rec` group, had a supertype or was not final was relinked
  to type 0.
- **`readwasm`** prints `(memory x)` exports and `call_indirect` /
  `return_call_indirect` types as `(type x)`, so its output reassembles.

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
