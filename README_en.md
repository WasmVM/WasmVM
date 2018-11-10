# WasmVM

#### A standalone WebAssembly process virtual machine

**[正體中文](README.md)**

## Prerequisite

* CMake >= 2.6

* C++ compiler supporting c++11 standard with Standard Library

### Developer Tool - SkyPat Prerequisite 
#### Ubuntu/Debian Users 
```
$ sudo apt-get update
$ sudo apt-get install wget automake autoconf libtool build-essential 
```

#### Ubuntu/Debian Developers
```
$ sudo apt-get update
$ sudo apt-get install wget automake autoconf libtool build-essential cppcheck astyle colordiff
```

#### Fedora/CentOS Users
```
$ sudo yum install wget automake autoconf libtool make automake gcc gcc-c++
$ sudo yum install kernel-devel 
```

## Notice

1. Our System call module is for Linux only.
  
  You can disable it in `USE_SYSCALL` cmake configuration if you're running Wasm without syscall module in other operating system.
  
 
2. This project uses **"Taiwan Traditional Chinese"** as primary, English as secondary language in documents.
  
  Other languages (Ex: Simplified Chinese) are only used as references or translations.

3. Developers need to pay attention to the format specification of the original code (according to `K&R style`) through **AStyle**. Run instruction: `astyle --style=kr --indent=spaces=4 --indent-switches --suffix=none --recursive *.c *.h *.hpp` to format the code

## Build

1. Run CMake

```shell
mkdir build && cd build && cmake ..
```

2. Run Make

```shell
make -j4
```

## Run

1. Prepare your WebAssembly binary file postfixed with `.wasm`
  
   You can convert text format with [wabt](https://github.com/WebAssembly/wabt) or [wat2wasm online](https://cdn.rawgit.com/WebAssembly/wabt/013802ca01035365e2459c70f0508481393ac075/demo/wast2wasm/).
  
2. Run WasmVM

```shell
./WasmVM \<Filename of main module file\>
```

The imported module will be searched in the same directory of main module.

If you want to load more module, just append those modules by space-separated list after your command.
