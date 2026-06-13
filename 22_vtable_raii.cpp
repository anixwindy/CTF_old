//
// Created by user on 2026/5/23.
//
// C++ VTable & RAII 完整教學

// 🧱 Part 1：VTable（虛擬函式表）
//
// 這就是 VTable 存在的理由。
//
// VTable 的本質
//
// 📌 每個有 virtual 函式的 class，編譯器會自動幫它建一張函式指標陣列，叫做 VTable。
//
// Dog 的 VTable：
// ┌─────────────────────────────┐
// │  &Dog::speak()  → "汪！"   │  ← index 0
// │  &Dog::move()   → 跑步邏輯 │  ← index 1
// └─────────────────────────────┘
//
// 每個 Dog 物件內部：
// ┌──────────┬──────────────┐
// │  vptr    │  其他成員    │
// │  ↓       │              │
// │  Dog的   │              │
// │  VTable  │              │
// └──────────┴──────────────┘

// 完整程式碼範例
#include <iostream>
#include <mutex>
#include <fstream>
#include <stdexcept>
#include <memory>

using namespace std;




class Animal {
public:
    // virtual 關鍵字 → 觸發 VTable 機制
    virtual void speak() {
        std::cout << "???" << std::endl;
    }
    virtual void move() {
        std::cout << "移動中..." << std::endl;
    }
//  重要！有 virtual function 就必須有 virtual destructor
    virtual ~Animal() {
        std::cout << "[Animal] 析構" << std::endl;
    }
};

class Dog : public Animal {
public:
    void speak() override {  // override 明確覆寫
        std::cout << "汪！" << std::endl;
    }
    ~Dog() {
        std::cout << "[Dog] 析構" << std::endl;
    }
};

class Cat : public Animal {
public:
    void speak() override {
        std::cout << "喵～" << std::endl;
    }
    ~Cat() {
        std::cout << "[Cat] 析構" << std::endl;
    }
};
// ______________RAII________________________

class Base {
public:
    virtual void foo() {}
    int x = 42;
};
// ______________RAII________________________
// RAII 解法

// 自製 RAII File 管理器
class FileGuard {
private:
    FILE* file_;

public:
    // 建構子：拿資源
    FileGuard(const char* path, const char* mode) {
        file_ = fopen(path, mode);
        if (!file_) throw std::runtime_error("開檔失敗");
        std::cout << "[FileGuard] 開檔成功\n";
    }

    // 析構子：還資源（不管怎麼離開作用域，一定會執行）
    ~FileGuard() {
        if (file_) {
            fclose(file_);
            std::cout << "[FileGuard] 自動關檔\n";
        }
    }

    FILE* get() { return file_; }

    // 禁止複製（避免雙重釋放）
    FileGuard(const FileGuard&) = delete;
    FileGuard& operator=(const FileGuard&) = delete;
};
// ______________RAII________________________
void safe_read() {
    FileGuard fg("data.txt", "r");  // 開檔

    // 做任何事... 拋出例外也好、提早 return 也好
    // 離開這個作用域，~FileGuard() 一定被呼叫！

} // ← 在這裡自動關檔
// ______________RAII________________________
void modern_cpp_raii() {

    // 1️⃣ unique_ptr → 自動 delete
    auto ptr = std::make_unique<int>(100);
    // 不需要 delete ptr！離開作用域自動釋放

    // 2️⃣ lock_guard → 自動解鎖 mutex
    std::mutex mtx;
    {
        std::lock_guard<std::mutex> lock(mtx);
        // 做執行緒安全的事
    } // ← 自動 unlock，不會死鎖

    // 3️⃣ fstream → 自動關檔
    std::fstream file("test.txt", std::ios::out);
    file << "Hello";
    // 離開作用域自動 close
};


int main() {
    Animal* animals[2];
    animals[0] = new Dog();
    animals[1] = new Cat();

    for (auto* a : animals) {
        a->speak();  // 執行時期才決定！這就是多型
    }

    for (auto* a : animals) {
        delete a;    // 因為有 virtual ~，會正確呼叫子類析構
    }
    cout<<"------_____________________---"<<endl;


// 🔬 實驗：看 vptr 在記憶體中的位置
//



   Base obj;

    // 物件的前 8 bytes（64位元）就是 vptr！
    void** vptr = *reinterpret_cast<void***>(&obj);

    cout << "物件位址:  " << &obj << std::endl;
    cout << "vptr 位址: " << vptr << std::endl;
    cout << "第一個虛函式位址: " << vptr[0] << std::endl;
    cout<<"------_____________________---"<<endl;

// 💡 你可以在 GDB 裡用 info vtbl obj 直接看 VTable！


// 🛡️ Part 2：RAII（Resource Acquisition Is Initialization）
// 核心思想
//
// 📌 資源的生命週期 = 物件的生命週期
// 在建構子裡拿資源，在析構子裡還資源，讓 C++ 自動管理。
//
//
// 為什麼需要 RAII？
// 先看沒有它的慘況：
//
//
// // ❌ 危險寫法（沒有 RAII）
    //
// void dangerous() {
//     FILE* f = fopen("data.txt", "r");
//
//     if (something_fails()) {
//         return;  // 💀 忘記 fclose！記憶體洩漏！
//     }
//
//     // 如果中間 throw exception...
//     process(f);
//
//     fclose(f);  // 可能根本跑不到這裡
// }
//
    //
    //
    // RAII 解法

    try {
        safe_read();
    } catch (const std::exception& e) {
        std::cout << "錯誤：" << e.what() << std::endl;
    }

//___________________________
// 標準庫中的 RAII 例子





// 🔗 VTable + RAII 的關係圖
// VTable  解決的問題：「執行時期，該呼叫哪個函式？」
//           ↓
//         多型 (Polymorphism)
//_____________________________________________
// RAII    解決的問題：「資源什麼時候釋放？」
//           ↓
//         生命週期管理 (Lifetime Management)

// 兩者的交叉點：
//   virtual ~Destructor()
//   ↑
//   如果沒有這個，用 Base* 指向 Derived 物件
//   delete 時不會呼叫 Derived 的析構子
//   → RAII 失效！資源洩漏！

// 🔴 Red Team 視角
// Red teamer 看到 VTable 和 RAII，腦子裡轉的是：

// VTable 攻擊面
// 技術說明VTable Hijacking若能覆寫物件的 vptr（堆上的 overflow），
//     呼叫 virtual function 就會跳到攻擊者控制的位址
    //
//     → 控制流劫持Type Confusion把 Dog* 當 Cat* 用，vtable 不同
//     → 呼叫錯誤的函式指標UAF (Use-After-Free)物件被 delete 後 vtable 指標仍可能有效，再次呼叫
//     → 經典 PWN 題型


// 攻擊者的想法：
// heap 上的物件 → [vptr | data]
//                     ↓
//               vtable [func0, func1, ...]
//
// 如果我能 overflow 蓋掉 vptr，
// 讓它指向我偽造的 fake vtable，
// fake vtable 裡放 system("/bin/sh") 的位址...
// 呼叫 virtual function = getshell！



// RAII 攻擊面
//
// Double Free：繞過 = delete 的複製保護，讓兩個物件管理同一資源 → free() 兩次 → heap 腐敗
// 析構順序攻擊：在多執行緒環境，析構子執行時機可被競爭 → TOCTOU / Race Condition
// 例外安全漏洞：RAII 實作不完整，exception 路徑資源未釋放 → 洩漏敏感資料殘留記憶體
//

// 🧠 Red teamer 的核心問題永遠是：
// 「這個物件的生命週期，有沒有任何邊界條件我可以操控？」
//
//
//





}
