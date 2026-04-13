// UTF-8
// ═══════════════════════════════════════════════════════════════
// 01b const 的各种位置
// ═══════════════════════════════════════════════════════════════
//
// 记忆技巧：const 在 * 左边 → 值不能改；const 在 * 右边 → 指向不能改
//   int* p              普通指针：指向可变，值可变
//   const int* p        值不可改，指向可变
//   int* const p        指向不可变，值可改
//   const int* const p  都不可变
//
// const 成员函数：不能修改任何成员变量（除了 mutable）
//   const 对象只能调用 const 成员函数
//   mutable 成员变量在 const 函数里也能改（常用于缓存/计数器）
//
// {} vs () vs = 初始化语法：
//   T x = val;  拷贝初始化，不能用于 atomic/unique_ptr 等禁拷贝类型
//   T x(val);   直接初始化，允许窄化转换（double→int 悄悄截断）
//   T x{val};   列表初始化，禁止窄化转换，推荐统一用这个
//
// Java 对照：Java 用 final 修饰变量，没有 const 成员函数的概念
//            C++ 的 const 更细粒度，能精确控制"谁能改什么"
// ═══════════════════════════════════════════════════════════════
// C++ const 全面演示 + 类初始化方式
#include <iostream>
#include <string>
#include <windows.h>

// ═══════════════════════════════════════════════════════════════
// 1. const 修饰变量
// ═══════════════════════════════════════════════════════════════
// const 修饰的变量必须在声明时初始化，之后不能修改
// 相当于 Java 的 final
void demo_const_var() {
    const int32_t x = 10;  // const 变量必须在声明时初始化
    // x = 20;              // 编译错误：不能修改 const 变量
    std::cout << "const 变量 x = " << x << std::endl;
}

// ═══════════════════════════════════════════════════════════════
// 2. const 修饰指针（从右往左读）
// ═══════════════════════════════════════════════════════════════
// 记忆技巧：const 在 * 左边 → 值不能改
//           const 在 * 右边 → 指向不能改
void demo_const_pointer() {
    int32_t a = 1, b = 2;

    // 普通指针：指向可变，值也可变
    int32_t* p1 = &a;
    *p1 = 9;    // OK：可以改值
    p1 = &b;    // OK：可以改指向

    // 指向常量：值不可改，指向可变
    const int32_t* p2 = &a;
    // *p2 = 9;    // 编译错误：值不可改
    p2 = &b;    // OK：可以改指向

    // 常量指针：指向不可变，值可改
    int32_t* const p3 = &a;
    *p3 = 9;    // OK：可以改值
    // p3 = &b;   // 编译错误：指向不可改

    // 指向常量的常量指针：都不可变
    const int32_t* const p4 = &a;
    // *p4 = 9;   // 编译错误
    // p4 = &b;   // 编译错误

    std::cout << "p1=" << *p1 << " p2=" << *p2
              << " p3=" << *p3 << " p4=" << *p4 << std::endl;
}

// ═══════════════════════════════════════════════════════════════
// 3. const 引用传参（只读，不拷贝）
// ═══════════════════════════════════════════════════════════════
// 用 const 引用传参：既不拷贝对象，又能保证不修改原对象
// 等价 Java 的 final String s，但 Java 是引用传递
void printLength(const std::string& s) {
    std::cout << "字符串长度: " << s.size() << std::endl;
    // s = "modified";  // 编译错误：const 引用不能修改
}

// ═══════════════════════════════════════════════════════════════
// 4. const 成员函数
// ═══════════════════════════════════════════════════════════════
// const 成员函数承诺不修改任何成员变量（除了 mutable）
// const 对象只能调用 const 成员函数
class Counter {
    int32_t count = 0;                    // 普通成员变量
    mutable int32_t hitCount = 0;         // mutable：即使在 const 函数里也能改
public:
    // const 成员函数：不能修改成员变量
    int32_t get() const {
        hitCount++;  // 合法！因为 hitCount 是 mutable
        return count;
    }
    // 非 const 成员函数：可以修改成员变量
    void increment() { count++; }
    int32_t getHitCount() const { return hitCount; }
};

// ═══════════════════════════════════════════════════════════════
// 5. 返回 const 引用
// ═══════════════════════════════════════════════════════════════
// 返回 const 引用：调用方不能通过返回值修改内部数据
// 保护内部数据不被意外修改
class Box {
    std::string label;
public:
    // 初始化列表：直接初始化成员，比构造体内赋值更高效
    Box(const std::string& l) : label(l) {}
    
    // 返回 const 引用：外部只能读取，不能修改内部 label
    const std::string& getLabel() const { return label; }
};

// ═══════════════════════════════════════════════════════════════
// 6. 类的初始化方式
// ═══════════════════════════════════════════════════════════════

// 6.1 默认成员初始化器（C++11）
//    在成员声明处直接给默认值，构造时自动使用
//    优点：代码简洁，清晰展示默认值
class Person {
public:
    int32_t age = 0;              // 默认值 0
    std::string name = "unknown"; // 默认值 "unknown"
    Person() = default;           // 显式要求编译器生成默认构造，使用默认值
};

// 6.2 构造体内赋值（不推荐）
//    先用默认值初始化成员，再在构造体内赋值
//    缺点：多走一步，性能稍差
class Person2 {
public:
    int32_t age;       // 默认构造后值为 0
    std::string name;  // 默认构造后值为 ""
    Person2(int32_t a, const std::string& n) {
        age = a;       // 先默认构造 age=0，再赋值
        name = n;      // 先默认构造 name=""，再赋值
    }
};

// 6.3 初始化列表（推荐）
//    在构造函数的参数列表后用 : 初始化成员
//    优点：直接初始化，不经过默认构造，性能最好
class Person3 {
public:
    int32_t age;
    std::string name;
    Person3(int32_t a, const std::string& n)
        : age(a), name(n) {}  // 直接初始化，不经过默认构造
};

// 6.4 委托构造（C++11）
//    一个构造函数委托给另一个构造函数完成初始化
//    优点：避免代码重复
class Person4 {
public:
    int32_t age = 0;
    std::string name = "unknown";
    
    // 无参构造委托给有参构造
    Person4() : Person4(0, "unknown") {}
    
    // 有参构造
    Person4(int32_t a, const std::string& n)
        : age(a), name(n) {}
};

// 6.5 = default / = delete
//    = default：显式要求编译器生成默认实现
//    = delete：显式禁用某个函数，调用会导致编译错误
class NonCopyable {
public:
    NonCopyable() = default;  // 使用默认构造
    
    // 禁用拷贝构造：禁止 NonCopyable a = b;
    NonCopyable(const NonCopyable&) = delete;
    
    // 禁用拷贝赋值：禁止 a = b;
    NonCopyable& operator=(const NonCopyable&) = delete;
    
    // 移动构造和移动赋值也可以禁用（如果需要）
    // NonCopyable(NonCopyable&&) = delete;
    // NonCopyable& operator=(NonCopyable&&) = delete;
};

// 6.6 构造函数参数默认值
//    在参数声明处给默认值，调用时可以省略参数
//    等价 Dart 的 Person({this.age = 0, this.name = "unknown"})
//    区别：C++ 只能按位置传参，没有命名参数
class Person5 {
public:
    int32_t age;
    std::string name;
    
    // 参数默认值：调用时可以不传，用默认值
    Person5(int32_t a = 0, const std::string& n = "unknown")
        : age(a), name(n) {}
};

void demo_class_init() {
    std::cout << "\n=== 6. 类初始化方式 ===" << std::endl;

    // 6.1 默认成员初始化器
    Person p1;
    std::cout << "Person 默认: age=" << p1.age << " name=" << p1.name << std::endl;

    // 6.2 构造体内赋值
    Person2 p2(25, "Alice");
    std::cout << "Person2: age=" << p2.age << " name=" << p2.name << std::endl;

    // 6.3 初始化列表（推荐）
    Person3 p3(30, "Bob");
    std::cout << "Person3: age=" << p3.age << " name=" << p3.name << std::endl;

    // 6.4 委托构造
    Person4 p4;       // 调用无参构造，实际委托给有参构造
    Person4 p4b(20, "Carol");
    std::cout << "Person4: age=" << p4.age << " name=" << p4.name << std::endl;

    // 6.5 = delete（编译错误，无法演示）
    // NonCopyable a;
    // NonCopyable b = a;  // 编译错误！拷贝构造被禁用
    // a = b;              // 编译错误！拷贝赋值被禁用
    std::cout << "NonCopyable 类已定义，拷贝被禁止" << std::endl;

    // 6.6 构造函数参数默认值
    Person5 p5a;              // age=0, name="unknown"
    Person5 p5b(25);          // age=25, name="unknown"
    Person5 p5c(30, "Dave");  // age=30, name="Dave"
    std::cout << "Person5: p5a age=" << p5a.age << " name=" << p5a.name << std::endl;
    std::cout << "Person5: p5b age=" << p5b.age << " name=" << p5b.name << std::endl;
    std::cout << "Person5: p5c age=" << p5c.age << " name=" << p5c.name << std::endl;

    // ─────────────────────────────────────────────────────────
    // 6.7 {} vs () vs = 初始化语法对比
    // ─────────────────────────────────────────────────────────
    std::cout << "\n=== 6.7 {} vs () vs = 初始化语法 ===" << std::endl;

    // 普通类型
    int a1 = 10;    // 拷贝初始化
    int a2(10);     // 直接初始化
    int a3{10};     // 列表初始化（推荐，最严格）
    std::cout << "int: a1=" << a1 << " a2=" << a2 << " a3=" << a3 << std::endl;

    // {} 防止窄化转换（编译错误）
    // int b{3.14};   // 编译错误！double → int 是窄化转换
    int b(3.14);     // 编译通过，但截断为 3（可能不是你想要的）
    int c = 3.14;    // 编译通过，但截断为 3
    std::cout << "窄化: b(3.14)=" << b << " c=3.14=" << c << std::endl;

    // std::string
    std::string s1 = "hello";   // 拷贝初始化
    std::string s2("hello");    // 直接初始化
    std::string s3{"hello"};    // 列表初始化
    std::cout << "string: s1=" << s1 << " s2=" << s2 << " s3=" << s3 << std::endl;

    // atomic：= 不可用（禁用拷贝构造）
    // std::atomic<bool> flag = false;  // 编译错误！
    // std::atomic<bool> flag(false);   // OK
    // std::atomic<bool> flag{false};   // OK（推荐）
    std::cout << "atomic<bool>: 只能用 () 或 {}，不能用 =" << std::endl;

    // 总结
    std::cout << "\n--- 初始化语法总结 ---" << std::endl;
    std::cout << "T x = val;   拷贝初始化，不能用于 atomic/unique_ptr 等禁拷贝类型" << std::endl;
    std::cout << "T x(val);    直接初始化，允许窄化转换" << std::endl;
    std::cout << "T x{val};    列表初始化，禁止窄化转换（推荐）" << std::endl;
}

// ═══════════════════════════════════════════════════════════════
// main 函数
// ═══════════════════════════════════════════════════════════════
int main() {
    // 设置控制台为 UTF-8 编码
    SetConsoleOutputCP(CP_UTF8);  // 输出编码
    SetConsoleCP(CP_UTF8);        // 输入编码  // 设置控制台 UTF-8 输出

    std::cout << "=== 1. const 变量 ===" << std::endl;
    demo_const_var();

    std::cout << "\n=== 2. const 指针 ===" << std::endl;
    demo_const_pointer();

    std::cout << "\n=== 3. const 引用传参 ===" << std::endl;
    printLength("hello world");

    std::cout << "\n=== 4. const 成员函数 ===" << std::endl;
    Counter c;
    c.increment();
    c.increment();
    std::cout << "count=" << c.get() << " hitCount=" << c.getHitCount() << std::endl;

    // const 对象只能调用 const 成员函数
    const Counter cc;
    std::cout << "const对象 count=" << cc.get() << std::endl;
    // cc.increment();  // 编译错误：const 对象不能调用非 const 函数

    std::cout << "\n=== 5. 返回 const 引用 ===" << std::endl;
    Box box("my box");
    const std::string& lbl = box.getLabel();  // 返回 const 引用
    std::cout << "label=" << lbl << std::endl;

    // ═══════════════════════════════════════════════════════════
    // 栈 vs 堆对象演示
    // ═══════════════════════════════════════════════════════════
    std::cout << "\n=== 栈 vs 堆对象 ===" << std::endl;
    
    // 栈对象：函数结束自动销毁
    Counter stackCounter;           // 栈上
    const Counter constStackCounter; // 栈上（const 也是栈）
    Box stackBox("stack box");      // 栈上
    
    std::cout << "栈对象地址:" << std::endl;
    std::cout << "stackCounter: " << &stackCounter << std::endl;
    std::cout << "constStackCounter: " << &constStackCounter << std::endl;
    std::cout << "stackBox: " << &stackBox << std::endl;
    
    // 堆对象：需要手动 delete
    Counter* heapCounter = new Counter();        // 堆上
    const Counter* constHeapCounter = new Counter(); // 堆上（指针指向 const 对象）
    Box* heapBox = new Box("heap box");         // 堆上
    
    std::cout << "\n堆对象地址:" << std::endl;
    std::cout << "heapCounter: " << heapCounter << std::endl;
    std::cout << "constHeapCounter: " << constHeapCounter << std::endl;
    std::cout << "heapBox: " << heapBox << std::endl;
    
    // 手动释放堆内存
    delete heapCounter;
    delete constHeapCounter;
    delete heapBox;
    
    std::cout << "\n内存位置总结:" << std::endl;
    std::cout << "Counter cc;           → 栈（函数结束自动销毁）" << std::endl;
    std::cout << "const Counter cc;     → 栈（const 不影响内存位置）" << std::endl;
    std::cout << "Box box(\"test\");      → 栈（函数结束自动销毁）" << std::endl;
    std::cout << "new Counter();        → 堆（需要手动 delete）" << std::endl;

    demo_class_init();

    return 0;
}