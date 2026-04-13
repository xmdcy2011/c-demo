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

    // ─────────────────────────────────────────────────────────
    // .h 和 .cpp 的编译过程
    //
    //   .h 不参与编译，只是文本片段，#include 就是原样粘贴
    //   .cpp 才是编译单元，每个生成一个 .obj，最后链接成 .exe
    //
    //   Buffer.cpp  →  [Buffer.h 内容 + Buffer.cpp 内容]  →  Buffer.obj
    //   main.cpp    →  [Buffer.h 内容 + main.cpp 内容]    →  main.obj
    //   链接：Buffer.obj + main.obj  →  程序.exe
    //
    //   为什么模板必须全放 .h：
    //     编译器看到 Stack<int> 时需要立刻生成代码，必须能看到完整实现
    //     如果实现放 .cpp：
    //       Stack.cpp 编译时不知道 T 是什么 → 无法生成代码
    //       main.cpp 看到 Stack<int> 但看不到实现 → 链接时找不到符号 → 报错
    //
    //   为什么 .h 要有 #pragma once：
    //     A.h 和 B.h 都 include Stack.h，main.cpp include A.h 和 B.h
    //     → Stack.h 被粘贴两次 → class Stack 重复定义 → 编译错误
    //     #pragma once 告诉预处理器：这个文件只粘贴一次
    //
    //   模板实例化：每种类型生成一份独立代码
    //     Stack<int>    → 编译器生成 Stack_int 的所有方法
    //     Stack<string> → 编译器生成 Stack_string 的所有方法
    //     Java 泛型是类型擦除，运行时只有一份；C++ 模板编译期生成多份
    //     → C++ 性能更好，但编译慢，错误信息也更难看
    // ─────────────────────────────────────────────────────────
    std::cout << "\n=== 模板实例化：每种类型独立生成代码 ===" << std::endl;
    {
        // Stack<int> 和 Stack<double> 是完全不同的两个类
        // 编译器为每种类型各生成一份 push/pop/top 的代码
        Stack<int> si;
        Stack<double> sd;
        Stack<std::string> ss2;

        si.push(42);
        sd.push(3.14);
        ss2.push("template");

        std::cout << "Stack<int>:    "; si.print();
        std::cout << "Stack<double>: "; sd.print();
        std::cout << "Stack<string>: "; ss2.print();

        // 函数模板：编译器根据参数自动推导 T
        auto add = [](auto a, auto b) { return a + b; };
        std::cout << "add(1, 2)     = " << add(1, 2) << "  (T=int)" << std::endl;
        std::cout << "add(1.5, 2.5) = " << add(1.5, 2.5) << "  (T=double)" << std::endl;
        std::cout << "add(\"a\", \"b\") = " << add(std::string("a"), std::string("b"))
                  << "  (T=string)" << std::endl;
    }

    return 0;
}
