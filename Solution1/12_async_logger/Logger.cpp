// UTF-8
#include "Logger.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>

// 获取当前时间字符串，格式 HH:MM:SS
static std::string timestamp() {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    char buf[32];
    struct tm tm_info;
    localtime_s(&tm_info, &t);
    strftime(buf, sizeof(buf), "%H:%M:%S", &tm_info);
    return buf;
}

Logger::Logger(const char* filename) : file_(filename) {
    if (!file_.is_open())
        throw std::runtime_error(std::string("无法打开日志文件: ") + filename);

    // 启动后台消费者线程，绑定到 Logger::consume 成员函数
    // this 作为第一个参数传入，让 consume 能访问成员变量
    worker_ = std::thread(&Logger::consume, this);
    std::cout << "[Logger] 启动，日志文件: " << filename << std::endl;
}

Logger::~Logger() {
    {
        // 加锁修改 done_，保证 consume 线程能看到这个变化
        // 用大括号限制锁的作用域，出了大括号立刻解锁
        // 必须先解锁再 notify，否则 consume 被唤醒后拿不到锁
        std::unique_lock<std::mutex> lock(mtx_);
        done_ = true;
    }  // ← 解锁

    // 唤醒后台线程：让它从 cv_.wait 里出来，检查 done_=true 后退出
    // notify 不需要持锁，解锁后再 notify 是正确做法
    cv_.notify_all();

    // 等待后台线程处理完队列里剩余的消息后退出
    // joinable() 检查线程是否可以 join（防止重复 join 崩溃）
    if (worker_.joinable()) worker_.join();
    std::cout << "[Logger] 关闭，后台线程已退出" << std::endl;
}

void Logger::log(const std::string& msg) {
    {
        // 加锁保护 queue_，防止多线程同时 push 导致数据竞争
        std::unique_lock<std::mutex> lock(mtx_);
        queue_.push("[" + timestamp() + "] " + msg);
    }  // ← 解锁，先解锁再 notify，让后台线程能立刻拿到锁

    // 通知后台线程：队列里有新消息了，可以起来处理
    // notify_one 只唤醒一个等待线程（这里只有一个后台线程，够用）
    cv_.notify_one();
}

void Logger::consume() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx_);

        // cv_.wait 做了三件事（原子）：
        //   1. 释放 lock，让 log() 能进来 push 消息
        //   2. 阻塞当前线程，等待 notify
        //   3. 被唤醒后重新加锁，检查条件
        // 条件：队列非空（有消息要处理）或 done_=true（要退出）
        // 用 lambda 防止虚假唤醒（spurious wakeup）：
        //   条件不满足时即使被唤醒也继续等待
        cv_.wait(lock, [this] { return !queue_.empty() || done_; });

        // 持锁状态下，把队列里所有消息都处理完
        // 不逐条 notify，而是一次性清空，减少线程切换开销
        while (!queue_.empty()) {
            const std::string& msg = queue_.front();
            file_ << msg << "\n";
            file_.flush();  // 立即刷盘，保证崩溃时日志不丢失
            std::cout << "[Logger 后台] 写入: " << msg << std::endl;
            queue_.pop();
        }

        // 队列已清空，且主线程通知退出 → 安全退出循环
        // 注意顺序：先清空队列再检查 done_，保证不丢消息
        if (done_) break;
    }
    // 退出循环后 lock 析构自动解锁
}
