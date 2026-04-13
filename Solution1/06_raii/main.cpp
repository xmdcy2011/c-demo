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
#include <fstream>
#include <string>
#include <mutex>
#include <memory>
#include <thread>
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

    // ─────────────────────────────────────────────────────────
    // ifstream 的 RAII 实现原理（和 FileHandle 本质一样）
    //
    //   class ifstream {
    //       FILE* fp = nullptr;
    //   public:
    //       ifstream(const char* path) { fp = fopen(path, "r"); }
    //       ~ifstream()               { if (fp) fclose(fp); }   // 自动关闭
    //
    //       ifstream(const ifstream&) = delete;           // 禁止拷贝（double-close）
    //       ifstream(ifstream&& o) noexcept {             // 允许移动（转移所有权）
    //           fp = o.fp; o.fp = nullptr;
    //       }
    //   };
    //
    //   区别：
    //     FileHandle  → 用 FILE* + fopen/fclose，手动封装
    //     ifstream    → 同样封装 FILE*，但额外提供 >> / getline 操作符
    //     ifstream    → 支持移动语义（可从函数返回），FileHandle 没加
    //
    //   标准库里的 RAII 全家桶（同一个模式）：
    //     std::ifstream / ofstream   → 文件句柄
    //     std::unique_ptr            → 堆内存
    //     std::lock_guard            → mutex 锁
    //     std::shared_lock           → 读写锁
    // ─────────────────────────────────────────────────────────
    std::cout << "\n=== ifstream RAII 演示 ===" << std::endl;
    {
        // 先用 FileHandle 写一个文件
        FileHandle writer("demo_ifstream.txt", "w");
        writer.writeLine("line one");
        writer.writeLine("line two");
        writer.writeLine("line three");
    }  // → [FileHandle] 自动关闭，文件写入完成

    {
        // 用 ifstream 读取（RAII，离开作用域自动关闭）
        std::ifstream ifs("demo_ifstream.txt");
        if (!ifs.is_open()) {
            std::cout << "文件打开失败" << std::endl;
        } else {
            std::cout << "ifstream 打开成功，逐行读取：" << std::endl;
            std::string line;
            while (std::getline(ifs, line)) {
                std::cout << "  " << line << std::endl;
            }
        }
    }  // ifs 析构 → 自动 fclose，不需要手动关闭

    std::cout << "ifstream 已自动关闭（析构函数调用）" << std::endl;

    // 对比 Java try-with-resources：
    //   Java:  try (BufferedReader br = new BufferedReader(...)) { ... }
    //   C++:   { std::ifstream ifs("file.txt"); ... }  // 大括号就够了

    // ─────────────────────────────────────────────────────────
    // lock_guard vs unique_lock
    //
    //   lock_guard：简单场景，锁住整个作用域，不能手动解锁
    //     class lock_guard {
    //         mutex& mtx;
    //     public:
    //         lock_guard(mutex& m) : mtx(m) { mtx.lock(); }
    //         ~lock_guard()                 { mtx.unlock(); }
    //         lock_guard(const lock_guard&) = delete;  // 禁止拷贝
    //     };
    //
    //   unique_lock：更灵活，可以手动 unlock/lock，配合 condition_variable
    //     额外支持：延迟加锁、超时加锁、移动转移所有权
    //
    //   scoped_lock（C++17）：同时锁多个 mutex，内部用死锁避免算法
    //     std::scoped_lock lk(mtx1, mtx2);  // 同时锁两个，析构同时解
    //
    //   shared_lock（C++17）：读锁，允许多个线程同时持有
    //     配合 shared_mutex 实现读多写少场景
    // ─────────────────────────────────────────────────────────
    std::cout << "\n=== lock_guard vs unique_lock ===" << std::endl;
    {
        std::mutex mtx;

        // lock_guard：锁住整个作用域
        std::cout << "lock_guard：" << std::endl;
        {
            std::lock_guard<std::mutex> lg(mtx);  // 构造 → 加锁
            std::cout << "  临界区内（已加锁）" << std::endl;
            // lg.unlock();  // ❌ 编译错误，lock_guard 没有 unlock
        }  // 析构 → 自动解锁
        std::cout << "  离开作用域，自动解锁" << std::endl;

        // unique_lock：可以手动控制
        std::cout << "unique_lock：" << std::endl;
        {
            std::unique_lock<std::mutex> ul(mtx);  // 构造 → 加锁
            std::cout << "  加锁后" << std::endl;
            ul.unlock();                            // 手动解锁
            std::cout << "  手动解锁后（可以做不需要锁的事）" << std::endl;
            ul.lock();                              // 再次加锁
            std::cout << "  再次加锁" << std::endl;
        }  // 析构 → 自动解锁
        std::cout << "  离开作用域，自动解锁" << std::endl;
    }

    // ─────────────────────────────────────────────────────────
    // unique_ptr / shared_ptr RAII
    //
    //   unique_ptr：
    //     class unique_ptr {
    //         T* ptr;
    //     public:
    //         unique_ptr(T* p) : ptr(p) {}
    //         ~unique_ptr() { delete ptr; }          // 自动 delete
    //         unique_ptr(const unique_ptr&) = delete; // 禁止拷贝
    //         unique_ptr(unique_ptr&& o) noexcept { ptr = o.ptr; o.ptr = nullptr; }
    //     };
    //
    //   shared_ptr：内部多一个引用计数块（control block）
    //     拷贝时计数+1，析构时计数-1，归零时才真正 delete
    // ─────────────────────────────────────────────────────────
    std::cout << "\n=== unique_ptr / shared_ptr RAII ===" << std::endl;
    {
        struct Res {
            std::string name;
            Res(const std::string& n) : name(n) { std::cout << "  [Res] 构造 " << name << std::endl; }
            ~Res() { std::cout << "  [Res] 析构 " << name << std::endl; }
        };

        std::cout << "unique_ptr：" << std::endl;
        {
            auto p = std::make_unique<Res>("unique");  // 构造 → 持有指针
            std::cout << "  持有中..." << std::endl;
        }  // 析构 → 自动 delete Res

        std::cout << "shared_ptr：" << std::endl;
        {
            auto p1 = std::make_shared<Res>("shared");
            std::cout << "  引用计数=" << p1.use_count() << std::endl;  // 1
            {
                auto p2 = p1;  // 拷贝，计数+1
                std::cout << "  引用计数=" << p1.use_count() << std::endl;  // 2
            }  // p2 析构，计数-1
            std::cout << "  引用计数=" << p1.use_count() << std::endl;  // 1
        }  // p1 析构，计数归零 → 自动 delete Res
    }

    // ─────────────────────────────────────────────────────────
    // thread vs jthread
    //
    //   std::thread：析构时如果没 join/detach 会直接 terminate（程序崩溃）
    //     不是真正的 RAII，需要手动处理
    //
    //   std::jthread（C++20）：析构时自动 join，才是真正的 RAII
    //     class jthread {
    //         thread t;
    //     public:
    //         ~jthread() { if (t.joinable()) t.join(); }  // 自动 join
    //     };
    // ─────────────────────────────────────────────────────────
    std::cout << "\n=== thread vs jthread ===" << std::endl;
    {
        std::cout << "std::thread 必须手动 join/detach：" << std::endl;
        std::thread t([] {
            std::cout << "  线程执行中" << std::endl;
        });
        t.join();  // 必须手动 join，否则析构时 terminate
        std::cout << "  手动 join 完成" << std::endl;

        // std::jthread（C++20）：
        // std::jthread jt([] { ... });
        // }  // 析构自动 join，不需要手动处理
        std::cout << "  jthread（C++20）析构时自动 join，真正的 RAII" << std::endl;
    }

    return 0;
}
