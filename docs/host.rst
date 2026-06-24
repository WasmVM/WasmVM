Host modules
============

WasmVM ships with a built-in ``sysenv`` host that exposes a small POSIX-style
interface to wasm modules. It is split into two sub-modules — ``sys_fs`` for
filesystem operations and ``sys_proc`` for process / environment access — both
registered automatically by ``wasmvm`` unless ``--no-system`` is supplied.

WasmVM targets the 64-bit memory model (wasm64), so every function below is
registered with ``i64`` pointer and length parameters. Other scalar arguments
(``fd``, ``flags``, ``mode``, ``whence``, ``clk_id`` …) remain ``i32``.

Pointer / length convention
---------------------------

For functions that take buffers, parameter pairs named ``ptr`` and ``len`` are
``i64`` and refer to a contiguous range in memory 0 of the calling module.
Negative ``i32`` return values are POSIX-style errors: the magnitude equals the
host's ``errno`` after the failed syscall.

sys_fs
------

Filesystem operations.

.. list-table::
    :header-rows: 1
    :widths: 18 32 50

    * - Function
      - Signature
      - Description
    * - ``open``
      - ``(path_ptr, path_len, flags:i32, mode:i32) -> i32``
      - Open a path; returns a wasm fd (``>= 3``) or a negative error.
    * - ``close``
      - ``(fd:i32) -> i32``
      - Close a wasm fd.
    * - ``read``
      - ``(fd:i32, buf_ptr, buf_len) -> i32``
      - Read up to ``buf_len`` bytes; returns bytes read or negative error.
    * - ``write``
      - ``(fd:i32, buf_ptr, buf_len) -> i32``
      - Write up to ``buf_len`` bytes; returns bytes written or negative error.
    * - ``lseek``
      - ``(fd:i32, offset:i64, whence:i32) -> i64``
      - Seek; returns the new offset or a negative error.
    * - ``stat``
      - ``(path_ptr, path_len, stat_buf) -> i32``
      - Stat a path into ``stat_buf``.
    * - ``fstat``
      - ``(fd:i32, stat_buf) -> i32``
      - Stat by file descriptor.
    * - ``unlink``
      - ``(path_ptr, path_len) -> i32``
      - Remove a file.
    * - ``rename``
      - ``(old_ptr, old_len, new_ptr, new_len) -> i32``
      - Rename or move a path.
    * - ``mkdir``
      - ``(path_ptr, path_len, mode:i32) -> i32``
      - Create a directory.
    * - ``rmdir``
      - ``(path_ptr, path_len) -> i32``
      - Remove an empty directory.
    * - ``getcwd``
      - ``(buf_ptr, buf_len) -> i32``
      - Write the current working directory into ``buf``; returns the length
        written, including the terminating null byte.
    * - ``opendir``
      - ``(path_ptr, path_len) -> i32``
      - Open a directory iterator; returns a directory fd.
    * - ``readdir``
      - ``(dir_fd:i32, entry_ptr) -> i32``
      - Read the next entry into ``entry_ptr``; returns ``0`` when an entry was
        written, ``1`` at end-of-directory, or a negative error.
    * - ``closedir``
      - ``(dir_fd:i32) -> i32``
      - Close a directory iterator.

sys_proc
--------

Process and environment access.

.. list-table::
    :header-rows: 1
    :widths: 22 30 48

    * - Function
      - Signature
      - Description
    * - ``exit``
      - ``(code:i32) -> ()``
      - Terminate the wasm program with the given exit code.
    * - ``argc``
      - ``() -> i32``
      - Number of command-line arguments. Argument 0 is the main module
        path; subsequent arguments come from ``wasmvm --args``.
    * - ``argv_len``
      - ``(idx:i32) -> i32``
      - Length in bytes of argument ``idx``.
    * - ``argv``
      - ``(idx:i32, buf_ptr, buf_len) -> i32``
      - Copy argument ``idx`` into ``buf``; returns bytes written.
    * - ``getenv``
      - ``(name_ptr, name_len, buf_ptr, buf_len) -> i32``
      - Look up an environment variable and copy it into ``buf``;
        returns bytes written or a negative error.
    * - ``clock_gettime``
      - ``(clk_id:i32, ts_ptr) -> i32``
      - Read a monotonic / realtime clock into a ``timespec``-shaped buffer.

Disabling the host
------------------

Pass ``--no-system`` (``-ns``) to ``wasmvm`` to disable automatic
instantiation of ``sys_fs`` and ``sys_proc``. This is useful when running
modules that should not be granted filesystem or process access.
