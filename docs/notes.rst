Notes
========

Difference from Wasm spec
-------------------------

* No SIMD
* Multiple memory support
* Multiple function results & block parameters/results
* 64-bit memory address space (``memory64``) is supported alongside the
  32-bit default; per-memory selection is carried by :cpp:member:`Limits::is64`.
* GC proposal. Struct, array, ``sub`` and ``rec`` type definitions and the
  typed reference value types ``(ref null? heaptype)`` are parsed, encoded,
  decoded, dumped, linked, validated and executed; everything a guest
  allocates is reclaimed; and a table or an elem segment may hold any
  reference type. A table's element type must be nullable, since every slot
  starts null and the type carries no initialiser expression.

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

* The custom ``name`` section is kept: the decoder reads it into
  ``WasmModule::names`` (the spec's subsections and the extended-name-section
  proposal's), it is copied to ``ModuleInst::names`` at instantiation, and the
  encoder writes it back. ``wasmvm-as`` fills it from the text's ids,
  ``wasmvm-ld`` carries it through linking, ``readwasm`` prints the names, and
  both ``wasmvm-as`` and ``wasmvm-ld`` take ``--strip-names``. A malformed
  name section is dropped with a warning. Custom sections are now read at any
  position, several in a row.
* Debugger, under the new CMake option ``WASMVM_DEBUG``: the
  :cpp:struct:`WasmVM::Debugger` class stops the guest between wasm
  instructions at breakpoints, on steps, on pause, on a trap and on an
  uncaught exception, and ``wasmvm --dap [address:]port`` drives it from a
  Debug Adapter Protocol client over TCP. See :doc:`programs/vm`.
* Profiler, under the new CMake option ``WASMVM_PROFILE``: the
  :cpp:struct:`WasmVM::Profiler` class records instruction counts per
  function and opcode, call counts and inclusive / exclusive time, and
  ``wasmvm --profile <prefix>`` writes them to ``<prefix>.json`` and
  ``<prefix>.folded``. See :doc:`programs/vm`.
* :cpp:struct:`WasmVM::Stack` and ``module_instanciate`` take an
  :cpp:struct:`WasmVM::Observers` parameter for the debugger and profiler;
  its members depend on ``WASMVM_DEBUG`` and ``WASMVM_PROFILE``, so code
  built against the installed headers must define them as the library was
  built. ``Stack::frames``, ``Frame::labels`` and ``Label::values`` can be
  walked read-only.
* JSON, as read by ``wasmvm-ld --config``: negative numbers, 64-bit
  integers, ``\u`` escapes and empty arrays and objects are parsed
  correctly, and strings are escaped when written.

* A table's and an elem segment's element type is a
  :cpp:struct:`WasmVM::ValueType` that has to be a reference, where it was a
  two-valued ``RefType``. Any reference type may now be named, and the
  instructions moving values through a table are checked against the
  subtyping relation rather than equality. ``RefType`` and ``Ref`` are gone
  from the interface; a table slot and an elem segment item hold a
  :cpp:type:`WasmVM::Value`.

* Everything a guest allocates is reclaimed, cycles included. Reference
  counting does the everyday work, and a cycle collector -- Bacon and Rajan's
  synchronous trial deletion -- settles what a count cannot see. It works
  from the objects whose count was decremented without reaching zero, so it
  never enumerates roots, which matters because a :cpp:type:`WasmVM::Value`
  can be held by the embedder where no tracing collector could find it.
  :cpp:func:`WasmVM::gc_collect` forces a collection and
  :cpp:func:`WasmVM::gc_live_count` reports what is outstanding; an embedder
  need call neither.

* The GC proposal's struct and array instructions execute. A struct or array
  is an object on the heap carrying the index of its type; a packed field
  keeps its own width, and the spec's traps -- a null reference, an index
  outside the array, a segment read past its end -- are raised as
  :cpp:struct:`WasmVM::Exception::Trap`.
* A value type is a number type or a reference type
  (:cpp:struct:`WasmVM::ValueType`), and a reference type is
  ``(ref null? heaptype)`` over an abstract heap type or a type index. The
  legacy spellings are the nullable abstract forms: ``funcref`` is
  ``(ref null func)``. A reference to a struct or array type can be a
  parameter, a result, a local, a global, a struct field and an array
  element.
* The subtyping relation is implemented and used: operands are accepted at
  any subtype of what is expected, and a declared supertype must actually be
  one.
* ``ref.null`` takes any heap type, abstract or concrete. It took only
  ``func`` and ``extern``, which left a null struct or array reference
  impossible to write.
* ``ref.test``, ``ref.cast``, ``br_on_cast`` and ``br_on_cast_fail`` are
  validated. They executed but had no case in the function validator, so
  ``ref.test`` did not push its ``i32`` and ``ref.cast`` did not refine its
  operand -- which left a cast unable to do the one thing it is for, turning
  an abstract reference into a concrete one.

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