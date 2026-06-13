// ============================================================
// Clau__CTF_cpp_examples.cpp
// CTF 玩家「碰到 C++ 編譯產物或 C++ 原始碼」要能看懂的範例
// 所有解說：繁體中文
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cstdint>
#include <cstring>

// ------------------------------------------------------------
// [初階 1] 命名空間 / using
// ------------------------------------------------------------
// C++ 把符號包在 namespace 內 → 編譯後變成 mangled name
// IDA / Ghidra 看到 std::__cxx11::basic_string<...> 都是這個來的
using std::cout;
using std::endl;

// ------------------------------------------------------------
// [初階 2] class、建構/解構、this 指標
// ------------------------------------------------------------
// 反編譯看到 sub_xxx(obj, 1, 2); 其實是 obj.method(1,2);
// 第一個參數一定是 this（rcx on Windows x64, rdi on Linux x64）。
class Point {
public:
    int x, y;
    Point(int x_, int y_) : x(x_), y(y_) {}     // 建構：類似 __ct_
    ~Point() {}                                  // 解構：__dt_
    void print() const { cout << x << "," << y << endl; }
};

// ------------------------------------------------------------
// [初階 3] virtual / vtable（CTF C++ 題核心）
// ------------------------------------------------------------
// 物件首 8 bytes（64-bit）放著 vtable 指標
// vtable 是一張函式指標陣列；呼叫 virtual function 變成：
//   call qword ptr [rax]      // 第 0 個 virtual
//   call qword ptr [rax+8]    // 第 1 個 virtual
// pwn 中經常透過 UAF 換掉 vtable 指標 → 控制執行流。
class Base {
public:
    virtual void hello() { cout << "Base::hello\n"; }
    virtual void bye()   { cout << "Base::bye\n";   }
    virtual ~Base() {}
};
class Child : public Base {
public:
    void hello() override { cout << "Child::hello\n"; }
    void bye()   override { cout << "Child::bye\n";   }
};

void demo_vtable() {
    Base *b = new Child();
    b->hello();             // 動態派發 → 跑 Child::hello
    // 觀察記憶體：*(void**)b 是 vtable 指標
    void **vptr = *reinterpret_cast<void***>(b);
    cout << "vtable[0] @ " << vptr[0] << "\n";
    delete b;
}

// ------------------------------------------------------------
// [初階 4] 參考 vs 指標
// ------------------------------------------------------------
// 反編譯時參考(reference)看起來跟指標一樣（都是位址）
// 差別只在語法層，二進位差不多。
void by_ref(int &r)  { r = 99; }
void by_ptr(int *p)  { *p = 99; }

// ------------------------------------------------------------
// [初階 5] new / delete / new[] / delete[]
// ------------------------------------------------------------
// new 底層通常呼叫 operator new(size_t) → 內部 malloc
// CTF pwn：new/delete 也吃 glibc heap，所以 tcache 攻擊照樣有效
void demo_new() {
    int *arr = new int[4]{10, 20, 30, 40};
    for (int i = 0; i < 4; i++) cout << arr[i] << " ";
    cout << "\n";
    delete[] arr;
}

// ------------------------------------------------------------
// [中階 1] template（模板）
// ------------------------------------------------------------
// 編譯時每個型別會「實體化」出一份函式 → 在二進位裡會看到多個版本
// mangled name 含型別資訊：_Z3addIiET_S0_S0_ 之類
template <typename T>
T add(T a, T b) { return a + b; }

// ------------------------------------------------------------
// [中階 2] STL 常見容器（題目最常給你看）
// ------------------------------------------------------------
void demo_stl() {
    // string：底層常是 SSO（短字串內嵌） + heap buffer
    std::string s = "flag{stl}";
    cout << "string size = " << s.size() << "\n";

    // vector：連續記憶體（data, size, capacity）三指標
    std::vector<int> v = {1, 2, 3};
    v.push_back(4);
    for (int x : v) cout << x << " "; cout << "\n";

    // map：紅黑樹（key→value）；unordered_map 才是 hash table
    std::map<std::string, int> m;
    m["a"] = 1;
    m["b"] = 2;
    for (auto &kv : m) cout << kv.first << "=" << kv.second << " ";
    cout << "\n";
}

// ------------------------------------------------------------
// [中階 3] smart pointer
// ------------------------------------------------------------
// unique_ptr：唯一所有權，離開 scope 自動 delete
// shared_ptr：引用計數；底層多一塊 control block（強/弱計數）
// CTF 較少直接出現，但讀現代 C++ source 會看到。
void demo_smart_ptr() {
    auto up = std::make_unique<Point>(1, 2);
    up->print();
    auto sp = std::make_shared<Point>(3, 4);
    sp->print();
}

// ------------------------------------------------------------
// [中階 4] name mangling（很關鍵！）
// ------------------------------------------------------------
// C++ 編譯後函式名稱會被改名，IDA 沒 demangle 會看到：
//   _ZN5Child5helloEv  ←→  Child::hello()
//   _Znwm              ←→  operator new(unsigned long)
//   _Znam              ←→  operator new[](unsigned long)
//   _ZdlPv             ←→  operator delete(void*)
// 工具：c++filt   例：echo _ZN5Child5helloEv | c++filt
// 想避開 mangling 可用 extern "C"：
extern "C" void plain_c_function(int x) {     // 編譯後就叫 plain_c_function
    cout << "plain: " << x << "\n";
}

// ------------------------------------------------------------
// [中階 5] exception / RTTI（少見但要認得）
// ------------------------------------------------------------
// throw / try / catch 編譯後會在 .gcc_except_table / .eh_frame 留資訊
// dynamic_cast / typeid 用 RTTI → 二進位多出 typeinfo 符號（_ZTI...）。
void demo_exception() {
    try {
        throw std::runtime_error("boom");
    } catch (const std::exception &e) {
        cout << "caught: " << e.what() << "\n";
    }
}

// ------------------------------------------------------------
// [中階 6] 運算子重載
// ------------------------------------------------------------
// 反編譯看到「+ 」變成 operator+(this, &other) → 別誤判為一般函式
struct Vec2 {
    int x, y;
    Vec2 operator+(const Vec2 &o) const { return {x + o.x, y + o.y}; }
};

// ------------------------------------------------------------
// [RE 視角] C++ 物件記憶體佈局（單繼承 + virtual）
// ------------------------------------------------------------
// class Child : public Base { virtual void f(); int extra; };
// 佈局：
//   offset 0:  vptr (8 bytes, 64-bit)
//   offset 8:  Base 成員...
//   offset N:  Child 自己加的成員 (extra)
// 多重繼承會有多個 vptr，更複雜，CTF 題不多碰。

int main() {
    Point p(3, 4); p.print();
    demo_vtable();
    int a = 0; by_ref(a); by_ptr(&a);
    cout << "a = " << a << "\n";
    demo_new();
    cout << "add<int>    = " << add<int>(2, 3) << "\n";
    cout << "add<double> = " << add<double>(1.5, 2.5) << "\n";
    demo_stl();
    demo_smart_ptr();
    plain_c_function(7);
    demo_exception();
    Vec2 r = Vec2{1,2} + Vec2{3,4};
    cout << "vec sum: " << r.x << "," << r.y << "\n";
    return 0;
}
