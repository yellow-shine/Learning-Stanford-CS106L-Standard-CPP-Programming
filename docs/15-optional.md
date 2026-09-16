# 15 · optional 与类型安全：没有结果也是一种结果

[上一课](14-move.md) · [目录](../README.md) · [下一课](16-raii-build.md)

## 先修与目标

理解值、引用、模板和 lambda。目标是用类型表达“可能没有值”，不把合法的 0、空字符串和查找失败混为一谈。官方 [A6](assignments.md) 还要求 C++23 单子操作，本课先掌握 C++17 基础。

## 为什么不用特殊数值表示失败

查找一个位置，返回 -1 可能暂时可行；但如果结果本来允许负数，或者类型改成无符号整数，这种约定很脆弱。`std::optional<T>` 明确表示两种状态：包含一个 T，或者没有值。`std::nullopt` 表示缺失。

optional 是值包装，不是自动动态分配的指针，也不是“默认的 T”。`optional<int>{0}` 有值且值为 0；空 optional 的布尔检查为 false。读取前使用 `if (result)` 或 `has_value()`；`value()` 在空时抛 `bad_optional_access`，`*result` 在 C++17 空值上不能使用，不能依靠它抛异常。

## 完整示例

源文件：[15-optional.cpp](../examples/15-optional.cpp)。返回负数的绝对值，但拒绝最小 int，以免取反溢出。

```cpp
#include <cassert>
#include <iostream>
#include <limits>
#include <optional>

std::optional<int> safe_magnitude(int value) {
    if (value == std::numeric_limits<int>::min()) {
        return std::nullopt;
    }
    return value < 0 ? -value : value;
}

int main() {
    const auto present = safe_magnitude(-7);
    assert(present && *present == 7);
    const auto zero = safe_magnitude(0);
    assert(zero.has_value() && *zero == 0);
    const auto absent = safe_magnitude(std::numeric_limits<int>::min());
    assert(!absent && absent.value_or(99) == 99);
    bool threw = false;
    try {
        (void)absent.value();
    } catch (const std::bad_optional_access&) {
        threw = true;
    }
    assert(threw);
    std::cout << present.value_or(0) << '\n';
}
```

输出 7。检查 0 的案例很重要：有值不等于值非零。示例把不可表示的结果解释为缺失；如果应用需要告诉调用者具体失败原因，optional 就不够，应该设计带错误信息的返回类型。

## C++23 补充：三个单子操作

这些不是 C++17 的 optional 成员，A6 使用 `-std=c++23` 正是因为它们：

| 操作 | 有值时 | 没值时 | 回调返回类型 |
| --- | --- | --- | --- |
| `transform` | 转换其中的值并包装 | 继续为空 | 普通 U |
| `and_then` | 调用可能失败的下一步 | 继续为空 | `optional<U>` |
| `or_else` | 保留原值 | 计算替代 optional | 与当前 optional 相匹配的类型 |

区别在于是否引入新的“可能失败”步骤，而不是名字长短。比如一个可选测量值可以 transform 为显示字符串；若下一步解析本身也可能失败，则用 and_then。or_else 的回调不接收旧值，因为调用它时本来就没有值。

A6 README 中简化签名和示例存在笔误，例如 `std::end` 不能当输出换行使用，or_else 的描述不能当精确模板声明。以上语义以标准库为准；作业输出要求仍以其 README 为准。这里不组合出 A6 的最终管道。

`value_or(expensive())` 的参数表达式会在调用前求值，不是“没值才执行 expensive”；需要惰性替代计算时考虑条件分支或适用的 or_else。

## 类型安全的进一步选择

若结果可能是几种明确类型，C++17 `std::variant` 比 `void*` 加手动标签更能限制错误组合，访问常用 `std::visit`。`enum class` 可避免不同枚举或整数无意混用。C++23 `std::expected<T,E>` 可表达成功值或错误信息；不是本地 C++17 示例的前提，也不保证目标 BusTub 版本可以使用。

## 易错点与 BusTub 联系

- C++17 标准 optional 不支持引用类型 `optional<T&>`；需要借用时另行设计，例如 `reference_wrapper`，并明确寿命。
- optional 有值只证明包装中存在对象，不证明对象内部的借用指针仍有效。
- 查询无匹配与磁盘读取失败不是同一语义，不要都吞成一个 nullopt 而丢掉必须处理的错误。

## 自测

1. 空 optional 和 `optional<int>{0}` 的布尔检查结果分别是什么？
2. 回调返回 `optional<U>`，应优先考虑 transform 还是 and_then？

<details>
<summary>参考答案</summary>

1. false 和 true；检查的是有无值，不是值本身的真假。
2. and_then，用于衔接可能没有结果的步骤，避免 transform 形成嵌套 optional。

</details>

## 来源

- [官方 optional & Type Safety](https://web.stanford.edu/class/cs106l/lectures/2026Spring-15-Optional%26TypeSafety.pdf)
- [std::optional 标准库参考](https://en.cppreference.com/w/cpp/utility/optional)
- [来源与版本](sources.md)
