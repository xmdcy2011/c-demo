// UTF-8
#include "Shape.h"
#include <vector>
#include <iomanip>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "=== 工厂模式 + 多态 ===" << std::endl;

    // 工厂创建，调用方只持有 shared_ptr<Shape>，不知道具体类型
    std::vector<std::shared_ptr<Shape>> shapes = {
        createShape("circle",   5.0),
        createShape("rect",     4.0, 6.0),
        createShape("triangle", 3.0, 8.0),
        createShape("circle",   2.5),
        createShape("rect",     10.0, 2.0),
    };

    std::cout << "\n--- 统一调用 draw()（运行时多态）---" << std::endl;
    for (const auto& s : shapes) {
        s->draw();
    }

    std::cout << "\n--- 统一计算 area() ---" << std::endl;
    double total = 0;
    for (const auto& s : shapes) {
        double a = s->area();
        total += a;
        std::cout << "  " << std::left << std::setw(10) << s->name()
                  << " area=" << std::fixed << std::setprecision(2) << a << std::endl;
    }
    std::cout << "  总面积=" << total << std::endl;

    // 测试未知类型异常
    std::cout << "\n--- 未知类型异常 ---" << std::endl;
    try {
        auto bad = createShape("hexagon", 1.0);
    } catch (const std::invalid_argument& e) {
        std::cout << "  捕获异常: " << e.what() << std::endl;
    }

    // shapes 离开作用域，shared_ptr 自动 delete 所有 Shape 对象
    return 0;
}
