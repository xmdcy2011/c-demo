// UTF-8
#pragma once
#include "Animal.h"

class Cat : public Animal {
public:
	Cat() = default;
    void speak() const override;
    std::string name() const override;
};

class Cat1 : public Cat {
public:
    void speak() const override {
        __super::speak();  // 调用 Cat 的 speak()，输出 "猫 说: Meow!"
        std::cout << name() << " 说: Meow! (Cat1)" << std::endl;
    }
    std::string name() const override { return "猫1"; }
};
