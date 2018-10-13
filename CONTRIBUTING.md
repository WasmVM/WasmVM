Discord: https://discord.gg/yHBRrj4

# 開發 & 提交流程

1. 到相關 Issue 找開出來的需求 Or 提出自己想貢獻的想法
2. 被 Assign Issue 之後開始實作
3. 實作完發 Pull Request
4. 檢查(Review)和修改
5. Merge

# 程式碼相關

* 請用符合 C11 規格的純 C 語言實作，且不要破壞封裝的架構
* 非必要不得使用全域變數
* 除了 for 迴圈迭代需要的暫時變數 i,j,k....，以及太長的單字之外，變數命名請使用全名
  Ex:
  - ctx (X) => context (Ｏ)
  - int a, aa (X)
  - for(int i = 0; i < 3; ++i)  (Ｏ)
* 在原始碼檔(.c)內使用的函式，如果沒有出現在標頭檔，請宣告為 static
* 請不要隨意增減函式的參數和回傳值
* 除了標準函式庫之外，非必要不要加入其他函式庫
