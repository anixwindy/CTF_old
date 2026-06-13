# RE -> Pwn Long-Term Roadmap

適用對象：

- 單人、業餘、長期學習
- 主線是 `RE -> Pwn`
- `Crypto` 只補到能辨識常見 CTF 題型，不走專精

---

## 1. 先講結論：主線順序

最穩的主線是：

1. `C` 語言行為
2. `Linux` 與工具鏈
3. 電腦底層觀念
4. `x86-64` 組語與呼叫慣例
5. 基礎 `RE`
6. 進一步 `RE`
7. `ELF` / `glibc` / 保護機制
8. 基礎 `Pwn`
9. 進一步 `Pwn`
10. `Heap` 與較難題型
11. 視興趣再補 `Windows RE`、`ARM`、`Kernel`、`Symbolic Execution`

原因：

- `Pwn` 幾乎一直在借用 `RE` 的能力。
- 你如果看不懂函式流程、stack frame、變數位置、條件跳轉，`Pwn` 不會真的走得動。
- `RE` 讓你「看懂程式在做什麼」，`Pwn` 讓你「利用程式做錯的事」。

---

## 2. 你真正要建立的能力，不是背題型

長期來看，你需要反覆使用的是這四種能力：

1. 觀察能力  
   看出常數、字串、比較點、控制流、資料流、保護機制、可疑輸入點。

2. 翻譯能力  
   把 `C`、反編譯碼、組語、記憶體內容互相對照。

3. 驗證能力  
   用最小步驟驗證假設，例如先看一個比較、先斷一個函式、先確認一個 offset。

4. 表達能力  
   能說清楚「這段程式在做什麼」「為什麼這個洞成立」「為什麼這個 leak 有用」。

如果只會抄 exploit 或背 writeup，通常卡在第 2 和第 3 點。

---

## 3. 基礎層：一定先打穩

這一層不是附屬品，而是之後所有分析的母體。

### 3.1 `C` 語言行為

你至少要熟到下面這些不需要猜：

- 整數型別、signed / unsigned
- overflow 的表現
- cast 與截斷
- 指標、陣列、字串
- `struct` / `union`
- stack 上的區域變數
- 函式參數與回傳值
- `malloc` / `free`
- `memcpy` / `strcpy` / `printf` 類常見危險點
- 位元運算：`& | ^ ~ << >>`

為什麼先學這個：

- `RE` 會一直問你：「編譯前原本像什麼 C？」
- `Pwn` 會一直問你：「這個 C 行為怎麼踩爛記憶體？」

學到的標準：

- 看到一小段 C，能大致想像它編譯後的流程。
- 看到組語，能反推可能對應的 C 結構。

常見卡點：

- 把陣列跟指標當成完全一樣
- 不理解 signed / unsigned 比較
- 不理解字串結尾 `\\0`
- 不理解 `printf(user_input)` 為什麼危險

### 3.2 `Linux` 基本操作

至少熟這些：

- 檔案與權限
- 標準輸入 / 輸出 / 錯誤
- pipe 與 redirect
- 環境變數
- process / pid
- `gcc` / `clang`
- `make` 或最基本編譯指令
- `file` / `strings` / `xxd` / `hexdump`
- `readelf` / `objdump` / `nm`

為什麼重要：

- `RE` 靜態分析第一步就靠它們。
- `Pwn` 的 exploit 環境、libc、loader、保護機制、stdin/stdout 全都跟 Linux userland 有關。

### 3.3 電腦底層觀念

這裡不是考科，而是實戰必備詞彙。

你要理解：

- bit / byte / word
- little-endian
- 記憶體位址
- text / data / bss / heap / stack
- 虛擬記憶體
- page 基本觀念
- 函式呼叫與返回
- 靜態連結 / 動態連結
- syscall 是什麼

學到的標準：

- 能畫出一個簡單 process 的記憶體區域圖。
- 能說出 overflow 為什麼會撞到 return address。

---

## 4. 組語層：RE 和 Pwn 的共同核心

### 4.1 先專注 `x86-64`

對大多數 CTF 來說，先打熟 `x86-64` 比分散去碰多架構更有效。

先熟這些指令與觀念：

- `mov`
- `lea`
- `cmp`
- `test`
- `add` / `sub`
- `xor`
- `and` / `or`
- `shl` / `shr` / `sar`
- `push` / `pop`
- `call` / `ret`
- `jmp`
- 條件跳轉：`je` `jne` `jg` `jl` `ja` `jb`

你還要理解：

- flag register 基本用途
- 何時用 `cmp`，何時用 `test`
- 為什麼 `lea` 常不是單純「取地址」

### 4.2 呼叫慣例與 stack frame

重點是 `System V AMD64 ABI`：

- 參數常放在哪些 register
- 回傳值在哪
- `rbp` / `rsp` 在做什麼
- prologue / epilogue 長什麼樣
- local variable 怎麼從 `[rbp - x]` 讀
- return address 在哪

這一層對 `Pwn` 特別重要，因為：

- 你要知道你覆蓋到哪裡了
- 你要知道怎麼把控制流送到你想去的位置

學到的標準：

- 看到一個小函式的組語，能指出參數、局部變數、回傳邏輯。

---

## 5. RE 第一階段：靜態分析能力

這一階段的目標不是「解很難」，而是：

- 不執行可疑檔案也能先看出不少資訊
- 建立對程式流程的閱讀能力

### 5.1 先從最便宜的線索開始

固定先看：

- `file`
- `strings`
- `checksec` 類資訊
- `readelf -h -S -s`
- `objdump -d`
- 匯入函式與符號
- 明顯常數

要養成的習慣：

- 先看字串與 API，再猜程式類型
- 先找輸入點、比較點、輸出點
- 先找固定常數、table、magic value

### 5.2 學會看反編譯，不迷信反編譯

工具：

- `Ghidra`
- `IDA Free`

你要知道反編譯的限制：

- 型別可能猜錯
- `if / else` 結構可能重建得不漂亮
- 優化後流程可能很不像原始碼
- 有時候組語比反編譯更清楚

所以正確習慣是：

- 反編譯看高階輪廓
- 組語確認關鍵比較、跳轉、資料位置

### 5.3 先熟最常見的 RE 模式

最常見的 beginner 題型：

- 字串比對
- 分段字元檢查
- 常數表轉換
- XOR / ADD / SUB / ROL / ROR
- 簡單 checksum
- switch / jump table
- 陣列重排
- 函式拆散驗證

學到的標準：

- 看到這些模式時，不會第一反應就暴力。
- 能先說出資料是怎麼被轉換的。

---

## 6. RE 第二階段：動態分析能力

### 6.1 `gdb` 是主工具，不只是除錯器

你要會：

- 下中斷點
- 單步 `stepi` / `nexti`
- 看 register
- 看 stack
- 看某個地址的 bytes / string / qword
- 改 register 或記憶體做小實驗
- 在關鍵比較前後停住

重點不是背指令，而是會問：

- 這個輸入進來後去哪了？
- 哪裡決定成功或失敗？
- 這個值是原始輸入、轉換後輸入，還是某個常數？

### 6.2 動態分析的正確用途

不是一開始就亂跑 binary，而是：

- 靜態分析卡在資料流時再進動態
- 想驗證某個比較、某個 offset、某個 table 時再進動態

這樣效率高，也比較安全。

### 6.3 RE 第二階段應掌握的題型

- 拆多層函式的檢查流程
- 簡單 anti-debug 觀察
- 混合字串與數值轉換
- table-based 檢查
- 小型 VM / state machine 入門

---

## 7. RE 第三階段：系統化理解 binary

這一層是為了之後接 `Pwn`。

你要慢慢補：

- `ELF` 結構
- section 與 segment 差別
- symbol、relocation、PLT、GOT
- loader 大概做了什麼
- libc 呼叫大概怎麼接上去

為什麼重要：

- `Pwn` 的 leak、hook、GOT overwrite、ret2libc 都靠這些觀念。
- 只會把 binary 當黑箱，之後很難升級。

---

## 8. Pwn 前置：先建立「漏洞模型」

很多人一開始學 `Pwn` 只記 exploit 模板，後面會崩。

你先要建立的是這些概念：

- 什麼叫 memory corruption
- 什麼叫越界讀 / 越界寫
- 什麼叫控制流被改變
- 什麼叫資訊洩漏
- 什麼叫 primitive

你要會把一題拆成：

1. 漏洞是什麼
2. 能得到什麼 primitive
3. 有哪些保護
4. 還缺什麼資訊
5. 最終控制目標是什麼

這比背 `pwntools` 模板重要得多。

---

## 9. Pwn 第一階段：Stack 類基礎

先學最乾淨的主線。

### 9.1 Stack layout

你要熟：

- local buffer 在哪
- saved `rbp` 在哪
- return address 在哪
- 覆蓋順序是什麼
- 為什麼長度一超過就出事

### 9.2 常見起手式

先做這些類型：

- `ret2win`
- 控制 `RIP`
- 找 offset
- 用 cyclic pattern 驗證

### 9.3 保護機制

這一段要很熟，因為每題都會先看：

- `NX`
- `Canary`
- `PIE`
- `ASLR`
- `RELRO`

你要能講出：

- 這個保護防的是哪種路
- 少了它可以直接做什麼
- 有了它就需要補哪種技巧

學到的標準：

- 拿到一個小 binary，可以先說出保護組合與大方向。

---

## 10. Pwn 第二階段：ROP 與 ret2libc

這是 beginner 到真正入門之間的分水嶺。

### 10.1 為什麼需要 `ROP`

因為：

- `NX` 讓你不能再把 shellcode 直接丟上 stack 就跑
- 所以你得重組現成程式碼片段

你要理解：

- gadget 是什麼
- 為什麼 `ret` 鏈能串邏輯
- 參數怎麼塞到對應 register

### 10.2 `ret2libc`

這是必學主線：

- leak 一個 libc 位址
- 算出 libc base
- 找 `system`
- 找 `"/bin/sh"`

你會同時用到：

- `PLT` / `GOT`
- libc symbol
- `ASLR`
- 呼叫慣例

如果這一段不穩，後面很多題都只能照抄。

### 10.3 實戰工具

你要開始熟：

- `pwntools`
- `pwndbg` 或類似輔助插件
- libc 資訊確認

但記住：

- 工具只是加速器
- 不是理解的替代品

---

## 11. Pwn 第三階段：Format String

這是很值得早學但不要一開始就硬上的題型。

你要理解：

- 為什麼 `printf(user_input)` 會把輸入當格式字串
- 為什麼能 leak stack / libc / pie
- 為什麼 `%n` 類寫入危險

這一類題會強化你對：

- stack 內容排列
- 參數讀取位置
- 任意讀 / 任意寫 primitive

的理解。

學到的標準：

- 能分辨一題 format string 主要用來 leak、write，還是兩者都要。

---

## 12. Pwn 第四階段：Heap

Heap 請晚一點進，不要太早。

因為它需要前面很多觀念已經穩：

- C 指標
- 記憶體配置
- glibc allocator 基礎
- leak 的作用
- 任意寫 primitive 的價值

你至少要依序理解：

- `malloc` / `free` 真正在做什麼
- chunk header 長什麼樣
- `tcache`
- double free
- use-after-free
- off-by-one
- unsorted bin 基本用途

Heap 初期不要追求「版本特化神招」，先把：

- chunk 流向
- 何時重複使用
- 哪裡能改 metadata

看懂。

---

## 13. RE 和 Pwn 之間會反覆共用的知識

這些不是獨立章節，而是一路都會重用：

### 13.1 資料表示

- 十六進位
- little-endian
- signed / unsigned
- bit mask
- alignment

### 13.2 控制流

- compare
- branch
- loop
- switch
- early return

### 13.3 記憶體觀念

- pointer arithmetic
- buffer 長度
- copy 邊界
- object lifetime
- stack / heap 差異

### 13.4 函式與連結

- import
- symbol
- relocation
- GOT / PLT
- libc base

---

## 14. 工具順序：不要一次全學

推薦順序：

1. `file` / `strings` / `xxd`
2. `readelf` / `objdump`
3. `gdb`
4. `Ghidra`
5. `checksec`
6. `pwndbg`
7. `pwntools`

理由：

- 先學低層觀察工具，會幫你理解 binary 本身。
- 如果一開始就只靠高階 GUI 或 exploit 框架，後面容易「會按，不會判斷」。

---

## 15. Crypto 只補到哪裡最划算

你不走深 crypto 的話，補到下面最實用：

- Base16 / Base32 / Base64
- XOR 單位元組與重複 key
- Caesar / ROT
- Vigenere
- 簡單 substitution
- 常見 hash 辨識
- 模運算基本概念
- 很入門的 RSA 題型辨識

重點不是證明，而是：

- 看到輸出長相，能猜類型
- 知道先驗證什麼
- 知道何時該寫一個小腳本

---

## 16. 你每個階段要能做到什麼，才算可以往下

### 階段 A：可進 RE

你應該能：

- 讀小型 C 程式
- 看懂基本 stack frame
- 追簡單比較與迴圈

### 階段 B：RE 基礎穩

你應該能：

- 在 Ghidra / gdb 間來回驗證
- 找出輸入檢查邏輯
- 講清楚資料如何被轉換

### 階段 C：可進 Pwn

你應該能：

- 看出危險函式與可疑輸入點
- 解釋保護機制的影響
- 畫出 stack layout

### 階段 D：Pwn 基礎穩

你應該能：

- 自己找 offset
- 自己解釋為什麼要 leak
- 自己搭出簡單 ret2win / ret2libc

### 階段 E：可進 Heap / 較難題

你應該能：

- 分辨題目主要是 leak、write、control flow 哪一路
- 解釋 chunk 行為與 allocator 互動

---

## 17. 最常見的錯誤順序

不建議這樣學：

1. 先狂背 `pwntools` 模板  
   你會寫 payload，但不知道為什麼這樣寫。

2. 一開始就衝 heap  
   很容易被細節淹沒，動機與模型都不清楚。

3. RE 只看反編譯、不看組語  
   關鍵比較點與資料位置常常會看錯。

4. 只靠跑程式，不先做靜態觀察  
   你會花很多時間在低價值試錯。

5. 把 Crypto 當成主線一起重壓  
   對你這種路線來說，投報比通常不高。

---

## 18. 長期建議：你的時間怎麼分配

如果你是長期單人路線，比例大概可以這樣：

- `RE`：`55%` 到 `60%`
- `Pwn`：`30%` 到 `35%`
- `Crypto`：`10%`

原因：

- `RE` 是主引擎
- `Pwn` 吃掉很多 `RE` 能力
- `Crypto` 先維持基本生存能力即可

---

## 19. 最推薦的長期主線

如果只保留一條最實用的路：

1. `C`
2. `Linux`
3. 記憶體與 process 基礎
4. `x86-64`
5. 呼叫慣例
6. `gdb`
7. `readelf` / `objdump` / `strings`
8. `Ghidra`
9. 小型 `RE`
10. `ELF` / `PLT` / `GOT`
11. 保護機制
12. stack overflow
13. ret2win
14. leak
15. `ROP`
16. ret2libc
17. format string
18. heap

這條路的核心思想是：

- 先學「看懂」
- 再學「打穿」
- 最後才碰「很會繞」

---

## 20. 你現在最應該記住的一句話

對你的路線來說，真正要追求的不是「我會幾個 exploit 模板」，而是：

**我能不能把 binary 的行為、資料流、控制流、保護機制、漏洞效果講清楚。**

只要這一點持續變強，你的 `RE` 和 `Pwn` 會一起長。

---

## 21. 一個很適合你的小練習

拿一個非常小的 C 程式，自己做下面五件事：

1. 畫出 stack frame
2. 找出輸入點
3. 找出比較點
4. 用 `gdb` 驗證變數位置
5. 說明如果把某個長度檢查拿掉，會變成什麼 `Pwn` 類型

這個練習很小，但它正好把 `RE -> Pwn` 的橋接核心都碰到了。
