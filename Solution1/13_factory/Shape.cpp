// UTF-8
#include "Shape.h"
#include <cmath>
#include <stdexcept>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ── 具体子类 ──────────────────────────────────────────────────

class Circle : public Shape {
    double radius_;
public:
    Circle(double r) : radius_(r) {}
    void draw() const override {
        std::cout << "  ○ Circle(r=" << radius_ << ")" << std::endl;
    }
    double area() const override { return M_PI * radius_ * radius_; }
    std::string name() const override { return "Circle"; }
};

class Rect : public Shape {
    double w_, h_;
public:
    Rect(double w, double h) : w_(w), h_(h) {}
    void draw() const override {
        std::cout << "  □ Rect(" << w_ << "x" << h_ << ")" << std::endl;
    }
    double area() const override { return w_ * h_; }
    std::string name() const override { return "Rect"; }
};

class Triangle : public Shape {
    double base_, height_;
public:
    Triangle(double b, double h) : base_(b), height_(h) {}
    void draw() const override {
        std::cout << "  △ Triangle(base=" << base_ << " h=" << height_ << ")" << std::endl;
    }
    double area() const override { return 0.5 * base_ * height_; }
    std::string name() const override { return "Triangle"; }
};

// ── 工厂函数 ──────────────────────────────────────────────────
std::shared_ptr<Shape> createShape(const std::string& type, double p1, double p2) {
    if (type == "circle")   return std::make_shared<Circle>(p1);
    if (type == "rect")     return std::make_shared<Rect>(p1, p2);
    if (type == "triangle") return std::make_shared<Triangle>(p1, p2);
    throw std::invalid_argument("未知形状类型: " + type);
}
