// UTF-8
#include "ThreadPool.h"
#include <iostream>
#include <chrono>
#include <mutex>
#include <windows.h>

std::mutex printMtx;

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "=== 线程池任务调度器 ===" << std::endl;

    ThreadPool pool(4);  // 4个工作线程

    // 提交有返回值的任务，拿到 future
    std::cout << "\n--- 提交计算任务 ---" << std::endl;
    std::vector<std::future<int>> futures;
    for (int i = 0; i < 8; i++) {
        futures.push_back(pool.submit([i] {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            int result = i * i;
            std::lock_guard<std::mutex> lg(printMtx);
            std::cout << "  任务" << i << " 完成，结果=" << result
                      << " 线程=" << std::this_thread::get_id() << std::endl;
            return result;
        }));
    }

    // 等待所有任务完成，收集结果
    std::cout << "\n--- 收集结果 ---" << std::endl;
    int total = 0;
    for (int i = 0; i < (int)futures.size(); i++) {
        int val = futures[i].get();  // 阻塞等待该任务完成
        total += val;
        std::cout << "  future[" << i << "].get() = " << val << std::endl;
    }
    std::cout << "  总和=" << total << std::endl;

    // 提交无返回值的任务
    std::cout << "\n--- 提交 void 任务 ---" << std::endl;
    std::vector<std::future<void>> voids;
    for (int i = 0; i < 4; i++) {
        voids.push_back(pool.submit([i] {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            std::lock_guard<std::mutex> lg(printMtx);
            std::cout << "  void任务" << i << " 完成" << std::endl;
        }));
    }
    for (auto& f : voids) f.get();  // 等待所有完成

    std::cout << "\n所有任务完成，线程池析构..." << std::endl;
    // pool 析构：stop=true，唤醒所有线程，join 等待退出
    return 0;
}
