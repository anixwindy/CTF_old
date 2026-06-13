# CCC_CTF 初中階核心技術總整理

這份筆記把 CTF 初中階常見的 RE、Pwn、Crypto 核心知識放在同一份索引中。
「全部」在 CTF 裡沒有絕對邊界，因為題型會一直變形；這裡的目標是覆蓋初中階最常遇到、最需要先看懂的技術骨架。

配套範例：

- `D:\vs_vs\ol_py\Gpy_CCC_ctf_core_examples.py`
- `D:\Cthis\GCc_CCC_ctf_core_examples.c`
- `D:\Cthis\GCp_CCC_ctf_core_examples.cpp`

---

## CCC_使用方式

先讀本筆記的分類，再跑 Python 範例看輸出，最後讀 C/C++ 範例理解原始碼層級的常見模式。

建議順序：

1. `CCC_RE`：先能看懂 byte、字串、常數、迴圈、比較。
2. `CCC_Crypto`：把 encoding、XOR、ROT、base、mod、hash 分清楚。
3. `CCC_Pwn`：先理解記憶體、保護機制、漏洞型態，再談 payload。
4. `CCC_C` / `CCC_CPP`：補上 RE/Pwn 常需要讀懂的語言行為。

---

## CCC_RE_初中階需要會的技術

RE 的主軸是「觀察程式如何把輸入轉成檢查值」，不是一開始就猜 flag。

常見觀察點：

- `strings`：找錯誤訊息、成功訊息、flag 格式、key、路徑、函式名。
- hex / bytes：看不可見字元、`\0`、檔頭 magic bytes、封包格式。
- 常數：例如 `0x1337`、`0xdeadbeef`、`0x55`、`0x41`，常代表 mask、key、ASCII 或比較值。
- endian：`0x41424344` 在小端序記憶體中會像 `44 43 42 41`。
- bitwise：`xor`、`and`、`or`、`shift`、`rotate` 常用來混淆字元。
- 迴圈：注意 index 起點、終點、步長、是否倒著跑。
- 比較：`strcmp` 看 C string，`memcmp` 可比較包含 `\0` 的 bytes。
- 長度：`strlen` 遇到 `\0` 會停，`sizeof` 是物件大小。
- encode/decode 方向：題目常寫 `check(input)`，解題要反推 `input`。
- 簡單反混淆：XOR 回去、shift 回去、減法加回去、陣列順序反過來。

常見初中階題型：

- crackme：輸入字串經過轉換後與固定陣列比較。
- xor string：每個 byte 和 key 做 XOR。
- rolling key：key 隨 index 變化，例如 `key + i`。
- table lookup：用陣列替換字元，例如 substitution。
- base / hex 包裝：輸出看似亂碼，其實是 base64、hex、binary。
- 簡單 VM：opcode + operand，用 switch 模擬小指令集。
- C++ RE：看懂 constructor、virtual function、`std::string`、`std::vector`。

---

## CCC_Pwn_初中階需要會的技術

Pwn 的主軸是「先確認保護與漏洞型態，再決定最小 exploit 路線」。

基礎記憶體：

- `.text`：程式碼。
- `.data` / `.bss`：全域變數。
- stack：區域變數、return address、saved frame pointer。
- heap：`malloc` / `new` 配置的資料。
- GOT / PLT：動態連結常見目標，初中階常用來 leak 或改寫。

保護機制：

- NX：stack 不能直接執行 shellcode，常轉向 ROP。
- Canary：return address 前有檢查值，溢位會先破壞它。
- PIE：程式本體位址隨機化，需要 leak base。
- ASLR：libc / stack / heap 位址隨機化。
- RELRO：影響 GOT 是否容易被改寫。

常見漏洞：

- stack buffer overflow：寫超過陣列邊界。
- off-by-one：只多寫 1 byte，但可能影響長度或 frame。
- format string：使用者輸入被當成 format，例如 `printf(user_input)`。
- integer overflow：長度計算繞回，導致配置或檢查錯誤。
- signed / unsigned bug：負數轉成很大的 unsigned。
- heap bug：UAF、double free、overflow，初中階先理解概念即可。

常見 exploit 路線：

- ret2win：跳到程式內已存在的 win function。
- ret2plt：呼叫 PLT 內的函式。
- leak address：先印出 libc 或 binary 位址。
- ret2libc：用 libc 的 `system("/bin/sh")`。
- ROP：串接 gadget 控制暫存器與呼叫。

必備觀念：

- little endian payload：位址要用小端序塞進 bytes。
- offset：先找幾個 byte 後會碰到 return address。
- calling convention：x86_64 Linux 常見前幾個參數在 `rdi`、`rsi`、`rdx`。
- payload shape：通常是 padding + controlled value + optional chain。
- 不要先亂打 exploit：先問「保護是什麼、bug 是什麼、能控制什麼」。

---

## CCC_Crypto_初中階需要會的技術

Crypto 題要先分清楚：encoding、encryption、hash 不是同一件事。

基本分類：

- encoding：base64、hex、binary、URL encode，只是表示法。
- encryption：有 key，可逆，例如 XOR、Caesar、Vigenere、AES。
- hash：通常不可逆，例如 MD5、SHA1、SHA256，只能猜或查表。

常見初中階技術：

- ASCII / bytes：先把文字、數字、bytes 轉換弄清楚。
- hex decode：`414243` -> `ABC`。
- base64 decode：常見尾巴有 `=` padding。
- ROT / Caesar：字母平移。
- XOR single byte：嘗試 0 到 255 的 key。
- XOR repeating key：key 重複使用，常用 known plaintext 反推。
- frequency analysis：英文中 `e t a o i n` 常見。
- Vigenere：多個 Caesar 組合，先找 key 長度。
- modular arithmetic：`(a + b) % n`、`pow(a, e, n)`。
- RSA 入門：`n = p*q`、`phi=(p-1)(q-1)`、`d = e^-1 mod phi`。
- small factor：如果 `n` 很小，可以分解。
- hash brute force：候選空間小時才可行。

常見判斷：

- 看起來只有 `0-9a-f`：可能是 hex 或 hash。
- 看起來有 `A-Z a-z 0-9 + / =`：可能是 base64。
- byte 值和明文差固定：可能是 Caesar 或加減法。
- XOR 後出現可讀英文：方向通常對了。
- RSA 題如果 `p`、`q` 或 `phi` 被給出，通常就是要求算 `d`。

---

## CCC_C_需要看懂的 C 行為

C 在 RE/Pwn 中非常重要，因為很多 binary 都來自 C。

必懂語法與行為：

- 指標：`char *p` 是位址，不是字串本身。
- 陣列退化：函式參數中的 `char arr[]` 常變成 `char *`。
- `sizeof` vs `strlen`：一個看物件大小，一個看 C string 長度。
- `\0`：C string 結尾，`memcmp` 不會因此停止。
- integer type：`char`、`short`、`int`、`long`、`uint32_t` 大小不同。
- signedness：`signed char` 和 `unsigned char` 在比較時可能完全不同。
- pointer arithmetic：`p + 1` 前進一個元素，不一定是一個 byte。
- struct padding：欄位之間可能有對齊空洞。
- function pointer：常見於 callback、switch table、簡單 VM。
- unsafe input：`gets`、無寬度限制的 `scanf("%s")`、錯誤的 `strcpy`。

讀 C 反組譯時常見線索：

- `for` / `while` 可能變成 conditional jump。
- `switch` 可能變成 jump table。
- `strcmp` / `memcmp` 呼叫附近通常有檢查邏輯。
- `malloc` / `free` 附近要注意 heap 生命週期。
- `printf` 第一個參數如果不是固定字串，可能是 format string bug。

---

## CCC_CPP_需要看懂的 C++ 行為

C++ RE 常比 C 多一層語言結構，但核心仍是資料與控制流。

必懂行為：

- constructor / destructor：物件建立和離開 scope 時自動呼叫。
- `std::string`：不是單純 `char *`，但常能在記憶體中看到內容。
- `std::vector`：連續元素，常有 begin/end/capacity。
- reference：語法像值，底層常像指標。
- virtual function：物件內通常有 vptr 指向 vtable。
- inheritance：父類指標可呼叫子類 virtual function。
- overload：同名函式靠參數型別區分，反組譯中名字可能被 mangling。
- template：編譯期展開，會產生具體型別版本。
- exception：會引入額外控制流，RE 時不要把它誤認成主邏輯。
- RAII：資源釋放藏在 destructor 裡。

Pwn/RE 相關注意：

- `vector[i]` 不檢查邊界，`vector.at(i)` 會檢查。
- `std::string::c_str()` 回傳內部 buffer 指標，生命週期要注意。
- virtual call 在反組譯中常像 `call [vtable + offset]`。
- C++ binary 可能有大量 runtime 符號，先找題目自己的函式。

---

## CCC_最小解題檢查表

RE：

- 檔案類型是什麼？
- 有哪些可讀字串？
- 有沒有固定 key、固定 table、固定比較陣列？
- 輸入長度如何檢查？
- 每個 byte 如何被轉換？
- 比較用 `strcmp` 還是 `memcmp`？
- 能不能把轉換反過來寫成小腳本？

Pwn：

- 保護機制有哪些？
- crash 是不是可控？
- offset 是多少？
- 能控制 return address、function pointer、GOT、hook 或 heap metadata 嗎？
- 需要 leak 嗎？
- payload 最小形狀是什麼？
- 本機與遠端 libc 是否一致？

Crypto：

- 這是 encoding、encryption 還是 hash？
- 字元集像什麼？
- 有 key 嗎？key 長度可能是多少？
- 有 known plaintext 嗎？例如 `flag{`。
- 是否需要 brute force？搜尋空間是否合理？
- modular / RSA 題有沒有小 `n`、已知 `p/q/phi/d`、共用 modulus？

---

## CCC_小練習

1. 修改 Python 範例中的 XOR key，觀察輸出什麼時候變成可讀文字。
2. 把 C 範例中的 `uint32_t value = 0x41424344` 改成 `0x31323334`，觀察小端序 bytes。
3. 把 C++ 範例的 `vector.at()` 改成 `vector[]`，思考為什麼一個會報錯、一個不保證安全。
4. 自己寫一個 `check(input)`：每個字元先 XOR，再加 index，最後跟陣列比較；再寫反推腳本。
