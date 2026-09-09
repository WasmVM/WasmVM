Notes
========

Difference from Wasm spec
-------------------------

* No SIMD
* Multiple memory support
* Multiple function results & block parameters/results
* 64-bit memory address space (``memory64``) is supported alongside the
  32-bit default; per-memory selection is carried by :cpp:member:`Limits::is64`.
* GC proposal, partially: struct, array, ``sub`` and ``rec`` type definitions
  are parsed, encoded, decoded, dumped, linked and validated, and the
  ``struct.*`` / ``array.*`` instructions are validated against them. The
  typed reference value types ``(ref null? $t)`` are not representable yet,
  so a reference to a struct or array type exists on the validator's operand
  stack only -- it cannot be held in a local, global, parameter or result --
  and the subtyping relation between two composite types is not checked.
  The interpreter does not execute the GC instructions: they trap.

What's new in 1.3
-----------------

* New custom recursive-descent WAT parser replacing the previous ANTLR4-based
  one — faster builds and a smaller binary.
* Parser, validator and encoder support for the exception-handling proposal
  (``try_table``, ``throw``, ``throw_ref``) and for the GC / typed-references
  opcodes (struct, array, ``ref.test`` / ``ref.cast``, ``i31``).
* New POSIX-style host modules ``sys_fs`` and ``sys_proc`` — see
  :doc:`host`.
* ``wasmvm --args ...`` forwards command-line arguments to the running module
  via ``sys_proc.argv*``.
* Windows / MSVC build support; CI runs on Linux, macOS and Windows.

Unreleased
----------

* Type section entries are subtypes (:cpp:struct:`WasmVM::SubType`): a
  function, struct or array composite type, optionally ``(sub final? $super
  comptype)``, optionally inside a ``(rec ...)`` group recorded in
  :cpp:member:`WasmVM::WasmModule::recs`. Fields and array elements have a
  storage type -- a value type or the packed ``i8`` / ``i16`` -- and a
  mutability. See the GC entry under *Difference from Wasm spec* for what is
  still missing.
* The validator checks every ``struct.*`` / ``array.*`` instruction against
  the type it names, and the type section's own rules (one supertype at
  most, defined earlier, not final, same kind).
* Type indices in ``call_ref``, ``return_call_ref``,
  ``return_call_indirect``, the GC instructions and the concrete heap types
  of ``ref.test`` / ``ref.cast`` / ``br_on_cast`` now follow type
  deduplication in the parser and relocation in the linker.