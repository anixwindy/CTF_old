// Day1_stack_vs_heap.c
// 為什麼 CTF 在乎：BOF 攻擊 stack，UAF / heap overflow 攻擊 heap，
//                  要先知道哪些變數住哪裡。
#include <stdio.h>
#include <stdlib.h>

char *get_stack(void) {
    char local[16] = "I am on stack";
    return local;            // ⚠️ 回傳 stack 變數 → 函式結束就釋放，外面拿到的是垃圾
}

char *get_heap(void) {
    char *p = malloc(16);
    snprintf(p, 16, "I am on heap");
    return p;                // heap 上的記憶體，外面用完要自己 free()
}

int main(void) {
    char *s = get_stack();
    char *h = get_heap();
    printf("stack: %s\n", s);  // 可能亂碼 / segfault
    printf("heap : %s\n", h);  // 正常
    free(h);
    return 0;
}
