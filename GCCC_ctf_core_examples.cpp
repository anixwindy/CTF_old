/*
 * CCC_CTF C++ 核心範例
 *
 * 目的：
 * - 示範初中階 RE / Pwn 常遇到的 C++ 語言行為。
 * - 範例保持可讀：物件生命週期、std::string、vector、virtual call、
 *   有檢查與沒檢查的索引、簡單 crypto 風格 byte 轉換。
 *
 * 編譯參考：
 *   g++ GCp_CCC_ctf_core_examples.cpp -std=c++17 -o GCp_CCC_ctf_core_examples
 */

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

static void CCC_line(const std::string &title) {
    std::cout << "\n--------\n";
    std::cout << title << "\n";
}

static void CCC_RE_string_bytes() {
    CCC_line("CCC_RE_string_bytes");

    std::string s = std::string("CTF\0RE", 6);

    /*
     * std::string 可以存 NUL byte。
     * 用 operator<< 印出時，終端顯示可能讓人誤會長度，
     * 所以 RE 常需要直接看 raw bytes。
     */
    std::cout << "size: " << s.size() << "\n";
    std::cout << "bytes:";
    for (unsigned char c : s) {
        std::cout << " 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(c);
    }
    std::cout << std::dec << "\n";
}

static void CCC_RE_vector_layout() {
    CCC_line("CCC_RE_vector_layout");

    std::vector<int> values = {10, 20, 30, 40};

    /*
     * std::vector 會連續存放元素。
     * 在 RE 裡，vector 常可以理解成 data pointer + size + capacity。
     */
    std::cout << "size: " << values.size() << "\n";
    std::cout << "data address: " << static_cast<const void *>(values.data()) << "\n";
    std::cout << "values:";
    for (int x : values) {
        std::cout << " " << x;
    }
    std::cout << "\n";
}

class CCC_BaseCheck {
public:
    virtual ~CCC_BaseCheck() = default;

    virtual int score(int x) const {
        return x + 1;
    }
};

class CCC_DerivedCheck : public CCC_BaseCheck {
public:
    int score(int x) const override {
        return (x * 3) ^ 0x12;
    }
};

static void CCC_RE_virtual_dispatch() {
    CCC_line("CCC_RE_virtual_dispatch");

    CCC_DerivedCheck derived;
    CCC_BaseCheck *base = &derived;

    /*
     * 透過 base pointer 呼叫 virtual function 時，實際會派發到 derived method。
     * 在反組譯中，這常像是先讀 vtable pointer，
     * 再從 vtable 裡取出 function pointer 來 call。
     */
    std::cout << "score(7): " << base->score(7) << "\n";
}

static void CCC_PWN_checked_vs_unchecked_index() {
    CCC_line("CCC_PWN_checked_vs_unchecked_index");

    std::vector<int> values = {100, 200, 300};

    /*
     * values.at(i) 會檢查邊界，越界時丟出 exception。
     * values[i] 不檢查邊界，可能變成漏洞模式。
     */
    std::cout << "values[1]  : " << values[1] << "\n";
    std::cout << "values.at(1): " << values.at(1) << "\n";

    try {
        std::cout << values.at(99) << "\n";
    } catch (const std::out_of_range &e) {
        std::cout << "values.at(99) blocked: " << e.what() << "\n";
    }
}

static void CCC_PWN_heap_lifetime_raii() {
    CCC_line("CCC_PWN_heap_lifetime_raii");

    auto number = std::make_unique<int>(1337);

    /*
     * RAII 代表 unique_ptr 會自動釋放 heap 配置。
     * 在 C++ RE 中，清理動作可能藏在 scope 結束時呼叫的 destructor 裡。
     */
    std::cout << "heap value: " << *number << "\n";
}

static std::vector<uint8_t> CCC_xor_repeating_key(const std::vector<uint8_t> &data,
                                                  const std::vector<uint8_t> &key) {
    std::vector<uint8_t> out;
    out.reserve(data.size());

    for (std::size_t i = 0; i < data.size(); i++) {
        out.push_back(static_cast<uint8_t>(data[i] ^ key[i % key.size()]));
    }

    return out;
}

static void CCC_CRYPTO_repeating_xor() {
    CCC_line("CCC_CRYPTO_repeating_xor");

    std::vector<uint8_t> plain = {'C', 'T', 'F', '_', 'X', 'O', 'R'};
    std::vector<uint8_t> key = {'K', 'E', 'Y'};

    std::vector<uint8_t> encrypted = CCC_xor_repeating_key(plain, key);
    std::vector<uint8_t> decoded = CCC_xor_repeating_key(encrypted, key);

    std::cout << "encrypted hex:";
    for (uint8_t b : encrypted) {
        std::cout << " " << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(b);
    }
    std::cout << std::dec << "\n";

    std::cout << "decoded: ";
    for (uint8_t b : decoded) {
        std::cout << static_cast<char>(b);
    }
    std::cout << "\n";
}

static int CCC_mod_inverse_small(int a, int mod) {
    /*
     * 這是教學用的小型暴力反元素搜尋。
     * 真實 crypto 會用更好的演算法，但這裡重點是看懂條件：
     * (a * x) % mod == 1.
     */
    for (int x = 1; x < mod; x++) {
        if ((a * x) % mod == 1) {
            return x;
        }
    }
    return -1;
}

static void CCC_CRYPTO_mod_inverse() {
    CCC_line("CCC_CRYPTO_mod_inverse");

    int a = 17;
    int mod = 3120;
    int inv = CCC_mod_inverse_small(a, mod);

    std::cout << "inverse of 17 mod 3120: " << inv << "\n";
    std::cout << "check: (17 * inv) % 3120 = " << (17 * inv) % mod << "\n";
}

int main() {
    CCC_RE_string_bytes();
    CCC_RE_vector_layout();
    CCC_RE_virtual_dispatch();

    CCC_PWN_checked_vs_unchecked_index();
    CCC_PWN_heap_lifetime_raii();

    CCC_CRYPTO_repeating_xor();
    CCC_CRYPTO_mod_inverse();

    return 0;
}
