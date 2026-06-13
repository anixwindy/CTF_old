/* ============================================================
 * Clau__CTF_c_examples.c
 * CTF 玩家「看得懂 C 原始碼 / 反編譯結果」必備範例
 * 不是用來編譯跑的，是用來「讀」的
 * 所有解說：繁體中文
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

/* ------------------------------------------------------------
 * [初階 1] 指標、位址、解參考
 * ------------------------------------------------------------
 * 反編譯結果常見：
 *   *(_DWORD *)(a1 + 0x10) = 0x41414141;
 * 對應 C：把 (a1+0x10) 當成 int* 寫 0x41414141 進去。
 * 看到 _DWORD = 4 bytes、_QWORD = 8 bytes、_BYTE = 1 byte。
 */
void demo_pointer(void) {
    int x = 0xdeadbeef;
    int *p = &x;          /* p 指向 x */
    *p = 0x11223344;      /* 透過 p 改 x */
    printf("x = %x\n", x);

    /* 指標算術：p+1 跳一個 int (4 bytes) */
    int arr[3] = {1, 2, 3};
    int *q = arr;
    printf("arr[2] = %d\n", *(q + 2));
}

/* ------------------------------------------------------------
 * [初階 2] stack buffer overflow（最經典的 pwn 漏洞）
 * ------------------------------------------------------------
 * gets 不檢查長度 → 輸入超過 buf 大小就會蓋到 saved RBP、saved RIP
 * 控制 saved RIP → 控制執行流。
 */
void vuln_gets(void) {
    char buf[64];
    gets(buf);            /* 危險函式：永遠不要用 */
    puts(buf);
}

/* strcpy / strcat / sprintf / scanf("%s") 也都沒長度檢查 */
void vuln_strcpy(const char *src) {
    char buf[16];
    strcpy(buf, src);     /* src 太長就溢位 */
    puts(buf);
}

/* ------------------------------------------------------------
 * [初階 3] ret2win：題目藏個 win 函式，目標是讓程式跳到它
 * ------------------------------------------------------------
 * payload 結構（64-bit）：
 *   'A' * offset_to_saved_rip + p64(&win)
 * 32-bit 則是 'A' * offset + p32(&win)
 */
void win(void) {
    system("/bin/sh");    /* 拿 shell */
}

/* ------------------------------------------------------------
 * [初階 4] format string 漏洞
 * ------------------------------------------------------------
 * 把使用者輸入直接當 printf 的 format string → 災難
 * 攻擊者可以：
 *   %x / %p   : 洩漏 stack 內容（leak canary / libc / PIE）
 *   %s        : deref 指標 → 洩漏記憶體內容
 *   %n        : 把目前已輸出字元數「寫」回某位址 → 任意寫
 */
void vuln_fmtstr(char *user_input) {
    printf(user_input);              /* 漏洞：應該是 printf("%s", user_input); */
}

/* ------------------------------------------------------------
 * [初階 5] 整數溢位 / 有號無號比較
 * ------------------------------------------------------------
 * if (n < SIZE) { read(0, buf, n); }
 * 看似安全，若 n 是 signed int 而被輸入 -1：
 *   -1 < SIZE 為真 → read 把 n 當 size_t 解 → 變成 0xffffffff → 巨大讀取
 */
void vuln_intoverflow(void) {
    char buf[64];
    int n;
    scanf("%d", &n);
    if (n < (int)sizeof(buf)) {       /* signed 比較 → 負數會通過 */
        read(0, buf, n);              /* read 第三參數是 size_t → 變超大 */
    }
}

/* ------------------------------------------------------------
 * [中階 1] heap 漏洞：UAF (Use-After-Free)
 * ------------------------------------------------------------
 * 釋放後仍持有 dangling pointer 並使用 → 攻擊者可控其內容
 */
struct user {
    char name[32];
    void (*print_fn)(struct user *);
};
void normal_print(struct user *u) { printf("hi %s\n", u->name); }

void vuln_uaf(void) {
    struct user *u = malloc(sizeof(*u));
    u->print_fn = normal_print;
    free(u);                       /* 釋放但指標還在 */
    /* 攻擊者讓 malloc 拿回同一塊（tcache LIFO） */
    char *atk = malloc(sizeof(*u));
    *(void**)(atk + offsetof(struct user, print_fn)) = (void*)win;
    u->print_fn(u);                /* 跳到 win！ */
}

/* ------------------------------------------------------------
 * [中階 2] double free
 * ------------------------------------------------------------
 * glibc tcache：對同一 chunk free 兩次 → tcache 出現兩個指到同位置的節點
 * 下一次 malloc 拿到第一份 → 修改 fd → 之後 malloc 拿到任意位址
 */
void vuln_double_free(void) {
    char *a = malloc(32);
    free(a);
    free(a);                        /* glibc 新版會偵測，舊版會過 */
}

/* ------------------------------------------------------------
 * [中階 3] off-by-one：少算一個 byte 也會出大事
 * ------------------------------------------------------------
 * 例：char buf[16]; for (i=0; i<=16; i++) buf[i]=...;
 * 第 17 次寫入會把下一個 chunk 的 size 最低 byte 蓋掉（poison null byte）
 */
void vuln_off_by_one(char *src) {
    char buf[16];
    int i;
    for (i = 0; i <= 16; i++) {     /* 應該是 i < 16 */
        buf[i] = src[i];
    }
}

/* ------------------------------------------------------------
 * [RE 視角 1] 位元運算 / endian
 * ------------------------------------------------------------
 * CTF 題目常見「把 32-bit 數字看成 4 個 byte」的處理。
 */
void demo_endian(void) {
    uint32_t v = 0x11223344;
    uint8_t *p = (uint8_t*)&v;
    /* x86/x64 是 little-endian → p[0]=0x44 p[1]=0x33 p[2]=0x22 p[3]=0x11 */
    printf("byte order: %02x %02x %02x %02x\n", p[0], p[1], p[2], p[3]);

    /* 旋轉位移（ROR/ROL）在很多 RE 自製演算法常用 */
    uint32_t x = 0x12345678;
    uint32_t rotl = (x << 5) | (x >> (32 - 5));   /* 左轉 5 bit */
    printf("rotl5 = %08x\n", rotl);
}

/* ------------------------------------------------------------
 * [RE 視角 2] 函式指標 / vtable 雛形
 * ------------------------------------------------------------
 * 反編譯看到 (*(void(**)(void))(obj+0x18))(); 就是「呼叫物件偏移 0x18 的函式指標」。
 */
typedef struct {
    int  id;
    void (*hello)(void);
} obj_t;
void say_hi(void) { puts("hi"); }
void demo_funcptr(void) {
    obj_t o = { 1, say_hi };
    o.hello();
}

/* ------------------------------------------------------------
 * [RE 視角 3] 字串混淆：runtime 解 XOR
 * ------------------------------------------------------------
 * 為避免 strings 直接看到 flag，作者常把字串 XOR 過後寫死，runtime 才解。
 */
void demo_xor_string(void) {
    /* 加密：原字串 "flag{demo}" XOR 0x55 */
    unsigned char enc[] = {0x33,0x39,0x34,0x32,0x2e,0x31,0x30,0x38,0x2f,0x00};
    for (int i = 0; enc[i]; i++) enc[i] ^= 0x55;
    printf("decoded: %s\n", enc);
}

/* ------------------------------------------------------------
 * [中階] system() / execve() 是 pwn 終點
 * ------------------------------------------------------------
 * pwn 目標通常是讓程式呼叫：
 *   system("/bin/sh");
 *   execve("/bin/sh", NULL, NULL);
 * 在 ROP / GOT overwrite / shellcode 都圍繞這幾個函式打。
 */

int main(void) {
    /* 這裡只示意呼叫，實際題目通常 main 是另外的邏輯 */
    demo_pointer();
    demo_endian();
    demo_funcptr();
    demo_xor_string();
    return 0;
}
