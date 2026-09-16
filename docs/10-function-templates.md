# 10 · 函数模板：复用算法而不是复制代码

[上一课](09-class-templates-const.md) · [目录](../README.md) · [下一课](11-functions-lambdas.md)

## 先修与目标

先理解类模板和迭代器。学会从参数推导模板类型，并识别泛型函数对传入类型的实际要求。泛型不等于“任何类型都能用”。

## 把变化点写成参数

如果两个函数只有 int/string 不同，可以把类型提取为模板参数 T。但算法使用的操作也会成为隐含约束：调用 `a < b` 就要求该表达式合法；复制 T 就要求 T 可复制。

`template <typename T> T smaller(T a, T b)` 可以从两个 int 参数推导 T=int。如果传入 int 和 double，同一个 T 的推导可能冲突，不是编译器总会挑一个“更大的类型”。可以明确转换、显式指定模板参数，或重新设计多类型接口；不要在不理解精度损失时强制转换消除错误。

标准库算法不绑定某个容器，而是接收迭代器范围。这样 vector、list 或数组只要满足迭代器要求，就能复用遍历逻辑。算法本身不拥有底层元素，也不延长它们的寿命。

## 完整示例

源文件：[10-function-templates.cpp](../examples/10-function-templates.cpp)。

```cpp
#include <cassert>
#include <cstddef>
#include <iostream>
#include <list>
#include <string>
#include <vector>

template <typename T>
T smaller(T a, T b) {
    return b < a ? b : a;
}

template <typename InputIt, typename T>
std::size_t count_equal(InputIt first, InputIt last, const T& target) {
    std::size_t result = 0;
    for (; first != last; ++first) {
        if (*first == target) {
            ++result;
        }
    }
    return result;
}

int main() {
    assert(smaller(4, 7) == 4);
    assert(smaller(std::string{"b"}, std::string{"a"}) == "a");
    std::vector<int> values{1, 2, 1};
    std::list<int> linked{1, 3, 1, 1};
    assert(count_equal(values.begin(), values.end(), 1) == 2);
    assert(count_equal(linked.begin(), linked.end(), 1) == 3);
    assert(count_equal(values.begin(), values.begin(), 1) == 0);
    std::cout << count_equal(linked.begin(), linked.end(), 1) << '\n';
}
```

输出 3。`count_equal` 不使用 `+n` 或下标，因此链表迭代器也可用。first 是迭代器的副本，自增不会修改调用者保存的迭代器变量，也不会改动容器内容。空范围自然返回 0。

这是为了理解算法接口而写的练习；实际代码直接使用 `std::count`，不要重复实现标准库已有功能。

## 返回值与返回引用

smaller 按值返回，调用者获得自己的对象。若为了“省复制”把返回类型改成 `const T&`，却仍按值接收参数，就会返回局部参数的引用，函数结束后悬空。即使参数也改为引用，传入临时对象时仍需要仔细分析调用者如何保存结果。

优先让接口清楚，再考虑复制成本。标准库某些返回引用的函数确实有使用限制，查文档比根据函数名猜安全得多。

## C++20 补充：concepts

C++17 的模板要求常体现在较长的实例化错误中。C++20 的 concepts/requires 可以把要求放在接口处，比如要求某类型支持特定比较操作。它们能改进错误信息，但不能证明算法逻辑正确；“满足语法约束”不等于比较器满足数学上的严格弱序。

本课可运行示例保持 C++17，不使用 concepts；读到新语法先确认编译标准，不要简单删除 requires 以求通过。

## 易错点与 BusTub 联系

- 字符串字面量传给按值模板可能推导成指针，用 `<` 比较地址而不是文本。比较文本请使用 string 或明确的文本比较接口。
- 写泛型容器/索引前，说明键比较、复制和生命周期要求，比添加更多模板参数重要。
- 编译错误从你调用模板的位置向内追踪，先看第一个不满足的表达式，不必阅读所有后续报错。

## 自测

1. 为什么此算法能用 list，而 `first + 1` 版本不一定能？
2. `smaller(1, 2.5)` 为什么不能直接推导本例的 T？

<details>
<summary>参考答案</summary>

1. list 迭代器支持递增，但不支持随机访问算术。本例只依赖前者。
2. 一个参数推导出 int，另一个推导出 double，对同一个 T 产生冲突；模板推导不自动执行所有普通隐式转换来协调类型。

</details>

## 来源

- [官方 Function Templates](https://web.stanford.edu/class/cs106l/lectures/2026Spring-10-TemplateFunctions.pdf)
- [模板参数推导](https://en.cppreference.com/w/cpp/language/template_argument_deduction)
- [来源与版本](sources.md)
