# 12 · 运算符重载：让自定义类型像它的含义那样工作

[上一课](11-functions-lambdas.md) · [目录](../README.md) · [下一课](13-special-members.md)

## 先修与目标

理解类、const 和普通函数。目标是为类型定义自然的比较、输出等行为，而不是为了缩短代码任意赋予符号新含义。

## 运算符本质仍是函数

`a == b` 可以调用 `operator==`，`out << object` 可以调用 `operator<<`。重载不能创造新运算符，也不能改变运算符优先级或参数个数。至少一个操作数需要是类或枚举类型，不能重写两个普通 int 的加法。

成员函数中左操作数是当前对象，非成员函数则显式接收两个操作数。流输出的左边是 `std::ostream`，不能把它作为你的类的成员函数，因此通常写非成员 `operator<<`，必要时用 friend 授权访问 private 数据。

friend 不是成员函数，也不是“任何地方都能访问所有私有数据”；它只授予被声明的特定函数或类访问权。若 public getter 足够，也可以不用 friend。

## 完整示例

源文件：[12-operators.cpp](../examples/12-operators.cpp)。这个类型只表达一个标识符，不实现官方 Treebook 的任何逻辑。

```cpp
#include <cassert>
#include <iostream>
#include <set>
#include <sstream>

class RecordId {
public:
    explicit RecordId(int value) : value_(value) {}
    bool operator==(const RecordId& rhs) const {
        return value_ == rhs.value_;
    }
    bool operator<(const RecordId& rhs) const {
        return value_ < rhs.value_;
    }
    friend std::ostream& operator<<(std::ostream& out, const RecordId& id) {
        return out << "RecordId(" << id.value_ << ')';
    }
private:
    int value_;
};

int main() {
    RecordId a{2};
    RecordId b{5};
    assert(a < b && a == RecordId{2});
    std::set<RecordId> ids{b, a, a};
    assert(ids.size() == 2 && *ids.begin() == a);
    std::ostringstream out;
    out << a << ' ' << b;
    assert(out.str() == "RecordId(2) RecordId(5)");
    std::cout << out.str() << '\n';
}
```

输出 `RecordId(2) RecordId(5)`。输出函数返回原流引用，所以可以继续链接 `<<`；不要在里面硬编码 `std::cout`，否则字符串流、文件流都无法正常复用。输出函数没有擅自添加换行，由调用者决定布局。

`operator<` 末尾 const 表示比较不修改左边对象，参数 const 引用表示不修改右边对象。set 会通过比较关系判断等价：既不是 a<b，也不是 b<a，就认为等价，不是必须调用 `operator==`。

## 设计合同

排序关系应满足严格弱序。例如 a<a 必须为 false，若 a<b 且 b<c 则 a<c。比较规则如果依赖外部不断变化的状态，可能让容器中已有元素的顺序失去意义。

对于算术式类型，`+=` 常修改自身并返回自身引用，`+` 常返回新对象；但不要把这样的常见约定误认为编译器强制行为。C++20 可以使用 `<=>` 与某些默认比较功能，本课 C++17 示例显式实现所需操作。

## 易错点与 BusTub 联系

- 键比较不是显示格式比较：按字符串输出排序可能把 10 放在 2 前面。
- operator 重载仍可能抛异常、复制对象、访问悬空引用；符号简短不代表成本为零。
- 数据库键需要一致的相等、排序和哈希语义；哈希容器要求相等键得到相等哈希值，不要求不同键一定没有碰撞。

## 自测

1. 为什么输出运算符返回 `std::ostream&` 而不是 void？
2. set 去重一定使用 `operator==` 吗？

<details>
<summary>参考答案</summary>

1. 允许链式输出，并继续操作调用者传入的同一个流对象。
2. 不一定。set 使用比较器定义的等价关系：两方向比较都为 false。

</details>

## 来源

- [官方 Operator Overloading](https://web.stanford.edu/class/cs106l/lectures/2026Spring-12-OperatorOverloading.pdf)
- [运算符重载参考](https://en.cppreference.com/w/cpp/language/operators)
- [来源与版本](sources.md)
