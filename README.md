# WasmVM

#### A standalone WebAssembly virtual machine 

**[online document](https://wasmvm.github.io/WasmVM/)**

**[正體中文](README_zh.md)**

## Prerequisite

* CMake >= 3.16

* C compiler with C11 standard

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