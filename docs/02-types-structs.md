# 02 · 类型与结构体：把数据组织成对象

[上一课](01-welcome.md) · [目录](../README.md) · [下一课](03-initialization-references.md)

## 先修与目标

先读第 01 课。学完应能区分类型与对象，用结构体表达一条记录，判断复制后两个对象是否互相影响。

## 类型不是标签，而是操作规则

`int` 表示整数，`double` 表示浮点数，`bool` 表示真假，`std::string` 管理字符串。类型决定允许的操作、转换与存储规则。例如整数的 `+` 做加法，字符串的 `+` 做拼接；`3 / 2` 是整数除法，结果为 1，而 `3.0 / 2` 得到 1.5。

`auto n = 3;` 让编译器根据初始化表达式推导 `n` 为 `int`，不是动态类型。之后给 `n` 赋其他类型时仍要遵守向 `int` 转换的规则。`auto` 也不自动成为引用，这一点下一课会反复使用。

初始化创建对象并给出初值，赋值修改已有对象。`int n = 3;` 有等号但仍是初始化。花括号常能防止窄化转换：`int n{3.5};` 是**故意不合法的例子，会编译失败**；不要把它加入可运行示例。

## 为什么需要 struct

把书名、页数、借出状态放进三个无关变量，编译器不知道它们属于同一本书。定义 `struct Book` 是定义新的类型；`Book original{...}` 才是在创建这个类型的对象。

本例使用聚合初始化：按成员声明顺序给值，不需要手写构造函数。`int pages{};` 和 `bool borrowed{};` 是默认成员初始化器，避免忘填时得到不确定的整数状态。普通 `struct` 的成员默认 public，可通过点号访问；`class` 的默认访问权限是 private，除此以外两者都能有函数和构造函数。

## 完整示例

源文件：[02-types-structs.cpp](../examples/02-types-structs.cpp)。运行方法见[目录](../README.md)。

```cpp
#include <cassert>
#include <iostream>
#include <string>

struct Book {
    std::string title;
    int pages{};
    bool borrowed{};
};

int main() {
    Book original{"Database Systems", 500, false};
    Book copy = original;
    copy.title = "Operating Systems";
    copy.borrowed = true;
    assert(original.title == "Database Systems");
    assert(!original.borrowed && copy.borrowed);
    Book empty{};
    assert(empty.pages == 0 && !empty.borrowed);
    auto pages = original.pages;
    pages += 1;
    assert(original.pages == 500 && pages == 501);
    std::cout << original.title << ": " << original.pages << '\n';
}
```

输出 `Database Systems: 500`。`copy` 创建时复制各成员。`std::string` 的复制保留独立的字符串值，整数和布尔也复制值；所以改变 `copy` 不改变 `original`。这不是“结构体总是递归深复制”的规则：若成员是原始指针，默认复制只会复制地址。

`Book empty{}` 使用默认成员初始化器，字符串默认构造为空，整数为 0，布尔为 false。这里可以安全读取；不要推广成“任何局部变量都自动为零”。例如未初始化的普通局部 `int` 不能直接读取。

## 易错点与 BusTub 联系

- `std::string` 不是裸 `char*`：它有自己的值语义和资源管理。
- `sizeof`、整数宽度和有符号性不是随意猜的。需要固定宽度时查看 `<cstdint>`，如 `std::int32_t`；项目中的别名以源码定义为准。
- 浮点数不能精确表示所有十进制小数，不适合不加设计地表示金额。
- 记录对象的内存布局不是磁盘格式；不要把包含 `std::string` 的结构体直接按字节写入数据库文件。它可能包含地址和实现细节，必须序列化。

## 自测

1. `auto x = original.pages; x = 0;` 会不会改变 `original.pages`？
2. 把 `Book` 换成 `class Book`，其余一字不改，外部还能按本例直接访问成员吗？

<details>
<summary>参考答案</summary>

1. 不会。`x` 是独立的 `int` 副本，不是引用。
2. 不能，成员默认 private，且本例的聚合初始化条件也发生变化。不要把 struct/class 的拼写替换当作纯装饰。

</details>

## 来源

- [官方 Types & Structs](https://web.stanford.edu/class/cs106l/lectures/2026Spring-02-TypesAndStructs.pdf)
- [聚合初始化参考](https://en.cppreference.com/w/cpp/language/aggregate_initialization)
- [来源与版本](sources.md)
