// UTF-8
// ═══════════════════════════════════════════════════════════════
// 13 工厂模式 + 抽象基类
// ═══════════════════════════════════════════════════════════════
//
// 工厂模式：调用方只知道接口（Shape），不关心具体类型
//   createShape("circle", 5.0) → 返回 shared_ptr<Shape>
//   调用方统一用 draw() / area()，运行时多态决定实际行为
//
// 纯虚基类作为接口：
//   virtual void draw() const = 0  → 子类必须实现
//   virtual ~Shape() = default     → 多态基类必须有虚析构
//
// shared_ptr 管理多态对象：
//   工厂返回 shared_ptr<Shape>，调用方不需要手动 delete
//   即使通过基类指针析构，也会正确调用子类析构函数
// ═══════════════════════════════════════════════════════════════
#pragma once
#include <memory>
#include <string>
#include <iostream>

class Shape {
public:
    virtual void draw() const = 0;
    virtual double area() const = 0;
    virtual std::string name() const = 0;
    virtual ~Shape() = default;
};

// 工厂函数：根据类型字符串创建对应子类
// type: "circle", "rect", "triangle"
std::shared_ptr<Shape> createShape(const std::string& type, double p1, double p2 = 0);
