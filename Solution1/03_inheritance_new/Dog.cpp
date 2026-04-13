// UTF-8
#include "Dog.h"

void Dog::speak() const {
    std::cout << name() << " 说: Woof!" << std::endl;
}

std::string Dog::name() const { return "狗"; }
