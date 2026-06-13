// Day1_strlen_vs_sizeof.c
// 為什麼 CTF 在乎：RE 看到 buffer 比較長度時，要分清楚比的是「字串內容長度」
//                  還是「buffer 容量」，這決定了哪邊有溢位空間。
#include <stdio.h>
#include <string.h>

int main(void) {
    char buf[64] = "hello";
    printf("strlen(buf) = %zu\n", strlen(buf));   // 5（字串實際內容到 '\0'）
    printf("sizeof(buf) = %zu\n", sizeof(buf));   // 64（buffer 大小）
    // 漏洞點：if (strlen(input) < sizeof(buf)) 看似安全，但 strncpy 沒補 '\0' 就會出事
    return 0;
}
