// UTF-8
#include "Cat.h"

void Cat::speak() const {
    std::cout << name() << " ˵: Meow!" << std::endl;
}

std::string Cat::name() const { return "è"; }