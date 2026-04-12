// UTF-8
#include <memory>
#include <vector>
#include <iostream>
#include <windows.h>

struct Node {
    int32_t val;
    Node(int32_t v) : val(v) {
        std::cout << "[Node] 构造 val=" << v << std::endl;
    }
    ~Node() {
        std::cout << "[Node] 析构 val=" << val << std::endl;
    }
};

void demo_unique_ptr() {
    std::cout << "=== unique_ptr（独占所有权）===" << std::endl;
    auto p1 = std::make_unique<Node>(1);
    // auto p2 = p1;  // 编译错误，不能拷贝
    auto p2 = std::move(p1);  // 转移所有权
    std::cout << "p1 是否为空: " << (p1 == nullptr ? "是" : "否") << std::endl;
    std::cout << "p2->val=" << p2->val << std::endl;
}  // p2 析构，Node 自动释放

void demo_shared_ptr() {
    std::cout << "\n=== shared_ptr（引用计数）===" << std::endl;
    auto p1 = std::make_shared<Node>(2);
    std::cout << "引用计数: " << p1.use_count() << std::endl;  // 1

    {
        auto p2 = p1;
        std::cout << "引用计数: " << p1.use_count() << std::endl;  // 2
        auto p3 = p1;
        std::cout << "引用计数: " << p1.use_count() << std::endl;  // 3
    }  // p2, p3 析构
    std::cout << "引用计数: " << p1.use_count() << std::endl;  // 1
}  // p1 析构，计数归0，Node 释放

void demo_weak_ptr() {
    std::cout << "\n=== weak_ptr（不增加引用计数）===" << std::endl;
    std::weak_ptr<Node> wp;
    {
        auto sp = std::make_shared<Node>(3);
        wp = sp;
        std::cout << "sp 引用计数: " << sp.use_count() << std::endl;  // 1，weak_ptr 不增加
        std::cout << "wp.expired(): " << wp.expired() << std::endl;   // false

        if (auto locked = wp.lock()) {  // 安全访问：先 lock 升级为 shared_ptr
            std::cout << "locked->val=" << locked->val << std::endl;
        }
    }  // sp 析构，Node 释放
    std::cout << "wp.expired(): " << wp.expired() << std::endl;  // true
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    demo_unique_ptr();
    demo_shared_ptr();
    demo_weak_ptr();
    return 0;
}
