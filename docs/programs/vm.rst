wasmvm
======

``wasmvm`` - A WebAssembly process virtual machine

.. contents::
    :local:

Synopsis
--------

wasmvm [-h | --help] [-v | --version] [-ns | --no-system] [-np | --no-parent] [-f | --force] [-e | --extra]... [--dap [address:]port] [--profile prefix] [--] main_module [args...]

``--dap`` exists only in a build with ``WASMVM_DEBUG``, and ``--profile`` only in a build with ``WASMVM_PROFILE``.

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

--dap [address:]port
    Only with ``WASMVM_DEBUG``. Run the program under a Debug Adapter Protocol
    client instead of directly; see `Debugging with DAP`_. ``address`` defaults
    to ``127.0.0.1``; an IPv6 address is written in brackets, e.g.
    ``[::1]:4711``. Port ``0`` lets the system choose one.

--profile prefix
    Only with ``WASMVM_PROFILE``. Profile the program and write
    ``<prefix>.json`` and ``<prefix>.folded``; see `Profile output`_. If either
    file cannot be written, an error is printed and a run that would have
    exited with 0 exits with -1.

\--
    End of options. All arguments after ``--`` are treated as positional,
    allowing ``main_module`` or ``args`` values that begin with ``-``.

Debugging with DAP
------------------

``wasmvm --dap [address:]port main.wasm args...`` loads and decodes every
module, prints the address it listens on to stderr, and waits for one DAP
client to connect over TCP. The program and the other options stay on the
command line; nothing is instantiated until the client has sent ``launch`` and
``configurationDone``. The connection is TCP because the guest uses the
process's standard input and output.

Example:

.. code-block:: shell

    wasmvm --dap 4711 main.wasm

The client connects to that port, for example through ``debugServer`` in a
VS Code ``launch.json``. WasmVM does not ship a VS Code extension, so the debug
type that configuration names has to be declared by an extension of your own.

What the server provides:

* **Sources.** Each function defined in a module is a read-only virtual WAT
  source: line 1 is the function header, line *k* + 2 is instruction *k* of the
  body, and the function's trailing ``end`` is the last line. Names from the
  name section are shown, parameters included.
* **Breakpoints** on those lines, set before or during the run. Breakpoints
  in a module with a start function are bound before that start function runs.
* **Stopping** on entry (``stopOnEntry`` in ``launch``), at a breakpoint, on
  ``pause``, after ``next`` / ``stepIn`` / ``stepOut``, and on two exception
  filters, ``trap`` and ``uncaught`` (an uncaught wasm exception), both on by
  default. Steps stop only between wasm instructions, so a call to a host
  function is stepped over as one unit.
* **Inspection** while stopped: the call stack, and per frame the scopes
  *Locals*, *Operand stack* (grouped by label), *Globals* and *Memory*.
  Integers are shown in signed decimal and in hexadecimal. Mutable globals can
  be changed (``setVariable``), and memory can be read (``readMemory``).
* **Ending.** ``disconnect`` ends the program unless ``terminateDebuggee`` is
  ``false``; with ``false`` the program runs on with no breakpoints and no
  exception stops. A client that closes the connection without
  ``disconnect`` ends the program too. A program ended by the client exits
  with status 1; a guest inside a long host function call ends at its next
  wasm instruction. After the program ends by itself, the server waits for
  the client's ``disconnect``.

Supported requests: ``initialize``, ``launch``, ``configurationDone``,
``setBreakpoints``, ``setExceptionBreakpoints``, ``threads``, ``stackTrace``,
``scopes``, ``variables``, ``source``, ``loadedSources``, ``continue``,
``next``, ``stepIn``, ``stepOut``, ``pause``, ``setVariable``,
``readMemory``, ``disconnect``. Events: ``initialized``, ``loadedSource``,
``stopped``, ``exited``, ``terminated``.

``--dap`` can be combined with ``--profile``. Time the program spends stopped
in the debugger is not counted in the profile.

Profile output
--------------

``wasmvm --profile <prefix>`` records, for every function that runs, how many
times it was called, its inclusive and exclusive time, and how many times each
instruction ran in it. The two files are written however the program ends:
normal return, trap, uncaught exception or ``sys_proc`` exit.

* Every ``Stack::run()`` is a separate section, so each start function is
  reported on its own.
* Times are in nanoseconds of a steady clock, read on function entry and exit
  only. A function's inclusive time counts only its outermost call while it is
  recursive.
* A tail call (``return_call``, ``return_call_indirect``, ``return_call_ref``)
  is the exit of the calling function and the entry of the called one, under
  the original caller.
* When an exception or a trap unwinds frames, they exit at that moment.
* Host functions are listed with their calls and time, but no instruction
  counts.
* A function is named ``$name`` from the name section; otherwise by its export
  name; otherwise ``func N`` (its index in the module); otherwise
  ``funcaddr A`` (its store address). The module is named after its file.

``<prefix>.json`` is the full report:

.. code-block:: json

    {
        "stacks": [
            {
                "entry": "main.wasm:$main",
                "functions": [
                    {
                        "calls": 1,
                        "exclusive_ns": 21000,
                        "funcaddr": 3,
                        "host": false,
                        "inclusive_ns": 52000,
                        "instructions": { "call": 2, "end": 1, "i32.const": 4 },
                        "instructions_total": 7,
                        "module": "main.wasm",
                        "name": "$main"
                    }
                ],
                "index": 0
            }
        ],
        "time_unit": "ns",
        "version": 1
    }

``stacks`` has one entry per section, in the order they started; ``entry`` is
the function the section started in. Keys are written in alphabetical order. ``instructions_total`` and
``instructions`` are absent for host functions. The numbers above are only an
illustration of the shape.

``<prefix>.folded`` holds collapsed stacks, the input format of flame graph
tools such as ``flamegraph.pl``: one line per call path, frames written
``module:name`` (only ``name`` when the module has no name) and separated by
``;``, then a space and the exclusive
nanoseconds spent on that path. A ``;`` or a space inside a frame name is
written as ``_``. The lines of all sections follow one another.
