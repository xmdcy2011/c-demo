// UTF-8
#pragma once
#include <cstdint>
#include <cstring>
#include <utility>
#include <iostream>

// 在第4题基础上加入移动语义
class Buffer {
private:
    char* data;
    size_t size;
public:
    Buffer(const char* str);
    Buffer(const Buffer& other);                 // 拷贝构造
    Buffer& operator=(const Buffer& other);      // 拷贝赋值
    Buffer(Buffer&& other) noexcept;             // 移动构造：偷走 other.data
    Buffer& operator=(Buffer&& other) noexcept;  // 移动赋值
    ~Buffer();

    const char* get() const { return data; }
    bool empty() const { return data == nullptr; }
    void print(const char* label) const;
};
