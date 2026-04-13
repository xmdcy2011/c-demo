// UTF-8
// ═══════════════════════════════════════════════════════════════
// 06 RAII（Resource Acquisition Is Initialization）
// ═══════════════════════════════════════════════════════════════
//
// 核心思想：构造时获取资源，析构时释放资源
//   构造函数 → fopen / new / lock
//   析构函数 → fclose / delete / unlock（确定性调用，离开作用域必然执行）
//
// 好处：即使中途抛异常，析构函数也会被调用，资源不会泄漏
//
// 标准库里的 RAII 例子：
//   std::lock_guard<std::mutex>  → 构造时加锁，析构时解锁
//   std::unique_ptr              → 构造时持有指针，析构时 delete
//   std::ifstream                → 构造时打开文件，析构时关闭
//
// 禁用拷贝：FileHandle(const FileHandle&) = delete
//   文件句柄不应该被复制（两个对象析构时会 double-close）
//
// Java 对照：Java 用 try-with-resources + AutoCloseable
//            C++ 用 RAII，析构函数确定性调用，不需要 try 块
// ═══════════════════════════════════════════════════════════════
#include "FileHandle.h"
#include <iostream>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "=== RAII FileHandle ===" << std::endl;

    {
        FileHandle f("test.txt", "w+");  // 构造时打开
        f.writeLine("第一行");
        f.writeLine("第二行");
        f.writeLine("第三行");
        f.readAll();
    }  // 离开作用域，析构函数自动关闭文件，不需要手动 fclose

    std::cout << "\n文件已自动关闭" << std::endl;

    // 测试异常：打开不存在的路径
    try {
        FileHandle bad("/nonexistent/path/file.txt", "r");
    } catch (const std::runtime_error& e) {
        std::cout << "捕获异常: " << e.what() << std::endl;
    }

    return 0;
}
