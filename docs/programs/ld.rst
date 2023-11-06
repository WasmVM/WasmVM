wasmvm-ld
=========

``wasmvm-ld`` - WasmVM WebAssembly linker

Synopsis
--------

wasmvm-ld [-h | --help] [-v | --version] [-c | --config]? [-s | --start]? [-e | --exports]? [-i | --imports]? output modules...

Description
-----------

``wasmvm-ld`` link multiple WebAssembly (Wasm) binary files into single file. Imports/exports will be resolved in the output module. The addresses in each function and segments will be updated to referenced to the new ones in the output module.
Besides, the linker can also collect the start function indices of linked modules, and compose a new start function in the output module.
It can also be configured by a JSON file for more flexibility.

Command line arguments and options
----------------------------------

Arguments
^^^^^^^^^

output
    The file name of output module binary, normally with ``.wasm`` file extension.

modules...
    One or more Wasm binary files to linked with, normally with ``.wasm`` file extension.

Options
^^^^^^^

--help, -h
    Show usage and additional help messages.

--version, -v
    Print version information.

--config, -c
    Path of JSON configuration file.

--start, -s
    Specify start function.

    Format: <module>[:<func_index>],<module>[:<func_index>]... | all

    Example:

        --start all

        --start mod.wasm

        --start mod.wasm:1

        --start mod1:1,mod2:2

--exports, -e
    Specify explicit exports

    Format: <module>:<name>:<desc_type>:<index>...

    Example:

        --exports mod1.wasm:table1:table:2

        --exports mod1.wasm:table1:table:2,mod2.wasm:m1:mem:1

--imports, -i
    Specify explicit imports

    Format: <module>:<name>...

    Example:

        --imports mod1.wasm:table1

        --imports mod1.wasm:table1,mod2.wasm:m1

Configuration file
------------------

The configuration file is in JSON format. It'll replace the default values, however, it will be overrided by command line option.

The priorities are: Command line option > Configure file > default values

Entries:

* "output": string

    The output file name, same as ``output`` argument. 

    Example:
    
    .. code-block:: json
        
        {
            "output": "out.wasm"
        }

* "modules": [string]

    List of Wasm binary files to linked, same as ``modules`` argument. 

    Example:

    .. code-block:: json

        {
            "modules": ["mod1.wasm", "mod2.wasm"]
        }

* "start": string | [object | string],

    Specify start function, same as ``--start`` option.

    2 formats are provided for flexibility:

    - Single string: same as ``--start`` option.

        Example: 

        .. code-block:: json

            {
                "start": "all"
            }

        .. code-block:: json
            
            {
                "start": "mod.wasm"
            }

        .. code-block:: json
            
            {
                "start": "mod.wasm:1"
            }

        .. code-block:: json
            
            {
                "start": "mod1.wasm:1,mod2.wasm:3"
            }

    - Array of objects or strings:

        Object format:

        1. Specify module file path, then use the function index in start section.

            .. code-block:: json
                
                {
                    "path": // string: file path
                }

        2. Specify module file path and function index.

            .. code-block:: json
                
                {
                    "path": // string: file path
                    "index": // number: func index
                }

        Strings and onjects can be mixed within the array.

        Example:

        .. code-block:: json
            
            {
                "start": [{
                    "path": "mod1"
                }]
            }

        .. code-block:: json
            
            {
                "start": [
                    {
                        "path": "mod1",
                        "index": 2
                    }
                ]
            }

        .. code-block:: json
            
            {
                "start": [ { "path": "mod1", "index": 2 }, "mod2.wasm:1", { "path": "mod3" } ]
            }


* "exports": string | [object | string],

    Explicit specify exports, same as ``--exports`` option.

    2 formats are provided for flexibility:

    - Single string: same as ``--exports`` option.

        Example: 

        .. code-block:: json
            
            {
                "exports": "mod1.wasm:table1:table:2"
            }

        .. code-block:: json
            
            {
                "exports": "mod1.wasm:table1:table:2,mod2.wasm:m1:mem:1"
            }

    - Array of objects or strings:

        Object format:

        .. code-block:: json
            
            {
                "path": // string: file path
                "name": // string: export name
                "type": // "func" | "table" | "mem" | "global": export descriptor type
                "index": // number: export descriptor index
            }

        Strings and onjects can be mixed within the array.

        Example:

        .. code-block:: json
            
            {
                "exports": [{
                    "path": "mod1",
                    "name": "fun1",
                    "type": "func",
                    "index": 2
                }]
            }

        .. code-block:: json
            
            {
                "exports": ["mod2.wasm:tab1:table:3"]
            }

        .. code-block:: json
            
            {
                "exports": [ { "path": "mod1", "name": "fun1", "type": "func", "index": 2}, "mod2.wasm:tab1:table:3"]
            }

* "imports": string | object,

    Explicit specify imports, same as ``--imports`` option.

    2 formats are provided for flexibility:

    - Single string: same as ``--imports`` option.

        Example: 

        .. code-block:: json
            
            {
                "imports": "mod1.wasm:table1"
            }

        .. code-block:: json
            
            {
                "imports": "mod1.wasm:table1,mod2.wasm:m1"
            }
    
    - Single object: a key-value map describing module and name

        Example: 

        .. code-block:: json
                
            {
                "mod1": "func1",
                "mod2.wasm": "table2"
            }

