// UTF-8
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
