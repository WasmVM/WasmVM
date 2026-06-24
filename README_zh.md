# WasmVM

#### 獨立的 WebAssembly 虛擬機

**[線上文件](https://wasmvm.github.io/WasmVM/)**

**[English](README.md)**

## 先決條件

* CMake >= 3.16

* 支援 C11 與 C++20 標準的 C/C++ 編譯器

## 注意事項

1. 本專案文件主要使用 **「台灣繁體中文」**，英文為次要語言。
  
  其他語言（例如：簡體中文）僅作為參考或翻譯。

## 建置

1. 執行 CMake

```shell
mkdir build && cd build && cmake ..
```

2. 執行 Make

```shell
make -j4
```

## 執行單元測試

1. 執行 CMake

```shell
mkdir build && cd build && cmake ..
```

2. 執行 Make 測試目標

```shell
make test
```

## 生成 HTML 文件

1. 執行 CMake

```shell
mkdir build && cd build && cmake ..
```

2. 執行 Make

```shell
make docs
```

生成的文件將位於 `build/docs/html`