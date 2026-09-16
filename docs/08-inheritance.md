# 08 · 可选：继承与动态多态

[上一课](07-classes.md) · [目录](../README.md) · [下一课](09-class-templates-const.md)

## 先修与目标

先读类与引用。本课在官方课表标为可选，但阅读 BusTub 的执行器接口时有用。目标是理解通过基类引用调用派生类行为，而不是为了复用几行代码就建立继承树。

## “是一个”与“有一个”

public 继承常表示替代关系：派生类对象可以在需要基类接口的位置使用。组合表示包含关系：例如日志器有一个输出流，不代表日志器是一个输出流。若只是需要存储另一个对象，优先考虑成员组合。

基类函数加 `virtual`，调用经过基类引用或指针时可以动态选择实际对象的重写版本。派生类写 `override`，让编译器检查签名确实覆盖了基类函数，避免拼错参数或 const 后意外定义新函数。

`virtual int evaluate() const = 0;` 是纯虚函数，让基类成为抽象类：它规定接口，但不能直接构造该基类对象。通过基类指针删除派生对象时，基类需要虚析构函数，否则可能产生未定义行为。不是每个类都需要 virtual；为多态删除设计的基类才需要相应合同。

## 完整示例

源文件：[08-inheritance.cpp](../examples/08-inheritance.cpp)。

```cpp
#include <cassert>
#include <iostream>

class Expression {
public:
    virtual ~Expression() = default;
    virtual int evaluate() const = 0;
};

class Literal final : public Expression {
public:
    explicit Literal(int value) : value_(value) {}
    int evaluate() const override { return value_; }
private:
    int value_;
};

int read_expression(const Expression& expression) {
    return expression.evaluate();
}

int main() {
    Literal literal{42};
    const Expression& base = literal;
    assert(base.evaluate() == 42);
    assert(read_expression(literal) == 42);
    std::cout << read_expression(base) << '\n';
}
```

输出 42。`base` 的静态类型是 `const Expression&`，引用的实际对象是 Literal。调用虚函数时使用实际对象的实现。示例没有动态分配，多态并不要求 `new`。

`final` 表示不允许再从 Literal 派生，本例用它表达“叶子类型”的意图，不是必须写的优化咒语。

## 切片与生命周期

如果基类不是抽象类，把派生对象按值复制到一个基类对象，会只保留基类部分，这叫对象切片。需要多态时传引用或指针，而不是按值接收基类。引用仍有借用寿命问题；多态不会解决悬空。

基类构造先于派生类成员，析构顺序相反。不要在构造/析构期间依赖“最派生对象的虚函数行为”：那个对象部分可能尚未建立或已经销毁。尤其不能用纯虚接口去访问尚未初始化的派生状态。

## 易错点与 BusTub 联系

- 少了 `virtual`，基类接口调用不会因为函数同名就自动动态分派。
- 私有数据仍遵守访问权限，继承不等于派生类可以随意访问所有 private 成员。
- 执行器基类可以规定统一的读取接口，不同执行器提供行为；具体类名、函数签名与项目年份有关，本例不是 BusTub 实现。
- 不要用运行时继承替代所有模板问题，也不要用模板替代所有运行时接口；取决于类型在编译期还是运行期决定。

## 自测

1. 示例没有 new，为什么仍有动态多态？
2. 为什么建议重写函数都加 override？

<details>
<summary>参考答案</summary>

1. 多态来自虚函数以及通过基类引用/指针调用，不来自内存分配方式。
2. 它让签名不匹配成为编译错误，尤其能发现漏写 const 等难查问题。

</details>

## 来源

- [官方可选 Inheritance Practice](https://web.stanford.edu/class/cs106l/lectures/2026Spring-08-Inheritance.pdf)
- [virtual 函数参考](https://en.cppreference.com/w/cpp/language/virtual)
- [来源与版本](sources.md)
