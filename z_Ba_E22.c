//
// Created by user on 2026/5/16.
//
// ============================================================
//  C 演算法與資料結構大全
//  💪 最硬核：沒有 STL，全部手刻！
//  但好處：你會徹底理解底層怎麼運作（對 RE/PWN 超有用）
// ============================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

// ============================================================
// 【0】C 沒有 STL —— 對照表
// ============================================================
/*
  C++ STL          C 怎麼辦
  ------------------------------------------
  vector           手刻動態陣列 (malloc/realloc)
  stack            陣列 + top 指標
  queue            陣列 + front/rear 指標（環形）
  priority_queue   手刻 heap
  list             手刻鏈表 (struct + pointer)
  set / map        手刻 BST / 雜湊表
  sort             qsort（內建！）或手刻
  next_permutation 手刻
*/

// ============================================================
// 【1】動態陣列（模擬 vector）
// ============================================================
typedef struct {
    int *data;
    int size;       // 目前元素數
    int cap;        // 容量
} Vector;

void vec_init(Vector *v) {
    v->cap = 4;
    v->size = 0;
    v->data = malloc(v->cap * sizeof(int));
}
void vec_push(Vector *v, int x) {
    if (v->size == v->cap) {            // 滿了就擴容
        v->cap *= 2;
        v->data = realloc(v->data, v->cap * sizeof(int));
    }
    v->data[v->size++] = x;
}
void vec_free(Vector *v) { free(v->data); }

void demo_vector() {
    Vector v;
    vec_init(&v);
    for (int i = 0; i < 10; i++) vec_push(&v, i * 10);
    for (int i = 0; i < v.size; i++) printf("%d ", v.data[i]);
    printf("\n");
    vec_free(&v);
}

// ============================================================
// 【2】Stack（堆疊）— 陣列實作
// ============================================================
#define MAXN 1000
typedef struct {
    int data[MAXN];
    int top;             // 指向頂端，-1 表示空
} Stack;

void st_init(Stack *s)        { s->top = -1; }
int  st_empty(Stack *s)       { return s->top == -1; }
void st_push(Stack *s, int x) { s->data[++s->top] = x; }
int  st_pop(Stack *s)         { return s->data[s->top--]; }
int  st_peek(Stack *s)        { return s->data[s->top]; }

void demo_stack() {
    Stack s; st_init(&s);
    st_push(&s, 1);
    st_push(&s, 2);
    st_push(&s, 3);
    printf("%d\n", st_peek(&s));  // 3
    st_pop(&s);
    printf("%d\n", st_peek(&s));  // 2

    // 括號匹配
    char *str = "(())";
    Stack chk; st_init(&chk);
    int valid = 1;
    for (int i = 0; str[i]; i++) {
        if (str[i] == '(') st_push(&chk, '(');
        else if (str[i] == ')') {
            if (st_empty(&chk)) { valid = 0; break; }
            st_pop(&chk);
        }
    }
    printf("有效: %d\n", valid && st_empty(&chk));
}

// ============================================================
// 【3】Queue（佇列）— 環形陣列
// ============================================================
typedef struct {
    int data[MAXN];
    int front, rear, count;
} Queue;

void q_init(Queue *q)  { q->front = q->rear = q->count = 0; }
int  q_empty(Queue *q) { return q->count == 0; }
void q_push(Queue *q, int x) {
    q->data[q->rear] = x;
    q->rear = (q->rear + 1) % MAXN;   // 環形！
    q->count++;
}
int q_pop(Queue *q) {
    int x = q->data[q->front];
    q->front = (q->front + 1) % MAXN;
    q->count--;
    return x;
}

void demo_queue() {
    Queue q; q_init(&q);
    q_push(&q, 1);
    q_push(&q, 2);
    q_push(&q, 3);
    printf("%d ", q_pop(&q));  // 1
    printf("%d\n", q_pop(&q)); // 2
}

// ============================================================
// 【4】Priority Queue（手刻最小堆）
// ============================================================
typedef struct {
    int data[MAXN];
    int size;
} Heap;

void heap_init(Heap *h) { h->size = 0; }

void heap_push(Heap *h, int x) {
    h->data[h->size] = x;
    int i = h->size++;
    while (i > 0) {                      // 上浮
        int parent = (i - 1) / 2;
        if (h->data[parent] <= h->data[i]) break;
        int t = h->data[parent];
        h->data[parent] = h->data[i];
        h->data[i] = t;
        i = parent;
    }
}

int heap_pop(Heap *h) {                  // 取最小
    int top = h->data[0];
    h->data[0] = h->data[--h->size];
    int i = 0;
    while (1) {                          // 下沉
        int l = 2*i+1, r = 2*i+2, smallest = i;
        if (l < h->size && h->data[l] < h->data[smallest]) smallest = l;
        if (r < h->size && h->data[r] < h->data[smallest]) smallest = r;
        if (smallest == i) break;
        int t = h->data[i];
        h->data[i] = h->data[smallest];
        h->data[smallest] = t;
        i = smallest;
    }
    return top;
}

void demo_heap() {
    Heap h; heap_init(&h);
    int arr[] = {3, 1, 4, 1, 5, 9, 2};
    for (int i = 0; i < 7; i++) heap_push(&h, arr[i]);
    while (h.size) printf("%d ", heap_pop(&h));  // 1 1 2 3 4 5 9
    printf("\n");
}

// ============================================================
// 【5】鏈表（Linked List）
// ============================================================
typedef struct Node {
    int val;
    struct Node *next;
} Node;

Node* list_create(int val) {
    Node *n = malloc(sizeof(Node));
    n->val = val;
    n->next = NULL;
    return n;
}

// 頭插法
Node* list_push_front(Node *head, int val) {
    Node *n = list_create(val);
    n->next = head;
    return n;       // 新的 head
}

// 反轉鏈表（經典！）
Node* list_reverse(Node *head) {
    Node *prev = NULL;
    while (head) {
        Node *next = head->next;  // 暫存
        head->next = prev;        // 反向
        prev = head;
        head = next;
    }
    return prev;
}

void list_print(Node *head) {
    while (head) {
        printf("%d ", head->val);
        head = head->next;
    }
    printf("\n");
}

void list_free(Node *head) {
    while (head) {
        Node *t = head;
        head = head->next;
        free(t);          // 別忘了釋放！
    }
}

void demo_list() {
    Node *head = NULL;
    for (int i = 1; i <= 5; i++)
        head = list_push_front(head, i);   // 5 4 3 2 1
    list_print(head);
    head = list_reverse(head);             // 1 2 3 4 5
    list_print(head);
    list_free(head);
}

// ============================================================
// 【6/7】Set / Map — 用二叉搜尋樹 BST 模擬
// ============================================================
typedef struct BSTNode {
    int key;
    int value;                  // map 用；set 可忽略
    struct BSTNode *left, *right;
} BSTNode;

BSTNode* bst_insert(BSTNode *root, int key, int value) {
    if (!root) {
        BSTNode *n = malloc(sizeof(BSTNode));
        n->key = key; n->value = value;
        n->left = n->right = NULL;
        return n;
    }
    if (key < root->key)      root->left  = bst_insert(root->left, key, value);
    else if (key > root->key) root->right = bst_insert(root->right, key, value);
    else root->value = value;   // 已存在則更新
    return root;
}

BSTNode* bst_find(BSTNode *root, int key) {
    while (root) {
        if (key == root->key) return root;
        root = (key < root->key) ? root->left : root->right;
    }
    return NULL;
}

void bst_inorder(BSTNode *root) {   // 中序 = 排序輸出
    if (!root) return;
    bst_inorder(root->left);
    printf("%d ", root->key);
    bst_inorder(root->right);
}

void demo_bst() {
    BSTNode *root = NULL;
    int keys[] = {5, 3, 7, 1, 4, 6, 8};
    for (int i = 0; i < 7; i++)
        root = bst_insert(root, keys[i], keys[i]*10);
    bst_inorder(root); printf("\n");  // 1 3 4 5 6 7 8（自動排序）
    printf("找7: %s\n", bst_find(root, 7) ? "有" : "無");
    // 注：要 O(log n) 保證需平衡樹(AVL/紅黑樹)，這裡是普通BST
}

// ============================================================
// 【8】Sort — qsort 是 C 內建武器！
// ============================================================
int cmp_asc(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);     // 升序
}
int cmp_desc(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);     // 降序
}

typedef struct { int x, y; } Point2;
int cmp_point(const void *a, const void *b) {
    return ((Point2*)a)->x - ((Point2*)b)->x;  // 依 x 排序
}

void demo_sort() {
    int a[] = {5, 2, 8, 1, 9, 3};
    int n = sizeof(a) / sizeof(a[0]);

    qsort(a, n, sizeof(int), cmp_asc);
    for (int i = 0; i < n; i++) printf("%d ", a[i]);
    printf("\n");

    qsort(a, n, sizeof(int), cmp_desc);
    for (int i = 0; i < n; i++) printf("%d ", a[i]);
    printf("\n");

    Point2 ps[] = {{3,1},{1,5},{2,8}};
    qsort(ps, 3, sizeof(Point2), cmp_point);
    for (int i = 0; i < 3; i++) printf("(%d,%d) ", ps[i].x, ps[i].y);
    printf("\n");
}

// ============================================================
// 【9】next_permutation（手刻）
// ============================================================
int next_permutation(int *a, int n) {
    int i = n - 2;
    while (i >= 0 && a[i] >= a[i+1]) i--;
    if (i < 0) return 0;                  // 已是最後
    int j = n - 1;
    while (a[j] <= a[i]) j--;
    int t = a[i]; a[i] = a[j]; a[j] = t;  // 交換
    // 反轉 i+1 到末尾
    for (int l = i+1, r = n-1; l < r; l++, r--) {
        t = a[l]; a[l] = a[r]; a[r] = t;
    }
    return 1;
}

void demo_permutation() {
    int a[] = {1, 2, 3};
    do {
        for (int i = 0; i < 3; i++) printf("%d", a[i]);
        printf(" ");
    } while (next_permutation(a, 3));
    printf("\n");  // 123 132 213 231 312 321
}

// ============================================================
// 【10/11】DFS & BFS（鄰接表）
// ============================================================
int graph[100][100];   // 簡單用鄰接矩陣
int gn;                // 節點數
int visited[100];

void dfs(int u) {
    visited[u] = 1;
    printf("%d ", u);
    for (int v = 0; v < gn; v++)
        if (graph[u][v] && !visited[v])
            dfs(v);
}

void bfs(int start) {
    Queue q; q_init(&q);
    int vis[100] = {0};
    q_push(&q, start);
    vis[start] = 1;
    while (!q_empty(&q)) {
        int u = q_pop(&q);
        printf("%d ", u);
        for (int v = 0; v < gn; v++)
            if (graph[u][v] && !vis[v]) {
                vis[v] = 1;        // 入隊就標記
                q_push(&q, v);
            }
    }
}

void demo_graph_search() {
    gn = 5;
    memset(graph, 0, sizeof(graph));
    int edges[][2] = {{0,1},{0,2},{1,3},{2,3},{3,4}};
    for (int i = 0; i < 5; i++) {
        graph[edges[i][0]][edges[i][1]] = 1;
        graph[edges[i][1]][edges[i][0]] = 1;  // 無向圖
    }
    memset(visited, 0, sizeof(visited));
    printf("DFS: "); dfs(0);   printf("\n");
    printf("BFS: "); bfs(0);   printf("\n");
}

// ============================================================
// 【12】並查集（Union-Find）
// ============================================================
int parent[1000], rnk[1000];

void dsu_init(int n) {
    for (int i = 0; i < n; i++) { parent[i] = i; rnk[i] = 0; }
}
int dsu_find(int x) {
    if (parent[x] != x)
        parent[x] = dsu_find(parent[x]);  // 路徑壓縮
    return parent[x];
}
void dsu_union(int x, int y) {
    int rx = dsu_find(x), ry = dsu_find(y);
    if (rx == ry) return;
    if (rnk[rx] < rnk[ry]) { int t=rx; rx=ry; ry=t; }
    parent[ry] = rx;
    if (rnk[rx] == rnk[ry]) rnk[rx]++;
}

void demo_dsu() {
    dsu_init(5);
    dsu_union(0, 1);
    dsu_union(2, 3);
    printf("0,1 同組: %d\n", dsu_find(0) == dsu_find(1));  // 1
    printf("0,2 同組: %d\n", dsu_find(0) == dsu_find(2));  // 0
}

// ============================================================
// 【13】二叉樹走訪（用上面的 BSTNode）
// ============================================================
void preorder(BSTNode *r)  {  // 根左右
    if (!r) return;
    printf("%d ", r->key);
    preorder(r->left);
    preorder(r->right);
}
void postorder(BSTNode *r) {  // 左右根
    if (!r) return;
    postorder(r->left);
    postorder(r->right);
    printf("%d ", r->key);
}
int tree_height(BSTNode *r) {
    if (!r) return 0;
    int lh = tree_height(r->left);
    int rh = tree_height(r->right);
    return 1 + (lh > rh ? lh : rh);
}

// ============================================================
// 【14】線段樹（Segment Tree）
// ============================================================
long long seg[4 * MAXN];

void seg_build(int *a, int node, int l, int r) {
    if (l == r) { seg[node] = a[l]; return; }
    int m = (l + r) / 2;
    seg_build(a, 2*node,   l,   m);
    seg_build(a, 2*node+1, m+1, r);
    seg[node] = seg[2*node] + seg[2*node+1];
}
void seg_update(int node, int l, int r, int idx, int val) {
    if (l == r) { seg[node] = val; return; }
    int m = (l + r) / 2;
    if (idx <= m) seg_update(2*node,   l,   m, idx, val);
    else          seg_update(2*node+1, m+1, r, idx, val);
    seg[node] = seg[2*node] + seg[2*node+1];
}
long long seg_query(int node, int l, int r, int ql, int qr) {
    if (qr < l || r < ql) return 0;
    if (ql <= l && r <= qr) return seg[node];
    int m = (l + r) / 2;
    return seg_query(2*node, l, m, ql, qr)
         + seg_query(2*node+1, m+1, r, ql, qr);
}

void demo_segtree() {
    int a[] = {1, 3, 5, 7, 9, 11};
    seg_build(a, 1, 0, 5);
    printf("區間[1,3]和: %lld\n", seg_query(1, 0, 5, 1, 3)); // 15
    seg_update(1, 0, 5, 1, 10);
    printf("更新後: %lld\n", seg_query(1, 0, 5, 1, 3));       // 22
}

// ============================================================
// 【15】樹狀數組（BIT / Fenwick）
// ============================================================
int bit[MAXN];
int bit_n;

void bit_update(int i, int delta) {
    for (; i <= bit_n; i += i & (-i))   // lowbit
        bit[i] += delta;
}
int bit_query(int i) {                  // 前綴和 [1,i]
    int s = 0;
    for (; i > 0; i -= i & (-i))
        s += bit[i];
    return s;
}

void demo_bit() {
    bit_n = 6;
    memset(bit, 0, sizeof(bit));
    int a[] = {1, 3, 5, 7, 9, 11};
    for (int i = 0; i < 6; i++) bit_update(i+1, a[i]);
    printf("區間[2,4]和: %d\n", bit_query(4) - bit_query(1)); // 15
}

// ============================================================
// 【16】貪心（Greedy）— 活動選擇
// ============================================================
typedef struct { int start, end; } Activity;
int cmp_act(const void *a, const void *b) {
    return ((Activity*)a)->end - ((Activity*)b)->end;
}

void demo_greedy() {
    Activity acts[] = {{1,3},{2,5},{4,7},{6,8}};
    int n = 4;
    qsort(acts, n, sizeof(Activity), cmp_act);  // 按結束時間排
    int cnt = 0, lastEnd = -1;
    for (int i = 0; i < n; i++) {
        if (acts[i].start >= lastEnd) {
            cnt++;
            lastEnd = acts[i].end;
        }
    }
    printf("最多 %d 個活動\n", cnt);
}

// ============================================================
// 【17】分治（Merge Sort）
// ============================================================
void merge_sort(int *a, int l, int r) {
    if (l >= r) return;
    int m = (l + r) / 2;
    merge_sort(a, l, m);          // 分
    merge_sort(a, m+1, r);        // 分
    int *tmp = malloc((r-l+1) * sizeof(int));
    int i = l, j = m+1, k = 0;
    while (i <= m && j <= r)      // 治：合併
        tmp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    while (i <= m) tmp[k++] = a[i++];
    while (j <= r) tmp[k++] = a[j++];
    for (int x = 0; x < k; x++) a[l+x] = tmp[x];
    free(tmp);
}

void demo_divide() {
    int a[] = {5, 2, 8, 1, 9, 3};
    merge_sort(a, 0, 5);
    for (int i = 0; i < 6; i++) printf("%d ", a[i]);
    printf("\n");
}

// ============================================================
// 【18】減治（二分搜尋 + 快速冪）
// ============================================================
int binary_search(int *a, int n, int target) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;   // 防溢位
        if (a[mid] == target) return mid;
        else if (a[mid] < target) lo = mid + 1;  // 只走右半
        else hi = mid - 1;                        // 只走左半
    }
    return -1;
}

long long fast_pow(long long a, long long n, long long mod) {
    long long res = 1;
    a %= mod;
    while (n > 0) {
        if (n & 1) res = res * a % mod;
        a = a * a % mod;
        n >>= 1;
    }
    return res;
}

void demo_decrease() {
    int a[] = {1, 3, 5, 7, 9, 11};
    printf("找7在: %d\n", binary_search(a, 6, 7));      // 3
    printf("2^10 %% 1000 = %lld\n", fast_pow(2, 10, 1000)); // 24
}

// ============================================================
// 【19】DP 動態規劃（簡單 → 難）
// ============================================================
void demo_dp() {
    // --- 費氏數列 ---
    long long fib[50];
    fib[0] = 0; fib[1] = 1;
    for (int i = 2; i < 50; i++) fib[i] = fib[i-1] + fib[i-2];
    printf("fib(10) = %lld\n", fib[10]);  // 55

    // --- 0/1 背包 ---
    int w[] = {2,3,4,5}, val[] = {3,4,5,6}, W = 8, ni = 4;
    int dp[100] = {0};
    for (int i = 0; i < ni; i++)
        for (int j = W; j >= w[i]; j--)        // 倒序！
            dp[j] = dp[j] > dp[j-w[i]]+val[i] ? dp[j] : dp[j-w[i]]+val[i];
    printf("背包最大價值: %d\n", dp[W]);

    // --- 最長公共子序列 LCS ---
    char A[] = "ABCBDAB", B[] = "BDCAB";
    int la = strlen(A), lb = strlen(B);
    int L[20][20] = {0};
    for (int i = 1; i <= la; i++)
        for (int j = 1; j <= lb; j++)
            if (A[i-1] == B[j-1]) L[i][j] = L[i-1][j-1] + 1;
            else L[i][j] = L[i-1][j] > L[i][j-1] ? L[i-1][j] : L[i][j-1];
    printf("LCS 長度: %d\n", L[la][lb]);  // 4

    // --- 最長遞增子序列 LIS（O(n²)版）---
    int arr[] = {10, 9, 2, 5, 3, 7, 101, 18}, an = 8;
    int lis[8], best = 0;
    for (int i = 0; i < an; i++) {
        lis[i] = 1;
        for (int j = 0; j < i; j++)
            if (arr[j] < arr[i] && lis[j]+1 > lis[i])
                lis[i] = lis[j] + 1;
        if (lis[i] > best) best = lis[i];
    }
    printf("LIS 長度: %d\n", best);  // 4
    // 進階：編輯距離、區間DP、樹形DP（觀念同C++/Python）
}

// ============================================================
// 【20】字串演算法 — KMP
// ============================================================
void kmp(char *text, char *pat) {
    int n = strlen(text), m = strlen(pat);
    int *pi = calloc(m, sizeof(int));
    int k = 0;
    for (int i = 1; i < m; i++) {       // 建 failure function
        while (k > 0 && pat[i] != pat[k]) k = pi[k-1];
        if (pat[i] == pat[k]) k++;
        pi[i] = k;
    }
    k = 0;
    for (int i = 0; i < n; i++) {       // 匹配
        while (k > 0 && text[i] != pat[k]) k = pi[k-1];
        if (text[i] == pat[k]) k++;
        if (k == m) {
            printf("匹配位置: %d\n", i - m + 1);
            k = pi[k-1];
        }
    }
    free(pi);
}

// 字串雜湊
unsigned long long str_hash(char *s) {
    unsigned long long h = 0, base = 131;
    for (int i = 0; s[i]; i++)
        h = h * base + s[i];
    return h;
}

void demo_string_algo() {
    kmp("ababcabab", "ab");                  // 0, 2, 5, 7
    printf("hash: %llu\n", str_hash("hello"));
    // 進階：Trie(用陣列), Z-function, Manacher
}

// ============================================================
// 【21】數論（Number Theory）⭐ CTF Crypto 核心
// ============================================================
long long gcd(long long a, long long b) {
    return b == 0 ? a : gcd(b, a % b);
}
long long lcm(long long a, long long b) {
    return a / gcd(a, b) * b;
}

// 擴展歐幾里得（求模逆元用）
long long extgcd(long long a, long long b, long long *x, long long *y) {
    if (b == 0) { *x = 1; *y = 0; return a; }
    long long x1, y1;
    long long g = extgcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;
    return g;
}

// 模逆元
long long mod_inverse(long long a, long long m) {
    long long x, y;
    long long g = extgcd(a, m, &x, &y);
    if (g != 1) return -1;            // 無逆元
    return (x % m + m) % m;
}

void demo_number_theory() {
    printf("gcd(12,18) = %lld\n", gcd(12, 18));  // 6
    printf("lcm(4,6)   = %lld\n", lcm(4, 6));     // 12

    // 質數篩
    int N = 30;
    int isPrime[31];
    for (int i = 0; i <= N; i++) isPrime[i] = 1;
    isPrime[0] = isPrime[1] = 0;
    for (int i = 2; i*i <= N; i++)
        if (isPrime[i])
            for (int j = i*i; j <= N; j += i)
                isPrime[j] = 0;
    printf("質數: ");
    for (int i = 2; i <= N; i++) if (isPrime[i]) printf("%d ", i);
    printf("\n");

    // ⭐ RSA 簡易示範
    long long p = 61, q = 53;
    long long n = p * q;                       // 公鑰 n = 3233
    long long phi = (p-1) * (q-1);             // 3120
    long long e = 17;                          // 公鑰 e
    long long d = mod_inverse(e, phi);         // 私鑰 d
    long long msg = 65;
    long long cipher = fast_pow(msg, e, n);    // 加密
    long long plain  = fast_pow(cipher, d, n); // 解密
    printf("RSA: %lld -> %lld -> %lld\n", msg, cipher, plain);
    // 這就是 CTF Crypto 最常見題型的數學核心！
}

// ============================================================
// 【22】圖論 — Dijkstra（鄰接矩陣版）
// ============================================================
#define INF 0x3f3f3f3f

void dijkstra(int g[][6], int n, int src) {
    int dist[6], vis[6] = {0};
    for (int i = 0; i < n; i++) dist[i] = INF;
    dist[src] = 0;
    for (int it = 0; it < n; it++) {
        int u = -1, best = INF;
        for (int i = 0; i < n; i++)        // 找最近未訪節點
            if (!vis[i] && dist[i] < best) { best = dist[i]; u = i; }
        if (u == -1) break;
        vis[u] = 1;
        for (int v = 0; v < n; v++)        // 鬆弛
            if (g[u][v] && dist[u] + g[u][v] < dist[v])
                dist[v] = dist[u] + g[u][v];
    }
    for (int i = 0; i < n; i++)
        printf("到 %d: %d\n", i, dist[i]);
}

void demo_dijkstra() {
    int g[6][6] = {0};
    g[0][1] = 4; g[0][2] = 1;
    g[2][1] = 2; g[1][3] = 1; g[2][3] = 5;
    dijkstra(g, 4, 0);
    // 其他：Bellman-Ford、Floyd、Kruskal/Prim、拓撲排序
    // 觀念與 C++/Python 相同，只是手刻優先佇列較麻煩
}

// ============================================================
// 【23】計算幾何（Computational Geometry）
// ============================================================
typedef struct { double x, y; } Pt;

double cross(Pt o, Pt a, Pt b) {        // 叉積
    return (a.x-o.x)*(b.y-o.y) - (a.y-o.y)*(b.x-o.x);
}
double dist(Pt a, Pt b) {
    double dx = a.x-b.x, dy = a.y-b.y;
    return __builtin_sqrt(dx*dx + dy*dy);
}

void demo_geometry() {
    Pt A = {0,0}, B = {4,0}, C = {0,3};
    printf("A-B 距離: %.2f\n", dist(A, B));            // 4.00
    printf("三角形面積: %.2f\n",
           __builtin_fabs(cross(A, B, C)) / 2.0);      // 6.00
    double d = cross(A, B, C);
    printf("方向: %s\n",
           d > 0 ? "逆時針" : (d < 0 ? "順時針" : "共線"));
    // 進階：凸包(Graham/Andrew)、線段相交、點在多邊形內
}

// ============================================================
//  C 在 CP / CTF 的定位
// ============================================================
/*
  ✅ 優點：
     - 最快（執行速度），最省記憶體
     - 徹底理解底層：指標、記憶體、bit 操作
     - 對 RE（逆向）/ PWN（漏洞利用）幫助巨大！
       因為反組譯後看到的就是 C 等級的邏輯

  ❌ 缺點：
     - 沒 STL，所有資料結構都要手刻
     - 字串處理麻煩（char[] + \0）
     - 大數運算超痛苦（RSA 要自己寫大數庫，建議用 GMP）

  ⭐ 對你的 CTF：
     - RE / PWN：C 是必修！反組譯就是讀 C 邏輯
     - Crypto 大數：C 不方便，用 Python 比較好
     - 學 C 的真正價值：懂記憶體、堆疊、緩衝區溢位原理
*/

int main() {
    printf("=== Vector ===\n");     demo_vector();
    printf("=== Stack ===\n");      demo_stack();
    printf("=== Queue ===\n");      demo_queue();
    printf("=== Heap ===\n");       demo_heap();
    printf("=== List ===\n");       demo_list();
    printf("=== BST ===\n");        demo_bst();
    printf("=== Sort ===\n");       demo_sort();
    printf("=== Permutation ===\n");demo_permutation();
    printf("=== DFS/BFS ===\n");    demo_graph_search();
    printf("=== DSU ===\n");        demo_dsu();
    printf("=== SegTree ===\n");    demo_segtree();
    printf("=== BIT ===\n");        demo_bit();
    printf("=== Greedy ===\n");     demo_greedy();
    printf("=== Divide ===\n");     demo_divide();
    printf("=== Decrease ===\n");   demo_decrease();
    printf("=== DP ===\n");         demo_dp();
    printf("=== String ===\n");     demo_string_algo();
    printf("=== NumberTheory ===\n"); demo_number_theory();
    printf("=== Dijkstra ===\n");   demo_dijkstra();
    printf("=== Geometry ===\n");   demo_geometry();
    return 0;
}