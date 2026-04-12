// UTF-8
#include "Stack.h"
#include <string>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "=== Stack<int32_t> ===" << std::endl;
    Stack<int32_t> si;
    si.push(1);
    si.push(2);
    si.push(3);
    si.print();
    std::cout << "top=" << si.top() << std::endl;
    si.pop();
    si.print();

    std::cout << "\n=== Stack<std::string> ===" << std::endl;
    Stack<std::string> ss;
    ss.push("hello");
    ss.push("world");
    ss.print();

    std::cout << "\n=== 空栈异常 ===" << std::endl;
    Stack<int32_t> empty;
    try {
        empty.top();
    } catch (const std::underflow_error& e) {
        std::cout << "捕获异常: " << e.what() << std::endl;
    }

    return 0;
}
