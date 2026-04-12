// UTF-8
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
