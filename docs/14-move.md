# 14 · 移动语义：允许转移，不是强制搬走

[上一课](13-special-members.md) · [目录](../README.md) · [下一课](15-optional.md)

## 先修与目标

先理解复制构造、复制赋值和资源所有权。目标是读懂 `T&&`、`std::move`，并避免对移动后的对象作无根据的假设。

## 为什么需要移动

复制字符串或 vector 通常需要复制元素；如果源对象的内容已经不再需要，可以允许目标接管资源，避免做一份昂贵副本。对象仍有自己的身份，转移的是资源或状态，不是把变量名字搬到另一行。

表达式有值类别。日常先记住：有名字、可定位的对象表达式通常是左值；临时结果可用于移动。`T&&` 是右值引用（模板推导中的特殊情形下一段再说），可为转移资源提供重载入口。

`std::move(x)` 本身不搬运、不释放、不清空 x；它把表达式转换为可被移动重载接收的形式。实际调用的是移动还是复制，取决于类型、const、可用重载以及后续使用方式。`std::move` 一个 const 对象常无法匹配需要修改源的移动构造，最终可能复制。

## 完整示例

源文件：[14-move.cpp](../examples/14-move.cpp)。用标准 unique_ptr 观察明确的所有权转移，不实现官方 A7 的智能指针。

```cpp
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

int main() {
    std::string source = "payload";
    std::string destination = std::move(source);
    assert(destination == "payload");
    // 不断言 source 为空：对 string 通常只保证有效但状态未指定。
    source = "reused";
    assert(source == "reused");
    auto owner = std::make_unique<int>(42);
    int* borrowed = owner.get();
    auto receiver = std::move(owner);
    assert(!owner && receiver && *receiver == 42);
    assert(borrowed == receiver.get());
    std::vector<std::unique_ptr<int>> values;
    values.push_back(std::move(receiver));
    assert(!receiver && *values.front() == 42);
    assert(*borrowed == 42);
    std::cout << destination << ' ' << *values.front() << '\n';
}
```

输出 `payload 42`。string 的移动后内容不拿来猜；重新赋值是合法的。unique_ptr 则有更强的特定合同：转移后源为空，所指对象继续由新所有者持有。borrowed 是借用指针，移动 unique_ptr 没有移动那个 int 对象；但最终所有者释放资源后，borrowed 就不能使用。

## 有名字的右值引用仍是左值表达式

即使参数声明成 `T&& other`，函数体中的名字 `other` 仍是左值表达式。若要把它的成员交给另一个移动操作，需要明确表达转移意图。不要因为看见两个 `&` 就认为函数体里任何使用都会自动移动。

模板中的 `template<class T> void f(T&& x)`，当 T 由调用推导时，可能是**转发引用**，可以接收左值和右值；`std::forward<T>(x)` 保留原调用者的值类别。它和无条件 `std::move(x)` 不同。先能解释普通移动，再读泛型转发，别把 forward 当更高级的 move。

## noexcept 与返回值

移动操作如果可以保证不抛异常，声明 noexcept 可帮助容器在重新分配时选择安全的移动路径。但不应为了性能虚假声明：真正抛出时会终止程序。

返回局部值通常直接写 `return result;`，不要机械加 `std::move`。编译器可能执行返回值优化，某些写法在 C++17 有保证的复制消除；多写 move 反而可能妨碍优化。正确性不依赖“观察到构造函数打印了几次”的偶然结果。

## 易错点与 BusTub 联系

- 移动后对象通常仍可析构和重新赋值，但其他操作要遵守该类型的后置条件，不能泛称所有对象都变空。
- 转移 guard 意味着转移释放责任，不意味着加锁保护的数据也被复制。
- 为避免复制而随处 move 可能提前放弃仍需使用的资源；先画出所有者再写代码。

## 自测

1. 仅执行 `std::move(x);`，不把结果交给任何操作，是否一定改变 x？
2. 本例为什么能断言 unique_ptr 源为空，却不能照搬断言 string 源为空？

<details>
<summary>参考答案</summary>

1. 不会仅因为这个转换就改变对象；移动发生在后续所选择的操作中。
2. 两种类型的移动后合同不同。unique_ptr 明确转移后源为空；string 通常只保证有效但状态未指定。

</details>

## 来源

- [官方 Move Semantics](https://web.stanford.edu/class/cs106l/lectures/2026Spring-14-MoveSemantics.pdf)
- [std::move](https://en.cppreference.com/w/cpp/utility/move)
- [来源与版本](sources.md)
