readwasm
========

``readwasm`` - WasmVM WebAssembly binary file reader

Synopsis
--------

readwasm [-h | --help] [-v | --version] [-f | --force] [-a | --all] [-t | --type] [-i | --import] [-c | --func] [-b | --table] [-m | --memory] [-g | --global] [-x | --export] [-s | --start] [-e | --elem] [-d | --data] input_file

Description
-----------

``readwasm`` read WebAssembly (Wasm) binary file and dump text format.
The binary file will be decoded, verified and then dump into standard output.
Options are provided to toggle whether each section dumps or not.

Command line arguments and options
----------------------------------

Arguments
^^^^^^^^^

input_file
    The WebAssembly binary file, normally with ``.wasm`` file extension.

Options
^^^^^^^

--help, -h
    Show usage and additional help messages.

--version, -v
    Print version information.

--force, -f
    Skip validation, just decode and print the sections.

--all, -a
    Print all sections (default)

--type, -t
    Print type sections. This is ignored if --all appears.

--import, -i
    Print import sections. This is ignored if --all appears.

--func, -c
    Print func sections. This is ignored if --all appears.

--table, -b
    Print table sections. This is ignored if --all appears.

--memory, -m
    Print memory sections. This is ignored if --all appears.

--global, -g
    Print global sections. This is ignored if --all appears.

--export, -x
    Print export sections. This is ignored if --all appears.

--start, -s
    Print start section. This is ignored if --all appears.

--elem, -e
    Print elem sections. This is ignored if --all appears.

--data, -d
    Print data sections. This is ignored if --all appears.