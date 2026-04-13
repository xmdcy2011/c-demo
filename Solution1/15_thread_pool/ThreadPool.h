// UTF-8
// ═══════════════════════════════════════════════════════════════
// 15 线程池任务调度器
// ═══════════════════════════════════════════════════════════════
//
// 线程池：预先创建 N 个工作线程，复用线程避免频繁创建销毁开销
//   submit(f) → 把任务放入队列，返回 future 供调用方等待结果
//   工作线程循环：wait → 取任务 → 执行 → 继续等待
//
// std::function<void()>：类型擦除的可调用对象，可以存 lambda/函数指针
// std::packaged_task：把函数包装成可以获取返回值的任务
// std::future：异步结果的句柄，调用 get() 阻塞等待结果
//
// Java 对照：
//   ThreadPool      ≈ ExecutorService
//   submit()        ≈ submit()，返回 Future<T>
//   packaged_task   ≈ FutureTask
//   std::future     ≈ Future<T>
//   std::function   ≈ Runnable / Callable
// ═══════════════════════════════════════════════════════════════
#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <future>
#include <vector>
#include <stdexcept>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    // 提交任务，返回 future 供调用方等待结果
    template<typename F>
    auto submit(F&& f) -> std::future<std::invoke_result_t<F>>;

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

private:
    void workerLoop();  // 每个工作线程跑的循环

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool stop_ = false;
};

// ── 模板实现在 .h 里 ──────────────────────────────────────────
template<typename F>
auto ThreadPool::submit(F&& f) -> std::future<std::invoke_result_t<F>> {
    using ReturnType = std::invoke_result_t<F>;

    // packaged_task 包装 f，可以获取返回值
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(std::forward<F>(f));
    std::future<ReturnType> fut = task->get_future();

    {
        std::unique_lock<std::mutex> lock(mtx_);
        if (stop_) throw std::runtime_error("ThreadPool 已停止");
        // 把 task 包装成 void() 放入队列
        tasks_.push([task] { (*task)(); });
    }
    cv_.notify_one();  // 唤醒一个工作线程
    return fut;
}
