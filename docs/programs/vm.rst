wasmvm
======

``wasmvm`` - A WebAssembly process virtual machine

.. contents::
    :local:

Synopsis
--------

wasmvm [-h | --help] [-v | --version] [-ns | --no-system] [-np | --no-parent] [-f | --force] [-e | --extra]... [--] main_module [args...]

Description
-----------

``wasmvm`` is the main program to execute a WebAssembly application. It's responsible for providing host modules, resolving runtime dependencies, verifying binary codes, instanciating modules, and finally launch the application.

Command line arguments and options
----------------------------------

Arguments
^^^^^^^^^

main_module
    The main WebAssembly modules in binary format, with ``.wasm`` file extension normally.

args...
    Zero or more arguments forwarded to the running module. They are exposed to
    the wasm program through the ``sys_proc`` host module (``argc``, ``argv``)
    along with the main module path as argument 0.

Options
^^^^^^^

--help, -h
    Show usage and additional help messages.

--version, -v
    Print version information.

--no-system, -ns
    Disable importing modules from system module path.

--no-parent, -np
    Disable importing modules from parent path of main module.

--force, -f
    Skip validation and directly execute applicaion.

--extra, -e
    Add a path to search for referenced WebAssembly modules. May be specified
    multiple times.

\--
    End of options. All arguments after ``--`` are treated as positional,
    allowing ``main_module`` or ``args`` values that begin with ``-``.