// UTF-8
// ═══════════════════════════════════════════════════════════════
// 04 深拷贝
// ═══════════════════════════════════════════════════════════════
//
// 默认拷贝 = 浅拷贝（逐成员复制）：
//   对指针只复制地址，两个对象共享同一块内存
//   析构时 double-free → 崩溃
//
// 有裸指针必须手写（三法则）：
//   拷贝构造：Buffer(const Buffer& other)       → new + memcpy
//   拷贝赋值：Buffer& operator=(const Buffer&)  → 先 delete，再 new + memcpy
//   析构：    ~Buffer()                         → delete[]
//
// 拷贝赋值注意自赋值：if (this == &other) return *this;
//
// Java 对照：Java 对象赋值默认是浅拷贝（共享引用）
//            C++ 不写拷贝构造也是浅拷贝，但会导致 double-free
// ═══════════════════════════════════════════════════════════════
#include "Buffer.h"
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "=== 构造 ===" << std::endl;
    Buffer a("hello");
    a.print("a");

    std::cout << "\n=== 拷贝构造 ===" << std::endl;
    Buffer b = a;   // 深拷贝，b 有自己的内存
    b.print("b");
    a.print("a");   // a 不受影响

    std::cout << "\n=== 拷贝赋值 ===" << std::endl;
    Buffer c("world");
    c = a;
    c.print("c");

    std::cout << "\n=== 析构顺序 ===" << std::endl;
    // c, b, a 依次析构，各自释放自己的内存，不会 double-free
    return 0;
}
