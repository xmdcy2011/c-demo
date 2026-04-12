// UTF-8
#include "BankAccount.h"
#include <iostream>

BankAccount::BankAccount(const std::string& owner, double initialBalance)
    : owner(owner), balance(initialBalance) {
    if (initialBalance < 0)
        throw std::invalid_argument("初始余额不能为负数");
}

void BankAccount::deposit(double amount) {
    if (amount <= 0)
        throw std::invalid_argument("存款金额必须大于0");
    balance += amount;
}

void BankAccount::withdraw(double amount) {
    if (amount <= 0)
        throw std::invalid_argument("取款金额必须大于0");
    if (amount > balance)
        throw std::runtime_error("余额不足");
    balance -= amount;
}

double BankAccount::getBalance() const { return balance; }

const std::string& BankAccount::getOwner() const { return owner; }

void BankAccount::print() const {
    std::cout << "[" << owner << "] 余额: " << balance << std::endl;
}
