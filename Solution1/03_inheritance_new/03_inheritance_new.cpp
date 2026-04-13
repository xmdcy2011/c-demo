// UTF-8
// ═══════════════════════════════════════════════════════════════
// 03 继承 / 虚函数 / 多态
// ═══════════════════════════════════════════════════════════════
//
// 纯虚函数：virtual void speak() = 0  → 子类必须实现，类变抽象类
// override：显式标记覆盖父类方法，编译器帮你检查拼写错误
// 虚析构：多态基类必须有 virtual ~Animal() = default
//         否则 delete 基类指针时不会调用子类析构，内存泄漏
//
// 多态调用：
//   Animal* a = new Dog();
//   a->speak();  // 运行时决定调用 Dog::speak，不是 Animal::speak
//
// 用 unique_ptr 管理多态对象（推荐）：
//   std::unique_ptr<Animal> a = std::make_unique<Dog>();
//   不需要手动 delete，离开作用域自动释放
//
// Java 对照：virtual = Java 默认方法行为；= 0 = Java abstract
//            override 关键字两者相同；C++ 需要显式写 virtual
// ═══════════════════════════════════════════════════════════════
#include "Dog.h"
#include "Cat.h"
#include <memory>
#include <vector>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    // 用基类指针存不同子类，体会多态
    std::vector<std::unique_ptr<Animal>> animals;
    animals.push_back(std::make_unique<Dog>());
    animals.push_back(std::make_unique<Cat>());
    animals.push_back(std::make_unique<Cat1>());

    std::cout << "=== 多态调用 speak() ===" << std::endl;
    for (const auto& a : animals) {
        a->speak();  // 运行时决定调用哪个子类的 speak
    }

    return 0;
}
