// UTF-8
#include "Buffer.h"
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "=== 拷贝 vs 移动对比 ===" << std::endl;

    Buffer a("hello");
    a.print("a");

    std::cout << "\n--- 拷贝构造（分配新内存）---" << std::endl;
    Buffer b = a;           // 拷贝，a 不变
    a.print("a after copy");
    b.print("b");

    std::cout << "\n--- 移动构造（零拷贝，偷走指针）---" << std::endl;
    Buffer c = std::move(a);  // a 的内存转移给 c，a 变空
    a.print("a after move");  // null
    c.print("c");

    std::cout << "\n--- 移动赋值 ---" << std::endl;
    Buffer d("world");
    d = std::move(b);
    b.print("b after move");  // null
    d.print("d");

    std::cout << "\n=== 析构 ===" << std::endl;
    return 0;
}
