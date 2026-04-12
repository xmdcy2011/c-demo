// UTF-8
#include "FileHandle.h"
#include <iostream>

FileHandle::FileHandle(const char* filePath, const char* mode)
    : path(filePath) {
    fp = fopen(filePath, mode);
    if (!fp)
        throw std::runtime_error(std::string("无法打开文件: ") + filePath);
    std::cout << "[FileHandle] 打开文件: " << path << std::endl;
}

FileHandle::~FileHandle() {
    if (fp) {
        fclose(fp);
        std::cout << "[FileHandle] 自动关闭文件: " << path << std::endl;
    }
}

void FileHandle::writeLine(const char* text) {
    fprintf(fp, "%s\n", text);
}

void FileHandle::readAll() const {
    rewind(fp);  // 回到文件开头
    char buf[256];
    std::cout << "--- 文件内容 ---" << std::endl;
    while (fgets(buf, sizeof(buf), fp)) {
        std::cout << buf;
    }
    std::cout << "--- 结束 ---" << std::endl;
}
