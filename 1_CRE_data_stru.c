/* ═══════════════════════════════════════════════════════════════════
 *                    CTF RE 必備知識地圖
 * ═══════════════════════════════════════════════════════════════════
 *
 * ── C：核心，一定要會 ──
 *
 *   語言層面：
 *     - 基本型別與大小：char(1) / int(4) / long(8) / pointer(8)，小端序讀法
 *     - 指標算術：*p、p[i]、*(p+i) 三種寫法等價
 *     - struct 記憶體佈局、padding 規則
 *     - 函式呼叫慣例（x64 System V：前 6 參數 rdi/rsi/rdx/rcx/r8/r9，回傳值 rax）
 *     - stack frame：局部變數、saved rbp、return address 的相對位置
 *
 *   常見 idiom（反組譯時高頻出現）：
 *     - malloc/free、linked list 走訪
 *     - strcmp/strlen/memcpy/memset 的底層行為
 *     - printf format string 對應哪些暫存器
 *     - XOR、位移、bit mask 操作
 *
 *   不需要：複雜的泛型 macro、POSIX 網路程式設計
 *
 * ── C++：看得懂就好，不需精通 ──
 *
 *   主要障礙：name mangling 與 vtable
 *     - _ZN3Foo3barEv → Foo::bar()（c++filt 還原）
 *     - vtable 佈局：物件前 8 bytes 是 vtable pointer
 *     - std::string 的 SSO（小字串直接存在 stack）
 *     - std::vector 三指標結構：begin / end / capacity
 *
 *   不需要：template metaprogramming、SFINAE、現代 C++20 特性
 *
 * ── Python：工具語言，要熟 ──
 *
 *     一定要會：
 *       bytes / bytearray / int.from_bytes / .to_bytes
 *       pow(base, exp, mod)               # 模冪，RSA/crypto 必用
 *       struct.unpack('<I', data[:4])     # 小端 uint32
 *       subprocess.run / from pwn import *
 *
 *     腳本思維：「二進位 → 字串 → 整數 → 回字串」順暢轉換
 *
 * ── 真正的核心：組合語言讀寫能力 ──
 *
 *   語言知識只是橋樑，RE 實際戰場是 x86-64 組合語言 + Ghidra/IDA：
 *     - 讀 disasm：cmp/je/jne/jg → if/else
 *     - 追 data flow：值從哪個暫存器來、怎麼變
 *     - 識別 idiom：lea rax,[rbp-0x10] 是局部陣列
 *     - 動態分析：x64dbg / gdb 下斷點、觀察實際值
 *
 * ── 學習優先順序 ──
 *
 *   1. x86-64 組合語言基礎             ← 最優先
 *   2. C 指標 + struct + 呼叫慣例
 *   3. Python struct/pwntools 腳本
 *   4. C++ vtable/name mangling（遇到再查）
 *
 *   C++ 深度語法在 RE 中幾乎不需要主動寫，認得反組譯結果就夠了。
 *
 * ═══════════════════════════════════════════════════════════════════
 */

/*
 * 2_CaRE_data_structures.c — RE 常見的 C 資料結構與演算法
 *
 * 重點：「你在 IDA/Ghidra 反組譯時，這些 pattern 長甚麼樣子」
 *
 * 4 個 Level 漸進：
 *   Level 1: struct + 指標走訪（linked list） ── 認得 offset 存取
 *   Level 2: function pointer / jump table ── switch-case 真實長相
 *   Level 3: 自訂 hash + bit manipulation ── crackme 常見驗證套路
 *   Level 4: 迷你 VM interpreter ── 進階 RE 必碰到的 obfuscation
 *
 * 編譯（要看反組譯記得用 -O0 保留原始邏輯）：
 *   gcc -O0 -g -o ds 2_CaRE_data_structures.c
 *
 * 反組譯：
 *   objdump -d -M intel ds | less
 *   或丟進 Ghidra / IDA Free
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ════════════════════════════════════════════════════════════
 * Level 0：知識地圖各項目的「實作版」
 *
 * 把頂端知識地圖列出的項目全部跑一次：
 *   - 基本型別大小 + 小端序驗證
 *   - 指標算術三種寫法等價
 *   - struct padding 規則
 *   - 函式呼叫慣例（多參數函式對照 disasm）
 *   - stack frame（局部變數位址、saved rbp、return address）
 *   - 自製 strlen/strcmp/memcpy/memset（對照反組譯認 idiom）
 *   - printf format string 的暫存器對應
 *   - XOR / 位移 / bit mask 全套
 * ════════════════════════════════════════════════════════════ */

/* ── 0.1 型別大小 + 小端序 ── */
void l0_type_sizes(void) {
    printf("[L0.1] 基本型別大小（64-bit 平台）：\n");
    printf("  sizeof(char)      = %zu\n", sizeof(char));        /* 1 */
    printf("  sizeof(short)     = %zu\n", sizeof(short));       /* 2 */
    printf("  sizeof(int)      = %zu\n", sizeof(int));          /* 4 */
    printf("  sizeof(long)     = %zu  (Windows 4 / Linux 8)\n", sizeof(long));
    printf("  sizeof(long long)= %zu\n", sizeof(long long));    /* 8 */
    printf("  sizeof(void*)    = %zu\n", sizeof(void*));        /* 8 */
    printf("  sizeof(float)    = %zu\n", sizeof(float));        /* 4 */
    printf("  sizeof(double)   = %zu\n", sizeof(double));       /* 8 */

    /* 小端序：低位元組存低位址 ── x86/x64 全部是小端 */
    uint32_t x = 0x12345678;
    uint8_t *p = (uint8_t*)&x;
    printf("[L0.1] uint32_t 0x12345678 在記憶體：%02X %02X %02X %02X\n",
           p[0], p[1], p[2], p[3]);
    printf("        → 低位 0x78 先存（小端序）\n");
}

/* ── 0.2 指標算術三種寫法等價 ── */
void l0_pointer_arithmetic(void) {
    int arr[5] = {10, 20, 30, 40, 50};
    int *p = arr;

    printf("[L0.2] 取 arr[2] 的等價寫法：\n");
    printf("  arr[2]   = %d\n", arr[2]);
    printf("  *(arr+2) = %d\n", *(arr+2));
    printf("  p[2]     = %d\n", p[2]);
    printf("  *(p+2)   = %d\n", *(p+2));
    printf("  2[arr]   = %d   ← C 合法的怪寫法（編譯器會展成 *(2+arr)）\n", 2[arr]);

    /* 位址驗證：相鄰元素差 sizeof(int) = 4 */
    printf("[L0.2] &arr[0]=%p  &arr[1]=%p  差 %td bytes\n",
           (void*)&arr[0], (void*)&arr[1],
           (char*)&arr[1] - (char*)&arr[0]);
}

/* ── 0.3 struct padding 規則 ── */
typedef struct {
    char  a;     /* offset 0  (1 byte)  */
                 /* padding 3 bytes 對齊到 4 */
    int   b;    /* offset 4  (4 bytes) */
    char  c;     /* offset 8  (1 byte)  */
                 /* padding 7 bytes 對齊到 8 */
    double d;    /* offset 16 (8 bytes) */
} BadAlign;     /* 總大小 24 bytes */

typedef struct {
    double d;    /* offset 0  */
    int    b;    /* offset 8  */
    char   a;    /* offset 12 */
    char   c;    /* offset 13 */
                 /* padding 2 bytes 對齊到 8 */
} GoodAlign;    /* 總大小 16 bytes */

void l0_struct_padding(void) {
    printf("[L0.3] struct padding：欄位順序影響大小！\n");
    printf("  sizeof(BadAlign)  = %zu  (char,int,char,double：浪費 padding)\n", sizeof(BadAlign));
    printf("  sizeof(GoodAlign) = %zu  (大到小排：省 8 bytes)\n", sizeof(GoodAlign));

    BadAlign b = {0};
    printf("[L0.3] BadAlign 各欄位 offset：a=%td b=%td c=%td d=%td\n",
           (char*)&b.a - (char*)&b,
           (char*)&b.b - (char*)&b,
           (char*)&b.c - (char*)&b,
           (char*)&b.d - (char*)&b);
}

/* ── 0.4 函式呼叫慣例（多參數函式 → 看 disasm 哪些進暫存器）── */
int many_args(int a, int b, int c, int d, int e, int f, int g, int h) {
    /* Linux x64 System V：rdi=a, rsi=b, rdx=c, rcx=d, r8=e, r9=f, [rsp+8]=g, [rsp+16]=h
     * Windows x64：       rcx=a, rdx=b, r8=c,  r9=d, [rsp+32]=e..h
     * 編譯後丟 objdump 看就一目了然 */
    return a + b + c + d + e + f + g + h;
}

void l0_calling_convention(void) {
    printf("[L0.4] 呼叫慣例：many_args(1..8) = %d\n",
           many_args(1, 2, 3, 4, 5, 6, 7, 8));
    printf("       → objdump -d 看這支函式，對照前 4~6 個參數在哪個暫存器\n");
}

/* ── 0.5 stack frame 觀察 ── */
void l0_stack_frame(void) {
    int local_a = 0xAAAA;
    int local_b = 0xBBBB;
    char buf[16] = "hello";

    /* 局部變數在 stack 上「rbp 往下」的位置
     * saved rbp 在 [rbp]，return address 在 [rbp+8] */
    printf("[L0.5] stack frame 內三個變數的位址：\n");
    printf("  &local_a = %p\n", (void*)&local_a);
    printf("  &local_b = %p\n", (void*)&local_b);
    printf("  &buf[0]  = %p\n", (void*)buf);
    printf("       → 觀察位址差距，可看出編譯器如何排列局部變數\n");
}

/* ── 0.6 自製 mem 函式（對照反組譯認 idiom）── */
size_t my_strlen(const char *s) {
    const char *p = s;
    while (*p) p++;        /* 反組譯 idiom：loop + cmp byte ptr [rax], 0 + je */
    return p - s;
}

int my_strcmp(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

void *my_memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t*)dst;
    const uint8_t *s = (const uint8_t*)src;
    while (n--) *d++ = *s++;   /* rep movsb 在組譯就是這個 */
    return dst;
}

void *my_memset(void *dst, int v, size_t n) {
    uint8_t *d = (uint8_t*)dst;
    while (n--) *d++ = (uint8_t)v;   /* rep stosb */
    return dst;
}

void l0_mem_functions(void) {
    const char *s = "hello world";
    printf("[L0.6] my_strlen(\"%s\") = %zu  (vs strlen %zu)\n",
           s, my_strlen(s), strlen(s));
    printf("[L0.6] my_strcmp(\"abc\",\"abd\") = %d  (應為負)\n",
           my_strcmp("abc", "abd"));

    char buf[16];
    my_memset(buf, 'A', 8);
    buf[8] = '\0';
    my_memcpy(buf + 8, "XYZ", 3);
    buf[11] = '\0';
    printf("[L0.6] memset+memcpy 結果：\"%s\"\n", buf);
}

/* ── 0.7 printf format string 與暫存器對應 ── */
void l0_printf_format(void) {
    int     a = 42;
    double  b = 3.14;
    const char *c = "abc";
    void   *d = (void*)0xDEADBEEF;

    /* 呼叫時：rdi=fmt, rsi=a, xmm0=b, rdx=c, rcx=d (Linux SysV)
     * 浮點數走 xmm 暫存器是初學者最常忽略的點！ */
    printf("[L0.7] printf 參數：int=%d, double=%f, str=%s, ptr=%p\n", a, b, c, d);
    printf("       → %%d/%%s 從 rsi/rdx... 取，%%f 從 xmm0/xmm1... 取\n");

    /* format string 漏洞示意（CTF pwn 經典）：
     *   printf(user_input)  ← 危險！
     * 攻擊者輸入 "%x %x %x" 就能洩漏 stack 上的值 */
}

/* ── 0.8 XOR / 位移 / bit mask 全套 ── */
void l0_bit_tricks(void) {
    uint32_t x = 0xDEADBEEF;

    printf("[L0.8] 基本位元操作（x = 0x%08X）：\n", x);
    printf("  x << 4    = 0x%08X  (左移 4 = ×16)\n", x << 4);
    printf("  x >> 4    = 0x%08X  (右移 4 = ÷16)\n", x >> 4);
    printf("  x & 0xFF  = 0x%08X  (取低 8 bits)\n", x & 0xFF);
    printf("  x | 0xFF  = 0x%08X  (低 8 bits 全設 1)\n", x | 0xFF);
    printf("  x ^ x     = 0x%08X  (XOR 自己 = 0，組譯歸零招)\n", x ^ x);
    printf("  ~x        = 0x%08X  (按位取反)\n", ~x);

    printf("[L0.8] 經典 bit 招式：\n");
    int n = 0b1010100;
    printf("  n=0b1010100\n");
    printf("  n & (n-1) = 0x%X  (消掉最低位的 1)\n", n & (n-1));
    printf("  n & -n    = 0x%X  (取出最低位的 1，lowbit)\n", n & -n);

    /* XOR swap 不用 temp（CTF 中很常見） */
    int p = 5, q = 7;
    p ^= q; q ^= p; p ^= q;
    printf("  XOR swap：p=%d q=%d  (原 p=5 q=7)\n", p, q);

    /* 循環左移 rol（很多 crypto 用） */
    uint32_t v = 0x12345678;
    int r = 8;
    uint32_t rol = (v << r) | (v >> (32 - r));
    printf("  rol32(0x%08X, %d) = 0x%08X\n", v, r, rol);
}

void level0_demo(void) {
    l0_type_sizes();
    l0_pointer_arithmetic();
    l0_struct_padding();
    l0_calling_convention();
    l0_stack_frame();
    l0_mem_functions();
    l0_printf_format();
    l0_bit_tricks();
}


/* ════════════════════════════════════════════════════════════
 * Level 1：linked list + struct
 *
 * RE 觀察重點：
 *   - struct 在記憶體中是「連續」的 bytes
 *   - 編譯器會 padding 對齊（這裡 int 後接 pointer，總共 16 bytes）
 *   - 反組譯時看到 [rax+0x0] / [rax+0x8] 就是在存取 struct 欄位
 *   - 走 linked list 在 disasm 中是「mov rax, [rax+offset]」迴圈
 * ════════════════════════════════════════════════════════════ */
typedef struct Node {
    int          value;   /* offset 0 (4 bytes) */
    /* padding 4 bytes */
    struct Node *next;    /* offset 8 (8 bytes，因為 64-bit pointer) */
} Node;
/* sizeof(Node) == 16，記住這個！RE 中常要算 offset */

Node *list_create(int values[], int count) {
    /* 建立 linked list；反組譯時你會看到 malloc(16) 反覆呼叫 */
    Node *head = NULL;
    Node *tail = NULL;
    for (int i = 0; i < count; i++) {
        Node *n = (Node*)malloc(sizeof(Node));
        n->value = values[i];
        n->next  = NULL;
        if (head == NULL) head = n;
        else tail->next = n;
        tail = n;
    }
    return head;
}

int list_sum(Node *head) {
    /* 經典走訪 ── 反組譯會是：
     *   loop:
     *     mov  eax, [rdi+0x0]    ; 取 value
     *     add  ebx, eax
     *     mov  rdi, [rdi+0x8]    ; node = node->next
     *     test rdi, rdi
     *     jne  loop              ; 不為 NULL 繼續
     */
    int sum = 0;
    while (head != NULL) {
        sum += head->value;
        head = head->next;
    }
    return sum;
}

void level1_demo(void) {
    int data[] = {10, 20, 30, 40, 50};
    Node *list = list_create(data, 5);
    printf("[L1] linked list sum = %d (預期 150)\n", list_sum(list));
}


/* ════════════════════════════════════════════════════════════
 * Level 2：function pointer / jump table
 *
 * RE 觀察重點：
 *   - switch-case 編譯器常常優化成「jump table」
 *   - 反組譯會看到：jmp [rax*8 + JUMP_TABLE_ADDR]
 *   - function pointer array 在 .rodata 是「一連串 8-byte 位址」
 *   - 這是辨識「狀態機 / VM dispatch」的關鍵 pattern
 * ════════════════════════════════════════════════════════════ */
int op_add(int a, int b) { return a + b; }
int op_sub(int a, int b) { return a - b; }
int op_mul(int a, int b) { return a * b; }
int op_xor(int a, int b) { return a ^ b; }

/* 函式指標陣列 ── 在 .rodata 中是 4 個 8-byte 位址連續排列 */
typedef int (*BinOp)(int, int);
BinOp ops[] = { op_add, op_sub, op_mul, op_xor };

int dispatch(int opcode, int a, int b) {
    /* 反組譯會是：
     *   mov  rax, [ops + rdi*8]   ; rdi = opcode
     *   call rax
     * 看到這種 indirect call 就知道是 dispatch table
     */
    if (opcode < 0 || opcode > 3) return -1;
    return ops[opcode](a, b);
}

void level2_demo(void) {
    printf("[L2] dispatch(0, 10, 3) = %d (add → 13)\n", dispatch(0, 10, 3));
    printf("[L2] dispatch(3, 0xAB, 0xCD) = 0x%X (xor → 0x66)\n", dispatch(3, 0xAB, 0xCD));
}


/* ════════════════════════════════════════════════════════════
 * Level 3：自訂 hash + bit manipulation
 *
 * RE 觀察重點：
 *   - 移位、XOR、乘以「奇怪的質數」是 hash 函數特徵
 *   - 0x1F (= 31) 是字串 hash 的經典常數（Java String 也用這個）
 *   - 反組譯看到「shl/shr + xor + 累積」幾乎一定是 hash
 *   - CTF crackme 常常把密碼 hash 後跟某個常數比對
 * ════════════════════════════════════════════════════════════ */
uint32_t custom_hash(const char *s) {
    /* 經典 DJB2 變體 hash，CTF 中很常見 */
    uint32_t h = 5381;
    while (*s) {
        /* h = h * 33 + c   ── 33 = 32 + 1 = (1 << 5) + 1 */
        h = ((h << 5) + h) ^ (uint32_t)(*s);
        s++;
    }
    return h;
}

/* bit manipulation 經典招式 ── popcount（算 1 的個數） */
int popcount32(uint32_t x) {
    /* 反組譯會是 SWAR (SIMD Within A Register) pattern
     * 看到這串 0x55555555 / 0x33333333 / 0x0F0F0F0F 立刻認出！ */
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    return (x * 0x01010101) >> 24;
}

int check_password_l3(const char *input) {
    /* CTF 套路：hash 過後跟魔術值比對 */
    uint32_t target = 0x7C9A0E5F;  /* 預先算好的某字串 hash */
    return custom_hash(input) == target;
}

void level3_demo(void) {
    printf("[L3] hash(\"hello\") = 0x%08X\n", custom_hash("hello"));
    printf("[L3] popcount(0xDEADBEEF) = %d (預期 24)\n", popcount32(0xDEADBEEF));
}


/* ════════════════════════════════════════════════════════════
 * Level 4：迷你 VM interpreter（最重要！）
 *
 * 進階 RE 挑戰常見：題目把驗證邏輯編成 bytecode，主程式只剩
 * 一個 VM 在跑。你必須：
 *   1. 從反組譯認出「VM dispatch loop」（while + switch on opcode）
 *   2. 識別暫存器/stack/PC 的位置
 *   3. 把每個 opcode 對應的語意還原
 *   4. 從 .rodata 找到 bytecode 然後重寫一個 Python 模擬器
 *
 * 這裡實作一個 4-register、stack-less 的玩具 VM
 * ════════════════════════════════════════════════════════════ */
#define VM_REGS 4
#define VM_MEM  16

typedef enum {
    OP_HALT = 0x00,  /* 停止 */
    OP_LOAD = 0x01,  /* reg[a] = imm           ── 格式 [op][a][imm16] */
    OP_ADD  = 0x02,  /* reg[a] += reg[b]       ── 格式 [op][a][b][0] */
    OP_XOR  = 0x03,  /* reg[a] ^= reg[b]       ── 格式 [op][a][b][0] */
    OP_CMP  = 0x04,  /* 若 reg[a] != imm → reg[3]=1 */
    OP_JNZ  = 0x05,  /* 若 reg[3] != 0 跳到 imm */
    OP_OUT  = 0x06,  /* 印出 reg[a] */
} Opcode;

int vm_run(const uint8_t *code, int code_len) {
    uint32_t regs[VM_REGS] = {0};
    int pc = 0;

    /* 這個 while-switch 就是 RE 中要找的「dispatch loop」
     * 在 disasm 會看到一個大 switch jump table，每個 case 對應一個 opcode handler */
    while (pc < code_len) {
        uint8_t op = code[pc];
        switch (op) {
            case OP_HALT:
                return 0;
            case OP_LOAD: {
                uint8_t a    = code[pc+1];
                uint16_t imm = code[pc+2] | (code[pc+3] << 8);
                regs[a] = imm;
                pc += 4;
                break;
            }
            case OP_ADD: {
                regs[code[pc+1]] += regs[code[pc+2]];
                pc += 4;
                break;
            }
            case OP_XOR: {
                regs[code[pc+1]] ^= regs[code[pc+2]];
                pc += 4;
                break;
            }
            case OP_CMP: {
                uint8_t a    = code[pc+1];
                uint16_t imm = code[pc+2] | (code[pc+3] << 8);
                regs[3] = (regs[a] != imm) ? 1 : 0;
                pc += 4;
                break;
            }
            case OP_JNZ: {
                uint16_t target = code[pc+2] | (code[pc+3] << 8);
                if (regs[3] != 0) pc = target;
                else pc += 4;
                break;
            }
            case OP_OUT: {
                printf("[VM] r%d = 0x%X (%u)\n", code[pc+1], regs[code[pc+1]], regs[code[pc+1]]);
                pc += 4;
                break;
            }
            default:
                fprintf(stderr, "[VM] 未知 opcode 0x%02X @ pc=%d\n", op, pc);
                return -1;
        }
    }
    return 0;
}

void level4_demo(void) {
    /* 玩具程式：r0=0x1234, r1=0x00FF, r0 ^= r1, 印 r0
     * 對應的「bytecode」── RE 時你要從 binary 的 .rodata 找出這串 */
    uint8_t bytecode[] = {
        OP_LOAD, 0, 0x34, 0x12,   /* r0 = 0x1234 */
        OP_LOAD, 1, 0xFF, 0x00,   /* r1 = 0x00FF */
        OP_XOR,  0, 1,    0x00,   /* r0 ^= r1   → 0x12CB */
        OP_OUT,  0, 0,    0x00,   /* 印 r0 */
        OP_HALT, 0, 0,    0x00,
    };
    printf("[L4] 執行 VM bytecode（共 %zu bytes）：\n", sizeof(bytecode));
    vm_run(bytecode, sizeof(bytecode));
}


/* ════════════════════════════════════════════════════════════
 * main：呼叫慣例（calling convention）觀察
 *
 * x86-64 System V (Linux/macOS):
 *   參數順序：rdi, rsi, rdx, rcx, r8, r9，多的進 stack
 *   回傳值：rax
 *
 * Windows x64:
 *   參數順序：rcx, rdx, r8, r9，多的進 stack
 *   回傳值：rax
 *
 * 在 IDA 中看到 mov rdi, ... ; call func 就是 Linux 第一個參數
 * 在 IDA 中看到 mov rcx, ... ; call func 就是 Windows 第一個參數
 * ════════════════════════════════════════════════════════════ */
int main(void) {
    printf("===== Level 0: 基礎知識實作（型別/指標/padding/呼叫慣例/mem/bit）=====\n");
    level0_demo();
    printf("\n===== Level 1: struct + linked list =====\n");
    level1_demo();
    printf("\n===== Level 2: function pointer / jump table =====\n");
    level2_demo();
    printf("\n===== Level 3: hash + bit manipulation =====\n");
    level3_demo();
    printf("\n===== Level 4: 迷你 VM interpreter =====\n");
    level4_demo();
    return 0;
}
