//
// Created by user on 2026/5/16.
//
// ============================================================
//  C++ 語言完整複習手冊
//  適合：學過但忘光光的你 😄
//  C++ = C 的超集合，所以 C 的東西幾乎都能用
// ============================================================

#include <iostream>     // cin, cout
#include <string>       // std::string
#include <vector>       // std::vector
#include <map>          // std::map
#include <set>          // std::set
#include <algorithm>    // sort, find, min, max...
#include <sstream>      // stringstream
#include <fstream>      // 檔案
#include <memory>       // smart pointer
#include <functional>   // function, lambda
#include <iomanip>      // setw, setprecision

using namespace std;    // 省略 std:: 前綴（小專案常用）

// ============================================================
// 【1】基本型別 & 輸入輸出
// ============================================================
void demo_basic() {
    // 型別（和 C 一樣）
    int    a = 10;
    double b = 3.14;
    char   c = 'A';
    bool   d = true;        // C++ 原生 bool！
    string s = "Hello";     // C++ 字串（不是 char[]）

    // auto（自動推導型別，C++11）
    auto x = 42;            // int
    auto y = 3.14;          // double
    auto z = "Hi"s;         // std::string

    // cout / cin（比 printf/scanf 更安全）
    cout << "a = " << a << endl;
    cout << "b = " << b << "\n";
    cout << "bool = " << boolalpha << d << "\n";  // 印 true/false

    // 格式化輸出
    cout << fixed << setprecision(2) << 3.14159 << "\n";  // 3.14
    cout << setw(10) << right << 42 << "\n";               // 右對齊寬10
    cout << setfill('0') << setw(5) << 42 << "\n";        // 00042

    // cin 輸入
    // int n; cin >> n;
    // string line; getline(cin, line);  // 讀整行含空格

    // C++ 也可以用 printf（向下相容）
    printf("printf 也能用: %d\n", a);
}

// ============================================================
// 【2】C++ 字串（std::string）
// ============================================================
void demo_string() {
    string s1 = "Hello";
    string s2 = "World";

    // 串接
    string s3 = s1 + ", " + s2 + "!";
    cout << s3 << "\n";   // Hello, World!

    // 基本操作
    cout << s1.length()  << "\n";    // 5
    cout << s1.size()    << "\n";    // 5（同上）
    cout << s1[0]        << "\n";    // H
    cout << s1.at(1)     << "\n";    // e（有邊界檢查）
    cout << s1.substr(1, 3) << "\n"; // ell（從1開始取3個）
    cout << s1.find("ll")   << "\n"; // 2
    cout << (s1 == "Hello") << "\n"; // 1（true）

    s1.replace(0, 3, "ABC");  // "ABClo"
    s1.insert(0, ">>>");      // ">>>ABClo"
    s1.erase(0, 3);           // "ABClo"

    // 大小寫
    for (char &ch : s1) ch = toupper(ch);

    // 字串轉數字
    int  n = stoi("123");
    double d = stod("3.14");

    // 數字轉字串
    string ns = to_string(456);

    // stringstream（格式化字串）
    stringstream ss;
    ss << "x=" << 10 << ", y=" << 3.14;
    cout << ss.str() << "\n";
}

// ============================================================
// 【3】參考（Reference）← C++ 特有！
// ============================================================
void demo_reference() {
    int x = 10;
    int &ref = x;   // ref 是 x 的別名，不是複本！

    ref = 20;
    cout << x << "\n";   // 20（x 也被改了）

    // 指標 vs 參考
    // 指標：可以是 NULL，可以重新指向
    // 參考：不能是 NULL，建立後不能改指向

    // 函式用參考傳遞（比指標更乾淨）
    auto swap = [](int &a, int &b) {
        int tmp = a; a = b; b = tmp;
    };
    int a = 1, b = 2;
    swap(a, b);
    cout << a << " " << b << "\n";  // 2 1

    // const 參考（不能修改，但避免複製）
    auto print_str = [](const string &s) {
        cout << s << "\n";
    };
    print_str("Hello");
}

// ============================================================
// 【4】函式進階
// ============================================================

// 函式多載（同名不同參數）
int    add(int a,    int b)    { return a + b; }
double add(double a, double b) { return a + b; }
string add(string a, string b) { return a + b; }

// 預設參數
void greet(string name, string msg = "你好") {
    cout << msg << ", " << name << "!\n";
}

// 模板（泛型）
template<typename T>
T mymax(T a, T b) { return a > b ? a : b; }

// Lambda（C++11）
void demo_functions() {
    cout << add(1, 2)       << "\n";   // 3
    cout << add(1.5, 2.5)   << "\n";   // 4.0
    cout << add("Hi", "!")  << "\n";   // Hi!

    greet("Alice");            // 你好, Alice!
    greet("Bob", "哈囉");     // 哈囉, Bob!

    cout << mymax(10, 20)    << "\n";   // 20
    cout << mymax(3.14, 2.7) << "\n";   // 3.14

    // Lambda
    auto square = [](int x) { return x * x; };
    cout << square(5) << "\n";  // 25

    // 捕獲外部變數
    int base = 100;
    auto add_base = [base](int x) { return x + base; };  // 值捕獲
    auto add_base2 = [&base](int x) { base += x; };       // 參考捕獲

    cout << add_base(5) << "\n";  // 105
}

// ============================================================
// 【5】STL 容器
// ============================================================
void demo_vector() {
    // vector（動態陣列，最常用！）
    vector<int> v = {10, 20, 30, 40, 50};

    v.push_back(60);        // 尾端加入
    v.pop_back();           // 移除尾端
    v.insert(v.begin(), 5); // 頭部插入
    v.erase(v.begin());     // 移除頭部

    cout << v.size()    << "\n";  // 大小
    cout << v.front()   << "\n";  // 第一個
    cout << v.back()    << "\n";  // 最後一個
    cout << v[2]        << "\n";  // 索引存取
    cout << v.at(2)     << "\n";  // 有邊界檢查

    // 走訪
    for (int x : v) cout << x << " ";   // range-based for
    for (auto &x : v) x *= 2;           // 用參考才能修改

    // 排序
    sort(v.begin(), v.end());            // 升序
    sort(v.begin(), v.end(), greater<int>());  // 降序
    sort(v.begin(), v.end(), [](int a, int b){ return a > b; });

    // 搜尋
    auto it = find(v.begin(), v.end(), 20);
    if (it != v.end()) cout << "找到了\n";

    // 二維 vector
    vector<vector<int>> mat(3, vector<int>(3, 0));  // 3x3 全0
    mat[1][2] = 99;
}

void demo_map() {
    // map（有序鍵值對，底層紅黑樹）
    map<string, int> scores;
    scores["Alice"] = 95;
    scores["Bob"]   = 88;
    scores.insert({"Carol", 92});

    cout << scores["Alice"] << "\n";       // 95
    cout << scores.count("Bob") << "\n";   // 1（存在）
    cout << scores.count("Dave") << "\n";  // 0（不存在）

    for (auto &[name, score] : scores) {   // C++17 結構化綁定
        cout << name << ": " << score << "\n";
    }

    scores.erase("Bob");

    // unordered_map（雜湊表，更快，無序）
    // unordered_map<string, int> um;

    // map 取得所有 key
    for (auto it = scores.begin(); it != scores.end(); ++it) {
        cout << it->first << " " << it->second << "\n";
    }
}

void demo_set() {
    // set（有序，不重複）
    set<int> s = {5, 3, 1, 4, 2, 3, 1};
    // 自動排序且去重：{1, 2, 3, 4, 5}

    s.insert(6);
    s.erase(3);
    cout << (s.count(4) ? "有" : "沒有") << "\n";

    for (int x : s) cout << x << " ";

    // unordered_set（雜湊，O(1) 查找，無序）
    // unordered_set<int> us;
}

// ============================================================
// 【6】類別（Class）← C++ 核心！
// ============================================================
class Student {
private:
    string name;
    int    age;
    double score;

public:
    // 建構子
    Student(string name, int age, double score)
        : name(name), age(age), score(score) {}

    // 預設建構子
    Student() : name("Unknown"), age(0), score(0.0) {}

    // Getter / Setter
    string getName()  const { return name; }
    int    getAge()   const { return age; }
    double getScore() const { return score; }
    void   setScore(double s) { score = s; }

    // 方法
    void greet() const {
        cout << "我是 " << name << "，" << age << " 歲\n";
    }

    // 運算子多載
    bool operator<(const Student &other) const {
        return score < other.score;
    }

    bool operator==(const Student &other) const {
        return name == other.name;
    }

    // 輸出運算子多載
    friend ostream& operator<<(ostream &os, const Student &s) {
        os << "Student(" << s.name << ", " << s.score << ")";
        return os;
    }

    // 解構子（釋放資源）
    ~Student() {
        // cout << name << " 被銷毀了\n";
    }
};

// 繼承
class GradStudent : public Student {
private:
    string thesis;

public:
    GradStudent(string name, int age, double score, string thesis)
        : Student(name, age, score), thesis(thesis) {}

    void greet() const override {   // override 確保覆寫
        Student::greet();
        cout << "論文：" << thesis << "\n";
    }
};

void demo_class() {
    Student s1("Alice", 20, 95.5);
    s1.greet();
    cout << s1 << "\n";   // 用運算子多載

    vector<Student> students = {
        {"C", 20, 70}, {"A", 22, 95}, {"B", 21, 85}
    };
    sort(students.begin(), students.end());  // 用 operator<
    for (auto &st : students) cout << st << "\n";

    GradStudent g("Bob", 25, 88, "AI研究");
    g.greet();

    // 多型
    Student *ptr = &g;   // 父類別指標指向子類別
    // ptr->greet();     // 需要 virtual 才能多型！
}

// ============================================================
// 【7】智慧指標（Smart Pointer，C++11）
// ============================================================
void demo_smart_ptr() {
    // unique_ptr：獨占所有權，自動釋放
    unique_ptr<int> p1 = make_unique<int>(42);
    cout << *p1 << "\n";   // 42
    // 不需要 delete！離開 scope 自動釋放

    // shared_ptr：共享所有權
    shared_ptr<int> p2 = make_shared<int>(100);
    shared_ptr<int> p3 = p2;   // 兩個都指向同一個
    cout << p2.use_count() << "\n";  // 2（引用計數）
    p2.reset();    // p2 放棄，引用計數 -1
    cout << *p3 << "\n";   // 100（p3 還在，資料沒被刪）

    // 傳統 new/delete（盡量用 smart pointer）
    int *raw = new int(999);
    cout << *raw << "\n";
    delete raw;
}

// ============================================================
// 【8】例外處理
// ============================================================
void demo_exception() {
    try {
        int x = -1;
        if (x < 0) throw runtime_error("不能是負數！");
        cout << x << "\n";
    }
    catch (const runtime_error &e) {
        cerr << "錯誤: " << e.what() << "\n";
    }
    catch (...) {
        cerr << "未知錯誤\n";
    }

    // 自訂例外
    struct MyException : exception {
        const char* what() const noexcept override {
            return "我的自訂例外";
        }
    };

    try {
        throw MyException();
    } catch (const MyException &e) {
        cout << e.what() << "\n";
    }
}

// ============================================================
// 【9】檔案 I/O
// ============================================================
void demo_file() {
    // 寫入
    ofstream ofs("test.txt");
    ofs << "Hello, File!" << "\n";
    ofs << "數字: " << 42 << "\n";
    ofs.close();

    // 讀取（整行）
    ifstream ifs("test.txt");
    string line;
    while (getline(ifs, line)) {
        cout << line << "\n";
    }
    ifs.close();

    // 二進位模式
    ofstream bin("data.bin", ios::binary);
    int val = 0xDEADBEEF;
    bin.write(reinterpret_cast<char*>(&val), sizeof(val));
    bin.close();
}

// ============================================================
// 【10】CTF 常用技巧
// ============================================================
void demo_ctf() {
    // XOR 加解密
    string msg = "Hello";
    char key = 0x42;
    for (char &c : msg) c ^= key;
    cout << "加密 hex: ";
    for (unsigned char c : msg)
        printf("%02X ", c);
    cout << "\n";
    for (char &c : msg) c ^= key;   // 再 XOR 解密
    cout << "解密: " << msg << "\n";

    // 進位轉換
    int n = 255;
    cout << hex << n << "\n";    // ff
    cout << oct << n << "\n";    // 377
    cout << dec << n << "\n";    // 255（要記得改回來！）

    // 字串→整數（不同進位）
    string hex_str = "ff";
    int val = stoi(hex_str, nullptr, 16);  // 255
    cout << val << "\n";

    // memcpy / memset（記憶體操作）
    char buf[10] = {0};
    memset(buf, 0x41, 9);         // 全部填 'A'
    buf[9] = '\0';
    cout << buf << "\n";          // AAAAAAAAA

    // 印出記憶體 bytes
    float f = 1.0f;
    unsigned char *bytes = reinterpret_cast<unsigned char*>(&f);
    cout << "1.0f bytes: ";
    for (int i = 0; i < 4; i++) printf("%02X ", bytes[i]);
    cout << "\n";  // 00 00 80 3F（小端序）

    // bit 操作
    uint32_t x = 0b10110100;
    cout << "bit 3: " << ((x >> 3) & 1) << "\n";  // 取第3位
    x |=  (1 << 5);   // 設定 bit 5
    x &= ~(1 << 4);   // 清除 bit 4
    x ^=  (1 << 2);   // 翻轉 bit 2
}

// ============================================================
// 主程式
// ============================================================
int main() {
    cout << "=== 基本 ===\n";      demo_basic();
    cout << "\n=== 字串 ===\n";    demo_string();
    cout << "\n=== 參考 ===\n";    demo_reference();
    cout << "\n=== 函式 ===\n";    demo_functions();
    cout << "\n=== vector ===\n";  demo_vector();
    cout << "\n=== map ===\n";     demo_map();
    cout << "\n=== set ===\n";     demo_set();
    cout << "\n=== 類別 ===\n";    demo_class();
    cout << "\n=== 智慧指標 ===\n";demo_smart_ptr();
    cout << "\n=== 例外 ===\n";    demo_exception();
    cout << "\n=== 檔案 ===\n";    demo_file();
    cout << "\n=== CTF ===\n";     demo_ctf();
    return 0;
}