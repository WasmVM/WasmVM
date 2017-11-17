# WasmVM

#### 可以獨立執行 WebAssembly 的程序性虛擬機器
 A standalone WebAssembly process virtual machine

## 先備條件 Prerequisite

* CMake >= 2.6

* 支援 C++11 及標準函式庫的 C++ 編譯器
 
  C++ compiler supporting c++11 standard with Standard Library

## 注意事項 Notice

1. 系統呼叫目前只支援 Linux 作業系統。 如果沒有使用到系統呼叫，可以在 CMake 的 `USE_SYSCALL` 設定中停用系統呼叫， 讓非 Linux 作業系統也能執行

  Our System call module is for Linux only.
  
  You can disable it in `USE_SYSCALL` cmake configuration if you're running Wasm without syscall module in other operating system.
  
 
2. 在文件方面，本專案以 **台灣正體中文** 為主要使用語言，英文為次要使用語言，其他語言 （例如: 簡體中文）僅能做為參考或翻譯使用。

  This project uses **"Taiwan Traditional Chinese"** as primary, English as secondary language in documents.
  
  Other languages (Ex: Simplified Chinese) are only used as references or translations.

## 編譯 Build

1. 執行 CMake

> mkdir build && cd build && cmake ..

2. 執行 Make

> make -j4
  
## 執行 Run

1. 準備好以`.wasm`為副檔名的 WebAssembly 位元檔

  Prepare your WebAssembly binary file postfixed with `.wasm`
  
  * 你可以使用 [wabt](https://github.com/WebAssembly/wabt) 或 [wat2wasm online](https://cdn.rawgit.com/WebAssembly/wabt/013802ca01035365e2459c70f0508481393ac075/demo/wast2wasm/) 將文字檔轉換成位元檔
    
    You can convert text format with [wabt](https://github.com/WebAssembly/wabt) or [wat2wasm online](https://cdn.rawgit.com/WebAssembly/wabt/013802ca01035365e2459c70f0508481393ac075/demo/wast2wasm/).
  
2. 執行 Run

> ./WasmVM 起始的檔案名稱\<Filename of main module file\>

其他需要的模組檔案會從和起始檔案相同的資料夾開始尋找

如果你希望載入更多模組，可以直接連接在後面，以空白分隔

The imported module will be searched in the same directory of main module.

If you want to load more module, just concate after and separated with space.
