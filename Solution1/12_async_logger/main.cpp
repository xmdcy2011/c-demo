// UTF-8
#include "Logger.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "=== 异步日志系统 ===" << std::endl;
    {
        Logger logger("app.log");

        // 主线程快速写入，后台线程异步落盘
        logger.log("程序启动");
        logger.log("用户登录: alice");

        // 模拟多线程同时写日志
        std::vector<std::thread> threads;
        for (int i = 0; i < 3; i++) {
            threads.emplace_back([&logger, i] {
                for (int j = 0; j < 3; j++) {
                    logger.log("线程" + std::to_string(i) + " 消息" + std::to_string(j));
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            });
        }
        for (auto& t : threads) t.join();

        logger.log("程序退出");
    }  // Logger 析构：等后台线程处理完所有消息，再关闭文件

    std::cout << "\n日志已写入 app.log" << std::endl;
    return 0;
}
