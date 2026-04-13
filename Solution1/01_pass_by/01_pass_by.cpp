// ═══════════════════════════════════════════════════════════════
// 01 值 / 指针 / 引用传递
// ═══════════════════════════════════════════════════════════════
//
// 地址分析（打印 &x 观察）：
//   main &a:                       0xF544   ← a 的地址
//   byValue   &x（副本地址）:       0xF520   ← 新地址，和 a 不同
//   byPointer &x（指针变量地址）:   0xF520   ← 新地址（指针变量本身）
//   byPointer  x（存的值）:         0xF544   ← 这才是 a 的地址
//   byReference &x:                0xF544   ← 直接就是 a 的地址，引用是别名
//
// 传递方式对比：
//   值传递   → 参数有新地址，&x != &a，不能改 a
//   指针传递 → 参数有新地址，但 *x 指向 a，通过 *x 能改 a
//   引用传递 → 参数没有自己的地址，&x == &a，直接改 a
//
// 注意：指针传递本质也是值传递，传的是地址值的副本
//
// 指针 vs 引用 选择：
//   用引用：参数不能为空 / 只读避免拷贝 / 修改调用方变量
//   用指针：参数可以为空 / 需要重新指向 / 传数组 / 动态分配
//   快速记忆：能用引用就用引用，需要表达"可空"才用指针
//
// 栈 vs 堆：
//   栈：局部变量，自动释放，速度快，空间小
//   堆：new 分配，手动 delete 或用智能指针，空间大
//   现代 C++ 建议：尽量用栈，需要堆时用 unique_ptr / shared_ptr
//
// Java 对照：Java 基本类型值传递，对象是引用传递（但引用本身是值传递）
//            C++ 三种方式都可以显式控制
// ═══════════════════════════════════════════════════════════════
#include <iostream>
#include <windows.h>

void byValue(int32_t x) {
	// x 是 a 的副本，有自己的地址，修改 x 不影响 a
	// 但 x 的地址在整个函数内是不变的（栈上分配的位置固定）
	std::cout << "byValue x的地址: " << &x << std::endl;
	std::cout << "byValue: " << x << std::endl;
}

void byPointer(int32_t* x) {
	// x 是指针变量，存的是 a 的地址，x 本身也有地址
	// 修改 *x 就修改了 a
	// x 的地址在整个函数内是不变的（栈上分配的位置固定）
	std::cout << "byPointer x的地址（指针变量本身）: " << &x << std::endl;
	std::cout << "byPointer x存的值（a的地址）: " << x << std::endl;
	*x = 5;
	std::cout << "byPointer: " << *x << std::endl;
}

void byReference(int32_t& x) {
	// x 是 a 的引用，没有自己的地址
	// &x 就是 a 的地址，和 main 里的 &a 完全一样
	// 修改 x 就直接修改了 a
	std::cout << "byReference &x（就是a的地址）: " << &x << std::endl;

	// 用指针来证明 &x 的地址就是 a 的地址
	int32_t* ptr_to_x = &x;  // ptr_to_x 存的是 &x 的值
	std::cout << "byReference ptr_to_x（&x的值）: " << ptr_to_x << std::endl;
	std::cout << "byReference &ptr_to_x（ptr_to_x的地址）: " << &ptr_to_x << std::endl;

	x = 5;
	std::cout << "byReference: " << x << std::endl;
}

int main()
{
	SetConsoleOutputCP(CP_UTF8);  // 设置控制台输出为 UTF-8
	auto a = int32_t{ 10 };
	std::cout << "main 里 a 的地址: " << &a << std::endl;

	// 用指针来存 a 的地址
	int32_t* pa = &a;
	std::cout << "main 里 pa（&a的值）: " << pa << std::endl;
	std::cout << "main 里 &pa（pa的地址）: " << &pa << std::endl;
	std::cout << std::endl;

	byValue(a);
	std::cout << "after byValue: " << a << std::endl;
	std::cout << std::endl;

	byPointer(&a);
	std::cout << "after byPointer: " << a << std::endl;
	std::cout << std::endl;

	byReference(a);
	std::cout << "after byReference: " << a << std::endl;
}
