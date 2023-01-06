# WasmVM 測試套件

## 簡介

WasmVM 測試套件是一套 C++ 函式庫，提供簡便的語法定義 WasmVM 的測試

## 撰寫測試

1. 在 `test/suite` 建立新的資料夾，並加入一個同名的 .cpp 檔

    例如: `test/suite/foo/foo.cpp`

2. 在 `test/suite/CMakeLists` 的 `# Add tests under here` 下面用 `add_test_suite` 加入測試

    例如: `add_test_suite(foo)`

3. 打開新建的 .cpp 檔，輸入以下的基本格式：

    ```cpp
    #include <harness.hpp>

    Testing::Suite 測試套件名稱 {
        
    };
    ```

4. 在 `Suite` 裡面用 `Test` 加入測試

    語法：

    ```cpp
    Test("測試名稱", {
        測試內容
    })
    ```

    `Test` 是把 C++ 函式封裝而成的巨集，所以內容本身就是用 C++ 語句撰寫而成，也支援 C++ 的註解

    範例：

    ```cpp
    #include <harness.hpp>

    Testing::Suite foo {
        Test("foo", {
            // 這邊可以放各種 C++ 語句
        })
    };
    ```

* 若要判斷測試結果，可以用 `Expect` 或 `Assert`。`Expect` 和 `Assert` 是把 if 封裝而成的巨集，接受一個布林值

    `Expect` 和 `Assert` 的區別在於 `Expect` 失敗後會繼續執行，`Assert` 則會中斷當前的測試
    
    語法：

    ```cpp
    Expect(布林值)
    Assert(布林值)
    ```

    範例：

    ```cpp
    #include <harness.hpp>

    Testing::Suite foo {
        Test("foo", {
            int i = 3;
            Expect(i < 5);
            Assert(i > 2 && i < 7);
        })
    };
    ```

* `ParseModule` 和 `DecodeModule` 可以用來載入 WebAssembly 文字檔或位元檔

    WebAssembly 文字檔或位元檔必須和 .cpp 檔放在相同資料夾

    語法：

    ```cpp
    ParseModule(變數名稱, "檔名")
    DecodeModule(變數名稱, "檔名")
    ```

    範例：

    ```cpp
    #include <harness.hpp>

    Testing::Suite foo {
        Test("foo", {
            // 等同於在 Test 裏宣告 WasmModule foo_module1 = module_parse("文字檔的內容");
            ParseModule(foo_module1, "foo1.wat");
            // 等同於在 Test 裏宣告 WasmModule foo_module2 = module_decode("位元檔的內容");
            DecodeModule(foo_module2, "foo2.wasm");
        })
    };
    ```

* 若要測試例外狀況，可以用 `Throw`。等同於使用 C++ try-catch 語法

    語法：

    ```cpp
    Throw(例外類別, {
        測試內容
    })
    ```

    範例：

    ```cpp
    #include <harness.hpp>

    Testing::Suite foo {
        Test("foo", {
            Throw({
                ParseModule(foo_module1, "foo1.wat");
            });
        })
    };
    ```

* 可以用 `Category` 把 `Test` 分類，`Category` 本身也可以再用 `Category` 細分

    語法：

    ```cpp
    Category("集合名稱", {
        內容
    })  
    ```

    範例：

    ```cpp
    #include <harness.hpp>

    Testing::Suite foo {
        Test("foo", {
            ParseModule(foo_module1, "foo1.wat");
        })
        Category("cat1", {
            Test("foo1", {
                Expect(true);
            })
        })
        Category("cat2", {
            Category("catcat2", {
                Test("foo2", {
                    Expect(true);
                })
            })
            Test("foo3", {
                Expect(true);
            })
        })
    };
    ```

## 執行測試

以下兩種方式都可以執行測試：

* 編譯完 WasmVM 後用 `ctest` 執行所有測試

* 在測試資料夾下用 `../../../build/test/suite/測試套件名稱/測試套件名稱` 執行單一測試

    例如：
    ```shell
    cd test/suite/token
    ../../../build/test/suite/token/token
    ```

執行完的結果如以下範例

```
 - Run test: Line comments ... [Fault] uncaught exception

 * Block comments :
   - Run test: Regular ... [Fault] uncaught exception
   - Run test: Not close ... [Pass]

   Block comments : 1 tests passed, 1 tests failed out of 2

 - Run test: Nested comments ... [Fault] uncaught exception

25% tests passed, 3 tests failed out of 4
```

+ `*` 開頭的是 category
+ `-` 開頭的是 test
+ `[Pass]` 表示通過
+ `[Fail]` 表示失敗
+ `[Fault]` 表示出現沒被 `Throw` 捕捉的例外而中止