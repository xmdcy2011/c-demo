// ═══════════════════════════════════════════════════════════════
// 02 指针 vs 引用 场景演示
// ═══════════════════════════════════════════════════════════════
//
// 用引用的场景（优先选引用）：
//   1. 参数不能为空，调用方保证传有效值
//   2. 只读参数，避免拷贝：void print(const Widget& w)
//   3. 修改调用方的变量：void swap(int& a, int& b)
//
// 用指针的场景：
//   1. 参数可以为空（可选参数）：void render(Texture* tex)
//   2. 需要重新指向别的对象
//   3. 数组传递：void fill(int* arr, size_t len)
//   4. 动态分配，需要管理生命周期
//
// 范围 for 循环：
//   for (auto v : arr)        值拷贝，修改 v 不影响原数组
//   for (auto& v : arr)       引用，修改 v 直接改原数组
//   for (const auto& v : arr) 只读引用，不拷贝也不能改
//
// vector 存对象三种方式：
//   vector<T>              存副本，vector 自动管理
//   vector<T*>             存指针，堆对象需手动 delete（容易出错）
//   vector<unique_ptr<T>>  智能指针，自动 delete（推荐）
//
// Java 对照：Java 对象是引用传递；C++ 可以显式选择值/指针/引用
// ═══════════════════════════════════════════════════════════════
#include <iostream>
#include <string>
#include <windows.h>
#include <vector>

// ---- 用引用的场景 ----

// 1. 只读参数，避免拷贝，不能为空
void print(const std::string& s) {
    std::cout << "print: " << s << std::endl;
}

// ---- const 引用传对象 ----

struct User {
    std::string name;
    int age;
    
    // 添加构造函数
    User(const std::string& n, int a) : name(n), age(a) {}
    
    std::string getName() const { return name; }  // const 函数，可以调用
    void rename(std::string n) { name = n; }      // 非 const 函数
};

void printUser(const User& u) {
    std::cout << "user: " << u.name << " age=" << u.age << std::endl;
    std::cout << "getName: " << u.getName() << std::endl;
    // u.age = 20;       // 编译错误！属性不能改
    // u.rename("bob");  // 编译错误！非 const 成员函数不能调用
}

// 2. 修改调用方的变量
void swap(int& a, int& b) {
    int tmp = a;
    a = b;
    b = tmp;
}

// ---- 用指针的场景 ----

// 3. 参数可以为空（可选参数）
void render(const std::string* tex) {
    if (tex == nullptr) {
        std::cout << "render: 无纹理，跳过" << std::endl;
    }
    else {
        std::cout << "render: " << *tex << std::endl;
    }
}

// 4. 数组传递
void fill(int* arr, size_t len, int val) {
    for (size_t i = 0; i < len; i++) {
        arr[i] = val;
    }
}

int main() {
    // 设置控制台为 UTF-8 编码
    SetConsoleOutputCP(CP_UTF8);  // 输出编码
    SetConsoleCP(CP_UTF8);        // 输入编码

    // 引用：只读，不拷贝
    std::string name = "hello";
    print(name);

    // const 引用传对象：属性和非 const 函数都不能修改
    User user{ "alice", 30 };
    printUser(user);

    // 引用：修改原值
    int x = 1, y = 2;
    std::cout << "swap 前: x=" << x << " y=" << y << std::endl;
    swap(x, y);
    std::cout << "swap 后: x=" << x << " y=" << y << std::endl;

    // 指针：可空
    std::string tex = "wood.png";
    render(&tex);   // 有纹理
    render(nullptr); // 无纹理

    // 指针：数组
    int arr[5] = {};
    fill(arr, 5, 42);
    std::cout << "fill 后: ";
    for (int v : arr) std::cout << v << " ";
    std::cout << std::endl;

    // ═══════════════════════════════════════════════════════════
    // 范围 for 循环：值拷贝 vs 引用
    // ═══════════════════════════════════════════════════════════
    std::cout << "\n=== 范围 for: 值拷贝 vs 引用 ===" << std::endl;
    
    int nums[3] = {10, 20, 30};
    
    std::cout << "\n--- for (int v : arr) 值拷贝 ---" << std::endl;
    for (int v : nums) {
        std::cout << "v=" << v << " &v=" << &v << std::endl;
        v = 999;  // 修改的是副本，不影响原数组
    }
    std::cout << "原数组: ";
    for (int n : nums) std::cout << n << " ";  // 仍然是 10, 20, 30
    std::cout << std::endl;
    
    std::cout << "\n--- for (int& v : arr) 引用 ---" << std::endl;
    for (int& v : nums) {
        std::cout << "v=" << v << " &v=" << &v << std::endl;
        v = 999;  // 修改的是原数组的元素
    }
    std::cout << "原数组: ";
    for (int n : nums) std::cout << n << " ";  // 变成 999, 999, 999
    std::cout << std::endl;
    
    // 对比：const 引用（只读不拷贝）
    std::cout << "\n--- for (const int& v : arr) const 引用 ---" << std::endl;
    for (const int& v : nums) {
        std::cout << "v=" << v << " &v=" << &v << std::endl;
        // v = 888;  // 编译错误！不能修改
    }

	// 注意：vector 内部重分配会导致元素地址改变
    std::cout << "\n--- 内部重分配会导致元素地址改变 ---" << std::endl;
    std::vector<int> vec = {1, 2, 3};
    for (const int& v : vec) {
        std::cout << "v=" << v << " &v=" << &v << std::endl;
    }
	vec.push_back(4);  // vector 内部会重新分配内存，导致元素地址改变
    for (const int& v : vec) {
        std::cout << "v=" << v << " &v=" << &v << std::endl;
    }
	vec.pop_back();    // 可能再次改变元素地址
	std::cout << "\n=== vector 内部重分配导致地址改变 ===" << std::endl;
    for (const int& v : vec) {
        std::cout << "v=" << v << " &v=" << &v << std::endl;
	}

    // ═══════════════════════════════════════════════════════════
    // vector 存对象：值拷贝 vs 引用 vs 指针
    // ═══════════════════════════════════════════════════════════
    std::cout << "\n=== vector 存对象 ===" << std::endl;

    // 1. 值拷贝：存对象的副本
    std::vector<User> users1;
    User u1{"alice", 25};
    User u2{"bob", 30};
    users1.push_back(u1);
    users1.push_back(u2);
    std::cout << "\n--- vector<User> 值拷贝 ---" << std::endl;
    std::cout << "u1 地址: " << &u1 << std::endl;
    std::cout << "users1[0] 地址: " << &users1[0] << " (不同！)" << std::endl;
    users1[0].name = "alice_new";  // 修改的是副本，不影响 u1
    std::cout << "u1.name 仍然是: " << u1.name << std::endl;
    std::cout << "users1[0].name 变成: " << users1[0].name << std::endl;

    // 2. 引用：存引用（需要包装）
    std::vector<std::reference_wrapper<User>> users2;
    users2.push_back(u1);
    users2.push_back(u2);
    std::cout << "\n--- vector<reference_wrapper<User>> 引用 ---" << std::endl;
    std::cout << "u1 地址: " << &u1 << std::endl;
    std::cout << "users2[0].get() 地址: " << &users2[0].get() << " (相同！)" << std::endl;
    users2[0].get().name = "alice_modified";  // 修改会影响原对象
    std::cout << "u1.name 变成: " << u1.name << std::endl;

    // 3. 指针：存地址
    std::vector<User*> users3;
    users3.push_back(&u1);
    users3.push_back(&u2);
    std::cout << "\n--- vector<User*> 指针 ---" << std::endl;
    std::cout << "u1 地址: " << &u1 << std::endl;
    std::cout << "users3[0] 地址: " << users3[0] << " (相同！)" << std::endl;
    users3[0]->name = "alice_via_ptr";  // 修改会影响原对象
    std::cout << "u1.name 变成: " << u1.name << std::endl;

    // 3b. 如果是堆对象，需要手动 delete
    std::vector<User*> users3b;
    users3b.push_back(new User("heap_user1", 40));
    users3b.push_back(new User("heap_user2", 45));
    std::cout << "\n--- vector<User*> 堆对象需要手动删除 ---" << std::endl;
    std::cout << "users3b[0]->name: " << users3b[0]->name << std::endl;
    
    // 手动删除堆对象（容易忘记，容易出错！）
    for (User* ptr : users3b) {
        delete ptr;
    }
    users3b.clear();

    // 4. 智能指针 - 自动管理内存！
    std::vector<std::unique_ptr<User>> users4;
    users4.push_back(std::make_unique<User>("charlie", 35));
    users4.push_back(std::make_unique<User>("dave", 40));
    std::cout << "\n--- vector<unique_ptr<User>> 智能指针 ---" << std::endl;
    std::cout << "users4[0]->name: " << users4[0]->name << std::endl;
    std::cout << "users4[0].get() 地址: " << users4[0].get() << std::endl;
    // 不需要手动 delete！unique_ptr 析构时自动删除

    std::cout << "\n=== 内存管理总结 ===" << std::endl;
    std::cout << "1. vector<User>: 栈对象拷贝，vector 自动管理" << std::endl;
    std::cout << "2. vector<User*> 指向栈: 不需要 delete" << std::endl;
    std::cout << "3. vector<User*> 指向堆: 需要手动 delete（容易出错）" << std::endl;
    std::cout << "4. vector<unique_ptr<User>>: 智能指针自动 delete" << std::endl;

    // ═══════════════════════════════════════════════════════════
    // 返回值的陷阱与解决方案
    // ═══════════════════════════════════════════════════════════
    std::cout << "\n=== 返回值的陷阱与解决方案 ===" << std::endl;

    // 危险示例：返回局部对象的引用/指针
    auto dangerousReturnRef = []() -> const User& {
        User localUser("local", 25);  // 栈上局部对象
        return localUser;  // 危险！返回局部对象的引用
        // 函数结束后 localUser 被销毁，返回的引用指向无效内存
    };

    auto dangerousReturnPtr = []() -> User* {
        User localUser("local", 25);  // 栈上局部对象
        return &localUser;  // 危险！返回局部对象的地址
        // 函数结束后 localUser 被销毁，返回的指针指向无效内存
    };

    std::cout << "\n--- 危险做法（不要这样做！）---" << std::endl;
    // const User& badRef = dangerousReturnRef();  // 未定义行为！
    // User* badPtr = dangerousReturnPtr();        // 未定义行为！
    std::cout << "以上代码会导致未定义行为，已注释掉" << std::endl;

    // 解决方案1：返回值拷贝（安全但可能性能差）
    auto safeReturnCopy = []() -> User {
        User localUser("copy", 30);
        return localUser;  // 返回拷贝，安全
    };

    // 解决方案2：返回成员的引用（安全，因为成员生命周期和对象一致）
    class UserContainer {
        User user;
    public:
        UserContainer(const std::string& name, int age) : user(name, age) {}
        const User& getUser() const { return user; }  // 安全！返回成员引用
        User& getUser() { return user; }              // 安全！返回成员引用
    };

    // 解决方案3：返回堆对象指针（调用方负责删除）
    auto returnHeapPtr = []() -> User* {
        return new User("heap", 35);  // 堆对象，调用方负责删除
    };

    // 解决方案4：返回智能指针（推荐）
    auto returnSmartPtr = []() -> std::unique_ptr<User> {
        return std::make_unique<User>("smart", 40);  // 自动管理内存
    };

    // 解决方案5：通过参数返回（输出参数）
    auto returnViaParam = [](User& outUser) -> void {
        outUser = User("param", 45);  // 通过参数返回
    };

    std::cout << "\n--- 安全做法演示 ---" << std::endl;

    // 1. 返回拷贝
    User copyUser = safeReturnCopy();
    std::cout << "1. 返回拷贝: " << copyUser.name << ", " << copyUser.age << std::endl;

    // 2. 返回成员引用
    UserContainer container("member", 32);
    const User& memberRef = container.getUser();
    std::cout << "2. 返回成员引用: " << memberRef.name << ", " << memberRef.age << std::endl;

    // 3. 返回堆指针（需要手动删除）
    User* heapPtr = returnHeapPtr();
    std::cout << "3. 返回堆指针: " << heapPtr->name << ", " << heapPtr->age << std::endl;
    delete heapPtr;  // 手动删除

    // 4. 返回智能指针（推荐）
    auto smartPtr = returnSmartPtr();
    std::cout << "4. 返回智能指针: " << smartPtr->name << ", " << smartPtr->age << std::endl;
    // 自动删除，无需手动管理

    // 5. 通过参数返回
    User paramUser("", 0);
    returnViaParam(paramUser);
    std::cout << "5. 通过参数返回: " << paramUser.name << ", " << paramUser.age << std::endl;

    std::cout << "\n--- 总结 ---" << std::endl;
    std::cout << "❌ 危险：返回局部对象的引用/指针" << std::endl;
    std::cout << "✅ 安全：返回拷贝（性能可能差）" << std::endl;
    std::cout << "✅ 安全：返回成员引用（生命周期一致）" << std::endl;
    std::cout << "✅ 安全：返回堆指针（需手动删除）" << std::endl;
    std::cout << "✅ 推荐：返回智能指针（自动管理）" << std::endl;
    std::cout << "✅ 安全：通过参数返回" << std::endl;

    // ═══════════════════════════════════════════════════════════
    // struct vs class 区别演示
    // ═══════════════════════════════════════════════════════════
    std::cout << "\n=== struct vs class 区别 ===" << std::endl;

    // struct：默认 public
    struct Point {
        int x, y;  // 默认 public
        void print() { std::cout << "Point(" << x << ", " << y << ")" << std::endl; }
    };

    // class：默认 private
    class Rectangle {
        int width, height;  // 默认 private
    public:
        Rectangle(int w, int h) : width(w), height(h) {}
        void print() { std::cout << "Rectangle(" << width << "x" << height << ")" << std::endl; }
        int getArea() { return width * height; }
        // 如果不写 public，外部无法访问这些方法
    };

    std::cout << "\n--- struct 默认 public ---" << std::endl;
    Point p1;
	Point p2{ 1 };// 只初始化 x，y 默认初始化为 0,这个是结构体的聚合初始化特性
	p2.print();  // Point(1, 0)，y 默认初始化为 0
    p1.x = 10;  // 可以直接访问，因为默认 public
    p1.y = 20;
    p1.print();

    std::cout << "\n--- class 默认 private ---" << std::endl;
    Rectangle rect{30, 40};// 只能通过构造函数初始化，不能直接访问成员变量
	// Rectangle rect(30,40);  // 也可以这样初始化，效果一样
    // rect.width = 50;  // 编译错误！width 是 private
    rect.print();  // 可以调用，因为在 public 区域
    std::cout << "面积: " << rect.getArea() << std::endl;

    std::cout << "\n--- 术语区别 ---" << std::endl;
    std::cout << "Point 是类型（蓝图）" << std::endl;
    std::cout << "p1 是对象/实例（具体的变量）" << std::endl;
    std::cout << "Rectangle 是类型（蓝图）" << std::endl;
    std::cout << "rect 是对象/实例（具体的变量）" << std::endl;
}
