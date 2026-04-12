// UTF-8
#pragma once
#include <cstdint>
#include <stdexcept>
#include <string>

class BankAccount {
private:
    std::string owner;
    double balance;
public:
    BankAccount(const std::string& owner, double initialBalance);
    void deposit(double amount);
    void withdraw(double amount);   // 余额不足 throw std::runtime_error
    double getBalance() const;
    const std::string& getOwner() const;
    void print() const;
};
