// UTF-8
// ═══════════════════════════════════════════════════════════════
// 08 模板 Stack<T>
// ═══════════════════════════════════════════════════════════════
//
// 模板必须把声明和实现都放在 .h 里（不能分离到 .cpp）
// 原因：编译器需要在实例化时看到完整实现
//
// 函数模板：template<typename T> T add(T a, T b) { return a + b; }
// 类模板：  template<typename T> class Stack { ... };
//           成员函数实现：template<typename T> void Stack<T>::push(...) {}
//
// 模板参数推导：
//   add(1, 2)      → T 推导为 int
//   add(1.0, 2.0)  → T 推导为 double
//   Stack<int>     → 类模板需要显式指定
//
// Java 对照：Java 泛型是类型擦除（运行时无类型信息）
//            C++ 模板是编译期代码生成，每种类型生成一份独立代码
//            性能更好但编译慢，错误信息也更难看
// ═══════════════════════════════════════════════════════════════
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
