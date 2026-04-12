// UTF-8
#pragma once
#include "Animal.h"

class Cat : public Animal {
public:
    void speak() const override;
    std::string name() const override;
};
