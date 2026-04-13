// UTF-8
// ═══════════════════════════════════════════════════════════════
// 02 BankAccount 类
// ═══════════════════════════════════════════════════════════════
//
// 文件结构：BankAccount.h（声明）/ BankAccount.cpp（实现）/ main.cpp
//
// 要点：
//   构造函数用初始化列表 : balance(initial)，比构造体内赋值性能好
//   （直接初始化，不是先默认构造再赋值）
//   余额不足时 throw std::runtime_error
//   const 成员函数 getBalance() const 保证不修改状态
//
// Java 对照：Java 构造函数里直接赋值；C++ 推荐用初始化列表
// ═══════════════════════════════════════════════════════════════
#include "BankAccount.h"
#include <iostream>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    BankAccount acc("张三", 1000.0);
    acc.print();

    acc.deposit(500.0);
    std::cout << "存入500后: ";
    acc.print();

    acc.withdraw(200.0);
    std::cout << "取出200后: ";
    acc.print();

    // 测试异常
    try {
        acc.withdraw(9999.0);
    } catch (const std::runtime_error& e) {
        std::cout << "异常: " << e.what() << std::endl;
    }

    // ═══════════════════════════════════════════════════════════
    // 引用生命周期演示
    // ═══════════════════════════════════════════════════════════
    std::cout << "\n=== 引用生命周期演示 ===" << std::endl;
    
    // 1. 安全情况：对象生命周期足够长
    std::cout << "\n--- 安全情况 ---" << std::endl;
    {
        BankAccount account("李四", 500.0);
        const std::string& ownerRef = account.getOwner();  // 安全
        std::cout << "账户所有者: " << ownerRef << std::endl;
        std::cout << "引用地址: " << &ownerRef << std::endl;
        std::cout << "成员地址: " << &account.getOwner() << std::endl;
        // ownerRef 在 account 销毁前使用，安全
    }
    // 这里 account 和 ownerRef 都销毁了
    
    // 2. 危险情况演示（用函数模拟）
    std::cout << "\n--- 危险情况演示 ---" << std::endl;
    auto dangerousGetOwner = []() -> const std::string& {
        BankAccount temp("临时账户", 100.0);  // 栈上临时对象
        std::cout << "临时对象创建，owner地址: " << &temp.getOwner() << std::endl;
        return temp.getOwner();  // 危险！返回即将销毁对象的成员引用
        // temp 在函数结束时销毁，返回的引用变成悬空引用
    };
    
    // 注意：以下代码会导致未定义行为，仅作演示
    std::cout << "调用危险函数..." << std::endl;
    // const std::string& dangerousRef = dangerousGetOwner();  // 危险！
    // std::cout << "危险引用: " << dangerousRef << std::endl;  // 未定义行为
    std::cout << "危险代码已注释，避免未定义行为" << std::endl;
    
    // 3. 安全的使用方式
    std::cout << "\n--- 安全的使用方式 ---" << std::endl;
    
    // 3a. 立即使用
    std::cout << "立即使用: " << acc.getOwner() << std::endl;
    
    // 3b. 确保对象生命周期
    const std::string& safeRef = acc.getOwner();  // acc 还活着
    std::cout << "安全引用: " << safeRef << std::endl;
    
    // 3c. 拷贝解决方案
    auto getCopyOwner = [](const BankAccount& account) -> std::string {
        return account.getOwner();  // 返回拷贝，安全
    };
    std::string ownerCopy = getCopyOwner(acc);
    std::cout << "拷贝所有者: " << ownerCopy << std::endl;
    
    // 4. 地址对比
    std::cout << "\n--- 地址对比 ---" << std::endl;
    const std::string& ref1 = acc.getOwner();
    const std::string& ref2 = acc.getOwner();
    std::cout << "ref1 地址: " << &ref1 << std::endl;
    std::cout << "ref2 地址: " << &ref2 << std::endl;
    std::cout << "相同地址？" << (&ref1 == &ref2 ? "是" : "否") << std::endl;
    
    std::cout << "\n=== 总结 ===" << std::endl;
    std::cout << "✅ const std::string& getOwner() const 本身是安全的" << std::endl;
    std::cout << "✅ 只要确保对象生命周期比引用长就没问题" << std::endl;
    std::cout << "❌ 危险：返回临时对象的成员引用" << std::endl;
    std::cout << "✅ 解决：立即使用、确保生命周期、或返回拷贝" << std::endl;

    return 0;
}
