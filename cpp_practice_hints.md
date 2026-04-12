# C++ 练习提示手册

每道题只给：文件名、需要的头文件、关键 API、伪代码骨架。代码自己填。

---

## .h 还是 .cpp？

| 放 .h | 放 .cpp |
|-------|---------|
| 类声明 / 函数声明 | 函数实现 |
| `inline` 函数实现 | 普通成员函数实现 |
| 模板类/函数的完整实现（必须放 .h） | `main()` |
| `#define` 宏、常量 | 全局变量定义 |

典型结构：
```
BankAccount.h    ← class 声明
BankAccount.cpp  ← 成员函数实现，#include "BankAccount.h"
main.cpp         ← #include "BankAccount.h"
```

---

## C++ vs Java 速查

| Java | C++ |
|------|-----|
| `ArrayList<T>` | `std::vector<T>` |
| `LinkedList<T>` | `std::list<T>` |
| `HashMap<K,V>` | `std::unordered_map<K,V>` |
| `TreeMap<K,V>` | `std::map<K,V>` |
| `HashSet<T>` | `std::unordered_set<T>` |
| `Optional<T>` | `std::optional<T>` (C++17) |
| `interface` | 纯虚类（`virtual f() = 0`） |
| `package` | `namespace` |
| `import` | `#include` |
| `final class` | `class Foo final {}` |
| `throws IOException` | 直接 `throw`，无需声明 |
| GC 自动回收 | `delete` 或智能指针 |
| `finalize()` | 析构函数 `~Foo()`（确定性调用） |

---

## 栈 vs 堆

**栈**
- 局部变量、函数参数
- 自动释放，作用域结束就没了
- 速度快，但空间小（通常几 MB）

```cpp
void foo() {
    int a = 10;           // 栈
    std::string s = "hi"; // 栈（内部数据可能堆上）
    int arr[100];         // 栈
}
```

**堆**
- `new` / `malloc` 分配的
- 手动管理（`delete` / `free`），或用智能指针
- 空间大，但分配慢

```cpp
void foo() {
    int* p = new int(10);     // 堆，p 本身在栈上
    int* arr = new int[1000]; // 堆
    delete p;
    delete[] arr;
}
```

### 快速判断

| 情况 | 位置 |
|------|------|
| `int a = 10;` | 栈 |
| `int* p = new int(10);` | 指针在栈，数据在堆 |
| `std::vector<int> v;` | 对象在栈，内部数组在堆 |
| `static int a;` | 静态区（不是栈也不是堆） |
| 全局变量 | 静态区 |

### 现代 C++ 建议

尽量用栈，避免手动 `new`/`delete`。需要堆时用智能指针：

```cpp
auto p = std::make_unique<int>(10);  // 堆，自动释放
auto sp = std::make_shared<int>(10); // 堆，共享所有权
```

---

## 1. 值 / 指针 / 引用传递

文件：`01_pass_by/main.cpp`（单文件即可）

```cpp
#include <iostream>

void byValue(int x)      { /* x 是副本，修改不影响外部 */ }
void byPointer(int* p)   { /* *p = ... 才能修改外部 */ }
void byReference(int& r) { /* r = ... 直接修改外部 */ }

int main() {
    int a = 10;
    byValue(a);      // a 不变
    byPointer(&a);   // a 改变
    byReference(a);  // a 改变
    // 每次调用后打印 a，观察区别
}
```

### 地址分析（打印 &x 观察）

```
main &a:                       0xF544   ← a 的地址

byValue   &x（副本地址）:       0xF520   ← 新地址，和 a 不同
byPointer &x（指针变量地址）:   0xF520   ← 新地址（和 byValue 巧合相同，栈帧复用）
byPointer  x（存的值）:         0xF544   ← 这才是 a 的地址
byReference &x:                0xF544   ← 直接就是 a 的地址，引用是别名
```

关键结论：

| 传递方式 | 参数有自己的地址吗 | &x 等于 &a 吗 | 能改 a 吗 |
|---------|-----------------|--------------|---------|
| 值传递   | ✅ 有，新地址     | ❌ 不等       | ❌ 不能  |
| 指针传递 | ✅ 有，新地址     | ❌ 不等（但 *x 指向 a） | ✅ 通过 *x |
| 引用传递 | ❌ 没有          | ✅ 相等       | ✅ 直接改 |

注意：**指针传递本质也是值传递**，传的是地址值的副本，指针变量本身有自己的新地址。

### 引用不能取地址的地址

`&a` 是右值，不能再对它取地址：

```cpp
int a = 10;
&&a;       // 编译错误
&(&a);     // 编译错误，&a 是临时值，没有地址
```

想要"地址的地址"，必须先存到指针变量里：

```cpp
int* pa = &a;   // pa 有自己的地址
&pa;            // 合法，这是指针的指针（int**）
```

### 指针 vs 引用 如何选择

**用引用的场景（优先选引用）**

```cpp
// 1. 参数不能为空，调用方保证传有效值
void process(const std::string& s);  // 不可能传 nullptr

// 2. 只读参数，避免拷贝
void print(const Widget& w);

// 3. 修改调用方的变量
void swap(int& a, int& b);
```

**用指针的场景**

```cpp
// 1. 参数可以为空（可选参数）
void render(Texture* tex);  // tex 可以是 nullptr，表示不渲染

// 2. 需要重新指向别的对象
void advance(Node* p) {
    p = p->next;  // 指针可以改指向
}

// 3. 数组传递
void fill(int* arr, size_t len);

// 4. 动态分配，需要管理生命周期
Widget* create();
```

**快速记忆**

| 情况 | 选择 |
|------|------|
| 参数可能为空 | 指针 |
| 参数一定有效 | 引用 |
| 只读不修改 | `const T&` |
| 需要修改原值 | `T&` 或 `T*` |
| 数组 / 连续内存 | 指针 |
| 单个对象 | 引用 |

现代 C++ 原则：**能用引用就用引用，需要表达"可空"或"可重指向"才用指针**。

Java 对照：Java 基本类型是值传递，对象是引用传递（但引用本身是值传递）。C++ 三种方式都可以显式控制。

---

## 1b. const 的各种位置

文件：`01b_const/main.cpp`（单文件）

`const` 放的位置不同，含义完全不同，是 C++ 最容易搞混的地方之一。

### 修饰变量

```cpp
const int x = 10;   // x 不可修改，必须初始化
```

### 修饰指针（读法：从右往左读）

```cpp
int a = 1, b = 2;

int* p1 = &a;              // 普通指针：指向可变，值可变
const int* p2 = &a;        // 指向 const int：值不可改，指向可变（p2 = &b 合法）
int* const p3 = &a;        // const 指针：指向不可变，值可改（*p3 = 5 合法）
const int* const p4 = &a;  // 都不可变
```

记忆技巧：`const` 在 `*` 左边 → 值不能改；`const` 在 `*` 右边 → 指向不能改。

### 修饰引用（传参常用）

```cpp
void print(const std::string& s);  // 只读引用，不拷贝也不修改
// Java 没有直接等价的写法，final String s 只是禁止引用重指向，不能保证内容不变
```

### 修饰成员函数

```cpp
class Counter {
    int count = 0;
public:
    int get() const;       // const 成员函数：不能修改任何成员变量
    void increment();      // 非 const：可以修改
};

// const 对象只能调用 const 成员函数
const Counter c;
c.get();        // OK
c.increment();  // 编译错误
```

### mutable：const 函数里的例外

```cpp
class Cache {
    mutable int hitCount = 0;  // mutable：即使在 const 函数里也能修改
public:
    int get() const {
        hitCount++;   // 合法，因为 mutable
        return 0;
    }
};
```

### 函数返回 const

```cpp
const std::string& getName() const;  // 返回 const 引用，调用方不能通过返回值修改内部数据
```

### 练习

```cpp
// 补全下面代码，让它能编译通过
void printLength(/* ??? */ std::string& s) {
    std::cout << s.size();
    // s = "modified";  // 这行应该编译报错
}

class Box {
    double width;
public:
    double getWidth() /* ??? */;  // 加什么让 const Box 也能调用？
    void setWidth(double w);
};
```

Java 对照：Java 用 `final` 修饰变量（不可重新赋值），没有 `const` 成员函数的概念。C++ 的 `const` 更细粒度，能精确控制"谁能改什么"。

---

## 2. BankAccount 类

文件：
- `02_bank_account/BankAccount.h`
- `02_bank_account/BankAccount.cpp`
- `02_bank_account/main.cpp`

```cpp
// BankAccount.h
#pragma once
#include <stdexcept>   // std::runtime_error

class BankAccount {
private:
    double balance;
public:
    BankAccount(double initialBalance);  // 构造函数
    void deposit(double amount);
    void withdraw(double amount);        // 余额不足时 throw runtime_error
    double getBalance() const;
};
```

```cpp
// BankAccount.cpp
#include "BankAccount.h"

BankAccount::BankAccount(double initial) : balance(initial) {}
// 实现其余方法...
```

Java 对照：Java 构造函数里直接赋值；C++ 推荐用初始化列表 `: balance(initial)`，性能更好（直接初始化，不是先默认构造再赋值）。

---

## 3. Animal / Dog / Cat 继承

文件：
- `03_inheritance/Animal.h`
- `03_inheritance/Dog.h` / `Dog.cpp`
- `03_inheritance/Cat.h` / `Cat.cpp`
- `03_inheritance/main.cpp`

```cpp
// Animal.h
#pragma once
#include <iostream>

class Animal {
public:
    virtual void speak() = 0;    // 纯虚函数 = Java interface 方法
    virtual ~Animal() = default; // 多态基类必须有虚析构
};

// Dog.h
#pragma once
#include "Animal.h"

class Dog : public Animal {
public:
    void speak() override;  // 实现：输出 "Woof"
};
```

```cpp
// main.cpp
Animal* a = new Dog();
a->speak();   // 多态调用
delete a;
// 或用智能指针：
// std::unique_ptr<Animal> a = std::make_unique<Dog>();
```

Java 对照：`virtual` = Java 默认方法行为；`= 0` = Java `abstract`；`override` 关键字两者相同。

---

## 4. 深拷贝类

文件：
- `04_deep_copy/Buffer.h`
- `04_deep_copy/Buffer.cpp`
- `04_deep_copy/main.cpp`

```cpp
// Buffer.h
#pragma once
#include <cstring>   // memcpy, strlen

class Buffer {
private:
    char* data;
    size_t size;
public:
    Buffer(const char* str);                 // 构造：new[] 分配
    Buffer(const Buffer& other);             // 拷贝构造：深拷贝
    Buffer& operator=(const Buffer& other);  // 拷贝赋值：先 delete[] 再深拷贝
    ~Buffer();                               // 析构：delete[]
    const char* get() const;
};
```

Java 对照：Java 对象赋值默认是浅拷贝（共享引用）；C++ 不写拷贝构造也是浅拷贝，但会导致 double-free，必须手动实现深拷贝。

---

## 4b. 野指针 / 悬空引用

文件：`04b_dangling_ptr/main.cpp`（单文件）

### 问题场景

**返回栈上变量的指针/引用**

```cpp
int* danglingPointer() {
    int a = 10;       // a 在栈上
    return &a;        // 返回 a 的地址
}                     // a 在这里销毁！

int main() {
    int* p = danglingPointer();
    *p = 5;  // 野指针！a 的栈空间已经释放，行为未定义
}
```

引用也一样：

```cpp
int& danglingRef() {
    int a = 10;
    return a;   // 返回局部变量的引用，a 销毁后引用悬空
}

int main() {
    int& r = danglingRef();
    r = 5;  // 悬空引用，未定义行为
}
```

### 指针 vs 指针变量

关键理解：**指针变量在栈上，但它存的地址可以指向堆上的内存**

```cpp
int* safe() {
    int* p = new int(10);  // p 在栈上，存的是堆地址
    return p;              // 返回那个地址值的副本
}                          // p 这个变量销毁，但地址值被复制出去了

int main() {
    int* result = safe();  // result 也在栈上，存的是同一个堆地址
    *result;               // 堆内存还在，安全
    delete result;         // 用完要手动释放
}
```

内存布局：

```
栈上：  p [ 0x1234 ]  ← 函数结束，p 这个变量销毁
堆上：  0x1234 [ 10 ] ← 这块内存还在，没人管它
```

类比：`p` 就像一张写了地址的纸，纸烧了（`p` 销毁），但那个地址对应的房子（堆内存）还在。`return p` 就是在纸烧之前把地址抄给了调用方。

### 安全做法

**1. 堆上分配，生命周期由你控制**

```cpp
int* safe() {
    int* p = new int(10);
    return p;  // 堆上的，不会随函数结束销毁
}

int main() {
    int* p = safe();
    *p = 5;
    delete p;  // 必须手动释放，否则内存泄漏
}
```

**2. 用智能指针（推荐）**

```cpp
std::unique_ptr<int> safe() {
    return std::make_unique<int>(10);  // 自动管理生命周期
}

int main() {
    auto p = safe();
    *p = 5;
    // p 离开作用域自动 delete，无需手动管理
}
```

**3. 通过参数传出去（调用方负责内存）**

```cpp
void safe(int& out) {
    out = 10;  // 写入调用方的变量，没有生命周期问题
}

int main() {
    int a;
    safe(a);  // a 的生命周期由 main 控制
}
```

**4. 返回值（栈上的对象）**

```cpp
std::string safe() {
    std::string s = "hello";
    return s;  // 返回对象本身，编译器会优化（RVO/移动语义）
}

int main() {
    auto s = safe();  // 安全，s 在 main 的栈上
}
```

### 编译器检测

VS 和现代编译器通常会对明显的悬空引用给出警告，但不是所有情况都能检测到，所以这类 bug 很隐蔽。建议：
- 尽量用智能指针
- 避免返回局部变量的指针/引用
- 用 `-Wall -Wextra` 编译选项开启更多警告

---

## 5. 移动语义

文件：在 `04_deep_copy/` 基础上修改 `Buffer.h` / `Buffer.cpp`，新增 `05_move/main.cpp`

```cpp
// Buffer.h 额外加：
#include <utility>   // std::move

Buffer(Buffer&& other) noexcept;             // 移动构造：偷走 other.data，other.data = nullptr
Buffer& operator=(Buffer&& other) noexcept;  // 移动赋值
```

```cpp
// main.cpp
Buffer a("hello");
Buffer b = std::move(a);   // a.data 变 nullptr，b 接管，零拷贝
```

Java 对照：Java 没有移动语义，对象赋值永远是引用共享。C++ 移动语义是性能优化的核心，避免不必要的堆内存拷贝。

---

## 6. RAII FileHandle

文件：
- `06_raii/FileHandle.h`
- `06_raii/FileHandle.cpp`
- `06_raii/main.cpp`

```cpp
// FileHandle.h
#pragma once
#include <cstdio>    // FILE*, fopen, fclose
#include <stdexcept>

class FileHandle {
private:
    FILE* fp;
public:
    FileHandle(const char* path, const char* mode);  // fopen，失败 throw
    ~FileHandle();                                    // fclose，自动调用
    FILE* get() const;

    FileHandle(const FileHandle&) = delete;           // 禁止拷贝
    FileHandle& operator=(const FileHandle&) = delete;
};
```

Java 对照：Java 用 `try-with-resources` + `AutoCloseable`；C++ 用 RAII，析构函数确定性调用，离开作用域自动关闭，不需要 try 块。

---

## 7. shared_ptr 引用计数观察

文件：`07_smart_ptr/main.cpp`（单文件）

```cpp
#include <memory>    // std::shared_ptr, std::make_shared, std::weak_ptr
#include <iostream>

struct Node { int val; };

int main() {
    auto p1 = std::make_shared<Node>(Node{1});
    std::cout << p1.use_count();   // 1

    auto p2 = p1;
    std::cout << p1.use_count();   // 2

    {
        auto p3 = p1;
        std::cout << p1.use_count(); // 3
    }                                // p3 析构
    std::cout << p1.use_count();   // 2

    // 扩展：用 weak_ptr 观察不增加引用计数
    std::weak_ptr<Node> wp = p1;
    std::cout << wp.expired();     // false，对象还在
}
```

Java 对照：Java GC 是不确定性的引用计数（标记清除）；`shared_ptr` 是确定性引用计数，计数归零立刻析构。

---

## 8. 泛型 Stack\<T\>

文件：`08_template_stack/Stack.h`（模板必须全部放 .h）

```cpp
// Stack.h
#pragma once
#include <vector>
#include <stdexcept>

template<typename T>
class Stack {
private:
    std::vector<T> data;
public:
    void push(const T& val);
    void pop();           // 空栈 throw std::underflow_error
    T& top();             // 空栈 throw std::underflow_error
    bool empty() const;
    size_t size() const;
};

// 实现也在 .h 里（模板规则）
template<typename T>
void Stack<T>::push(const T& val) { /* ... */ }
```

Java 对照：Java 泛型是类型擦除（运行时无类型信息）；C++ 模板是编译期代码生成，每种类型生成一份独立代码，性能更好但编译慢。

---

## 9. 跨平台 EXPORT 宏

文件：`09_macro/platform.h`（纯头文件）

```cpp
// platform.h
#pragma once

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

// DEBUG 日志宏
#ifdef DEBUG
    #include <iostream>
    #define LOG(x) std::cout << "[DEBUG] " << x << "\n"
#else
    #define LOG(x)   // 空宏，release 下零开销
#endif
```

Java 对照：Java 没有预处理器，跨平台靠 JVM 抽象；C++ 直接操作编译器指令，`#ifdef` 在编译期决定代码路径。

---

## 10. 计算器 DLL

文件：
- `10_dll/calculator.h`
- `10_dll/calculator.cpp`
- `10_dll/main.cpp`（调用方）

```cpp
// calculator.h
#pragma once
#ifdef BUILDING_DLL
    #define API __declspec(dllexport)
#else
    #define API __declspec(dllimport)
#endif

extern "C" {          // 防止 C++ name mangling，让符号名保持简单
    API double calc_add(double a, double b);
    API double calc_sub(double a, double b);
    API double calc_mul(double a, double b);
    API double calc_div(double a, double b);
}
```

```cpp
// main.cpp（动态加载方式）
#include <windows.h>
typedef double(*CalcFn)(double, double);

HMODULE lib = LoadLibrary("calculator.dll");
CalcFn addFn = (CalcFn)GetProcAddress(lib, "calc_add");
// 用完 FreeLibrary(lib)
```

Java 对照：Java 用 JNI / JNA 调用 native 库；`extern "C"` 的作用类似 JNI 的命名规范，让外部能找到函数符号。

---

## 11. 多线程下载模拟器

文件：`11_multithread/main.cpp`（单文件）

```cpp
#include <thread>    // std::thread
#include <mutex>     // std::mutex, std::lock_guard
#include <atomic>    // std::atomic
#include <vector>
#include <iostream>
#include <chrono>    // std::this_thread::sleep_for, std::chrono::milliseconds

std::atomic<int> totalProgress{0};
std::mutex printMutex;

void downloadFile(int id, int totalSize) {
    for (int i = 0; i < totalSize; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        totalProgress++;
        // lock_guard 保护 cout，避免输出乱序
    }
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++)
        threads.emplace_back(downloadFile, i, 100);
    for (auto& t : threads)
        t.join();   // 等所有线程完成
}
```

Java 对照：`std::thread` ≈ Java `Thread`；`std::mutex` ≈ `synchronized`；`std::atomic<int>` ≈ `AtomicInteger`；`lock_guard` ≈ `synchronized` 块（自动释放锁）。

---

## 12. 异步日志系统（生产者/消费者）

文件：
- `12_async_logger/Logger.h`
- `12_async_logger/Logger.cpp`
- `12_async_logger/main.cpp`

```cpp
// Logger.h
#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <fstream>

class Logger {
private:
    std::queue<std::string> queue;
    std::mutex mtx;
    std::condition_variable cv;
    std::thread worker;
    std::ofstream file;
    bool done = false;

    void consume();   // 后台线程函数
public:
    Logger(const char* filename);
    ~Logger();        // done=true, cv.notify_all(), worker.join()
    void log(const std::string& msg);
};
```

Java 对照：`condition_variable` ≈ Java `Object.wait()` / `notify()`；`unique_lock` + `cv.wait()` ≈ `synchronized` + `wait()`。

---

## 13. Shape 工厂模式

文件：
- `13_factory/Shape.h`
- `13_factory/Circle.h` / `Circle.cpp`
- `13_factory/Rect.h` / `Rect.cpp`
- `13_factory/Triangle.h` / `Triangle.cpp`
- `13_factory/ShapeFactory.h` / `ShapeFactory.cpp`
- `13_factory/main.cpp`

```cpp
// Shape.h
#pragma once
#include <memory>
#include <string>

class Shape {
public:
    virtual void draw() const = 0;
    virtual double area() const = 0;
    virtual ~Shape() = default;
};

// ShapeFactory.h
#pragma once
#include "Shape.h"

std::shared_ptr<Shape> createShape(const std::string& type, double param1, double param2 = 0);
// type: "circle", "rect", "triangle"
```

```cpp
// Circle.h
#pragma once
#include "Shape.h"
#include <cmath>   // M_PI

class Circle : public Shape {
    double radius;
public:
    Circle(double r);
    void draw() const override;
    double area() const override;  // M_PI * radius * radius
};
```

Java 对照：工厂模式两者写法几乎一样；区别是 C++ 返回 `shared_ptr<Shape>` 而不是裸指针，避免内存泄漏。

---

## 14. 线程安全 Cache\<K, V\>

文件：`14_safe_cache/Cache.h`（模板全放 .h）

```cpp
// Cache.h
#pragma once
#include <shared_mutex>    // std::shared_mutex (C++17)
#include <unordered_map>
#include <optional>        // std::optional

template<typename K, typename V>
class Cache {
private:
    std::unordered_map<K, V> data;
    mutable std::shared_mutex mtx;  // mutable：允许 const 方法加锁
public:
    void set(const K& key, const V& val);
    std::optional<V> get(const K& key) const;  // 读锁
    void remove(const K& key);
};

template<typename K, typename V>
void Cache<K,V>::set(const K& key, const V& val) {
    std::unique_lock lock(mtx);   // 写锁，独占
    data[key] = val;
}

template<typename K, typename V>
std::optional<V> Cache<K,V>::get(const K& key) const {
    std::shared_lock lock(mtx);   // 读锁，允许并发读
    auto it = data.find(key);
    if (it == data.end()) return std::nullopt;
    return it->second;
}
```

Java 对照：`shared_mutex` ≈ Java `ReadWriteLock`；`shared_lock` ≈ `readLock()`；`unique_lock` ≈ `writeLock()`；`std::optional` ≈ `Optional<V>`。

---

## 15. 线程池任务调度器

文件：
- `15_thread_pool/ThreadPool.h`
- `15_thread_pool/ThreadPool.cpp`
- `15_thread_pool/main.cpp`

```cpp
// ThreadPool.h
#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>   // std::function
#include <vector>
#include <future>       // std::packaged_task, std::future

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop = false;

    void workerLoop();  // 每个线程跑的循环：wait → 取任务 → 执行
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();      // stop=true, cv.notify_all(), join all

    template<typename F>
    auto submit(F&& f) -> std::future<std::invoke_result_t<F>> {
        // 1. 用 packaged_task 包装 f
        // 2. 取出 future
        // 3. 把 task push 到 queue
        // 4. cv.notify_one()
        // 5. return future
    }
};
```

Java 对照：`ThreadPool` ≈ Java `ExecutorService`；`submit()` 返回 `std::future` ≈ Java `Future<T>`；`packaged_task` ≈ `FutureTask`。

---

## auto 类型推导

不需要头文件，C++11 起内置。

```cpp
auto x = 42;           // int
auto y = 3.14;         // double
auto s = std::string("hi");  // std::string
auto it = vec.begin(); // 省去写 std::vector<int>::iterator

// 配合范围 for
for (auto& item : vec) { /* item 是引用，不拷贝 */ }
for (const auto& item : map) { /* item 是 pair<K,V> */ }

// 函数返回类型推导（C++14）
auto add(int a, int b) -> int { return a + b; }
// 或直接：
auto add(int a, int b) { return a + b; }
```

`auto` 不是"不知道类型"，是让编译器帮你推导，类型在编译期完全确定。

---

## 固定宽度整数类型（平台无关）

头文件：`<cstdint>`

```cpp
#include <cstdint>

int8_t   / uint8_t    // 8位  有符号/无符号
int16_t  / uint16_t   // 16位
int32_t  / uint32_t   // 32位
int64_t  / uint64_t   // 64位

// 示例
uint32_t fileSize = 0xFFFFFFFF;
int64_t  timestamp = 1712800000LL;
```

什么时候用：
- 网络协议、文件格式、跨平台二进制数据 → 必须用
- 普通计数、循环变量 → `int` 够用
- 容器大小、数组索引 → 用 `size_t`（`<cstddef>`，本身就是平台无关的）

```cpp
// size_t 示例
for (size_t i = 0; i < vec.size(); i++) { /* vec.size() 返回 size_t */ }
// 或直接 auto：
for (auto i = 0u; i < vec.size(); i++) {}
```

Java 对照：Java 的 `int` 固定 32 位、`long` 固定 64 位，天然平台无关；C++ 的 `int` 大小依赖平台（通常 32 位），所以需要 `<cstdint>` 来保证。

---

## 提示速查

| 功能 | 头文件 |
|------|--------|
| 输入输出 | `<iostream>` |
| 字符串 | `<string>` |
| 动态数组 | `<vector>` |
| 哈希表 | `<unordered_map>` |
| 有序 map | `<map>` |
| 智能指针 | `<memory>` |
| 固定宽度整数 | `<cstdint>` |
| `size_t` | `<cstddef>` |
| 线程 | `<thread>` |
| 互斥锁 | `<mutex>` |
| 读写锁 | `<shared_mutex>` |
| 条件变量 | `<condition_variable>` |
| 原子操作 | `<atomic>` |
| 异步/Future | `<future>` |
| 函数对象 | `<functional>` |
| 可选值 | `<optional>` |
| 文件操作 | `<fstream>` |
| C 文件操作 | `<cstdio>` |
| 数学 | `<cmath>` |
| 异常 | `<stdexcept>` |
| 时间/sleep | `<chrono>` |
| Windows DLL | `<windows.h>` |