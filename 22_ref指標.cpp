//
// Created by user on 2026/5/21.
//
#include <iostream>
#include <vector>
#include <format>
#include <string>
#include <iomanip>
using namespace std;

int main() {
    float x =10.98711;
    cout<<format("hello{:.3f}",x)<<endl;


    int a = 1110;
    int b = 20;
    int c= 280;
    //___________________________________________
    int &ref = a;
    /*
    const int* ptr = &a;   // 鎖住"值"，能換指向
    int* const ptr = &a;   // 鎖住"指向"，能改值
    //        ↑ reference 天生就是這種！
     */


    //// ref 是 a 的別名
    // 從這行開始，ref 永遠綁定 a位置
    // 不能換！
    //___________________________________________
    cout << ref << endl;  // ?

    ref = b;              // 這是在做什麼？
    cout << a << endl;    // a 是多少？
    cout << ref << endl;  // ref 指向誰？

    int *ptr = &a;
    ptr = &c;             // 這又是在做什麼？
    cout << *ptr << endl; // ?
    cout << a << endl;    // a 有沒有變？
    return 0;
}