// Day1_endianness.c
// 為什麼 CTF 在乎：Pwn payload 寫地址時要 little-endian 排序
//                  Python 用 p64() 自動處理，但你要看得懂為什麼
#include <stdio.h>
int main(void) {
    unsigned int x = 0x12345678;
    unsigned char *p = (unsigned char *)&x;
    // 記憶體實際 byte 順序：78 56 34 12（不是 12 34 56 78）
    for (int i = 0; i < 4; i++) printf("p[%d] = 0x%02x\n", i, p[i]);
    return 0;
}
