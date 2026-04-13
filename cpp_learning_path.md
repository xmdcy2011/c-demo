# C++ 学习路线

## 基础篇

### 1. 内存和指针
- 值、指针、引用的区别（`int x`、`int* p`、`int& r`）
- 栈 vs 堆，`new` / `delete`
- `const` 的各种位置含义
- 练习：写一个函数，分别用值传递、指针传递、引用传递修改变量，观察区别

### 2. 类和对象
- 构造函数、析构函数
- 成员变量、成员函数、`this` 指针
- `static` 成员
- 访问控制：`public` / `private` / `protected`
- 练习：写一个 `BankAccount` 类，支持存款、取款、查余额

### 3. 继承和虚函数
- 继承语法、基类与派生类
- `virtual` 函数、纯虚函数（`= 0`）、抽象类
- `override` 关键字
- 练习：写一个 `Animal` 抽象基类，`Dog` / `Cat` 继承并实现 `speak()`

### 4. 拷贝语义
- 拷贝构造函数、拷贝赋值运算符
- 深拷贝 vs 浅拷贝
- `const T&` 传参 vs 传值的区别和性能影响
- 练习：写一个持有堆内存的类，实现正确的深拷贝

### 5. 移动语义
- 左值 vs 右值：有名字/有地址的是左值，临时对象/无名字的是右值
- 右值引用 `T&&`
- 移动构造函数、移动赋值运算符
- `std::move` 做了什么（只是类型转换，真正移动的是构造函数里的指针偷取）
- 默认拷贝是浅拷贝（逐成员复制），有裸指针必须手写五个方法（三/五法则）
- `noexcept` 的作用：让 vector 扩容时敢用移动而不是拷贝
- 练习：给第 4 节的类加上移动构造，用 `std::move` 转移所有权，观察性能差异

### 6. RAII
- 核心思想：构造时获取资源，析构时释放资源
- 为什么 RAII 能防止资源泄漏
- `std::lock_guard`、`std::unique_lock` 是 RAII 的典型例子
- 练习：写一个 `FileHandle` 类，构造时打开文件，析构时自动关闭

### 7. 智能指针
- `std::unique_ptr`：独占所有权
- `std::shared_ptr`：引用计数共享所有权
- `std::weak_ptr`：打破循环引用
- 裸指针 vs 智能指针的区别
- 练习：用 `shared_ptr` 管理一组对象，观察引用计数变化

### 8. 模板基础
- 函数模板、类模板
- 模板参数推导
- 为什么标准库容器都用尖括号（`std::vector<int>`、`std::map<K,V>`）
- 练习：写一个泛型 `Stack<T>`，支持 push / pop / top

### 9. 命名空间 + 预处理器 + 类型转换
- `namespace` 的作用，`using namespace std` 的利弊
- `#define`、`#ifdef`、`#pragma` 常见用法
- `static_cast`、`reinterpret_cast`、`const_cast` 的区别和使用场景
- 练习：用条件编译写一个跨平台的 `EXPORT` 宏（模拟项目里的 `FFI_EXPORT`）

---

## C++ 实战篇

### 10. 动态库 + `extern "C"`
- 什么是动态库（`.dll` / `.so`）
- `extern "C"` 为什么能让 C++ 函数被外部调用
- `__declspec(dllexport)` vs `__attribute__((visibility("default")))`
- 实战：写一个计算器 `.dll`，导出加减乘除四个函数，用另一个程序加载调用

### 11. 多线程基础
- `std::thread` 创建和 join
- `std::mutex` + `std::lock_guard` 保护共享数据
- `std::atomic` 原子操作
- 实战：多线程下载模拟器，N 个线程并发"下载"文件，主线程汇总进度

### 12. 生产者/消费者队列
- `std::condition_variable` 的 wait / notify
- 线程安全队列的实现
- 实战：异步日志系统，主线程写日志消息，后台线程异步落盘

### 13. 抽象渲染器 + 工厂模式
- 纯虚基类作为接口
- 工厂函数根据条件返回不同子类实例
- `shared_ptr` 管理多态对象
- 实战：`Shape` 抽象基类，`Circle` / `Rect` / `Triangle` 子类，工厂函数创建，统一调用 `draw()` 和 `area()`

### 14. 线程安全容器
- 用 mutex 封装 `std::map` 实现线程安全
- 读写锁 `std::shared_mutex`（C++17）
- 实战：封装一个线程安全的 `Cache<K, V>`，支持 get / set / remove，多线程并发读写

### 15. 任务调度器（综合）
- 线程池基本实现
- 任务队列 + 工作线程
- 回调和 `std::function`
- 实战：实现一个简单任务调度器，支持提交任务、线程池执行、回调返回结果

---

## Flutter Windows 实战篇

### 16. Flutter Windows 插件结构
- CMakeLists.txt 的组织方式
- `RegisterWithRegistrar` 入口
- MethodChannel 收发消息
- 实战：写一个返回系统信息的插件（CPU 型号、内存大小、Windows 版本）

### 17. Dart FFI 调用 C++
- `dart:ffi` 基本用法
- 类型映射：Dart 类型 ↔ C 类型
- 加载动态库、查找符号、调用函数
- 实战：Dart 调 C++ 做字符串处理和数学运算，对比纯 Dart 实现的性能

### 18. Flutter Texture + C++ 渲染
- Flutter `Texture` Widget 的工作原理
- C++ 侧注册纹理、写入像素数据
- `MarkTextureFrameAvailable` 触发刷新
- 实战：C++ 每帧生成一张渐变色图，通过 Texture 显示在 Flutter Widget 里

### 19. C++ 后台线程 → Flutter UI 更新
- Flutter 的线程模型：UI 线程、Platform 线程、自定义线程
- 从 C++ 线程安全地回调到 Flutter
- 实战：C++ 后台线程每秒计时，通过回调更新 Flutter 界面上的计数器

### 20. 端到端综合
- 整合前面所有知识点
- 实战：Flutter 界面控制 C++ 播放一个 GIF（逐帧解码 → 写入 Texture → Flutter 显示），支持播放/暂停/循环
