# WasmVM

#### A standalone WebAssembly virtual machine implemented with C++

## Prerequisite

* CMake >= 2.6

* A c++ compiler supported c++11 standard with STL

## Bugs Report
- Error build on OSX with environment: 
```
OS: osx 10.12.6
Compiler: c++ [Apple LLVM version 8.1.0 (clang-802.0.42)]
Cmake Version: version 3.9.1
```

Our System call module is for Linux system call, so Windows and Mac aren't support currently.

You can disable it in `USE_SYSCALL` cmake configuration if you're running pure Wasm.

## Build

1. Run CMake

> mkdir build && cd build && cmake ..

2. Make

> make -j4
  
## Run

1. Prepare your WebAssembly file postfixed with `.wasm`
  
  * If you don't have one, go get with [wabt](https://github.com/WebAssembly/wabt), [wast2wasm online](https://cdn.rawgit.com/WebAssembly/wabt/013802ca01035365e2459c70f0508481393ac075/demo/wast2wasm/), [binaryen](https://github.com/WebAssembly/binaryen), or [Emscripten](http://webassembly.org/getting-started/developers-guide/)
  
2. Run

> ./WasmVM \<your main module\>.wasm

The imported module will be searched in the same directory of main module.

If you want to load more module, just concate as

> ./WasmVM \<your main module\>.wasm \<module1\>.wasm \<module2\>.wasm ...

## Future work

* Add Linux Syscall to `unreachable`
