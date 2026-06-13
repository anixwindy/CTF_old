// Day1_signed_unsigned.c
// 為什麼 CTF 在乎：很多 BOF 就是「長度欄位用 signed」造成的
//                  使用者送 -1 → 比較時當成「小於 buffer size」過關
//                  但接著當參數傳給 memcpy(size_t) 就變超大數 → 寫爆 buffer
#include <stdio.h>
#include <string.h>

void copy_data(char *src, int len) {
    char buf[64];
    if (len > 64) { puts("too big"); return; }   // ⚠️ len 是 signed
    memcpy(buf, src, len);                       // memcpy 第三參數是 size_t (unsigned)
    // 使用者送 len = -1 → -1 > 64 為 false 過關 → memcpy(buf, src, 0xFFFFFFFFFFFFFFFF)
}

int main(void) {
    int a = -1;
    unsigned int b = 1;
    if (a > b) puts("yes (因為 -1 被轉成 unsigned 變超大數)");
    // asm：cmp + ja/jb → 編譯器認為這是 unsigned 比較（RE 重要線索）
    //      cmp + jg/jl → signed 比較
    return 0;
}
