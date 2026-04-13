// UTF-8
#include "Cat.h"

void Cat::speak() const {
    std::cout << name() << " 说: Meow!" << std::endl;
}

std::string Cat::name() const { return "猫"; }
