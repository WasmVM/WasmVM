wasmvm-as
=========

``wasmvm-as`` - WasmVM WebAssembly assembler

.. contents::
    :local:

Synopsis
--------

wasmvm-as [-h | --help] [-v | --version] [-f | --force] [-o | --output]? input_file

Description
-----------

``wasmvm-as`` assembles WebAssembly (Wasm) text format to binary format. It'll parse Wasm text file (``.wat``) into Wasm modules, verify, and encode Wasm module into Wasm binary file (``.wasm``).
While parsing Wasm text, it will also re-order import sections to the lower index, and fix the index of  influenced instructions or sections.
This re-index is essential since import sections prior to functions, tables, memories and globals, so the indices may differ from the appearance in text format. The actual index is the one after applying this process.

Command line arguments and options
----------------------------------

Arguments
^^^^^^^^^

input_file
    The WebAssembly text file, normally with ``.wat`` file extension.

Options
^^^^^^^

--help, -h
    Show usage and additional help messages.

--version, -v
    Print version information.

--force, -f
    Skip validation and just parse and encode the module.

--output, -o
    Spcify the output file name. This will consume an additional argument as the file name.
    Without the ``--output`` option, output file will be named as replacing the file extension with ``.wasm``.