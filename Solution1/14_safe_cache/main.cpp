// UTF-8
#include "Cache.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "=== 线程安全 Cache<string, int> ===" << std::endl;

    Cache<std::string, int> cache;

    // 基本操作
    cache.set("a", 1);
    cache.set("b", 2);
    cache.set("c", 3);

    auto val = cache.get("b");
    std::cout << "get(b) = " << (val ? std::to_string(*val) : "nullopt") << std::endl;

    auto missing = cache.get("x");
    std::cout << "get(x) = " << (missing ? std::to_string(*missing) : "nullopt") << std::endl;

    cache.remove("a");
    std::cout << "remove(a) 后 size=" << cache.size() << std::endl;

    // 多线程并发读写
    std::cout << "\n=== 多线程并发读写 ===" << std::endl;
    Cache<int, std::string> shared;
    std::mutex printMtx;

    // 3个写线程
    std::vector<std::thread> writers;
    for (int i = 0; i < 3; i++) {
        writers.emplace_back([&shared, &printMtx, i] {
            for (int j = 0; j < 5; j++) {
                int key = i * 10 + j;
                shared.set(key, "val_" + std::to_string(key));
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        });
    }

    // 5个读线程
    std::vector<std::thread> readers;
    for (int i = 0; i < 5; i++) {
        readers.emplace_back([&shared, &printMtx, i] {
            for (int j = 0; j < 10; j++) {
                auto v = shared.get(i * 3);  // 并发读，shared_lock 允许
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
            }
        });
    }

    for (auto& t : writers) t.join();
    for (auto& t : readers) t.join();

    std::cout << "并发读写完成，cache size=" << shared.size() << std::endl;

    return 0;
}
