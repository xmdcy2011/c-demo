// UTF-8
#pragma once
#include <cstdio>
#include <stdexcept>
#include <string>

class FileHandle {
private:
    FILE* fp;
    std::string path;
public:
    FileHandle(const char* path, const char* mode);  // fopen，失败 throw
    ~FileHandle();                                    // fclose，自动调用

    FILE* get() const { return fp; }
    void writeLine(const char* text);
    void readAll() const;

    // 禁止拷贝（资源独占）
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
};
