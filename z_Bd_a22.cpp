// ============================================================
//  C++ 演算法與資料結構大全（競賽程式設計地圖）
//   C++ 是 CP 王者，因為 STL 把一切都包好了
// ============================================================
#include <bits/stdc++.h>
#include <iostream>
#include <array>
#include <algorithm>
#include <cmath>
#include <cctype>
#include <chrono>
#include <format>
#include <iomanip>
#include <limits>
#include <memory>
#include <map>
#include <unordered_map>
#include <numbers>
#include <ranges>
#include <string>
#include <vector>
// 競賽萬用標頭（包含所有 STL）
using namespace std;

// ============================================================
// 【1】STL 三大件：容器 / 迭代器 / 演算法
// ============================================================
void stl_overview() {
    // STL = Standard Template Library
    // 容器(Container) + 迭代器(Iterator) + 演算法(Algorithm)

    vector<int> v = {3, 1, 4, 1, 5};

    // 迭代器：像「指標」一樣指向容器元素
    vector<int>::iterator it = v.begin();
    cout << *it << "\n";        // 3
    ++it;                        // 移到下一個
    cout << *it << "\n";        // 1

    // begin() 指向第一個, end() 指向「最後一個的後面」
    for (auto i = v.begin(); i != v.end(); ++i) cout << *i << " ";
    cout << "\n";

    // 演算法套用在迭代器區間 [begin, end)
    sort(v.begin(), v.end());
}

// ============================================================
// 【2】Stack（堆疊）— LIFO 後進先出
// ============================================================
void demo_stack() {
    stack<int> st;
    st.push(1);          // 放入
    st.push(2);
    st.push(3);
    cout << st.top();    // 3（看頂端，不移除）
    st.pop();            // 移除頂端（3）
    cout << st.top();    // 2
    cout << st.size();   // 2
    cout << st.empty();  // 0（false）

    // 經典應用：括號匹配
    auto valid = [](string s) {
        stack<char> st;
        for (char c : s) {
            if (c == '(') st.push(c);
            else if (c == ')') {
                if (st.empty()) return false;
                st.pop();
            }
        }
        return st.empty();
    };
    cout << valid("(())") << "\n";  // 1
}

// ============================================================
// 【3】Queue（佇列）— FIFO 先進先出
// ============================================================
void demo_queue() {
    queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    cout << q.front();   // 1（最前面）
    cout << q.back();    // 3（最後面）
    q.pop();             // 移除最前面（1）
    cout << q.front();   // 2

    // deque：雙端佇列（兩頭都能進出）
    deque<int> dq;
    dq.push_front(1);
    dq.push_back(2);
    dq.push_front(0);    // {0, 1, 2}
    dq.pop_front();
    dq.pop_back();
}

// ============================================================
// 【4】Priority Queue（優先佇列 / 堆積 Heap）
// ============================================================
void demo_priority_queue() {
    // 預設：最大堆（大的先出）
    priority_queue<int> maxpq;
    maxpq.push(3);
    maxpq.push(1);
    maxpq.push(4);
    cout << maxpq.top();  // 4（最大的）
    maxpq.pop();
    cout << maxpq.top();  // 3

    // 最小堆（小的先出）
    priority_queue<int, vector<int>, greater<int>> minpq;
    minpq.push(3);
    minpq.push(1);
    minpq.push(4);
    cout << minpq.top();  // 1（最小的）

    // 自訂排序（pair 依第二個排）
    priority_queue<pair<int,int>,
                   vector<pair<int,int>>,
                   greater<pair<int,int>>> pq;
    // 應用：Dijkstra 最短路徑就靠這個！
}

// ============================================================
// 【5】List（雙向鏈表）
// ============================================================
void demo_list() {
    list<int> lst = {1, 2, 3};
    lst.push_back(4);
    lst.push_front(0);     // {0,1,2,3,4}
    lst.pop_front();
    lst.pop_back();

    // 在指定位置插入（鏈表強項：O(1) 插入）
    auto it = lst.begin();
    advance(it, 1);        // 移到第2個
    lst.insert(it, 99);    // {1, 99, 2, 3}

    lst.remove(99);        // 移除所有值為99的
    lst.sort();            // 鏈表專屬排序
    lst.reverse();         // 反轉
    lst.unique();          // 去除連續重複

    for (int x : lst) cout << x << " ";

    // forward_list = 單向鏈表（更省記憶體）
}

// ============================================================
// 【6】Set（集合，有序不重複）
// ============================================================
void demo_set() {
    set<int> s = {5, 3, 1, 4, 1, 3};   // 自動排序去重 → {1,3,4,5}

    s.insert(2);
    s.erase(3);
    cout << s.count(4)  << "\n";   // 1（存在）
    cout << *s.begin()  << "\n";   // 1（最小）
    cout << *s.rbegin() << "\n";   // 5（最大）

    // lower_bound / upper_bound（二分搜尋）
    auto lb = s.lower_bound(3);   // >= 3 的第一個
    auto ub = s.upper_bound(3);   // > 3 的第一個

    // multiset：允許重複
    multiset<int> ms = {1, 1, 2, 2, 3};
    cout << ms.count(1) << "\n";  // 2

    // unordered_set：雜湊，O(1) 查找，無序
    unordered_set<int> us = {1, 2, 3};
}

// ============================================================
// 【7】Map（鍵值對映射）
// ============================================================
void demo_map() {
    map<string, int> mp;     // 依 key 排序（紅黑樹）
    mp["apple"]  = 3;
    mp["banana"] = 5;
    mp["cherry"] = 2;

    cout << mp["apple"]      << "\n";  // 3
    cout << mp.count("kiwi") << "\n";  // 0

    // 走訪（C++17 結構化綁定）
    for (auto &[key, val] : mp)
        cout << key << " -> " << val << "\n";

    // 計數神器（統計詞頻）
    map<char, int> freq;
    string text = "hello";
    for (char c : text) freq[c]++;   // 不存在會自動建 0

    // unordered_map：雜湊版，更快
    unordered_map<string, int> ump;
}

// ============================================================
// 【8】Sort（排序）
// ============================================================
void demo_sort() {
    vector<int> v = {5, 2, 8, 1, 9, 3};

    sort(v.begin(), v.end());                    // 升序
    sort(v.begin(), v.end(), greater<int>());    // 降序
    sort(v.begin(), v.end(), [](int a, int b){   // 自訂
        return a > b;
    });

    // 結構體排序
    struct P { int x, y; };
    vector<P> ps = {{1,5},{3,2},{2,8}};
    sort(ps.begin(), ps.end(), [](const P&a, const P&b){
        return a.x < b.x;        // 依 x 排序
    });

    // 穩定排序（相同值保持原順序）
    stable_sort(v.begin(), v.end());

    // 部分排序（只要前 k 個有序）
    partial_sort(v.begin(), v.begin()+3, v.end());

    // nth_element（第 k 小，O(n)）
    nth_element(v.begin(), v.begin()+2, v.end());
    cout << v[2] << "\n";  // 第3小的數

    // 其他常用演算法
    reverse(v.begin(), v.end());          // 反轉
    cout << *max_element(v.begin(), v.end());  // 最大值
    cout << *min_element(v.begin(), v.end());  // 最小值
    cout << accumulate(v.begin(), v.end(), 0); // 求和（需<numeric>）
    int cnt = count(v.begin(), v.end(), 5);    // 計數
    bool found = binary_search(v.begin(), v.end(), 8);  // 二分搜
}

// ============================================================
// 【9】next_permutation（下一個排列）
// ============================================================
void demo_permutation() {
    vector<int> v = {1, 2, 3};

    // 列出所有排列（要先排序！）
    sort(v.begin(), v.end());
    do {
        for (int x : v) cout << x;
        cout << " ";
    } while (next_permutation(v.begin(), v.end()));
    // 123 132 213 231 312 321

    cout << "\n";
    // prev_permutation：上一個排列（要從大到小開始）
}

// ============================================================
// 【10】DFS（深度優先搜尋）
// ============================================================
vector<int> adj[100];   // 鄰接表
bool visited[100];

void dfs(int u) {
    visited[u] = true;
    cout << u << " ";
    for (int v : adj[u])
        if (!visited[v])
            dfs(v);              // 遞迴深入
}

// ============================================================
// 【11】BFS（廣度優先搜尋）
// ============================================================
void bfs(int start) {
    queue<int> q;
    vector<bool> vis(100, false);
    q.push(start);
    vis[start] = true;

    while (!q.empty()) {
        int u = q.front(); q.pop();
        cout << u << " ";
        for (int v : adj[u]) {
            if (!vis[v]) {
                vis[v] = true;   // 一發現就標記！
                q.push(v);
            }
        }
    }
    // BFS 求「無權圖最短路徑」必備
}

// ============================================================
// 【12】並查集（Union-Find / DSU）
// ============================================================
struct DSU {
    vector<int> parent, rnk;

    DSU(int n) : parent(n), rnk(n, 0) {
        iota(parent.begin(), parent.end(), 0);  // 0,1,2,...n
    }

    // 找根（含路徑壓縮）
    int find(int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);   // 壓縮路徑
        return parent[x];
    }

    // 合併兩集合（按秩合併）
    void unite(int x, int y) {
        int rx = find(x), ry = find(y);
        if (rx == ry) return;
        if (rnk[rx] < rnk[ry]) swap(rx, ry);
        parent[ry] = rx;
        if (rnk[rx] == rnk[ry]) rnk[rx]++;
    }

    bool same(int x, int y) { return find(x) == find(y); }
};
// 應用：Kruskal 最小生成樹、判斷連通性

// ============================================================
// 【13】二叉樹（Binary Tree）
// ============================================================
struct TreeNode {
    int val;
    TreeNode *left, *right;
    TreeNode(int v) : val(v), left(nullptr), right(nullptr) {}
};

// 三種走訪
void preorder(TreeNode* r) {   // 前序：根→左→右
    if (!r) return;
    cout << r->val << " ";
    preorder(r->left);
    preorder(r->right);
}
void inorder(TreeNode* r) {    // 中序：左→根→右（BST會得到排序結果）
    if (!r) return;
    inorder(r->left);
    cout << r->val << " ";
    inorder(r->right);
}
void postorder(TreeNode* r) {  // 後序：左→右→根
    if (!r) return;
    postorder(r->left);
    postorder(r->right);
    cout << r->val << " ";
}
// 層序走訪 = BFS（用 queue）

// 二叉搜尋樹 BST 插入
TreeNode* insertBST(TreeNode* r, int v) {
    if (!r) return new TreeNode(v);
    if (v < r->val) r->left  = insertBST(r->left, v);
    else            r->right = insertBST(r->right, v);
    return r;
}

// ============================================================
// 【14】線段樹（Segment Tree）區間查詢/修改
// ============================================================
struct SegTree {
    int n;
    vector<long long> tree;

    SegTree(int n) : n(n), tree(4*n, 0) {}

    void build(vector<int>& a, int node, int l, int r) {
        if (l == r) { tree[node] = a[l]; return; }
        int m = (l+r)/2;
        build(a, 2*node,   l,   m);
        build(a, 2*node+1, m+1, r);
        tree[node] = tree[2*node] + tree[2*node+1];   // 區間和
    }

    // 單點更新
    void update(int node, int l, int r, int idx, int val) {
        if (l == r) { tree[node] = val; return; }
        int m = (l+r)/2;
        if (idx <= m) update(2*node,   l,   m, idx, val);
        else          update(2*node+1, m+1, r, idx, val);
        tree[node] = tree[2*node] + tree[2*node+1];
    }

    // 區間查詢 [ql, qr]
    long long query(int node, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return 0;          // 完全不相交
        if (ql <= l && r <= qr) return tree[node]; // 完全包含
        int m = (l+r)/2;
        return query(2*node, l, m, ql, qr)
             + query(2*node+1, m+1, r, ql, qr);
    }
};
// O(log n) 區間查詢與修改

// ============================================================
// 【15】樹狀數組（Binary Indexed Tree / Fenwick）
// ============================================================
struct BIT {
    vector<int> tree;
    int n;

    BIT(int n) : n(n), tree(n+1, 0) {}

    // lowbit：取最低位的 1（x & -x）
    void update(int i, int delta) {
        for (; i <= n; i += i & (-i))
            tree[i] += delta;
    }

    int query(int i) {              // 前綴和 [1, i]
        int s = 0;
        for (; i > 0; i -= i & (-i))
            s += tree[i];
        return s;
    }

    int range(int l, int r) {       // 區間和 [l, r]
        return query(r) - query(l-1);
    }
};
// 比線段樹簡潔，常數更小，但功能較單一

// ============================================================
// 【16】貪心（Greedy）
// ============================================================
void demo_greedy() {
    // 例：活動選擇（選最多不重疊活動）
    // 策略：依「結束時間」排序，貪心選最早結束的
    vector<pair<int,int>> acts = {{1,3},{2,5},{4,7},{6,8}};
    sort(acts.begin(), acts.end(), [](auto&a, auto&b){
        return a.second < b.second;   // 按結束時間
    });

    int cnt = 0, lastEnd = -1;
    for (auto &[s, e] : acts) {
        if (s >= lastEnd) {     // 不衝突就選
            cnt++;
            lastEnd = e;
        }
    }
    cout << "最多選 " << cnt << " 個活動\n";
    // 貪心核心：每一步都選「當下最好」，期望全域最優
    // 注意：不是所有問題都能貪心！要能證明
}

// ============================================================
// 【17】分治（Divide and Conquer）
// ============================================================
// 例：合併排序 Merge Sort
void mergeSort(vector<int>& a, int l, int r) {
    if (l >= r) return;
    int m = (l+r)/2;
    mergeSort(a, l, m);        // 分：解左半
    mergeSort(a, m+1, r);      // 分：解右半
    // 治：合併兩個有序部分
    vector<int> tmp;
    int i = l, j = m+1;
    while (i <= m && j <= r)
        tmp.push_back(a[i] <= a[j] ? a[i++] : a[j++]);
    while (i <= m) tmp.push_back(a[i++]);
    while (j <= r) tmp.push_back(a[j++]);
    for (int k = 0; k < (int)tmp.size(); k++)
        a[l+k] = tmp[k];
}
// 分治三步：Divide（分）→ Conquer（解）→ Combine（合）
// 經典：快排、合併排序、快速冪、最近點對

// ============================================================
// 【18】減治（Decrease and Conquer）
// ============================================================
// 與分治不同：減治每次只縮小問題，不分成多份
// 例：二分搜尋（每次砍一半，只解一邊）
int binarySearch(vector<int>& a, int target) {
    int l = 0, r = a.size() - 1;
    while (l <= r) {
        int m = l + (r - l) / 2;     // 防溢位寫法
        if (a[m] == target) return m;
        else if (a[m] < target) l = m + 1;  // 只往右半
        else r = m - 1;                      // 只往左半
    }
    return -1;
}
// 快速冪也是減治：a^n → a^(n/2) 的平方
long long fastPow(long long a, long long n, long long mod) {
    long long res = 1;
    a %= mod;
    while (n > 0) {
        if (n & 1) res = res * a % mod;
        a = a * a % mod;
        n >>= 1;
    }
    return res;
}

// ============================================================
// 【19】DP 動態規劃（簡單 → 難）
// ============================================================
void demo_dp() {
    // --- 入門：費氏數列 ---
    int n = 10;
    vector<long long> fib(n+1);
    fib[0] = 0; fib[1] = 1;
    for (int i = 2; i <= n; i++)
        fib[i] = fib[i-1] + fib[i-2];

    // --- 經典：0/1 背包 ---
    // w[]重量, v[]價值, W容量
    vector<int> w = {2, 3, 4, 5};
    vector<int> val = {3, 4, 5, 6};
    int W = 8;
    vector<int> dp(W+1, 0);
    for (int i = 0; i < (int)w.size(); i++)
        for (int j = W; j >= w[i]; j--)         // 倒序（01背包關鍵）
            dp[j] = max(dp[j], dp[j-w[i]] + val[i]);
    cout << "背包最大價值: " << dp[W] << "\n";

    // --- 中級：最長遞增子序列 LIS ---
    vector<int> arr = {10, 9, 2, 5, 3, 7, 101, 18};
    vector<int> lis;
    for (int x : arr) {
        auto it = lower_bound(lis.begin(), lis.end(), x);
        if (it == lis.end()) lis.push_back(x);
        else *it = x;
    }
    cout << "LIS 長度: " << lis.size() << "\n";  // O(n log n)

    // --- 中級：最長公共子序列 LCS ---
    string A = "ABCBDAB", B = "BDCAB";
    int la = A.size(), lb = B.size();
    vector<vector<int>> L(la+1, vector<int>(lb+1, 0));
    for (int i = 1; i <= la; i++)
        for (int j = 1; j <= lb; j++)
            if (A[i-1] == B[j-1]) L[i][j] = L[i-1][j-1] + 1;
            else L[i][j] = max(L[i-1][j], L[i][j-1]);
    cout << "LCS 長度: " << L[la][lb] << "\n";

    // --- 進階：編輯距離 (Edit Distance) ---
    // --- 進階：區間 DP（石子合併）---
    // --- 高階：樹形 DP、狀態壓縮 DP、數位 DP ---
    // DP 核心：定義「狀態」+ 找「轉移方程」+ 處理「邊界」
}

// ============================================================
// 【20】字串演算法
// ============================================================
void demo_string_algo() {
    // --- KMP（字串匹配 O(n+m)）---
    auto buildKMP = [](string p) {
        int m = p.size();
        vector<int> pi(m, 0);
        for (int i = 1, k = 0; i < m; i++) {
            while (k > 0 && p[i] != p[k]) k = pi[k-1];
            if (p[i] == p[k]) k++;
            pi[i] = k;
        }
        return pi;
    };

    // --- 字串雜湊（Rolling Hash）---
    string s = "hello";
    const long long MOD = 1e9+7, BASE = 131;
    long long hash = 0;
    for (char c : s) hash = (hash * BASE + c) % MOD;

    // --- Trie 字典樹 ---
    struct Trie {
        Trie* child[26] = {};
        bool isEnd = false;
        void insert(string w) {
            Trie* cur = this;
            for (char c : w) {
                if (!cur->child[c-'a']) cur->child[c-'a'] = new Trie();
                cur = cur->child[c-'a'];
            }
            cur->isEnd = true;
        }
        bool search(string w) {
            Trie* cur = this;
            for (char c : w) {
                if (!cur->child[c-'a']) return false;
                cur = cur->child[c-'a'];
            }
            return cur->isEnd;
        }
    };

    // 其他：Z-function, Manacher(最長迴文),
    //       後綴陣列, AC自動機, 後綴自動機
}

// ============================================================
// 【21】數論（Number Theory）
// ============================================================
void demo_number_theory() {
    // --- 最大公因數 GCD（輾轉相除）---
    function<long long(long long,long long)> gcd =
        [&](long long a, long long b) {
            return b == 0 ? a : gcd(b, a % b);
        };
    cout << gcd(12, 18) << "\n";  // 6
    // 最小公倍數 LCM = a*b/gcd(a,b)

    // --- 質數篩（埃拉托斯特尼）---
    int N = 100;
    vector<bool> isPrime(N+1, true);
    isPrime[0] = isPrime[1] = false;
    for (int i = 2; i*i <= N; i++)
        if (isPrime[i])
            for (int j = i*i; j <= N; j += i)
                isPrime[j] = false;

    // --- 快速冪 + 模逆元（費馬小定理）---
    // a^(p-2) mod p = a 的逆元 (p為質數)

    // --- 擴展歐幾里得 ---
    function<long long(long long,long long,long long&,long long&)> extgcd =
        [&](long long a, long long b, long long& x, long long& y) -> long long {
            if (b == 0) { x = 1; y = 0; return a; }
            long long x1, y1;
            long long g = extgcd(b, a%b, x1, y1);
            x = y1;
            y = x1 - (a/b) * y1;
            return g;
        };

    // 其他：歐拉函數, 中國剩餘定理(CRT),
    //       Miller-Rabin質數測試, Pollard-Rho分解
    // ⭐ 數論在 CTF Crypto 超級重要！(RSA 就靠這些)
}

// ============================================================
// 【22】圖論（Graph Theory）
// ============================================================
void demo_graph() {
    int n = 5;
    vector<vector<pair<int,int>>> g(n);  // {鄰居, 權重}

    // --- Dijkstra 單源最短路（無負邊）---
    auto dijkstra = [&](int src) {
        vector<long long> dist(n, LLONG_MAX);
        priority_queue<pair<long long,int>,
                       vector<pair<long long,int>>,
                       greater<>> pq;
        dist[src] = 0;
        pq.push({0, src});
        while (!pq.empty()) {
            auto [d, u] = pq.top(); pq.pop();
            if (d > dist[u]) continue;
            for (auto [v, w] : g[u])
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    pq.push({dist[v], v});
                }
        }
        return dist;
    };

    // 其他重要演算法：
    // - Bellman-Ford：可處理負邊，偵測負環
    // - Floyd-Warshall：全點對最短路 O(n³)
    // - Kruskal / Prim：最小生成樹（MST）
    // - 拓撲排序：DAG 的線性排序
    // - Tarjan：強連通分量(SCC)、橋、割點
    // - 二分圖匹配（匈牙利演算法）
    // - 最大流 / 最小割（Dinic, ISAP）
}

// ============================================================
// 【23】計算幾何（Computational Geometry）
// ============================================================
struct Point {
    double x, y;
    Point operator-(const Point& p) const { return {x-p.x, y-p.y}; }
};

// 叉積：判斷方向、求面積
double cross(Point a, Point b) { return a.x*b.y - a.y*b.x; }

// 點積：判斷夾角
double dot(Point a, Point b) { return a.x*b.x + a.y*b.y; }

void demo_geometry() {
    Point a{0,0}, b{4,0}, c{0,3};

    // 兩點距離
    auto dist = [](Point p, Point q){
        return sqrt(pow(p.x-q.x,2) + pow(p.y-q.y,2));
    };

    // 三角形面積（叉積/2）
    double area = abs(cross(b-a, c-a)) / 2.0;
    cout << "三角形面積: " << area << "\n";  // 6

    // 判斷三點方向（>0 逆時針, <0 順時針, =0 共線）
    double d = cross(b-a, c-a);

    // 其他經典：
    // - 凸包（Convex Hull, Graham/Andrew 算法）
    // - 線段相交判斷
    // - 點在多邊形內（射線法）
    // - 最近點對（分治）
    // - 旋轉卡尺（最遠點對）
}

// ============================================================
//  學習路徑建議（給初學者）
// ============================================================
/*
  📍 第一階段（基礎）：
     STL → Stack/Queue → Sort → DFS/BFS

  📍 第二階段（進階）：
     並查集 → 二叉樹 → 貪心 → 分治/減治

  📍 第三階段（DP 是分水嶺）：
     簡單 DP → 背包 → LIS/LCS → 區間/樹形 DP

  📍 第四階段（高階）：
     線段樹 → 樹狀數組 → 圖論 → 字串演算法

  📍 第五階段（專精，CTF 重點）：
     數論（RSA！）→ 計算幾何

  ⭐ 對你的 CTF 目標：數論 + 字串演算法 最該優先攻克！
*/

int main() {
    demo_stack();
    demo_queue();
    // ... 各 demo 可逐一呼叫測試
    cout << "\n見每個函式內的範例與註解\n";
    return 0;
}