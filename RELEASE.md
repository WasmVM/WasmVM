`v1.6`
# v1.6 Allocation failure semantics, typed-reference correctness, validator boundaries

Highlights since v1.5:

- **Allocation failure**: `memory.grow` and `table.grow` answer `-1` when
  the host cannot allocate (they aborted or trapped before); instantiation
  fails cleanly instead of trapping when a memory or table cannot be
  allocated; 64-bit growth is bounded by `vector::max_size` and the
  resulting `length_error` is caught.
- **Validator boundaries**: a 32-bit memory is limited to 2^16 pages; a
  memory whose byte count overflows is refused rather than wrapped to an
  empty one; the `max` of an i32 table is bounded at 2^32-1, not 2^48;
  import limits of memories and tables are matched per spec.
- **Typed references**: a non-null `funcref`/`externref` no longer matches
  `nofunc`/`noextern` in `ref.test` / `ref.cast` / `br_on_cast`;
  structural refinement of a funcref is accepted; casts and tests against a
  concrete function type follow declared subtyping across modules (cross-
  module type identity for struct/array casts, and canonical import matching
  with host-function identities); a global's `ref.func` initialiser stores
  the function's store address; `call_indirect` compares its table index as
  unsigned explicitly, and `return_call_indirect` traps on a negative one;
  an embedder `StructObj`/`ArrayObj` with an unset canon matches no concrete
  type.
- **Arithmetic and embedder**: `f64.convert_i32_u` reads its operand as
  unsigned; a saturating truncation returns the value it truncates to (with
  tests); an embedder can catch what the library throws, LTO or not;
  `--force` bounds-checks function indices before indexing `funcaddrs[]`.

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
