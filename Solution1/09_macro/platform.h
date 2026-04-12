// UTF-8
#pragma once

// ── 跨平台导出宏 ──────────────────────────────────────────────
#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

// ── DEBUG 日志宏（release 下零开销）──────────────────────────
#ifdef _DEBUG
    #include <iostream>
    #define LOG(x) std::cout << "[DEBUG] " << x << std::endl
    #define LOG_VAR(x) std::cout << "[DEBUG] " #x " = " << (x) << std::endl
#else
    #define LOG(x)
    #define LOG_VAR(x)
#endif

// ── 平台检测宏 ────────────────────────────────────────────────
#ifdef _WIN32
    #define PLATFORM_NAME "Windows"
#elif __APPLE__
    #define PLATFORM_NAME "macOS"
#elif __linux__
    #define PLATFORM_NAME "Linux"
#else
    #define PLATFORM_NAME "Unknown"
#endif
