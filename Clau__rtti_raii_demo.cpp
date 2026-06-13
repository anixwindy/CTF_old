// ============================================================
// Clau__rtti_raii_demo.cpp
// RTTI vs RAII 對照範例（完整可編譯）
// 編譯：g++ -std=c++17 Clau__rtti_raii_demo.cpp -o demo
// 解說：繁體中文
// ============================================================
//
// 核心觀念：
//   RTTI = Run-Time Type Information，執行時取得「型別」資訊
//          用到的東西：typeid、dynamic_cast；需要 class 有 virtual function
//   RAII = Resource Acquisition Is Initialization，「資源」管理慣用法
//          建構時取得資源，解構時自動釋放；不是語言機制，是程式設計手法
//
// 兩者完全不同，只是縮寫長得像。
// ============================================================

#include <iostream>
#include <typeinfo>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <cstdio>

using std::cout;
using std::endl;


// ============================================================
//   PART 1 : RTTI 範例（識別實際型別）
// ============================================================

// 基底類別「必須有 virtual function」編譯器才會生 typeinfo
class Animal {
public:
    virtual ~Animal() = default;   // virtual 解構 → 啟動 RTTI
    virtual void speak() = 0;
};

class Dog : public Animal {
public:
    void speak() override { cout << "汪汪\n"; }
    void fetch()          { cout << "Dog 撿球\n"; }
};

class Cat : public Animal {
public:
    void speak() override { cout << "喵喵\n"; }
    void scratch()        { cout << "Cat 抓沙發\n"; }
};

void rtti_demo(Animal *a) {
    // (1) typeid：問「這個物件實際是哪個型別」
    cout << "[RTTI] typeid = " << typeid(*a).name() << "\n";

    // (2) dynamic_cast：安全向下轉型，失敗回 nullptr
    if (Dog *d = dynamic_cast<Dog*>(a)) {
        cout << "[RTTI] 偵測到 Dog → ";
        d->fetch();
    } else if (Cat *c = dynamic_cast<Cat*>(a)) {
        cout << "[RTTI] 偵測到 Cat → ";
        c->scratch();
    } else {
        cout << "[RTTI] 不認識的型別\n";
    }
    a->speak();   // 一般 virtual 派發（不靠 RTTI，靠 vtable）
}


// ============================================================
//   PART 2 : RAII 範例（自動釋放資源）
// ============================================================

// 自己寫一個 RAII wrapper：包住 C 的 FILE*
class FileLogger {
    FILE *fp;
public:
    explicit FileLogger(const char *path) {
        fp = std::fopen(path, "w");
        if (!fp) throw std::runtime_error("open failed");
        cout << "[RAII] FileLogger 建構 → 已開啟 " << path << "\n";
    }
    ~FileLogger() {                    // 解構子是 RAII 的關鍵
        if (fp) {
            std::fclose(fp);
            cout << "[RAII] FileLogger 解構 → 已關閉檔案\n";
        }
    }
    // 禁止複製，避免兩個物件共用同一個 fp 各自 fclose
    FileLogger(const FileLogger&) = delete;
    FileLogger& operator=(const FileLogger&) = delete;

    void write(const char *s) { std::fputs(s, fp); }
};

void raii_demo() {
    std::mutex mtx;

    cout << "--- 進入 scope ---\n";
    {
        FileLogger log("raii_test.txt");           // 建構 → 開檔
        std::lock_guard<std::mutex> g(mtx);        // 建構 → lock
        auto buf = std::make_unique<int[]>(100);   // 建構 → new[]

        log.write("hello RAII\n");
        cout << "[RAII] 中間做事...\n";

        // 即使這裡 throw，底下三個物件的解構子照樣會跑
        // throw std::runtime_error("boom");
    }
    // 離開 scope，反向解構：unique_ptr → lock_guard → FileLogger
    cout << "--- 離開 scope ---\n";
}


// ============================================================
//   PART 3 : 一個物件同時涉及 RTTI 和 RAII（很常見）
// ============================================================
//
// 觀念釐清：
//   下面 base 同時用到兩件事：
//     - unique_ptr 本身是 RAII（離開 scope 自動 delete）
//     - dynamic_cast 是 RTTI（執行期識別實際型別）
//   它們在「同一段程式碼」裡並不衝突，職責完全不同。
//
void combined_demo() {
    std::unique_ptr<Animal> a = std::make_unique<Dog>();   // RAII：自動 delete
    rtti_demo(a.get());                                    // RTTI：辨識成 Dog
}                                                          // 離開 scope → unique_ptr 解構 → delete Dog


// ============================================================
//   主程式
// ============================================================
int main() {
    cout << "========== RTTI 範例 ==========\n";
    Dog d;
    Cat c;
    rtti_demo(&d);
    rtti_demo(&c);

    cout << "\n========== RAII 範例 ==========\n";
    raii_demo();

    cout << "\n========== 結合範例 ==========\n";
    combined_demo();

    cout << "\n[main] 結束\n";
    return 0;
}

// ============================================================
// 一句話對照：
//   RTTI 問「你是誰？」     → 看 type
//   RAII 管「誰來收尾？」   → 看 lifetime
// ============================================================
