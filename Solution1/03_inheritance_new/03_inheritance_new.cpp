// UTF-8
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
    animals.push_back(std::make_unique<Dog>());

    std::cout << "=== 多态调用 speak() ===" << std::endl;
    for (const auto& a : animals) {
        a->speak();  // 运行时决定调用哪个子类的 speak
    }

    return 0;
}
