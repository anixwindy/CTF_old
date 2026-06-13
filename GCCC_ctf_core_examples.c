/*
 * CCC_CTF C 核心範例
 *
 * 目的：
 * - 示範初中階 RE / Pwn 常遇到的 C 語言行為。
 * - 範例只做本機教學，說明漏洞型態，不做真實目標攻擊。
 *
 * 編譯參考：
 *   gcc GCc_CCC_ctf_core_examples.c -o GCc_CCC_ctf_core_examples
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void CCC_line(const char *title) {
    printf("\n--------\n");
    printf("%s\n", title);
}

static void CCC_RE_endian_bytes(void) {
    CCC_line("CCC_RE_endian_bytes");

    uint32_t value = 0x41424344u;
    unsigned char *p = (unsigned char *)&value;

    /*
     * 在 little-endian 機器上，記憶體會先放最低位元組。
     * 所以 0x41424344 在記憶體中會像 44 43 42 41。
     */
    printf("value as number: 0x%08x\n", value);
    printf("value as bytes : %02x %02x %02x %02x\n", p[0], p[1], p[2], p[3]);
}

static void CCC_RE_xor_decode(void) {
    CCC_line("CCC_RE_xor_decode");

    unsigned char encrypted[] = {0x16, 0x01, 0x13, 0x0a, 0x0d, 0x1a, 0x07};
    unsigned char key = 0x55;
    size_t n = sizeof(encrypted) / sizeof(encrypted[0]);

    printf("decoded: ");
    for (size_t i = 0; i < n; i++) {
        unsigned char plain = encrypted[i] ^ key;
        putchar((int)plain);
    }
    putchar('\n');
}

static void CCC_RE_bit_masks(void) {
    CCC_line("CCC_RE_bit_masks");

    uint8_t b = 0xb6; /* 二進位：1011 0110 */
    uint8_t high = (uint8_t)((b >> 4) & 0x0f);
    uint8_t low = (uint8_t)(b & 0x0f);

    /*
     * bit mask 在 RE 很常見，因為程式會把多個小數值塞進
     * 一個 byte 或一個整數裡。
     */
    printf("byte       : 0x%02x\n", b);
    printf("high nibble: 0x%x\n", high);
    printf("low nibble : 0x%x\n", low);
}

static void CCC_RE_strlen_vs_sizeof(void) {
    CCC_line("CCC_RE_strlen_vs_sizeof");

    char data[] = {'C', 'T', 'F', '\0', 'X', 'Y', '\0'};

    /*
     * strlen 遇到第一個 NUL byte 就會停。
     * sizeof(data) 在這個 scope 會回傳整個陣列大小。
     * 題目比較 raw bytes 時，這個差異很重要。
     */
    printf("sizeof(data): %zu\n", sizeof(data));
    printf("strlen(data): %zu\n", strlen(data));
}

static void CCC_RE_memcmp_vs_strcmp(void) {
    CCC_line("CCC_RE_memcmp_vs_strcmp");

    char a[] = {'A', 'B', '\0', 'X'};
    char b[] = {'A', 'B', '\0', 'Y'};

    /*
     * strcmp 會覺得相等，因為兩邊都在第一個 NUL 結束。
     * memcmp 會看完整 4 bytes，所以能看出 X 和 Y 不同。
     */
    printf("strcmp result: %d\n", strcmp(a, b));
    printf("memcmp result: %d\n", memcmp(a, b, sizeof(a)));
}

static void CCC_PWN_stack_layout_concept(void) {
    CCC_line("CCC_PWN_stack_layout_concept");

    char buffer[8] = "AAAA";
    uint32_t marker = 0x12345678u;

    /*
     * 這裡只印出位址，用來理解相對位置。
     * 真正 stack 排列會受 compiler、最佳化、ABI、保護機制影響。
     */
    printf("buffer address: %p\n", (void *)buffer);
    printf("marker address: %p\n", (void *)&marker);
    printf("marker value  : 0x%08x\n", marker);
}

static void CCC_PWN_bounds_check_pattern(void) {
    CCC_line("CCC_PWN_bounds_check_pattern");

    char dst[8];
    const char *user_input = "TOO_LONG_INPUT";

    /*
     * 常見 bug 是沒有檢查長度就複製使用者輸入。
     * 這個範例改成安全檢查，不真的讓 dst overflow。
     */
    if (strlen(user_input) < sizeof(dst)) {
        strcpy(dst, user_input);
        printf("copied: %s\n", dst);
    } else {
        printf("blocked copy: input length %zu does not fit into %zu bytes\n",
               strlen(user_input), sizeof(dst));
    }
}

static void CCC_PWN_integer_wrap(void) {
    CCC_line("CCC_PWN_integer_wrap");

    uint8_t length = 250;
    uint8_t total = (uint8_t)(length + 20);

    /*
     * uint8_t 只能存 0..255。250 + 20 會繞回 14。
     * 真實漏洞中，這可能讓長度檢查或配置大小變得太小。
     */
    printf("length: %u\n", (unsigned)length);
    printf("length + 20 stored in uint8_t: %u\n", (unsigned)total);
}

static void CCC_PWN_signed_unsigned(void) {
    CCC_line("CCC_PWN_signed_unsigned");

    int signed_len = -1;
    unsigned int unsigned_len = (unsigned int)signed_len;

    /*
     * 負的 signed 數值轉成 unsigned 後，可能變成很大的正數。
     * 如果型別混用不小心，bounds check 很容易被破壞。
     */
    printf("signed_len  : %d\n", signed_len);
    printf("unsigned_len: %u\n", unsigned_len);
}

static void CCC_CRYPTO_modular_math(void) {
    CCC_line("CCC_CRYPTO_modular_math");

    int a = 17;
    int n = 5;

    /*
     * modular arithmetic 會出現在 Caesar shift、affine cipher、RSA。
     */
    printf("17 mod 5: %d\n", a % n);
    printf("(17 + 9) mod 5: %d\n", (17 + 9) % n);
    printf("(17 * 9) mod 5: %d\n", (17 * 9) % n);
}

static void CCC_C_pointer_arithmetic(void) {
    CCC_line("CCC_C_pointer_arithmetic");

    uint32_t nums[] = {0x11111111u, 0x22222222u, 0x33333333u};
    uint32_t *p = nums;
    unsigned char *byte_p = (unsigned char *)nums;

    /*
     * p + 1 會前進一個 uint32_t 元素。
     * byte_p + 1 只會前進一個 byte。
     */
    printf("nums[0] via p      : 0x%08x\n", *(p + 0));
    printf("nums[1] via p + 1  : 0x%08x\n", *(p + 1));
    printf("first raw byte     : 0x%02x\n", byte_p[0]);
}

int main(void) {
    CCC_RE_endian_bytes();
    CCC_RE_xor_decode();
    CCC_RE_bit_masks();
    CCC_RE_strlen_vs_sizeof();
    CCC_RE_memcmp_vs_strcmp();

    CCC_PWN_stack_layout_concept();
    CCC_PWN_bounds_check_pattern();
    CCC_PWN_integer_wrap();
    CCC_PWN_signed_unsigned();

    CCC_CRYPTO_modular_math();
    CCC_C_pointer_arithmetic();

    return 0;
}
