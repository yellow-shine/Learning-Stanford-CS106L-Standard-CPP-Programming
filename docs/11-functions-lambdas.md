# 11 · 函数与 Lambda：把行为传给算法

[上一课](10-function-templates.md) · [目录](../README.md) · [下一课](12-operators.md)

## 先修与目标

理解函数模板、迭代器和引用。学会给算法提供条件或转换，并区分 lambda 捕获的副本和借用。读完可准备 [A4 Ispell](assignments.md)。

## 可调用对象不只有普通函数

普通函数、函数指针、定义了 `operator()` 的对象、lambda 都可以用调用语法执行。函数对象可以保存状态；lambda 是编译器为你创建匿名函数对象的简洁语法。

`[limit](int x) { return x > limit; }` 包含捕获列表、参数和函数体。limit 按值复制进闭包；`[&limit]` 则借用外部变量。无捕获 `[]` 不使用外部局部变量；`[=]` 和 `[&]` 默认捕获虽然短，但容易掩盖借用关系，初学与异步代码优先显式列出。

值捕获的对象通常不能在 lambda 内修改；`mutable` 可以允许修改闭包自己的副本，不会因此改变外部原对象。引用捕获可以观察之后的外部变化，却也有悬空风险：回调比被捕获的局部变量活得长就危险。

## 完整示例

源文件：[11-functions-lambdas.cpp](../examples/11-functions-lambdas.cpp)。

```cpp
#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <vector>

int main() {
    std::vector<int> readings{12, 5, 20, 8};
    int threshold = 10;
    auto above = [threshold](int value) { return value > threshold; };
    auto live_above = [&threshold](int value) { return value > threshold; };
    threshold = 15;
    assert(above(12) && !live_above(12));
    std::vector<int> selected;
    std::copy_if(readings.begin(), readings.end(),
                 std::back_inserter(selected), above);
    assert((selected == std::vector<int>{12, 20}));
    std::vector<int> doubled;
    std::transform(selected.begin(), selected.end(),
                   std::back_inserter(doubled),
                   [](int value) { return value * 2; });
    std::sort(doubled.begin(), doubled.end(),
              [](int a, int b) { return a > b; });
    assert((doubled == std::vector<int>{40, 24}));
    std::cout << doubled.front() << '\n';
}
```

输出 40。above 在创建时复制 10，之后 threshold 改成 15 不影响它；live_above 借用现有变量，所以判断结果变化。两个 lambda 都在 threshold 销毁前使用，安全。

`copy_if` 选择元素，`transform` 映射元素，`sort` 排序。输出容器起初为空，所以使用 `back_inserter` 把写操作变成追加；不能把空 vector 的 begin 当成已分配元素的可写区间。比较器必须是严格弱序，不能用 `a >= b`，因为自身不能严格小于自身。

## C++20 补充：ranges 与惰性 view

A4 使用 C++20 ranges。view 通常描述如何观察底层数据，不一定立即创建一个拥有全部结果的新容器。遍历时才计算，底层数据和捕获对象因此必须活得足够久。

以下是独立的 **C++20 语法片段，不纳入 C++17 示例脚本**：

```cpp
// 需要 <ranges> 和一个仍然存活的 numbers 容器。
// auto even = numbers | std::views::filter([](int x) { return x % 2 == 0; });
// for (int x : even) { /* 此时才访问满足条件的元素 */ }
```

管道不是线程管道；只是把范围适配器组合起来。filter 保留符合条件的元素，transform 改变观察到的值；需要真正拥有结果时，要物化为容器。`std::ranges::to` 是 C++23，A4 明确要求只使用到 C++20，因此不要直接从较新教程抄它。

## 什么时候用 std::function

模板参数通常能接受多种可调用对象，保留具体类型。`std::function<R(Args...)>` 则把不同可调用对象统一成一个运行时类型，可能产生额外开销且对目标有要求。需要存储可替换回调时再考虑，不要给每个局部 lambda 都包一层。

## 易错点与 BusTub 联系

- 异步任务捕获局部引用，函数返回后再执行，容易悬空；`[this]` 也不会延长当前对象寿命。
- 使用 `<cctype>` 的 `std::isspace`/`std::isalpha` 时，除 EOF 外参数须可表示为 unsigned char；处理任意 char 应先转换，避免负 char 导致未定义行为。
- 数据库谓词、比较器和回调都属于行为参数；这个能力不等于已经掌握查询优化算法。

## 自测

1. 示例 threshold 改为 15 后，above 为什么仍用 10？
2. 为什么向空 vector 的 begin 写 transform 结果不安全？

<details>
<summary>参考答案</summary>

1. 创建闭包时按值捕获，保留副本；引用捕获才观察原变量。
2. 空 vector 没有可写元素。算法不会自动增加容器大小，back_inserter 才把输出转为追加。

</details>

## 来源

- [官方 Functions & Lambdas](https://web.stanford.edu/class/cs106l/lectures/2026Spring-11-LambdasAndFunctors.pdf)
- [Lambda 参考](https://en.cppreference.com/w/cpp/language/lambda)
- [来源与版本](sources.md)
