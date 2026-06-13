/* ═══════════════════════════════════════════════════════════════════
 *                    CTF RE 必備知識地圖
 * ═══════════════════════════════════════════════════════════════════
 *
 * ── C：核心，一定要會 ──
 *
 *   語言層面：
 *     - 基本型別與大小：char(1) / int(4) / long(8) / pointer(8)，小端序讀法
 *     - 指標算術：*p、p[i]、*(p+i) 三種寫法等價
 *     - struct 記憶體佈局、padding 規則
 *     - 函式呼叫慣例（x64 System V：前 6 參數 rdi/rsi/rdx/rcx/r8/r9，回傳值 rax）
 *     - stack frame：局部變數、saved rbp、return address 的相對位置
 *
 *   常見 idiom（反組譯時高頻出現）：
 *     - malloc/free、linked list 走訪
 *     - strcmp/strlen/memcpy/memset 的底層行為
 *     - printf format string 對應哪些暫存器
 *     - XOR、位移、bit mask 操作
 *
 *   不需要：複雜的泛型 macro、POSIX 網路程式設計
 *
 * ── C++：看得懂就好，不需精通 ──
 *
 *   主要障礙：name mangling 與 vtable
 *     - _ZN3Foo3barEv → Foo::bar()（c++filt 還原）
 *     - vtable 佈局：物件前 8 bytes 是 vtable pointer
 *     - std::string 的 SSO（小字串直接存在 stack）
 *     - std::vector 三指標結構：begin / end / capacity
 *
 *   不需要：template metaprogramming、SFINAE、現代 C++20 特性
 *
 * ── Python：工具語言，要熟 ──
 *
 *     一定要會：
 *       bytes / bytearray / int.from_bytes / .to_bytes
 *       pow(base, exp, mod)               # 模冪，RSA/crypto 必用
 *       struct.unpack('<I', data[:4])     # 小端 uint32
 *       subprocess.run / from pwn import *
 *
 *     腳本思維：「二進位 → 字串 → 整數 → 回字串」順暢轉換
 *
 * ── 真正的核心：組合語言讀寫能力 ──
 *
 *   語言知識只是橋樑，RE 實際戰場是 x86-64 組合語言 + Ghidra/IDA：
 *     - 讀 disasm：cmp/je/jne/jg → if/else
 *     - 追 data flow：值從哪個暫存器來、怎麼變
 *     - 識別 idiom：lea rax,[rbp-0x10] 是局部陣列
 *     - 動態分析：x64dbg / gdb 下斷點、觀察實際值
 *
 * ── 學習優先順序 ──
 *
 *   1. x86-64 組合語言基礎             ← 最優先
 *   2. C 指標 + struct + 呼叫慣例
 *   3. Python struct/pwntools 腳本
 *   4. C++ vtable/name mangling（遇到再查）
 *
 *   C++ 深度語法在 RE 中幾乎不需要主動寫，認得反組譯結果就夠了。
 *
 * ═══════════════════════════════════════════════════════════════════
 */

/*
 * 2_RE_cpp_internals_and_foundation.cpp
 * 合併自：
 *   - 2_CaRE_cpp_internals.cpp
 *   - 2_RE_cpp_foundation.cpp
 *
 * 排列順序：
 *   - Part A：2_CaRE_cpp_internals.cpp
 *   - Part B：2_RE_cpp_foundation.cpp（依要求放後面）
 *
 * 用途：
 *   - 保留兩份教材內容，不遺漏原本主題
 *   - 用單一檔案整合「C++ RE 奇景」與「C++ RE 基礎總檔」
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <cstdint>
#include <cstring>
#include <cstdio>

using namespace std;

/* ════════════════════════════════════════════════════════════
 * Part A：原 2_CaRE_cpp_internals.cpp
 *
 * 重點：C++ 的編譯結果跟 C 完全不同，要看得懂這些 pattern
 *
 * 4 個 Level：
 *   Level 1: name mangling ── 函式名稱被改成天書
 *   Level 2: vtable / 多型 ── virtual call 怎麼在 disasm 中認出
 *   Level 3: std::string SSO + std::vector 記憶體佈局
 *   Level 4: template + lambda + RAII ── 現代 C++ 反組譯地獄
 *
 * 編譯：
 *   g++ -O0 -g -o cpp_internals_foundation 2_RE_cpp_internals_and_foundation.cpp
 *
 * 觀察 mangling：
 *   nm cpp_internals_foundation | grep -i level
 *   再用 c++filt 還原：echo "_ZN3Foo3barEv" | c++filt
 * ════════════════════════════════════════════════════════════ */
namespace Math {
    int add(int a, int b)          { return a + b; }
    double add(double a, double b) { return a + b; }
    int sub(int a, int b)          { return a - b; }
}

extern "C" int add_c_style(int a, int b) {
    /* extern "C" 取消 mangling，符號就是 "add_c_style"
     * CTF 中要呼叫 C++ 函式時，常用這招產生乾淨符號 */
    return a + b;
}

void level1_demo(void) {
    std::cout << "[L1] Math::add(1, 2)        = " << Math::add(1, 2) << "\n";
    std::cout << "[L1] Math::add(1.5, 2.5)    = " << Math::add(1.5, 2.5) << "\n";
    std::cout << "[L1] Math::sub(10, 3)       = " << Math::sub(10, 3) << "\n";
    std::cout << "[L1] add_c_style(7, 8)      = " << add_c_style(7, 8) << "\n";
    /* 用 nm 看：3 個 _Z 開頭 + 1 個 add_c_style */
}


/* ════════════════════════════════════════════════════════════
 * Level 2：vtable / 多型（virtual function）
 *
 * RE 中最重要的 C++ 結構之一。
 *
 * 記憶體佈局（64-bit）：
 *   ┌─────────────────────────┐
 *   │ vtable pointer (8 bytes)│ ← 物件起始
 *   ├─────────────────────────┤
 *   │ 成員變數 ...             │
 *   └─────────────────────────┘
 *
 * vtable 內容（在 .rodata）：
 *   [0] RTTI pointer (typeinfo)
 *   [1] virtual func 1 address
 *   [2] virtual func 2 address
 *   ...
 *
 * virtual call 在 disasm：
 *   mov  rax, [rdi]        ; 取 vtable pointer (this 第一個 qword)
 *   mov  rax, [rax+0x10]   ; 取第 2 個 virtual func (offset 16，因為前 8 是 RTTI)
 *   call rax
 *
 * 看到「兩次 mov 後 call」就是 virtual call！
 * ════════════════════════════════════════════════════════════ */
class Animal {
public:
    virtual ~Animal() {}
    virtual void speak() = 0;
    virtual int legs() { return 4; }
};

class Dog : public Animal {
public:
    void speak() override { std::cout << "[L2] Woof!\n"; }
};

class Spider : public Animal {
public:
    void speak() override { std::cout << "[L2] (silent)\n"; }
    int legs() override { return 8; }
};

void poke(Animal *a) {
    /* 這裡的 a->speak() 和 a->legs() 都是 virtual call
     * 編譯器無法靜態決定要呼叫誰，必須查 vtable */
    a->speak();
    std::cout << "[L2] legs = " << a->legs() << "\n";
}

void level2_demo(void) {
    Dog d;
    Spider s;
    poke(&d);
    poke(&s);
    std::cout << "[L2] sizeof(Dog) = " << sizeof(Dog) << " (至少包含 vptr 8 bytes)\n";
}


/* ════════════════════════════════════════════════════════════
 * Level 3：std::string SSO + std::vector 記憶體佈局
 *
 * std::string (libstdc++ 實作)：
 *   - 小字串（≤15 bytes）直接存在 stack 上的 "buffer"（SSO）
 *   - 大字串才 heap 配置
 *   - 物件本體 32 bytes：
 *       [0..7]  pointer to data
 *       [8..15] size
 *       [16..31] 內嵌 buffer 或 capacity
 *
 * std::vector<T>：
 *   - 物件本體 24 bytes，三個指標：
 *       [0..7]  begin   (data 起點)
 *       [8..15] end     (有效資料結尾)
 *       [16..23] capacity_end (配置記憶體結尾)
 *   - 容量 = (capacity_end - begin)
 *   - 大小 = (end - begin)
 *
 * RE 中看到「物件後接 24 bytes 三個指標」幾乎一定是 vector
 * ════════════════════════════════════════════════════════════ */
void level3_demo(void) {
    std::string short_str = "hi";
    std::string long_str = std::string(50, 'A');

    std::cout << "[L3] sizeof(std::string) = " << sizeof(std::string)
              << " (通常 32 bytes，內含 SSO buffer)\n";
    std::cout << "[L3] short_str.data() = " << (void *)short_str.data()
              << " (應該指向 stack/物件內部)\n";
    std::cout << "[L3] long_str.data()  = " << (void *)long_str.data()
              << " (應該指向 heap)\n";

    std::vector<int> v = {10, 20, 30, 40, 50};
    std::cout << "[L3] sizeof(std::vector<int>) = " << sizeof(v) << " (預期 24)\n";
    std::cout << "[L3] vec.size = " << v.size() << ", capacity = " << v.capacity() << "\n";

    void **as_ptrs = reinterpret_cast<void **>(&v);
    std::cout << "[L3] vec[0..2] 三個指標：\n";
    std::cout << "      begin    = " << as_ptrs[0] << "\n";
    std::cout << "      end      = " << as_ptrs[1] << "\n";
    std::cout << "      cap_end  = " << as_ptrs[2] << "\n";
    std::cout << "      element_count = "
              << ((intptr_t)as_ptrs[1] - (intptr_t)as_ptrs[0]) / 4 << "\n";
}


/* ════════════════════════════════════════════════════════════
 * Level 4：template + lambda + RAII（現代 C++ 反組譯惡夢）
 *
 * 觀察重點：
 *   - template 每個型別實例化會產生「獨立的函式」
 *   - lambda 編譯成「匿名 class」，捕捉變數是該 class 的成員
 *   - std::function 內部用 type erasure，會有額外的 vtable-like 結構
 *   - unique_ptr 是 zero-overhead 包裝（純指標）
 *   - shared_ptr 有 control block（refcount），多一次間接
 *
 * RE tip：開 -O2 編譯後 template 很多會 inline，反而比較好讀
 *         開 -O0 看到的會是「過度抽象」的天書
 * ════════════════════════════════════════════════════════════ */
template<typename T>
T template_max(T a, T b) {
    return (a > b) ? a : b;
}

class Resource {
public:
    Resource(const char *name) : name_(name) {
        std::cout << "[L4] Resource(" << name_ << ") 配置\n";
    }
    ~Resource() {
        std::cout << "[L4] Resource(" << name_ << ") 釋放（RAII 自動觸發）\n";
    }
private:
    const char *name_;
};

void level4_demo(void) {
    std::cout << "[L4] max(3, 7)         = " << template_max(3, 7) << "\n";
    std::cout << "[L4] max(1.5, 0.5)     = " << template_max(1.5, 0.5) << "\n";

    int multiplier = 10;
    auto times = [multiplier](int x) {
        /* capture 變成匿名 class 的成員變數 */
        return x * multiplier;
    };
    std::cout << "[L4] lambda(5) = " << times(5) << "\n";
    std::cout << "[L4] sizeof(lambda) = " << sizeof(times) << " (內含捕捉的 multiplier)\n";

    std::function<int(int)> fn = times;
    std::cout << "[L4] std::function 包裝後呼叫 = " << fn(5) << "\n";
    std::cout << "[L4] sizeof(std::function) = " << sizeof(fn) << " (通常 32 bytes)\n";

    {
        Resource r("scoped");
        std::cout << "[L4] 在 scope 內使用 Resource\n";
    }

    auto ptr = std::make_unique<Resource>("heap_resource");
    (void)ptr;
}

void run_cpp_internals_demo(void) {
    std::cout << "===== Level 1: name mangling =====\n";
    level1_demo();
    std::cout << "\n===== Level 2: vtable / 多型 =====\n";
    level2_demo();
    std::cout << "\n===== Level 3: std::string + std::vector 佈局 =====\n";
    level3_demo();
    std::cout << "\n===== Level 4: template + lambda + RAII =====\n";
    level4_demo();
}


/* ============================================================
 * Part B：原 2_RE_cpp_foundation.cpp
 *
 * 原檔註記：
 *   合併自：
 *     - 2_re_concepts_cpp.cpp
 *     - 2_RE_must_know_cpp.cpp
 *
 * 用途：
 *   - 作為 C++ binary RE 的基礎總檔
 *   - 把 name mangling、class layout、vtable、this、繼承、STL
 *     這些最常見主題收成一份
 * ============================================================ */

/* ============================================================
 * 一、name mangling
 *   看到 _Z... 常常就知道這是 C++
 * ============================================================ */
namespace Foo {
    int bar(int x) { return x + 1; }
    int bar(double x) { return (int)x; }
}

void demo_name_mangling(void) {
    printf("Foo::bar(int)    = %d\n", Foo::bar(10));
    printf("Foo::bar(double) = %d\n", Foo::bar(2.7));
    puts("用 c++filt 或 Ghidra/IDA 的 demangle 看回原名");
}

/* ============================================================
 * 二、reference
 *   RE 時看起來很像指標，但語意上是別名
 * ============================================================ */
void demo_reference(void) {
    int x = 10;
    int &ref = x;
    ref = 20;
    printf("x = %d\n", x);

    auto print_val = [](const int &v) {
        printf("v = %d\n", v);
    };
    print_val(x);
}

/* ============================================================
 * 三、無虛函式 class layout
 *   沒有 virtual 時，幾乎就和 C struct 一樣
 * ============================================================ */
class PlainClass {
public:
    int x;
    float y;
    char z;
};

void demo_plain_class(void) {
    PlainClass obj;
    obj.x = 10;
    obj.y = 3.14f;
    obj.z = 'A';

    float *yp = (float *)((char *)&obj + 4);
    printf("y via offset+4 = %.2f\n", *yp);
    printf("sizeof(PlainClass) = %zu\n", sizeof(PlainClass));
}

/* ============================================================
 * 四、vtable 與多型
 *   C++ RE 最重要的一塊
 * ============================================================ */
class Base {
public:
    int data = 10;

    virtual void speak() { printf("Base::speak\n"); }
    virtual void move() { printf("Base::move\n"); }
    virtual ~Base() {}
};

class Derived : public Base {
public:
    int extra = 20;

    void speak() override { printf("Derived::speak\n"); }
};

void demo_vtable(void) {
    Base b;
    Derived d;
    Base *ptr = &d;

    ptr->speak();
    ptr->move();

    uintptr_t vptr = *(uintptr_t *)&b;
    printf("Base vptr = 0x%zx\n", (size_t)vptr);
    printf("sizeof(Base) = %zu\n", sizeof(Base));
    printf("sizeof(Derived) = %zu\n", sizeof(Derived));
}

/* ============================================================
 * 五、this 指標
 *   成員函式隱含第一個參數就是 this
 * ============================================================ */
class Counter {
public:
    int count = 0;

    void add(int n) {
        count += n;
    }
};

void demo_this_pointer(void) {
    Counter c;
    c.add(5);
    c.add(3);
    printf("count = %d\n", c.count);
    puts("Linux x64 常見 this 在 RDI，Windows x64 常見在 RCX");
}

/* ============================================================
 * 六、繼承 layout
 *   看 offset 可以判斷成員屬於 base 還是 derived
 * ============================================================ */
class Shape {
public:
    int color = 0;
    virtual int area() { return 0; }
};

class Rect : public Shape {
public:
    int w = 0;
    int h = 0;

    int area() override { return w * h; }
};

void demo_inheritance(void) {
    Rect r;
    r.color = 1;
    r.w = 5;
    r.h = 3;

    size_t color_off = (size_t)((char *)&r.color - (char *)&r);
    size_t w_off = (size_t)((char *)&r.w - (char *)&r);
    printf("color offset = +%zu, w offset = +%zu\n", color_off, w_off);
    printf("area = %d\n", r.area());
}

/* ============================================================
 * 七、STL layout 的最小觀察
 *   先懂 vector/string 就夠
 * ============================================================ */
void demo_stl_layout(void) {
    vector<int> v = {10, 20, 30};
    printf("vector size=%zu cap=%zu data=%p\n",
           v.size(), v.capacity(), (void *)v.data());
    printf("v[1] via raw ptr = %d\n", v.data()[1]);

    string s = "hello";
    printf("string size=%zu data=%p\n", s.size(), (void *)s.c_str());
    puts("RE 時常先認 data 指標、size、capacity 這種固定欄位");
}

/* ============================================================
 * 八、型別大小速查
 * ============================================================ */
void demo_sizes(void) {
    printf("sizeof(void*)      = %zu\n", sizeof(void *));
    printf("sizeof(int)        = %zu\n", sizeof(int));
    printf("sizeof(size_t)     = %zu\n", sizeof(size_t));
    printf("sizeof(PlainClass) = %zu\n", sizeof(PlainClass));
    printf("sizeof(Base)       = %zu\n", sizeof(Base));
}

void run_cpp_foundation_demo(void) {
    demo_name_mangling();
    puts("");

    demo_reference();
    puts("");

    demo_plain_class();
    puts("");

    demo_vtable();
    puts("");

    demo_this_pointer();
    puts("");

    demo_inheritance();
    puts("");

    demo_stl_layout();
    puts("");

    demo_sizes();
}

int main(void) {
    std::cout << "===== Part A: 2_CaRE_cpp_internals =====\n";
    run_cpp_internals_demo();

    std::cout << "\n===== Part B: 2_RE_cpp_foundation =====\n";
    run_cpp_foundation_demo();
    return 0;
}
