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
    data = new char[size];          // 新分配，不共享
    memcpy(data, other.data, size);
    std::cout << "[拷贝构造] data=" << data << " addr=" << (void*)data << std::endl;
}

Buffer& Buffer::operator=(const Buffer& other) {
    if (this == &other) return *this;  // 自赋值检查
    delete[] data;                     // 先释放旧内存
    size = other.size;
    data = new char[size];
    memcpy(data, other.data, size);
    std::cout << "[拷贝赋值] data=" << data << " addr=" << (void*)data << std::endl;
    return *this;
}

Buffer::~Buffer() {
    std::cout << "[析构] addr=" << (void*)data << std::endl;
    delete[] data;
}

void Buffer::print(const char* label) const {
    std::cout << label << ": data=" << (data ? data : "null")
              << " addr=" << (void*)data << std::endl;
}
