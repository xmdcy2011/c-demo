// UTF-8
// ═══════════════════════════════════════════════════════════════
// 09 命名空间 / 预处理器宏 / 类型转换
// ═══════════════════════════════════════════════════════════════
//
// 预处理器宏：
//   #define LOG(x)  → release 下空宏，零开销
//   #ifdef _WIN32   → 条件编译，跨平台代码
//   #pragma once    → 防止头文件重复包含（比 include guard 简洁）
//
// EXPORT 宏：
//   Windows：__declspec(dllexport)
//   Linux/Mac：__attribute__((visibility("default")))
//   用条件编译统一成 EXPORT，代码跨平台
//
// 类型转换（优先用 static_cast，避免 C 风格 (int)x）：
//   static_cast<T>       编译期安全转换，最常用
//   reinterpret_cast<T>  重新解释内存，危险，谨慎用
//   const_cast<T>        去掉 const，只在确定安全时用
//   dynamic_cast<T>      运行时多态类型检查，失败返回 nullptr
//
// namespace：
//   避免命名冲突，std:: 就是标准库的命名空间
//   using namespace std; 方便但污染全局，头文件里不要用
//
// Java 对照：Java 没有预处理器，跨平台靠 JVM 抽象
//            C++ 直接操作编译器指令，#ifdef 在编译期决定代码路径
// ═══════════════════════════════════════════════════════════════
#include "platform.h"
#include <iostream>
#include <cstdint>
#include <windows.h>

// 模拟一个导出函数（实际 DLL 场景用）
EXPORT int32_t add(int32_t a, int32_t b) {
    LOG("调用 add");
    LOG_VAR(a);
    LOG_VAR(b);
    return a + b;
}

// static_cast / reinterpret_cast / const_cast 演示
void demo_casts() {
    std::cout << "\n=== 类型转换 ===" << std::endl;

    // static_cast：编译期安全转换
    double d = 3.99;
    auto i = static_cast<int32_t>(d);  // 截断，不是四舍五入
    std::cout << "static_cast<int>(3.99) = " << i << std::endl;

    // reinterpret_cast：重新解释内存（危险，谨慎用）
    int32_t x = 0x41424344;
    auto* cp = reinterpret_cast<char*>(&x);
    std::cout << "reinterpret_cast 字节: "
              << cp[0] << cp[1] << cp[2] << cp[3] << std::endl;

    // const_cast：去掉 const（只在确定安全时用）
    const int32_t val = 100;
    auto* mutablePtr = const_cast<int32_t*>(&val);
    std::cout << "const_cast 读取: " << *mutablePtr << std::endl;
    // 注意：修改原本是 const 的变量是未定义行为，这里只演示语法
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "平台: " << PLATFORM_NAME << std::endl;

    int32_t result = add(3, 4);
    std::cout << "add(3, 4) = " << result << std::endl;

    demo_casts();

    return 0;
}
