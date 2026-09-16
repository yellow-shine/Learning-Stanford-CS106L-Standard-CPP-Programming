# 09 · 类模板与 const 正确性

[上一课](08-inheritance.md) · [目录](../README.md) · [下一课](10-function-templates.md)

## 先修与目标

先读第 07 课；第 08 课可跳过。学会把“保存一个 int 的类”推广到“保存一个 T 的类”，并为可写与只读调用者提供恰当接口。可回到 [A3](assignments.md) 检查自己的设计。

## 模板是一份生成类型的配方

`template <typename T> class Slot { ... };` 中 T 是类型参数。`Slot<int>` 与 `Slot<std::string>` 是不同类型，不是同一个类在运行时随意改变成员类型。编译器在使用时根据模板定义检查并生成所需的实例。

通常将模板定义放在头文件，让实例化处能看到实现。把定义藏进普通 `.cpp` 而没有显式实例化安排，可能在链接时找不到所需实例。不要因此养成随便 include 所有 cpp 的习惯；课程某些模板布局有专门约定，按其 starter 结构处理。

## const 是调用权限，不是返回策略

可写对象需要 `T& value()`，只读对象需要 `const T& value() const`。两者可作为重载共存：选择取决于当前对象是否 const。前面的 const 限制返回引用，末尾的 const 限制当前对象，位置不同含义不同。

`const int* p` 是“通过 p 不能修改整数，但 p 可改指向”；`int* const p` 是“p 不能改指向，但可修改整数”；`const int* const p` 两者都限制。const 通常不是深层递归不可变：const 对象中的指针值不能随意替换，但所指对象是否可写取决于指针目标类型。

## 完整示例

源文件：[09-class-templates-const.cpp](../examples/09-class-templates-const.cpp)。

```cpp
#include <cassert>
#include <iostream>
#include <string>

template <typename T>
class Slot {
public:
    explicit Slot(const T& value) : value_(value) {}
    T& value() { return value_; }
    const T& value() const { return value_; }
private:
    T value_;
};

int main() {
    Slot<int> number{7};
    number.value() = 9;
    const Slot<int>& view = number;
    assert(view.value() == 9);
    auto snapshot = view.value();
    number.value() = 10;
    assert(snapshot == 9 && view.value() == 10);
    std::string name = "Alice";
    Slot<std::string> text{name};
    name = "Bob";
    assert(text.value() == "Alice");
    int x = 1;
    int y = 2;
    const int* read_only = &x;
    read_only = &y;
    int* const fixed = &x;
    *fixed = 3;
    assert(*read_only == 2 && x == 3);
    std::cout << view.value() << ' ' << text.value() << '\n';
}
```

输出 `10 Alice`。`snapshot` 的 auto 推导成 int，复制了当时的值；`view.value()` 返回引用，随后读取会看到变化。Slot 成员是 T 而不是 `const T&`，因此字符串不依赖构造时借用的源变量。

## 设计选择与限制

返回可写引用会让调用者绕过成员函数修改数据。Slot 的合同就是直接存放任意 T，所以允许；第 07 课的 Score 有范围不变量，就不该暴露 `int&` 给外部随意修改。这不是哪个语法更高级，而是接口承诺不同。

`mutable` 可以允许 const 成员函数修改特定成员，常见于逻辑只读的缓存或互斥量，但不应该拿它绕过所有限制。const 也不是线程安全保证：其他线程可能经另一条可写路径改同一个对象，需要同步。

## 易错点与 BusTub 联系

- 模板只在实际使用时暴露某些类型错误，“模板文件能通过预处理”不等于所有 T 都支持。
- 借用返回引用不能越过持有对象的生命周期，也可能因容器内部变化而失效。
- 模板键类型与只读接口在索引代码中常见，但具体比较与所有权合同须读项目定义。

## 自测

1. 为什么 `const Slot<int>& view` 不能执行 `view.value() = 3`？
2. 如果成员改成 `const T& value_`，构造函数结束后会有什么新风险？

<details>
<summary>参考答案</summary>

1. 选择 const 重载，返回 `const int&`，不允许通过它赋值。
2. Slot 不再拥有值，只借用调用者的对象。调用者对象销毁后引用悬空，传临时对象尤其危险。

</details>

## 来源

- [官方 Class Templates & Const Correctness](https://web.stanford.edu/class/cs106l/lectures/2026Spring-09-TemplateClasses.pdf)
- [类模板参考](https://en.cppreference.com/w/cpp/language/class_template)
- [来源与版本](sources.md)
