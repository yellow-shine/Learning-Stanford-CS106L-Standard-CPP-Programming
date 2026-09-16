# 07 · Classes：构造有效对象，限制非法修改

[上一课](06-iterators-pointers.md) · [目录](../README.md) · [下一课](08-inheritance.md)

## 先修与目标

先理解结构体、引用和初始化。类的价值不仅是“把函数放进结构体”，而是把数据与必须遵守的规则放在一起。例如分数只能处于 0 到上限之间，外部不应随意写入负数。

## public 是合同，private 是实现

外部通过 public 函数使用对象，不依赖 private 存储细节。private 不等于加密或线程安全，只是编译期访问控制。成员函数里的 `this` 指向当前对象，所以 `score_` 可以理解为 `this->score_`。

构造函数名字等于类名，没有返回类型，负责创建后即可使用的对象。`Score(int initial, ...) : score_(initial), ... {}` 的冒号部分是成员初始化列表：先初始化成员，再进入函数体。成员实际按**声明顺序**初始化，因此列表也按这个顺序写。

在函数体内 `score_ = initial;` 是赋值，发生在成员初始化之后。对 `const` 或引用成员，不能用事后赋值替代必要的初始化。

## explicit 为什么不禁止括号

能用一个参数调用的构造函数可能参与隐式类型转换。`explicit Score(int initial)` 会阻止 `Score s = 10;` 或把 `10` 隐式传给需要 Score 的函数；但 `Score s{10};` 和 `Score s(10);` 都明确创建对象，合法。

explicit 不是每个构造函数的强制装饰，而是表达“不要替调用者猜测转换意图”。本例后两个参数有默认值，所以它仍可用一个整数调用，更应该明确考虑 explicit。

## const 成员函数究竟管什么

`int value() const` 中，前面的 `int` 决定返回一个独立整数值；后面的 `const` 限制函数通过当前对象修改普通成员。它不是修饰返回副本。

没有 const，函数完全可能先执行 `++score_` 再返回副本。加上 const 后，这样修改本例成员会编译失败；同时 `const Score&` 才允许调用该只读函数。即使实现恰好没有修改，编译器也依据函数声明而不是“分析函数体猜测”来决定是否允许调用。

## 完整示例

源文件：[07-classes.cpp](../examples/07-classes.cpp)。

```cpp
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

class Score {
public:
    explicit Score(int initial, int maximum = 100,
                   const std::string& owner = "anonymous")
        : score_(initial), maximum_(maximum), owner_(owner) {
        if (maximum_ < 0 || score_ < 0 || score_ > maximum_) {
            throw std::invalid_argument("invalid score");
        }
    }

    int value() const { return score_; }
    const std::string& owner() const { return owner_; }

    void add(int points) {
        if (points < 0 || points > maximum_ - score_) {
            throw std::out_of_range("score exceeds limit");
        }
        score_ += points;
    }

private:
    int score_;
    int maximum_;
    std::string owner_;
};

int main() {
    Score a{10, 100, "Alice"};
    Score b(10);
    const Score& view = a;
    a.add(5);
    assert(view.value() == 15 && b.value() == 10);
    int result = view.value();
    result = 99;
    assert(view.value() == 15 && result == 99);
    bool rejected = false;
    try {
        a.add(100);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected && a.value() == 15);
    std::cout << view.owner() << ' ' << view.value() << '\n';
}
```

输出 `Alice 15`。构造函数的字符串参数只是借用，`owner_` 自己保存副本。`owner()` 返回对成员的只读引用，避免复制，但调用者不得在 Score 销毁后继续使用该引用。失败的 add 不修改分数。

## 头文件与实现文件

通常 `.h` 中声明类，`.cpp` 中写 `int Score::value() const { ... }`。声明和定义的参数、const 必须一致。头文件需要 include guard 或常用的 `#pragma once` 防止重复包含；后者广泛支持但不是标准指令。第 16 课介绍多文件编译。

## 易错点与 BusTub 联系

- `const` 不是深层不可变：指针成员指向的外部对象不一定被 const 限制；第 09 课细讲。
- `private` 不能阻止类自己的函数破坏不变量，仍要写边界检查。
- 数据库对象常有“已打开、已锁定、已 pin”之类状态合同；类的接口应该让非法状态更难产生。

## 自测

1. 返回 int 副本既然安全，为什么还需要 `value() const`？
2. 初始化列表调换书写顺序，能否改变成员初始化顺序？

<details>
<summary>参考答案</summary>

1. 返回副本与调用期间修改对象是两件事。const 限制后者，也允许只读对象调用。
2. 不能。按类内声明顺序初始化；基类还会先于成员初始化。

</details>

## 来源

- [官方 Classes](https://web.stanford.edu/class/cs106l/lectures/2026Spring-07-Classes.pdf)
- [explicit 说明](https://en.cppreference.com/w/cpp/language/explicit)
- [来源与版本](sources.md)
