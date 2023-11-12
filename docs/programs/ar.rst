wasmvm-ar
=========

``wasmvm-ar`` - WasmVM archiver

.. contents::
    :local:

Synopsis
--------

wasmvm-ar [-h | --help] [-v | --version] mode archive_file module_files...

Description
-----------

``wasmvm-ar`` is the tool for manipulating WebAssembly (Wasm) archive file.
There are 3 operation modes in ``wasmvm-ar``:

* Create mode: create archive.

* Extract mode: extrace Wasm binary files from archive.

* List mode: list modules in archive.

``wasmvm-ar`` uses its own binary format for archive file. See `archive format`_ for more information.

Command line arguments and options
----------------------------------

Arguments
^^^^^^^^^

mode
    Specify operation mode:

    * ``create`` (c): create mode
    
    * ``extract`` (x): extract mode

    * ``list`` (l): list mode

archive_file
    File name of the archive file (.a)

module_files...

    Create mode:

        One or more Wasm binary files to archive with, normally with ``.wasm`` file extension.

        The file name and archived module path can be different, with the following format:

        Format: <file_name>[:<module_path>]

        Example:

            ``mod1.wasm``

            ``mod1.wasm:foo/mod2``

        If ``--config`` option specified, the module files will be appended with the configuration paths.

    Extract mode:
        One or more Wasm binary files to extract from archive, normally with ``.wasm`` file extension.

Options
^^^^^^^

--help, -h
    Show usage and additional help messages.

--version, -v
    Print version information.

--prefix, -p
    Create mode:
        Extra prefix for archived module path.
    
    Extract mode:
        The folder to extract into. If not specified, default to the current working directory.

    List mode:
        Extra prefix applied to archived module path while listing.

--config, -f
    Create mode:
        Path of JSON configuration file.

Configuration file
------------------

The configuration file is in JSON format. Only used with create mode for flexibility.

The priorities are: Command line option > Configure file

Entries:

* "prefix": string

    The extra prefix, same as ``--prefix`` option.

* "paths": [object | string]

    Array of paths in string or object formats.

    - String: same as ``module_files`` argument.

            Example: 

            .. code-block:: json

                {
                    "paths": ["mod1.wasm"]
                }

            .. code-block:: json
                
                {
                    "paths": ["mod.wasm:foo.wasm"]
                }

    - Object: Specify binary file path ond optional archived module path.

        .. code-block:: json
                
            {
                "file": // string: binary file path
            }

        .. code-block:: json
                
            {
                "file": // string: binary file path
                "module": // string: archived module path
            }

        Example: 

        .. code-block:: json

            {
                "paths": [{
                    "file": "mod1.wasm"
                }]
            }

        .. code-block:: json
            
            {
                "paths": [{
                    "file": "mod1.wasm",
                    "module": "foo1.wasm"
                }]
            }

    String and object can be mixed together within the path array.

    Example: 

    .. code-block:: json

        {
            "paths": ["mod.wasm:foo.wasm", {
                "file": "mod1.wasm"
            }]
        }

.. _`archive format`:

Archive format
--------------

Convention
^^^^^^^^^^

* ``uint64``: 64 bits (8 bytes) unsigned integer in little-endian.

* ``uint32``: 32 bits (4 bytes) unsigned integer in little-endian.

* ``array(N)``: Array of N encoded with an ``uint32`` element count followed by elements.

* ``byte``: a single byte.

* ``string``: ``array(byte)``.

* ``len(N)``: An ``uint64`` denotes the byte length of ``N``.

Structure
^^^^^^^^^

.. math::
   \begin{array}{l}
   {magic} &::=& \mathtt{0x56}~\mathtt{0x4D}~\mathtt{0x41}~\mathtt{0x52} \\
   {version}  &::=& \mathtt{0x00}~\mathtt{0x01}~\mathtt{0x02}~\mathtt{0x00} \\
   {paths} &::=& {len}({paths}) ~ {array}({path})\\
   {contents} &::=& {len}({module}) ~ {module} ~ {contents} \\
              &::=& {len}({module}) ~ {module} \\
   \end{array}

Magic
+++++

4 bytes magic number as string ``VMAR``

Version
+++++++

4 bytes version number.

* Byte 1-2: major version
* Byte 3: minor version
* Byte 4: patch version

Path
++++

.. math::
   \begin{array}{l}
   {name} &::=& {string} \\
   {address} &::=& {uint64} \\
   \end{array}

* name: module name, normally as a file name with ``.wasm`` file extension.
* address: address of module content, originated from the beginning of archive file.

Module
++++++

Wasm module in binary format