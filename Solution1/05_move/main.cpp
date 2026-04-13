// UTF-8
// ═══════════════════════════════════════════════════════════════
// 05 移动语义
// ═══════════════════════════════════════════════════════════════
//
// 左值：有名字、有地址的对象（Buffer a("x") → a 是左值）
// 右值：临时对象、无名字（Buffer("x") → 临时对象是右值）
//
// 默认拷贝是浅拷贝，有裸指针必须手写五个方法（五法则）：
//   拷贝构造 / 拷贝赋值 / 析构 / 移动构造 / 移动赋值
//
// 常用场景 → 触发的方法：
//   Buffer b = a;                  → [拷贝构造]  a 是左值
//   Buffer b = std::move(a);       → [移动构造]  std::move 把 a 变右值
//   Buffer b = make_buf();         → [移动构造] 或 RVO
//   d = std::move(b);              → [移动赋值]  d 已存在，先释放再接管
//   vec.push_back(x);              → [拷贝构造]  x 是左值
//   vec.push_back(std::move(x));   → [移动构造]  x 不再需要
//   vec.push_back(Buffer("t"));    → [移动构造]  临时对象是右值
//
// std::move 本身不移动任何东西，只是把左值强制转成右值引用
// 真正"偷走"内存的是移动构造函数里的：
//   data = other.data; other.data = nullptr;
//
// noexcept：vector 扩容时，没有 noexcept 的移动构造会退化成拷贝
//
// Java 对照：Java 没有移动语义，对象赋值永远是引用共享
// ═══════════════════════════════════════════════════════════════
#include "Buffer.h"
#include <windows.h>
#include <vector>

void demo_rvalue_reference();  // 前向声明

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "=== 拷贝 vs 移动对比 ===" << std::endl;

    Buffer a("hello");
    a.print("a");

    std::cout << "\n--- 拷贝构造（分配新内存）---" << std::endl;
    Buffer b = a;           // 拷贝，a 不变
    a.print("a after copy");
    b.print("b");

    std::cout << "\n--- 移动构造（零拷贝，偷走指针）---" << std::endl;
    Buffer c = std::move(a);  // a 的内存转移给 c，a 变空
    a.print("a after move");  // null
    c.print("c");

    std::cout << "\n--- 移动赋值 ---" << std::endl;
    Buffer d("world");
    d = std::move(b);
    b.print("b after move");  // null
    d.print("d");

    std::cout << "\n=== 析构 ===" << std::endl;

    // 调用右值引用详解演示
    demo_rvalue_reference();

    return 0;
}


// ═══════════════════════════════════════════════════════════════
// 场景化演示：左值 / 右值 / 移动语义
// ═══════════════════════════════════════════════════════════════
void demo_rvalue_reference() {
    std::cout << "\n\n=== 场景化演示 ===" << std::endl;

    // ─────────────────────────────────────────────────────────
    // 场景 0：为什么不能用默认拷贝（浅拷贝的危险）
    //   编译器默认拷贝构造 = 逐成员复制
    //   对指针只复制地址，两个对象共享同一块内存
    //   析构时 double-free → 崩溃
    //   所以 Buffer 必须自己实现深拷贝
    // ─────────────────────────────────────────────────────────
    std::cout << "\n【场景0】为什么有裸指针必须自己写拷贝构造" << std::endl;
    std::cout << "  默认拷贝 = 浅拷贝：b.data = a.data（两个指针指向同一块内存）" << std::endl;
    std::cout << "  析构 b → delete[] b.data" << std::endl;
    std::cout << "  析构 a → delete[] a.data  ← 💥 double-free，崩溃" << std::endl;
    std::cout << "  Buffer 自己实现了深拷贝（new + memcpy），所以下面是安全的：" << std::endl;
    {
        Buffer a("safe");           // → [构造]
        Buffer b = a;               // → [拷贝构造]  深拷贝，b 有自己独立的内存
        std::cout << "  a.data addr=" << (void*)a.data
                  << "  b.data addr=" << (void*)b.data
                  << "  （地址不同，各自独立）" << std::endl;
    } // → [析构] b（释放自己的内存），[析构] a（释放自己的内存），安全


    //   Buffer a("hello") → 触发 [构造]
    //   Buffer b = a      → 触发 [拷贝构造]，a 还活着，b 是独立副本
    // ─────────────────────────────────────────────────────────
    std::cout << "\n【场景1】把一个 Buffer 复制给另一个变量" << std::endl;
    std::cout << "  代码：Buffer a(\"hello\"); Buffer b = a;" << std::endl;
    {
        Buffer a("hello");          // → [构造]
        Buffer& b = a;               // → [拷贝构造]  a 有名字，是左值，只能拷贝
        Buffer* pa = &a;
        Buffer** ppa = &pa;
        Buffer* pb = &b;
        Buffer** ppb = &pb;
        std::cout << "pa:" << pa << " ppa:" << ppa << std::endl;
        std::cout << "pb:" << pb  << " ppb:" << ppb << std::endl;
        a.print("a 还在");
        b.print("b 是副本");
    } // → [析构] b, [析构] a

    // ─────────────────────────────────────────────────────────
    // 场景 2：函数返回临时对象
    //   工厂函数返回一个 Buffer，调用方接收
    //   临时对象没有名字，是右值 → 触发 [移动构造]（或 RVO 直接构造）
    // ─────────────────────────────────────────────────────────
    std::cout << "\n【场景2】函数返回临时对象，调用方接收" << std::endl;
    std::cout << "  代码：Buffer c = make_buffer();" << std::endl;
    {
        auto make_buffer = []() -> Buffer {
            return Buffer("from_factory");  // → [构造] 临时对象（右值）
        };
        Buffer c = make_buffer();  // → [移动构造] 或 RVO 直接构造，不会拷贝
        c.print("c 接收到工厂结果");
    } // → [析构] c

    // ─────────────────────────────────────────────────────────
    // 场景 3：明确放弃一个变量，把资源转移走
    //   std::move(a) 把左值 a 变成右值 → 触发 [移动构造]
    //   a 的内存被"偷走"，a 变空
    // ─────────────────────────────────────────────────────────
    std::cout << "\n【场景3】明确不再需要某个变量，把它的资源转移走" << std::endl;
    std::cout << "  代码：Buffer d = std::move(a);  // a 不再使用" << std::endl;
    {
        Buffer a("big_data");       // → [构造]
        a.print("转移前 a");

        Buffer d = std::move(a);    // → [移动构造]  std::move 把 a 变成右值
        a.print("转移后 a");        // a.data = null，内存已被 d 接管
        d.print("d 接管了内存");
    } // → [析构] d（释放内存），[析构] a（data=null，什么都不做）

    // ─────────────────────────────────────────────────────────
    // 场景 4：容器 push_back
    //   push_back(x)          → [拷贝构造]  x 还要用，不能动它
    //   push_back(std::move(x)) → [移动构造]  x 不再需要，直接偷走
    // ─────────────────────────────────────────────────────────
    std::cout << "\n【场景4】往容器里放 Buffer" << std::endl;
    {
        std::vector<Buffer> vec;
        vec.reserve(4);  // 预留空间，避免扩容干扰输出

        Buffer x("item_x");         // → [构造]
        std::cout << "  push_back(x)：x 还要用，触发拷贝" << std::endl;
        vec.push_back(x);           // → [拷贝构造]  x 是左值

        std::cout << "  push_back(move(x))：x 不再需要，触发移动" << std::endl;
        vec.push_back(std::move(x)); // → [移动构造]  x 变成右值
        x.print("x 被移走后");

        std::cout << "  push_back(临时对象)：临时对象是右值，触发移动" << std::endl;
        vec.push_back(Buffer("temp_item")); // → [构造] 临时 + [移动构造] 进容器
    } // → [析构] vec 里所有元素，[析构] x（null）

    // ─────────────────────────────────────────────────────────
    // 场景 5：移动赋值（已有对象接管资源）
    //   d = std::move(b) → 触发 [移动赋值]
    //   d 先释放自己的旧内存，再接管 b 的内存
    // ─────────────────────────────────────────────────────────
    std::cout << "\n【场景5】已有对象接管另一个对象的资源" << std::endl;
    std::cout << "  代码：d = std::move(b);  // d 原来有数据，b 的资源转给 d" << std::endl;
    {
        Buffer b("source");         // → [构造]
        Buffer d("old_data");       // → [构造]
        b.print("转移前 b");
        d.print("转移前 d");

        d = std::move(b);           // → [移动赋值]  d 先 delete 自己，再接管 b
        b.print("转移后 b");        // null
        d.print("转移后 d");        // 拿到了 b 的数据
    } // → [析构] d（释放内存），[析构] b（null）

    // ─────────────────────────────────────────────────────────
    // 场景 6：为什么 string / vector / unique_ptr 不需要手写拷贝
    //
    //   这些类型自己实现了深拷贝，编译器逐成员调用它们各自的拷贝构造
    //
    //   内存布局（对象本身在栈，实际数据在堆）：
    //
    //   void foo() {
    //       std::vector<int> scores = {1, 2, 3};
    //       std::string name = "hello_long_str";
    //       std::unique_ptr<int> p = std::make_unique<int>(42);
    //   }
    //
    //   栈上（函数栈帧）：
    //   ┌─────────────────────────────────┐
    //   │ scores │ data ptr → ──────────┐ │  ← vector 对象本身在栈（3个字段）
    //   │        │ size = 3             │ │
    //   │        │ capacity = 3         │ │
    //   ├─────────────────────────────────┤
    //   │ name   │ data ptr → ──────────┼─┼─┐  ← string 对象本身在栈
    //   │        │ size = 14            │ │ │
    //   │        │ capacity = ...       │ │ │
    //   ├─────────────────────────────────┤ │ │
    //   │ p      │ raw ptr → ───────────┼─┼─┼─┐  ← unique_ptr 对象本身在栈
    //   └─────────────────────────────────┘ │ │ │
    //                                       │ │ │
    //   堆上：                               │ │ │
    //   ┌──────────┐ ←──────────────────────┘ │ │
    //   │ 1, 2, 3  │  scores 的数据            │ │
    //   └──────────┘                          │ │
    //   ┌──────────────┐ ←────────────────────┘ │
    //   │"hello_long.." │  name 的字符数据        │
    //   └──────────────┘                        │
    //   ┌──────────┐ ←──────────────────────────┘
    //   │    42    │  p 指向的 int
    //   └──────────┘
    //
    //   注意：string 有 SSO（Small String Optimization）
    //         短字符串（≤15字符）直接存在栈上的控制结构里，不分配堆
    //         所以短字符串的 data ptr 指向栈，长字符串才指向堆
    //
    //   string 内部简化实现（和 Buffer 本质一样，只是帮你写好了）：
    //     class string {
    //         char* data;      ← 裸指针，和 Buffer 一样
    //         size_t size;
    //         size_t capacity;
    //     public:
    //         string(const string& o) { data = new char[...]; memcpy(...); }  // 深拷贝
    //         string(string&& o) noexcept { data = o.data; o.data = nullptr; } // 移动
    //         ~string() { delete[] data; }
    //     };
    // ─────────────────────────────────────────────────────────
    std::cout << "\n【场景6】string / vector 自己管理深拷贝" << std::endl;
    {
        // string 内部也有 char* data，但它自己实现了深拷贝构造
        // 所以包含 string 的类，编译器默认生成的拷贝构造就是正确的
        struct Person {
            std::string name;       // 对象本身在栈，字符数据在堆（长字符串）
            std::vector<int> scores; // 对象本身在栈，int数组在堆
            int age;                // 直接在栈上
        };

        Person p1{"Alice_LongName_NoSSO", {90, 85, 92}, 25};
        Person p2 = p1;  // 编译器默认拷贝构造：
                         //   name(p1.name)   → string::拷贝构造 → new char[] + memcpy
                         //   scores(p1.scores) → vector::拷贝构造 → new int[] + copy
                         //   age = p1.age    → 直接复制 int

        std::cout << "  p1.name addr=" << (void*)p1.name.data()
                  << "  p2.name addr=" << (void*)p2.name.data()
                  << "  （地址不同，各自独立）" << std::endl;

        p2.name = "Bob";  // 修改 p2 不影响 p1
        std::cout << "  修改 p2.name 后，p1.name=" << p1.name
                  << "  p2.name=" << p2.name << std::endl;

        // 对比：Buffer 里是裸指针 char*，不会自己深拷贝
        // string 里也是 char*，但 string 帮你写好了深拷贝
        // 本质一样，区别是谁来负责
        std::cout << "  结论：string/vector 自己实现了深拷贝，包含它们的类不需要手写" << std::endl;
    }
}
