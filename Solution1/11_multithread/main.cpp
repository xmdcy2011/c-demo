// UTF-8
// ═══════════════════════════════════════════════════════════════
// 11 多线程基础
// ═══════════════════════════════════════════════════════════════
//
// std::thread：创建线程，必须 join() 或 detach()，否则析构时 terminate
// std::mutex + std::lock_guard：保护共享数据，防止数据竞争
// std::atomic<T>：原子操作，比 mutex 轻量，适合简单计数器
//
// 数据竞争：多个线程同时读写同一变量，结果不确定
//   解决：mutex 加锁（重量级）或 atomic（轻量级）
//
// Java 对照：
//   std::thread     ≈ Thread / Runnable
//   std::mutex      ≈ synchronized
//   std::lock_guard ≈ synchronized 块（自动释放锁）
//   std::atomic<int>≈ AtomicInteger
// ═══════════════════════════════════════════════════════════════
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <chrono>
#include <windows.h>

std::mutex printMtx;  // 保护 cout，防止输出乱序

// ── 场景1：基本线程创建和 join ────────────────────────────────
void demo_basic_thread() {
    std::cout << "\n=== 场景1：基本线程创建 ===" << std::endl;

    // lambda 作为线程函数
    std::thread t1([] {
        std::lock_guard<std::mutex> lg(printMtx);
        std::cout << "  t1 线程 id=" << std::this_thread::get_id() << std::endl;
    });

    // 带参数的线程
    std::thread t2([](int id, const std::string& msg) {
        std::lock_guard<std::mutex> lg(printMtx);
        std::cout << "  t2 id=" << id << " msg=" << msg << std::endl;
    }, 42, "hello from thread");

    t1.join();  // 等待 t1 完成，必须 join 或 detach
    t2.join();  // 等待 t2 完成

    std::cout << "  主线程：t1 和 t2 都完成了" << std::endl;
}

// ── 场景2：数据竞争 vs mutex 保护 ────────────────────────────
void demo_mutex() {
    std::cout << "\n=== 场景2：数据竞争 vs mutex ===" << std::endl;

    // 没有保护：数据竞争，结果不确定
    int unsafe_counter = 0;
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < 5; i++) {
            threads.emplace_back([&unsafe_counter] {
                for (int j = 0; j < 1000; j++)
                    unsafe_counter++;  // 数据竞争！
            });
        }
        for (auto& t : threads) t.join();
    }
    std::cout << "  无保护 counter（应为5000，实际）: " << unsafe_counter << std::endl;

    // mutex 保护：结果确定
    int safe_counter = 0;
    std::mutex mtx;
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < 5; i++) {
            threads.emplace_back([&safe_counter, &mtx] {
                for (int j = 0; j < 1000; j++) {
                    std::lock_guard<std::mutex> lg(mtx);  // 加锁
                    safe_counter++;
                }  // 离开作用域自动解锁
            });
        }
        for (auto& t : threads) t.join();
    }
    std::cout << "  mutex 保护 counter（应为5000）: " << safe_counter << std::endl;
}

// ── 场景3：atomic 原子操作（比 mutex 轻量）───────────────────
void demo_atomic() {
    std::cout << "\n=== 场景3：atomic 原子操作 ===" << std::endl;

    std::atomic<int> counter{0};
    std::vector<std::thread> threads;

    for (int i = 0; i < 5; i++) {
        threads.emplace_back([&counter] {
            for (int j = 0; j < 1000; j++)
                counter++;  // 原子操作，线程安全，无需 mutex
        });
    }
    for (auto& t : threads) t.join();
    std::cout << "  atomic counter（应为5000）: " << counter.load() << std::endl;
}

// ── 场景5：memory_order 内存顺序 ─────────────────────────────
void demo_memory_order() {
    std::cout << "\n=== 场景5：memory_order 内存顺序 ===" << std::endl;

    // ─────────────────────────────────────────────────────────
    // 为什么需要 memory_order：
    //   CPU 和编译器为了性能会乱序执行指令
    //   单线程没问题，多线程下一个线程可能看到另一个线程的"中间状态"
    //
    // 六种顺序（从严到松）：
    //   seq_cst   全局顺序一致，最安全最慢，默认值
    //   acq_rel   acquire + release 合并，用于读改写操作
    //   release   写操作：之前的写不会重排到这条之后
    //   acquire   读操作：之后的读不会重排到这条之前
    //   consume   比 acquire 稍弱，实践中编译器当 acquire 处理，不用
    //   relaxed   只保证原子性，不保证顺序，最快
    //
    // 选择原则：
    //   纯计数器（不关心顺序）      → relaxed
    //   生产者写数据，消费者读数据  → release / acquire 配对
    //   读改写（CAS / fetch_add）   → acq_rel
    //   不确定用什么               → 默认 seq_cst（安全但慢）
    // ─────────────────────────────────────────────────────────

    // relaxed：纯计数器，只关心原子性，不关心顺序
    std::cout << "\n--- relaxed：纯计数器 ---" << std::endl;
    {
        std::atomic<int> hits{0};
        std::vector<std::thread> threads;
        for (int i = 0; i < 4; i++) {
            threads.emplace_back([&hits] {
                for (int j = 0; j < 1000; j++)
                    hits.fetch_add(1, std::memory_order_relaxed);  // 只要原子，不管顺序
            });
        }
        for (auto& t : threads) t.join();
        std::cout << "  hits（应为4000）: " << hits.load(std::memory_order_relaxed) << std::endl;
    }

    // release / acquire 配对：生产者/消费者
    // 保证消费者读到 ready=true 时，data 一定已经写好了
    std::cout << "\n--- release/acquire：生产者/消费者 ---" << std::endl;
    {
        std::atomic<bool> ready{false};
        int data = 0;

        std::thread producer([&] {
            data = 42;                                        // 1. 先写数据
            ready.store(true, std::memory_order_release);    // 2. release：1 不会重排到这之后
            std::lock_guard<std::mutex> lg(printMtx);
            std::cout << "  生产者：data=" << data << " ready=true" << std::endl;
        });

        std::thread consumer([&] {
            while (!ready.load(std::memory_order_acquire));  // 3. acquire：4 不会重排到这之前
            int val = data;                                  // 4. 保证能看到 data=42
            std::lock_guard<std::mutex> lg(printMtx);
            std::cout << "  消费者：读到 data=" << val << "（保证是42）" << std::endl;
        });

        producer.join();
        consumer.join();
    }

    // seq_cst（默认）：最严格，所有线程看到的操作顺序完全一致
    std::cout << "\n--- seq_cst（默认）：最严格 ---" << std::endl;
    {
        std::atomic<int> x{0}, y{0};
        int r1 = 0, r2 = 0;

        std::thread t1([&] {
            x.store(1);                    // seq_cst，默认
            r1 = y.load();                 // seq_cst，默认
        });
        std::thread t2([&] {
            y.store(1);
            r2 = x.load();
        });
        t1.join(); t2.join();

        // seq_cst 保证：r1=0 和 r2=0 不会同时发生
        // 即不可能两个线程都没看到对方的写入
        std::lock_guard<std::mutex> lg(printMtx);
        std::cout << "  r1=" << r1 << " r2=" << r2
                  << "（seq_cst 保证不会都是0）" << std::endl;
    }

    std::cout << "\n--- 性能对比（同样的计数器）---" << std::endl;
    std::cout << "  relaxed  最快：只保证原子性，CPU 可以自由重排" << std::endl;
    std::cout << "  acquire/release 中等：只在必要点插入内存屏障" << std::endl;
    std::cout << "  seq_cst  最慢：每次操作都插入完整内存屏障" << std::endl;
    std::cout << "  实践：普通业务用默认 seq_cst，性能敏感场景再考虑优化" << std::endl;
}
void demo_download_simulator() {
    std::cout << "\n=== 场景4：多线程下载模拟器 ===" << std::endl;

    const int FILE_COUNT = 4;
    const int FILE_SIZE = 5;  // 每个文件5个块
    std::atomic<int> totalDone{0};
    std::vector<std::thread> threads;

    for (int i = 0; i < FILE_COUNT; i++) {
        threads.emplace_back([i, FILE_SIZE, &totalDone] {
            for (int block = 0; block < FILE_SIZE; block++) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                totalDone++;
                std::lock_guard<std::mutex> lg(printMtx);
                std::cout << "  文件" << i << " 块" << block << " 完成，总进度="
                          << totalDone << "/" << FILE_COUNT * FILE_SIZE << std::endl;
            }
        });
    }

    for (auto& t : threads) t.join();
    std::cout << "  所有文件下载完成！" << std::endl;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    std::cout << "主线程 id=" << std::this_thread::get_id() << std::endl;

    demo_basic_thread();
    demo_mutex();
    demo_atomic();
    demo_memory_order();
    demo_download_simulator();

    return 0;
}
