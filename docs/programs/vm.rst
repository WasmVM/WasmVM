wasmvm
======

``wasmvm`` - A WebAssembly process virtual machine

.. contents::
    :local:

Synopsis
--------

wasmvm [-h | --help] [-v | --version] [-ns | --no-system] [-np | --no-parent] [-f | --force] [-a | --args ...] main_module extra_path...

Description
-----------

``wasmvm`` is the main program to execute a WebAssembly application. It's responsible for providing host modules, resolving runtime dependencies, verifying binary codes, instanciating modules, and finally launch the application.

Command line arguments and options
----------------------------------

Arguments
^^^^^^^^^

main_module
    The main WebAssembly modules in binary format, with ``.wasm`` file extension normally.

extra_path...
    Zero or more additional paths to find referenced WebAssembly modules. 

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

--args, -a
    Forward zero or more arguments to the running module. They are exposed to
    the wasm program through the ``sys_proc`` host module (``argc``, ``argv``)
    along with the main module path as argument 0.