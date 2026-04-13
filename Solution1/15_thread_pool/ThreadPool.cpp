// UTF-8
#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads) {
    for (size_t i = 0; i < numThreads; i++) {
        workers_.emplace_back(&ThreadPool::workerLoop, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mtx_);
        stop_ = true;
    }
    cv_.notify_all();  // 唤醒所有工作线程，让它们退出
    for (auto& t : workers_) {
        if (t.joinable()) t.join();
    }
}

void ThreadPool::workerLoop() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

            if (stop_ && tasks_.empty()) return;  // 停止且队列空，退出

            task = std::move(tasks_.front());
            tasks_.pop();
        }
        task();  // 在锁外执行任务，避免持锁时间过长
    }
}
