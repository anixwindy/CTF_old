//
// Created by user on 2026/5/16.
//
//
// Created by user on 2026/5/16.
//
// ============================================================
//  C 語言完整複習手冊
//  適合：學過但忘光光的你 😄
// ============================================================

#include <stdio.h>    // printf, scanf
#include <stdlib.h>   // malloc, free, exit
#include <string.h>   // strlen, strcpy, strcmp
#include <math.h>     // sqrt, pow  (編譯時加 -lm)

// ============================================================
// 【1】基本資料型別
// ============================================================
void demo_types() {
    // 整數
    int    a = 10;          // 4 bytes, -2147483648 ~ 2147483647
    short  b = 100;         // 2 bytes
    long   c = 100000L;     // 4 or 8 bytes
    long long d = 9999999LL;// 8 bytes

    // 無號整數（不能負數，範圍加倍）
    unsigned int e = 4000000000U;

    // 浮點數
    float  f = 3.14f;       // 4 bytes, 精度約 6-7 位
    double g = 3.14159265;  // 8 bytes, 精度約 15-16 位

    // 字元（本質是數字）
    char h = 'A';           // 'A' = 65
    printf("%c = %d\n", h, h); // 印出 A = 65

    // 布林（C99 之後）
    // #include <stdbool.h>
    // bool flag = true;

    printf("int size: %zu bytes\n", sizeof(int));
}

// ============================================================
// 【2】輸入 / 輸出
// ============================================================
void demo_io() {
    int age;
    char name[50];
    float score;

    // printf 格式符號
    // %d = int,  %f = float,  %lf = double
    // %c = char, %s = string, %p = 指標位址
    // %x = 16進位, %o = 8進位, %zu = size_t

    printf("請輸入名字和年齡: ");
    scanf("%s %d", name, &age);   // 注意：int要加 &，陣列不用

    printf("請輸入分數: ");
    scanf("%f", &score);

    printf("你好，%s，年齡 %d，分數 %.2f\n", name, age, score);

    // 讀一整行（含空格）
    // fgets(name, sizeof(name), stdin);

    // 格式化輸出技巧
    printf("|%10d|\n", 42);     // 右對齊，寬度10  |        42|
    printf("|%-10d|\n", 42);    // 左對齊          |42        |
    printf("|%05d|\n", 42);     // 補零            |00042|
    printf("|%.3f|\n", 3.14159);// 小數3位         |3.142|
}

// ============================================================
// 【3】運算子
// ============================================================
void demo_operators() {
    int a = 10, b = 3;

    // 算術
    printf("%d %d %d %d %d\n", a+b, a-b, a*b, a/b, a%b);
    // 10/3 = 3（整數除法，捨去小數！）
    // 10%3 = 1（餘數）

    // 位元運算（CTF 很常用！）
    printf("AND: %d\n",  a & b);   // 0000 1010 & 0000 0011 = 0000 0010 = 2
    printf("OR:  %d\n",  a | b);   // = 11
    printf("XOR: %d\n",  a ^ b);   // = 9  （XOR在加密很重要！）
    printf("NOT: %d\n",  ~a);      // = -11
    printf("左移: %d\n", a << 1);  // = 20  (* 2)
    printf("右移: %d\n", a >> 1);  // = 5   (/ 2)

    // 比較 (結果是 0 或 1)
    printf("%d\n", a > b);   // 1
    printf("%d\n", a == b);  // 0

    // 邏輯
    printf("%d\n", (a>5) && (b<5));  // 1 (AND)
    printf("%d\n", (a>5) || (b>5));  // 1 (OR)
    printf("%d\n", !(a>5));          // 0 (NOT)
}

// ============================================================
// 【4】流程控制
// ============================================================
void demo_control() {
    int x = 10;

    // if else if
    if (x > 10) {
        printf("大於10\n");
    } else if (x == 10) {
        printf("等於10\n");
    } else {
        printf("小於10\n");
    }

    // 三元運算子
    int max = (x > 5) ? x : 5;  // x>5 ? 真的值 : 假的值

    // switch（只能比整數或字元）
    char grade = 'B';
    switch (grade) {
        case 'A': printf("優秀\n"); break;
        case 'B': printf("良好\n"); break;
        case 'C': printf("普通\n"); break;
        default:  printf("其他\n");
        // 注意：沒有 break 會「穿透」到下一個 case！
    }

    // for 迴圈
    for (int i = 0; i < 5; i++) {
        printf("%d ", i);  // 0 1 2 3 4
    }

    // while 迴圈
    int n = 5;
    while (n > 0) {
        printf("%d ", n--);  // 5 4 3 2 1
    }

    // do-while（至少執行一次）
    do {
        printf("至少執行一次\n");
        n--;
    } while (n > 0);

    // break / continue
    for (int i = 0; i < 10; i++) {
        if (i == 3) continue;  // 跳過 3
        if (i == 7) break;     // 到7就停
        printf("%d ", i);      // 0 1 2 4 5 6
    }
}

// ============================================================
// 【5】函式（Function）
// ============================================================

// 宣告（Prototype）放上面，定義可以放下面
int add(int a, int b);
void swap(int *a, int *b);   // 要改變值，必須傳指標！

int add(int a, int b) {
    return a + b;
}

// 傳指標才能真正交換！
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// 遞迴
int factorial(int n) {
    if (n <= 1) return 1;         // base case
    return n * factorial(n - 1);  // 遞迴呼叫
}

void demo_functions() {
    printf("3+4 = %d\n", add(3, 4));
    printf("5!  = %d\n", factorial(5));  // 120

    int x = 10, y = 20;
    swap(&x, &y);
    printf("x=%d, y=%d\n", x, y);  // x=20, y=10
}

// ============================================================
// 【6】陣列（Array）
// ============================================================
void demo_array() {
    // 一維陣列
    int arr[5] = {10, 20, 30, 40, 50};
    int arr2[5] = {0};   // 全部初始化為 0

    // 存取
    printf("%d\n", arr[0]);  // 10
    printf("%d\n", arr[4]);  // 50

    // 遍歷
    int len = sizeof(arr) / sizeof(arr[0]);  // 計算長度 = 5
    for (int i = 0; i < len; i++) {
        printf("%d ", arr[i]);
    }

    // 二維陣列
    int matrix[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    printf("\n%d\n", matrix[1][2]);  // 第2行第3列 = 6

    // 陣列名稱 = 第一個元素的位址
    printf("%p\n", arr);        // 位址
    printf("%p\n", &arr[0]);    // 同上
}

// ============================================================
// 【7】指標（Pointer）← C 的靈魂！
// ============================================================
void demo_pointer() {
    int x = 42;
    int *p = &x;    // p 存的是 x 的位址

    printf("x 的值:  %d\n",  x);    // 42
    printf("x 的位址: %p\n", &x);   // 某個位址
    printf("p 存的值: %p\n",  p);   // 同上（p 存的就是位址）
    printf("*p 解參考: %d\n", *p);  // 42（透過位址取值）

    *p = 100;   // 透過指標修改 x
    printf("x 現在: %d\n", x);  // 100

    // 指標運算（陣列走訪）
    int arr[] = {10, 20, 30};
    int *q = arr;           // 指向第一個元素
    printf("%d\n", *q);     // 10
    printf("%d\n", *(q+1)); // 20（往後移一個 int）
    printf("%d\n", *(q+2)); // 30

    q++;                    // q 現在指向 arr[1]
    printf("%d\n", *q);     // 20

    // NULL 指標（未指向任何東西）
    int *null_p = NULL;
    if (null_p == NULL) {
        printf("空指標，不能使用！\n");
    }
}

// ============================================================
// 【8】字串（String）← 本質是 char 陣列
// ============================================================
void demo_string() {
    // 字串 = char 陣列 + 結尾的 '\0'
    char s1[] = "Hello";         // {'H','e','l','l','o','\0'}
    char s2[20] = "World";
    const char *s3 = "Literal";        // 字串常數（不能修改！）

    // 常用函式（#include <string.h>）
    printf("長度: %zu\n",   strlen(s1));         // 5（不含\0）
    printf("比較: %d\n",    strcmp(s1, s2));      // 負數（H < W）
    strcpy(s2, s1);                               // s2 = "Hello"
    strcat(s2, " World");                         // s2 = "Hello World"

    // 不安全版本 vs 安全版本
    // strcpy  → strncpy(dst, src, n)
    // strcat  → strncat(dst, src, n)
    // gets    → fgets(buf, size, stdin)  ← gets 已廢棄！

    // 字串轉數字
    char num_str[] = "123";
    int  num = atoi(num_str);     // "123" → 123
    double d = atof("3.14");      // "3.14" → 3.14

    // 數字轉字串
    char buf[20];
    sprintf(buf, "%d", 456);      // 456 → "456"

    // 走訪字串
    for (int i = 0; s1[i] != '\0'; i++) {
        printf("%c", s1[i]);
    }
}

// ============================================================
// 【9】結構體（Struct）
// ============================================================
typedef struct {
    char name[50];
    int  age;
    float score;
} Student;

void demo_struct() {
    // 建立
    Student s1 = {"Alice", 20, 95.5f};
    Student s2;
    strcpy(s2.name, "Bob");
    s2.age   = 22;
    s2.score = 88.0f;

    printf("%s, %d, %.1f\n", s1.name, s1.age, s1.score);

    // 指標存取結構體
    Student *p = &s1;
    printf("%s\n", p->name);   // 用 -> 而不是 .
    printf("%d\n", (*p).age);  // 等價寫法

    // 結構體陣列
    Student cclass[3] = {
        {"Alice", 20, 95.5f},
        {"Bob",   22, 88.0f},
        {"Carol", 21, 92.0f}
    };
    for (int i = 0; i < 3; i++) {
        printf("%s\n", cclass[i].name);
    }
}

// ============================================================
// 【10】動態記憶體（malloc / free）
// ============================================================
void demo_memory() {
    // malloc：配置 heap 記憶體
    int *arr = (int*) malloc(5 * sizeof(int));
    if (arr == NULL) {
        printf("記憶體不足！\n");
        exit(1);
    }

    for (int i = 0; i < 5; i++) arr[i] = i * 10;
    for (int i = 0; i < 5; i++) printf("%d ", arr[i]);

    free(arr);      // 一定要 free！否則記憶體洩漏
    arr = NULL;     // free 後設 NULL，避免懸空指標

    // calloc：配置並清零
    int *arr2 = (int*) calloc(5, sizeof(int));  // 全部為 0

    // realloc：重新調整大小
    arr2 = (int*) realloc(arr2, 10 * sizeof(int));

    free(arr2);
}

// ============================================================
// 【11】檔案 I/O
// ============================================================
void demo_file() {
    // 寫入檔案
    FILE *fp = fopen("test.txt", "w");   // "w"寫, "r"讀, "a"附加
    if (fp == NULL) {
        printf("無法開啟檔案\n");
        return;
    }
    fprintf(fp, "Hello, File!\n");
    fprintf(fp, "數字: %d\n", 42);
    fclose(fp);

    // 讀取檔案
    fp = fopen("test.txt", "r");
    char line[100];
    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);
    }
    fclose(fp);

    // 二進位模式（CTF 常用）
    fp = fopen("data.bin", "wb");
    int data[] = {0xDEAD, 0xBEEF, 0xCAFE};
    fwrite(data, sizeof(int), 3, fp);
    fclose(fp);
}

// ============================================================
// 【12】常用技巧 & CTF 相關
// ============================================================
void demo_tips() {
    // XOR 加解密（CTF Crypto 基礎！）
    char msg[] = "Hello";
    char key = 0x42;
    for (int i = 0; msg[i]; i++) msg[i] ^= key;
    printf("加密後: ");
    for (int i = 0; msg[i]; i++) printf("%02X ", (unsigned char)msg[i]);
    // 再 XOR 一次 = 解密
    for (int i = 0; msg[i]; i++) msg[i] ^= key;
    printf("\n解密後: %s\n", msg);

    // 型別轉換
    int a = 65;
    char c = (char) a;   // 65 → 'A'
    printf("%c\n", c);

    // 印出記憶體內容（看 bytes）
    float f = 1.0f;
    unsigned char *bytes = (unsigned char*)&f;
    printf("1.0f 的記憶體: ");
    for (int i = 0; i < 4; i++) printf("%02X ", bytes[i]);
    // 3F 80 00 00 (小端序)
}

// ============================================================
// 主程式
// ============================================================
int main() {
    printf("=== 資料型別 ===\n");      demo_types();
    printf("\n=== 輸入輸出 ===\n");    // demo_io();  // 需要互動，跳過
    printf("\n=== 運算子 ===\n");      demo_operators();
    printf("\n=== 流程控制 ===\n");    demo_control();
    printf("\n=== 函式 ===\n");        demo_functions();
    printf("\n=== 陣列 ===\n");        demo_array();
    printf("\n=== 指標 ===\n");        demo_pointer();
    printf("\n=== 字串 ===\n");        demo_string();
    printf("\n=== 結構體 ===\n");      demo_struct();
    printf("\n=== 動態記憶體 ===\n");  demo_memory();
    printf("\n=== 檔案IO ===\n");      demo_file();
    printf("\n=== 技巧 ===\n");        demo_tips();
    return 0;
}