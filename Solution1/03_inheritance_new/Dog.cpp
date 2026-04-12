// UTF-8
#include "Dog.h"

void Dog::speak() const {
    std::cout << name() << " หต: Woof!" << std::endl;
}

std::string Dog::name() const { return "นท"; }
