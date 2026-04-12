// UTF-8
// 模板实现必须全部放在 .h 里
#pragma once
#include <vector>
#include <stdexcept>
#include <iostream>

template<typename T>
class Stack {
private:
    std::vector<T> data;
public:
    void push(const T& val) {
        data.push_back(val);
    }

    void pop() {
        if (empty()) throw std::underflow_error("Stack is empty");
        data.pop_back();
    }

    T& top() {
        if (empty()) throw std::underflow_error("Stack is empty");
        return data.back();
    }

    const T& top() const {
        if (empty()) throw std::underflow_error("Stack is empty");
        return data.back();
    }

    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }

    void print() const {
        std::cout << "Stack [";
        for (size_t i = 0; i < data.size(); i++) {
            std::cout << data[i];
            if (i + 1 < data.size()) std::cout << ", ";
        }
        std::cout << "] top=" << (empty() ? "空" : "") << std::endl;
    }
};
