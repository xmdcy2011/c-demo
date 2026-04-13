// UTF-8
// ═══════════════════════════════════════════════════════════════
// platform.h：跨平台宏定义
// ═══════════════════════════════════════════════════════════════
//
// _WIN32 / __APPLE__ / __linux__ 是编译器预定义宏，不需要手动定义
// 编译器在编译开始前根据当前环境自动注入：
//   Windows 上用 MSVC/MinGW 编译  → 自动定义 _WIN32（32位和64位都有）
//   macOS 上用 Clang 编译          → 自动定义 __APPLE__
//   Linux 上用 GCC 编译            → 自动定义 __linux__
//
// 其他常用预定义宏：
//   _WIN64        Windows 64位
//   _MSC_VER      MSVC 编译器版本号（如 1930）
//   __GNUC__      GCC 编译器
//   __clang__     Clang 编译器
//   __cplusplus   C++ 标准版本：201703L=C++17, 202002L=C++20
//   __x86_64__    x64 架构（GCC/Clang）
//   _M_X64        x64 架构（MSVC）
//
// #pragma once：防止头文件被重复 include（比 include guard 简洁）
//   等价写法：
//     #ifndef PLATFORM_H
//     #define PLATFORM_H
//     ...
//     #endif
// ═══════════════════════════════════════════════════════════════
#pragma once

// ── 跨平台导出宏 ──────────────────────────────────────────────
// DLL 导出符号的平台差异：
//   Windows：__declspec(dllexport) 告诉链接器把符号暴露出去
//   Linux/macOS：__attribute__((visibility("default"))) 效果相同
// 用 EXPORT 宏统一，代码里只写 EXPORT，编译器自动选对应的写法
#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

// ── DEBUG 日志宏（release 下零开销）──────────────────────────
// _DEBUG 由 MSVC 在 Debug 配置下自动定义，Release 配置下不定义
// 空宏在 release 下被预处理器完全删除，不产生任何代码和开销
// #x 是字符串化运算符，把宏参数转成字符串字面量
//   LOG_VAR(count) → std::cout << "[DEBUG] " "count" " = " << (count)
#ifdef _DEBUG
    #include <iostream>
    #define LOG(x) std::cout << "[DEBUG] " << x << std::endl
    #define LOG_VAR(x) std::cout << "[DEBUG] " #x " = " << (x) << std::endl
#else
    #define LOG(x)      // release 下空宏，预处理器直接删除这行
    #define LOG_VAR(x)  // release 下空宏，零开销
#endif

// ── 平台检测宏 ────────────────────────────────────────────────
// 编译期确定平台名称，运行时直接是字符串常量，无判断开销
// 注意：#elif 后面不需要 defined()，直接写宏名也可以
//   #elif defined(__APPLE__)  和  #elif __APPLE__  效果相同
#ifdef _WIN32
    #define PLATFORM_NAME "Windows"
#elif __APPLE__
    #define PLATFORM_NAME "macOS"
#elif __linux__
    #define PLATFORM_NAME "Linux"
#else
    #define PLATFORM_NAME "Unknown"
#endif
