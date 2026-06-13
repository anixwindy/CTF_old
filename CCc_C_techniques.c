/*
 * CCc_C_techniques.c
 * C++ 程式員必學的 C 技術全覽
 * 重點：跟 C++ 不同的地方 + CTF/RE/Pwn 最常見的 C 模式
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>   /* uint8_t, uint32_t, uint64_t 等固定寬度型別 */
#include <stdarg.h>   /* 可變參數 */
#include <assert.h>

/* ============================================================
 * 1. 預處理器 — C 沒有 constexpr，只有 #define 和 inline
 * ============================================================ */

#define MAX_BUF     256
#define SQ(x)       ((x) * (x))          /* 巨集要括號，避免展開錯誤 */
#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))  /* 靜態陣列長度 */

/* 條件編譯 */
#ifdef DEBUG
    #define LOG(fmt, ...) fprintf(stderr, "[DBG] " fmt "\n", ##__VA_ARGS__)
#else
    #define LOG(fmt, ...) /* 空的，編譯時直接消失 */
#endif

/* ============================================================
 * 2. 固定寬度型別 — CTF/RE 最重要：大小要精確
 * ============================================================ */
void demo_types(void) {
    /* C++ 的 int 大小是平台相依的，CTF 要用固定寬度 */
    uint8_t  byte_val  = 0xFF;           /* 1 byte  */
    uint16_t word_val  = 0xDEAD;         /* 2 bytes */
    uint32_t dword_val = 0xDEADBEEF;     /* 4 bytes */
    uint64_t qword_val = 0xDEADBEEFCAFEBABEULL; /* 8 bytes */

    /* size_t: 陣列索引、記憶體大小用這個，32/64 bit 自動調整 */
    size_t sz = sizeof(dword_val);

    /* ptrdiff_t: 指標相減的結果型別 */
    int arr[10];
    ptrdiff_t diff = &arr[5] - &arr[0];  /* = 5 */

    printf("byte=%u word=0x%X dword=0x%X qword=0x%llX sz=%zu diff=%td\n",
           byte_val, word_val, dword_val, qword_val, sz, diff);
}

/* ============================================================
 * 3. 字串 — C++ 有 std::string，C 只有 char*
 * ============================================================ */
void demo_strings(void) {
    /* 字串字面值：唯讀，存在 .rodata 段 */
    const char *ro = "hello";       /* 不能修改 */

    /* 可修改的字串：stack 上的陣列 */
    char buf[64] = "hello";         /* buf 是陣列，可改 */
    buf[0] = 'H';

    /* 常見字串函式 — 全靠手動，沒有 std::string 幫你 */
    size_t len   = strlen(buf);             /* 不含 \0 的長度 */
    char dst[64];
    strcpy(dst, buf);                       /* 複製，不檢查長度！CTF overflow 來源 */
    strncpy(dst, buf, sizeof(dst) - 1);     /* 安全版，但不保證 \0 結尾 */
    dst[sizeof(dst)-1] = '\0';              /* 手動補 \0 */

    strcat(dst, " world");                  /* 串接，同樣不檢查長度 */
    strncat(dst, "!!!", 3);

    int cmp = strcmp(buf, dst);             /* 0=相等 <0=前者小 >0=前者大 */

    /* sprintf / snprintf — 格式化到字串 */
    char out[128];
    int written = snprintf(out, sizeof(out), "len=%zu cmp=%d", len, cmp);
    /* snprintf 回傳理論上要寫的長度（不含\0），若 >= sizeof 表示被截斷 */
    (void)written; /* 抑制 unused warning */

    /* 字串搜尋 */
    char *found = strstr(buf, "ell");   /* 找子字串，沒找到回傳 NULL */
    char *ch    = strchr(buf, 'l');     /* 找字元 */
    (void)found; (void)ch;

    /* 字串轉數字 */
    int  n  = atoi("42");               /* 簡單，但錯誤不報 */
    long ln = strtol("0xFF", NULL, 16); /* 進階：可指定進位，可偵測錯誤 */
    (void)n; (void)ln;

    printf("buf=%s dst=%s\n", buf, dst);
}

/* ============================================================
 * 4. 指標深度 — C++ 程式員最容易忽略的細節
 * ============================================================ */
void demo_pointers(void) {
    /* 陣列名稱 = 第一個元素的指標（decay） */
    int arr[5] = {10, 20, 30, 40, 50};
    int *p = arr;               /* 等同 &arr[0] */

    /* 指標算術：+1 移動 sizeof(int) bytes */
    printf("%d %d\n", *p, *(p+2));      /* 10, 30 */
    printf("%d\n", p[3]);               /* 等同 *(p+3) = 40 */

    /* 指標相減 = 元素數量差 */
    int *end = arr + 5;
    printf("distance = %td\n", end - arr);  /* 5 */

    /* void* — 通用指標，cast 前無法解引用 */
    void *vp = arr;
    int  *ip = (int *)vp;               /* 需要 explicit cast */
    printf("via void*: %d\n", *ip);

    /* 指向指標的指標 */
    int x = 99;
    int *px  = &x;
    int **ppx = &px;
    **ppx = 100;
    printf("x = %d\n", x);             /* 100 */

    /* const 位置的差異 */
    const int  ci  = 5;
    const int *ptr_to_const = &ci;      /* 不能改 *ptr，但能改 ptr 自己指向哪 */
    int * const const_ptr   = ip;       /* 能改 *const_ptr，但不能改 const_ptr */
    const int * const both  = &ci;      /* 兩個都不能改 */
    (void)ptr_to_const; (void)const_ptr; (void)both;
}

/* ============================================================
 * 5. struct — C 沒有 class，靠 struct + function pointer 模擬
 * ============================================================ */

/* typedef 讓你不用每次寫 struct Node */
typedef struct Node {
    int data;
    struct Node *next;   /* 自我參照要寫完整 struct Node，不能只用 Node */
} Node;

/* Bit field — 緊湊封裝，CTF 的 flag 結構很常見 */
typedef struct {
    uint32_t readable  : 1;
    uint32_t writable  : 1;
    uint32_t executable: 1;
    uint32_t reserved  : 29;
} Permission;

/* 模擬 C++ 的 method：把 struct 自己當第一個參數傳入 */
typedef struct {
    int x, y;
    void (*print)(struct { int x; int y; } *self);  /* function pointer */
} Point;

/* 更乾淨的寫法：先 typedef 再用 */
typedef struct Animal Animal;
struct Animal {
    char name[32];
    int  (*speak)(Animal *self);   /* 虛函式模擬 */
};

int cat_speak(Animal *self) {
    printf("%s: meow\n", self->name);
    return 0;
}

void demo_struct(void) {
    /* 初始化：designated initializer（C99，C++ 也有但用法不同） */
    Node n1 = { .data = 42, .next = NULL };
    (void)n1;

    Permission perm = { .readable = 1, .writable = 0, .executable = 1 };
    printf("perm bits: %u\n", *(uint32_t *)&perm);  /* 讀出原始位元 */

    Animal cat = { .name = "Kitty", .speak = cat_speak };
    cat.speak(&cat);

    /* 結構體的 padding — RE 時要注意！ */
    struct PaddedExample {
        char  a;       /* 1 byte */
        /* 3 bytes padding */
        int   b;       /* 4 bytes，對齊到 4 */
        char  c;       /* 1 byte */
        /* 3 bytes padding */
    };
    printf("PaddedExample size = %zu\n", sizeof(struct PaddedExample));  /* 12 */

    /* 強制緊湊排列（GCC/Clang） */
    struct __attribute__((packed)) PackedExample {
        char a;
        int  b;
        char c;
    };
    printf("PackedExample size = %zu\n", sizeof(struct PackedExample));  /* 6 */
}

/* ============================================================
 * 6. 動態記憶體 — C++ 有 new/delete，C 只有 malloc/free
 * ============================================================ */
void demo_memory(void) {
    /* malloc：分配 n bytes，內容未初始化（可能有舊資料） */
    int *arr = (int *)malloc(10 * sizeof(int));
    if (arr == NULL) {  /* 一定要檢查 NULL！ */
        perror("malloc");
        return;
    }

    /* calloc：分配並清零 */
    int *zeroed = (int *)calloc(10, sizeof(int));

    /* realloc：調整大小，可能移動到新地址 */
    int *bigger = (int *)realloc(arr, 20 * sizeof(int));
    if (bigger == NULL) {
        free(arr);      /* realloc 失敗時，原指標仍有效 */
        free(zeroed);
        return;
    }
    arr = bigger;       /* 更新指標 */

    /* 使用 */
    for (int i = 0; i < 20; i++) arr[i] = i * 2;

    /* 釋放 */
    free(arr);
    free(zeroed);
    /* arr = NULL; */  /* 好習慣：釋放後設 NULL，防止 dangling pointer */

    /* 常見錯誤（只列出，不執行）：
     * - double free: free(arr); free(arr);    → crash / 漏洞
     * - use-after-free: free(p); *p = 1;     → 漏洞
     * - heap overflow: arr[20] = 99;          → 破壞 heap metadata
     * - 忘記 free                             → memory leak
     */
}

/* ============================================================
 * 7. 函式指標 — 回呼、虛函式表（vtable）的底層原理
 * ============================================================ */

int add(int a, int b) { return a + b; }
int mul(int a, int b) { return a * b; }

void demo_func_ptr(void) {
    /* 宣告：int (*fp)(int, int) */
    int (*op)(int, int);
    op = add;
    printf("add(3,4) = %d\n", op(3, 4));
    op = mul;
    printf("mul(3,4) = %d\n", op(3, 4));

    /* 函式指標陣列 */
    int (*ops[2])(int, int) = { add, mul };
    printf("ops[0](5,6) = %d\n", ops[0](5, 6));

    /* typedef 讓宣告更清晰 */
    typedef int (*BinaryOp)(int, int);
    BinaryOp fn = mul;
    printf("fn(7,8) = %d\n", fn(7, 8));
}

/* ============================================================
 * 8. 位元操作 — CTF 必備（加解密、flag 提取）
 * ============================================================ */
void demo_bitwise(void) {
    uint32_t x = 0b10110100;

    /* 基本操作 */
    printf("AND  : 0x%X\n", x & 0x0F);          /* 取低 4 bits */
    printf("OR   : 0x%X\n", x | 0x0F);           /* 設 低 4 bits */
    printf("XOR  : 0x%X\n", x ^ 0xFF);           /* 反轉低 8 bits */
    printf("NOT  : 0x%X\n", ~x);
    printf("LSH  : 0x%X\n", x << 2);             /* 左移 */
    printf("RSH  : 0x%X\n", x >> 2);             /* 右移 */

    /* 常用技巧 */
    int bit3 = (x >> 3) & 1;                     /* 取第 3 bit */
    x |=  (1u << 5);                             /* 設 bit 5 */
    x &= ~(1u << 5);                             /* 清 bit 5 */
    x ^=  (1u << 5);                             /* 翻 bit 5 */

    /* 位元計數（GCC 內建） */
    printf("popcount: %d\n", __builtin_popcount(0b10110101)); /* 5 */
    printf("clz: %d\n",      __builtin_clz(16));              /* 前置零 = 27 */

    /* Endianness 手動轉換（CTF 常用） */
    uint32_t val = 0x12345678;
    uint32_t swapped = ((val & 0xFF000000) >> 24) |
                       ((val & 0x00FF0000) >>  8) |
                       ((val & 0x0000FF00) <<  8) |
                       ((val & 0x000000FF) << 24);
    printf("bswap: 0x%X -> 0x%X\n", val, swapped);
    /* 或直接用 GCC 內建 */
    printf("__builtin_bswap32: 0x%X\n", __builtin_bswap32(val));

    (void)bit3;
}

/* ============================================================
 * 9. 記憶體函式 — RE/Pwn 最常見
 * ============================================================ */
void demo_memory_funcs(void) {
    uint8_t src[8] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48};
    uint8_t dst[8];

    /* memcpy：複製 n bytes，src/dst 不能重疊 */
    memcpy(dst, src, 8);

    /* memmove：允許重疊 */
    memmove(src + 2, src, 4);  /* src 自己移位 */

    /* memset：填充固定值 */
    memset(dst, 0x00, sizeof(dst));   /* 清零 */
    memset(dst, 0xCC, sizeof(dst));   /* 填 0xCC（int3 斷點） */

    /* memcmp：比較記憶體（不會因 \0 停止，比 strcmp 適合二進制資料） */
    int eq = memcmp(src, dst, 8);    /* 0 = 相同 */

    /* memchr：在記憶體中找 byte */
    void *pos = memchr(src, 0x43, 8);
    if (pos) printf("found 0x43 at offset %td\n", (uint8_t *)pos - src);

    (void)eq;
}

/* ============================================================
 * 10. 危險函式 — CTF target 常見，要認識漏洞點
 * ============================================================ */
void demo_dangerous(void) {
    char buf[64];

    /* gets()：完全沒有長度限制，已從 C11 移除，CTF classic overflow */
    /* gets(buf); */  /* 不執行，只標記：看到這個 = buffer overflow */

    /* scanf("%s")：同樣沒有長度限制 */
    /* scanf("%s", buf); */  /* 危險；安全版：scanf("%63s", buf) */

    /* sprintf：沒有長度限制 */
    /* sprintf(buf, "%s", very_long_string); */  /* 危險；用 snprintf */

    /* strcpy：沒有長度限制 */
    /* strcpy(buf, attacker_input); */  /* 危險；用 strncpy */

    /* printf(user_input)：格式化字串漏洞！ */
    /* printf(buf); */   /* 危險；一定要用 printf("%s", buf) */

    (void)buf;
    printf("[注意] 上面這些函式是 CTF pwn 題的核心漏洞來源\n");
}

/* ============================================================
 * 11. 可變參數 — printf 的底層原理
 * ============================================================ */
int my_sum(int count, ...) {
    va_list ap;
    va_start(ap, count);      /* 從 count 後面開始讀 */
    int total = 0;
    for (int i = 0; i < count; i++) {
        total += va_arg(ap, int);   /* 每次取出一個 int */
    }
    va_end(ap);
    return total;
}

/* ============================================================
 * 12. 清理模式 — C 沒有 RAII，用 goto 集中清理資源
 * ============================================================ */
int do_work(const char *filename) {
    FILE *f = NULL;
    char *buf = NULL;
    int ret = -1;

    f = fopen(filename, "r");
    if (!f) { ret = -1; goto cleanup; }

    buf = (char *)malloc(1024);
    if (!buf) { ret = -2; goto cleanup; }

    /* 正常工作 */
    ret = 0;

cleanup:
    free(buf);          /* free(NULL) 是安全的，不會 crash */
    if (f) fclose(f);
    return ret;
}

/* ============================================================
 * 13. main 與命令列參數
 * ============================================================ */
int main(int argc, char *argv[]) {
    /* argc = 參數數量（含程式名稱本身）
     * argv[0] = 程式路徑
     * argv[1..argc-1] = 使用者傳入的參數
     * argv[argc] = NULL（哨兵）
     */
    printf("=== CCc_C_techniques demo ===\n");

    demo_types();
    demo_strings();
    demo_pointers();
    demo_struct();
    demo_memory();
    demo_func_ptr();
    demo_bitwise();
    demo_memory_funcs();
    demo_dangerous();

    printf("my_sum(3, 10,20,30) = %d\n", my_sum(3, 10, 20, 30));

    if (argc > 1) {
        printf("第一個參數: %s\n", argv[1]);
    }

    return 0;   /* 回傳 0 = 成功；非 0 = 錯誤碼 */
}
