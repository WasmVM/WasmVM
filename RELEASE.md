`v1.5`
# v1.5 GC types, exception handling, embedder API, trap correctness

Highlights since v1.4:

- **GC proposal**: `struct`, `array`, `sub` and `rec` type definitions
  are parsed, encoded, dumped and validated; a value type now carries
  nullability and a type index, so typed references such as
  `(ref null $t)` are real throughout the pipeline; the `struct.*` and
  `array.*` instructions execute, the cast instructions validate, and
  reference cycles a guest allocates are collected instead of leaked.
- **Exception handling**: `try_table`, `throw` and `throw_ref` are
  validated (a module carrying `try_table` was rejected outright
  before), catch labels resolve in the enclosing context, `exnref`
  decodes and is accepted as a local, the tag section is emitted so an
  assembled module with a tag loads, tags survive linking with their
  indices remapped, `(tag ...)` is printed when dumping to text, and an
  omitted typeuse on a tag is the empty type rather than type index 0.
- **Embedder API**: the installed `<exception.hpp>` now declares `Trap`,
  `Uncaught` and `interrupted`, so an embedder can catch what the
  headers document; `Stack::run()` takes an optional external interrupt
  flag to stop a guest that has entered an infinite loop.
- **Trap and arithmetic correctness**: integer division by zero and
  `INT_MIN / -1` trap; `f32`/`f64` `min` and `max` return NaN when
  either operand is NaN; shift counts are taken modulo the operand
  width; `memory.copy` checks the destination against its own memory
  and copies like `memmove`; `table.get` / `table.set` trap when out of
  bounds; `table.grow` answers with the length before the growth; an
  address or a count near the top of the range no longer wraps.
- **Validator hardening**: twelve instructions that could crash the host
  are validated instead, a named local is indexed after the parameters,
  and a branch to depth 0 hands on its operands once rather than twice.
- **Tables**: a table or an elem segment may name any reference type,
  and both are validated accordingly.
- **Encoder and parser fixes**: a typed select keeps its valtypes,
  `memory.init` keeps `memidx` and `dataidx` apart, and the text parser
  accepts unsigned `inf`/`nan` and integer-shaped float literals.
- **Tests & CI**: every instruction fixture is read back out of the
  binary, and a GitLab CI pipeline covers build, test and docs.

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
