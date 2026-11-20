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

### 建置選項

以 `-D<選項>=ON` 傳給 CMake。兩者預設皆為 `OFF`，可以同時開啟。

* `WASMVM_DEBUG`：除錯器支援。加入 `wasmvm --dap [address:]port`（Debug Adapter Protocol 伺服器）與 `Debugger.hpp` 中的 `Debugger` 類別。

* `WASMVM_PROFILE`：效能分析支援。加入 `wasmvm --profile <prefix>`（輸出 `<prefix>.json` 與 `<prefix>.folded`）與 `Profiler.hpp` 中的 `Profiler` 類別。

這兩個選項會改變 `Observers` 的成員，也因此改變 `Stack` 的結構，所以以安裝後的標頭檔建置的程式，必須與函式庫建置時一樣定義 `WASMVM_DEBUG` 與 `WASMVM_PROFILE`。

```shell
mkdir build && cd build && cmake -DWASMVM_DEBUG=ON -DWASMVM_PROFILE=ON ..
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