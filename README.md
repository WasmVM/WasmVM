# WasmVM

#### A standalone WebAssembly virtual machine 

**[online document](https://wasmvm.github.io/WasmVM/)**

**[正體中文](README_zh.md)**

## Prerequisite

* CMake >= 3.16

* A C/C++ compiler supporting C11 and C++20

## Notice
 
1. This project uses **"Taiwan Traditional Chinese"** as primary, English as secondary language in documents.
  
  Other languages (Ex: Simplified Chinese) are only used as references or translations.

## Build

1. Run CMake

```shell
mkdir build && cd build && cmake ..
```

2. Run Make

```shell
make -j4
```

### Build options

Pass these to CMake with `-D<option>=ON`. Both default to `OFF` and can be combined.

* `WASMVM_DEBUG`: debugger support. Adds `wasmvm --dap [address:]port`, a Debug Adapter Protocol server, and the `Debugger` class in `Debugger.hpp`.

* `WASMVM_PROFILE`: profiler support. Adds `wasmvm --profile <prefix>`, which writes `<prefix>.json` and `<prefix>.folded`, and the `Profiler` class in `Profiler.hpp`.

The options change the members of `Observers` and so the layout of `Stack`, so code built against the installed headers must define `WASMVM_DEBUG` and `WASMVM_PROFILE` exactly as the library was built.

```shell
mkdir build && cd build && cmake -DWASMVM_DEBUG=ON -DWASMVM_PROFILE=ON ..
```

## Run Unittests

1. Run CMake

```shell
mkdir build && cd build && cmake ..
```

2. Run Make with the test target

```shell
make test
```

## Generate HTML documents

1. Run CMake

```shell
mkdir build && cd build && cmake ..
```

2. Run Make

```shell
make docs
```

The generated document will under `build/docs/html`