/*
 * 1_RE_c_foundation.c
 * 合併自：
 *   - 1_re_concepts_c.c
 *   - 1_RE_must_know_c.c
 *
 * 用途：
 *   - 作為 RE -> Pwn 主線的 C 基礎總檔
 *   - 保留最常用、最值得反覆看的底層觀念
 *   - 把重複高的內容收斂成一份，減少切檔
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ============================================================
 * 一、型別大小與小端序
 *   RE 時計算 offset、步長、記憶體內容都靠這個
 * ============================================================ */
void demo_types_and_endian(void) {
    printf("=== type sizes ===\n");
    printf("char      = %zu\n", sizeof(char));
    printf("short     = %zu\n", sizeof(short));
    printf("int       = %zu\n", sizeof(int));
    printf("long      = %zu\n", sizeof(long));
    printf("long long = %zu\n", sizeof(long long));
    printf("pointer   = %zu\n", sizeof(void *));

    uint32_t x = 0x12345678;
    unsigned char *p = (unsigned char *)&x;
    printf("0x12345678 bytes = %02X %02X %02X %02X\n",
           p[0], p[1], p[2], p[3]);
    printf("x86/x64 是 little-endian，所以低位元組 78 先放\n");
}

/* ============================================================
 * 二、指標、陣列、字串
 *   RE 時最常看到的其實就是位址、offset、解參考
 * ============================================================ */
void demo_pointer_and_array(void) {
    int x = 42;
    int *ptr = &x;
    printf("x = %d, *ptr = %d\n", x, *ptr);

    *ptr = 100;
    printf("after *ptr=100, x = %d\n", x);

    int arr[4] = {10, 20, 30, 40};
    int *base = arr;
    printf("arr[2]   = %d\n", arr[2]);
    printf("*(arr+2) = %d\n", *(arr + 2));
    printf("base[2]  = %d\n", base[2]);

    char buf[16] = "hello";
    printf("strlen(buf) = %zu\n", strlen(buf));
    printf("sizeof(buf) = %zu\n", sizeof(buf));
    printf("C string 結尾一定有 null byte\n");
}

/* ============================================================
 * 三、struct、padding、offset
 *   [reg+offset] 很多時候就是 struct 成員
 * ============================================================ */
typedef struct {
    int   age;       /* +0  */
    float score;     /* +4  */
    char  name[8];   /* +8  */
} Student;           /* total 16 */

typedef struct {
    char a;          /* +0  */
    int  b;          /* +4  */
} Padded;            /* total 8, 不是 5 */

void demo_struct_layout(void) {
    Student s = {20, 95.5f, "Alice"};
    float *score_ptr = (float *)((char *)&s + 4);

    printf("score via offset+4 = %.1f\n", *score_ptr);
    printf("sizeof(Student) = %zu\n", sizeof(Student));
    printf("sizeof(Padded)  = %zu\n", sizeof(Padded));
}

/* ============================================================
 * 四、stack frame 與 calling convention
 *   Pwn 會一直用到這一層
 * ============================================================ */
int add_ints(int x, int y) {
    return x + y;
}

int demo_stackframe(int a, int b) {
    int local_a = a + 1;   /* 常見於 [rbp-4] */
    int local_b = b * 2;   /* 常見於 [rbp-8] */
    return local_a + local_b;
}

void demo_calling_convention(void) {
    int result = add_ints(3, 4);
    printf("add_ints(3,4) = %d\n", result);
    printf("Linux x64 前 6 個整數參數常用 RDI RSI RDX RCX R8 R9\n");
    printf("回傳值通常在 RAX\n");
}

/* ============================================================
 * 五、控制流與位元運算
 *   RE 題的比較、跳轉、簡單轉換常在這裡
 * ============================================================ */
void demo_control_and_bitops(int n) {
    if (n > 0) {
        printf("positive\n");
    } else if (n == 0) {
        printf("zero\n");
    } else {
        printf("negative\n");
    }

    for (int i = 0; i < n; i++) {
        printf("loop i=%d\n", i);
    }

    switch (n) {
        case 1: printf("one\n"); break;
        case 2: printf("two\n"); break;
        default: printf("other\n"); break;
    }

    unsigned int a = 0xB4;
    unsigned int b = 0x3A;
    printf("AND = %02X\n", a & b);
    printf("OR  = %02X\n", a | b);
    printf("XOR = %02X\n", a ^ b);
    printf("bit3 of a = %u\n", (a >> 3) & 1u);
}

/* ============================================================
 * 六、heap 與危險字串操作
 *   RE 要看懂指標流向，Pwn 要看懂危險點
 * ============================================================ */
void demo_heap_and_string_ops(void) {
    int *heap_arr = (int *)malloc(4 * sizeof(int));
    if (!heap_arr) {
        puts("malloc failed");
        return;
    }

    for (int i = 0; i < 4; i++) {
        heap_arr[i] = i * 10;
    }
    printf("heap_arr[2] = %d\n", heap_arr[2]);
    free(heap_arr);

    char dst[16] = "hi";
    const char *src = " there";
    strcat(dst, src);
    printf("dst = %s\n", dst);
    printf("strcpy/strcat/printf(user_input) 這類函式都要特別留意長度與格式字串\n");
}

/* ============================================================
 * 七、最小 RE 觀察重點
 * ============================================================ */
void demo_re_notes(void) {
    puts("RE 起手重點：");
    puts("1. 先看型別大小、offset、字串、常數");
    puts("2. 再看控制流和比較點");
    puts("3. 最後才進 gdb 驗證假設");
}

int main(void) {
    demo_types_and_endian();
    puts("");

    demo_pointer_and_array();
    puts("");

    demo_struct_layout();
    puts("");

    printf("stackframe result = %d\n", demo_stackframe(5, 3));
    demo_calling_convention();
    puts("");

    demo_control_and_bitops(3);
    puts("");

    demo_heap_and_string_ops();
    puts("");

    demo_re_notes();
    return 0;
}
