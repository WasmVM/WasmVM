# WasmVM

#### 可以獨立執行 WebAssembly 的程序性虛擬機器

**[document](https://wasmvm.github.io/WasmVM/)**

**[English README](README.md)**

## 先備條件

### 一般建置

* CMake >= 3.16

* 支援 C11 和 C++20 的 C/C++ 編譯器

### 開發者建置

#### VSCode 搭配 Docker

請參考 [使用 VSCode 搭配 Docker 開發 WasmVM](docs/vscode-docker.md)

## 注意事項

1. 本專案以 **台灣正體中文** 為主要使用語言，英文為次要使用語言，其他語言 （例如: 簡體中文）僅能做為參考或翻譯使用。

## 編譯

1. 執行 CMake

```shell
mkdir build && cd build && cmake ..
```

1. 執行 Make

```shell
make -j4
```

## 產生 HTML 文件

1. 執行 CMake

```shell
mkdir build && cd build && cmake ..
```

1. 執行 Make

```shell
make docs
```

產生出的文件會在 `build/docs/html` 之下

## 測試

```shell
cd build && ctest
```