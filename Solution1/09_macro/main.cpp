// UTF-8
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
