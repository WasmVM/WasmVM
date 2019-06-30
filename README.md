# WasmVM

#### 可以獨立執行 WebAssembly 的程序性虛擬機器

**[教學文件](https://luishsu.gitbook.io/wasmvm-tutorial/)**

**[English README](README_en.md)**

## 先備條件

### 一般建置

* CMake >= 2.6

* pthread

* 支援 C11 及標準函式庫的 C 編譯器

### 開發者建置

#### VSCode 搭配 Docker

請參考 [使用 VSCode 搭配 Docker 開發 WasmVM](docs/vscode-docker.md)

#### Ubuntu/Debian
```shell
$ sudo apt-get update
$ sudo apt-get install wget automake autoconf libtool build-essential cppcheck astyle colordiff
```

#### Fedora/CentOS
```shell
$ sudo yum install wget automake autoconf libtool make automake gcc gcc-c++
$ sudo yum install kernel-devel 
```

## 注意事項

1. 系統呼叫目前只支援 Linux 作業系統。 如果沒有使用到系統呼叫，可以在 CMake 的 `USE_SYSCALL` 設定中停用系統呼叫， 讓非 Linux 作業系統也能執行
 
2. 在文件方面，本專案以 **台灣正體中文** 為主要使用語言，英文為次要使用語言，其他語言 （例如: 簡體中文）僅能做為參考或翻譯使用。

3. 開發上需要注意，透過 **AStyle** 來做原始程式碼的格式規範（依照 `K&R style`）。運行指令： `astyle --style=kr --indent=spaces=4 --indent-switches --suffix=none --recursive *.c *.h *.hpp` 來對程式碼做格式編排

## 編譯

1. 執行 CMake (正常模式，不會編譯 test 資料夾)

```shell
mkdir build && cd build && cmake ..
```

* 執行 CMake (開發模式)
```shell
mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..
```

2. 執行 Make

```shell
make -j4
```
  
## 執行

1. 準備好以`.wasm`為副檔名的 WebAssembly 位元檔
  
    你可以使用 [wabt](https://github.com/WebAssembly/wabt) 或 [wat2wasm online](https://cdn.rawgit.com/WebAssembly/wabt/013802ca01035365e2459c70f0508481393ac075/demo/wast2wasm/) 將文字檔轉換成位元檔
  
2. 執行 Run

```shell
./wasmvm 起始的檔案名稱
```

其他需要的模組檔案會從和起始檔案相同的資料夾開始尋找

如果你希望載入更多模組，可以直接連接在後面，以空白分隔
