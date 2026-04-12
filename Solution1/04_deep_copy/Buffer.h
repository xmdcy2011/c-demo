// UTF-8
#pragma once
#include <cstdint>
#include <cstring>
#include <iostream>

class Buffer {
private:
    char* data;
    size_t size;
public:
    Buffer(const char* str);                    // 构造：new[] 分配
    Buffer(const Buffer& other);                // 拷贝构造：深拷贝
    Buffer& operator=(const Buffer& other);     // 拷贝赋值
    ~Buffer();                                  // 析构：delete[]

    const char* get() const { return data; }
    size_t getSize() const { return size; }
    void print(const char* label) const;
};
