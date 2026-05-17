Notes
========

Difference from Wasm spec
-------------------------

* No SIMD
* Multiple memory support
* Multiple function results & block parameters/results
* 64-bit memory address space (``memory64``) is supported alongside the
  32-bit default; per-memory selection is carried by :cpp:member:`Limits::is64`.

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