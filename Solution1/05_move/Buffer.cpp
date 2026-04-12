// UTF-8
#include "Buffer.h"

Buffer::Buffer(const char* str) {
    size = strlen(str) + 1;
    data = new char[size];
    memcpy(data, str, size);
    std::cout << "[构造] data=" << data << " addr=" << (void*)data << std::endl;
}

Buffer::Buffer(const Buffer& other) {
    size = other.size;
    data = new char[size];
    memcpy(data, other.data, size);
    std::cout << "[拷贝构造] data=" << data << " addr=" << (void*)data << std::endl;
}

Buffer& Buffer::operator=(const Buffer& other) {
    if (this == &other) return *this;
    delete[] data;
    size = other.size;
    data = new char[size];
    memcpy(data, other.data, size);
    std::cout << "[拷贝赋值] data=" << data << std::endl;
    return *this;
}

Buffer::Buffer(Buffer&& other) noexcept {
    data = other.data;    // 直接偷走指针，零拷贝
    size = other.size;
    other.data = nullptr; // 原对象置空，防止析构时 double-free
    other.size = 0;
    std::cout << "[移动构造] addr=" << (void*)data << std::endl;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    if (this == &other) return *this;
    delete[] data;        // 释放自己的旧内存
    data = other.data;
    size = other.size;
    other.data = nullptr;
    other.size = 0;
    std::cout << "[移动赋值] addr=" << (void*)data << std::endl;
    return *this;
}

Buffer::~Buffer() {
    std::cout << "[析构] addr=" << (void*)data << std::endl;
    delete[] data;  // nullptr 安全，delete nullptr 是合法的
}

void Buffer::print(const char* label) const {
    std::cout << label << ": data=" << (data ? data : "null(已被移走)")
              << " addr=" << (void*)data << std::endl;
}
