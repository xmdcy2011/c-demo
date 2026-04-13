// UTF-8
#pragma once
#include <string>
#include <iostream>

class Animal {
public:
    virtual void speak() const = 0;       // 纯虚函数,说说
    virtual std::string name() const = 0; // 纯虚函数
    virtual ~Animal() = default;          // 多态基类必须有虚析构
};
