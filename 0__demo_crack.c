/*
 * 1_CRE_D_crackme.c — RE 入門練習
 *
 * 三個關卡，難度遞增：
 *   Level 1: 明文密碼，strings 指令直接找到
 *   Level 2: XOR 編碼密碼，要逆向解碼邏輯
 *   Level 3: 逐字元比對，要看懂迴圈邏輯
 *
 * 編譯：gcc -O0 -o crackme 1_CRE_D_crackme.c
 * 執行：./crackme <level> <password>
 *   例：./crackme 1 hello
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ───── Level 1：明文密碼，strings 指令直接看到 ───── */
void level1(const char *input) {
    const char *secret = "CTF_easy_flag";
    if (strcmp(input, secret) == 0)
        puts("[Level 1] Correct! 你用 strings 就找到我了吧？");
    else
        puts("[Level 1] Wrong password.");
}

/* ───── Level 2：XOR 編碼，要逆向解碼邏輯 ───── */
void level2(const char *input) {
    /* 原始密碼 "xor_magic" 每個字元 XOR 0x42 後存起來 */
    unsigned char encoded[] = {0x3a, 0x2d, 0x30, 0x10, 0x2f, 0x27, 0x2b, 0x2f, 0x2b};
    int len = sizeof(encoded);

    if ((int)strlen(input) != len) {
        puts("[Level 2] Wrong password.");
        return;
    }

    for (int i = 0; i < len; i++) {
        if ((unsigned char)(input[i] ^ 0x42) != encoded[i]) {
            puts("[Level 2] Wrong password.");
            return;
        }
    }
    puts("[Level 2] Correct! 你看懂 XOR 邏輯了！");
}

/* ───── Level 3：逐字元比對 + 位移，要追蹤每個字元的運算 ───── */
void level3(const char *input) {
    /* 原始密碼 "rev_hard" 每個字元加上 index 後存起來 */
    unsigned char table[] = {0x72, 0x66, 0x78, 0x5f, 0x6c, 0x62, 0x72, 0x6b};
    int len = sizeof(table);

    if ((int)strlen(input) != len) {
        puts("[Level 3] Wrong password.");
        return;
    }

    for (int i = 0; i < len; i++) {
        if ((unsigned char)(input[i] + i) != table[i]) {
            puts("[Level 3] Wrong password.");
            return;
        }
    }
    puts("[Level 3] Correct! 你真的逆向成功了！");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("用法：./crackme <level> <password>\n");
        printf("  level 1 2 3\n");
        return 1;
    }

    int level = atoi(argv[1]);
    const char *input = argv[2];

    switch (level) {
        case 1: level1(input); break;
        case 2: level2(input); break;
        case 3: level3(input); break;
        default: puts("level 只有 1 2 3"); break;
    }
    return 0;
}
