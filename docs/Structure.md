# 系統架構

![](/docs/images/Structure.svg) 

WasmVM 主要分成三個元件，載入器(Loader)、執行器(Executor)和即時編譯器(JIT)，每個元件中有不同的模組執行各種工作。圖中包住許多零件的大長方形表示各個元件，白色的小長方形表示各個模組。

元件和模組之間會進行各種資料轉換，各種資料結構用藍色平行四邊形表示。

在執行器(Executor)元件中，執行的函式分成原生函式(Native)、直譯函式(Interpreted)和系統呼叫(Syscall)三種，以淺綠色圓角矩形表示。

以下將會介紹各個元件，資料格式的部分請參考 [WebAssembly 規格](https://webassembly.github.io/spec/core/index.html)

# 載入元件 (Loader)

在程式中直接操作 Wasm 位元碼很不方便，而且目前我們也沒辦法直接在機器上執行 Wasm 位元碼。載入元件的工作就是將 Wasm 解析成方便程式操作的格式，驗證要執行的 Wasm 模組，並且載入到執行器中。

載入元件包含以下模組：

## 解碼器 (Decoder)

解碼器的作用是讀取 Wasm 位元檔，解析每個 byte 所代表的意義，並轉成方便操作的 Wasm 模組，交給驗證器做驗證。

如果 Wasm 位元檔的格式不符合 Wasm 規格，解碼器會停止當下的解碼，並回報錯誤。

## 驗證器 (Validator)

在解碼器產生 Wasm 模組後，驗證器會依據規格書上的驗證定義，驗證 Wasm 模組的行為。

如果 Wasm 模組的行為不符合 Wasm 規格，驗證器會回報錯誤。

在驗證完成之後，驗證器會將 Wasm 模組傳給實例產生器和即時編譯元件的分析器。

## 實例產生器 (Instantiator)

解碼器產生的 Wasm 模組包含很多資訊，方便分析和驗證，但是不方便執行。

實例產生器會把 Wasm 模組轉換成 Wasm 實例，並存進執行元件的儲存空間(Store)，準備執行。

# 執行元件 (Executor)

執行元件在實例產生器處理完之後，執行主要模組的起始函式，讓程式開始運作。

執行元件包含以下模組：

## 儲存空間 (Store)

儲存空間的架構對應到規格裡的 [Store](https://webassembly.github.io/spec/core/exec/runtime.html#store)。接受實例產生器產生的結果，存到對應的欄位中，提供核心使用。

一個執行中的程式可能會使用多個核心，但是儲存空間是所有核心共用的。

## 核心 (Core)

核心是最主要的執行單位，擁有自己的堆疊([Stack](https://webassembly.github.io/spec/core/exec/runtime.html#stack)) 和計數器(Program Counter)。

核心會按照計數器依序提取指令，呼叫對應的執行環境或系統呼叫執行指令。

一個執行中的程式可能會使用多個核心，不同核心間的堆疊和計數器是獨立的，但是會共用儲存空間。

## 系統呼叫 (SysCall)

## 執行環境 (Runtime)

# 即時編譯元件 (JIT)

即時編譯 (Just-In-Time compile) 是在程式正在執行或快要執行的時候，把程式編譯成可以直接執行的機器碼，加快之後執行時的效能。

即時編譯元件會對 Wasm 模組做分析，把必要的函式編譯成機器碼加速執行。

即時編譯元件包含以下模組：