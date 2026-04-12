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
