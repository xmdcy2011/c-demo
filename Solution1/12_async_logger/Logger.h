// UTF-8
// ═══════════════════════════════════════════════════════════════
// 12 异步日志系统（生产者/消费者）
// ═══════════════════════════════════════════════════════════════
//
// 生产者/消费者模式：
//   生产者（主线程）：调用 log() 把消息放入队列
//   消费者（后台线程）：从队列取消息写入文件
//   两者通过 condition_variable 协调，队列空时消费者等待
//
// condition_variable：
//   cv.wait(lock, pred)  → 释放锁并阻塞，直到 pred 为 true
//   cv.notify_one()      → 唤醒一个等待的线程
//   cv.notify_all()      → 唤醒所有等待的线程
//
// Java 对照：
//   condition_variable ≈ Object.wait() / notify() / notifyAll()
//   unique_lock + cv.wait() ≈ synchronized + wait()
// ═══════════════════════════════════════════════════════════════
#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <fstream>
#include <functional>

class Logger {
public:
    Logger(const char* filename);
    ~Logger();  // 等后台线程处理完所有消息再退出

    void log(const std::string& msg);

    // 禁止拷贝（RAII 资源独占）
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    void consume();  // 后台线程函数：从队列取消息写文件

    std::queue<std::string> queue_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::thread worker_;
    std::ofstream file_;
    bool done_ = false;
};
