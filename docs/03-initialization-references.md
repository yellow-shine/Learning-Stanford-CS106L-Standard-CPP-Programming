# 03 · 初始化、引用与生命周期

[上一课](02-types-structs.md) · [目录](../README.md) · [下一课](04-streams.md)

## 先修与目标

先读类型和结构体。重点不是记住 `&` 的形状，而是每遇到一个变量就问：它是独立对象，还是另一个对象的别名？目标对象还活着吗？

## 初始化与赋值不是同一个阶段

`int x = 10;`、`int x(10);`、`int x{10};` 都是初始化。`x = 20;` 才是对已有对象赋值。花括号限制窄化转换，但不同语法可能选择不同构造函数，例如 `vector<int>(3, 10)` 是三个 10，而 `vector<int>{3, 10}` 是两个元素。

普通局部引用必须初始化：`int& r = x;` 把 `r` 绑定为 `x` 的别名。之后 `r = y;` 给 `x` 赋值，不会让引用改绑到 `y`。引用不像指针变量那样提供“换个地址存进去”的操作。

## 引用与指针的四个区别

| 问题 | `int& r = x` | `int* p = &x` |
| --- | --- | --- |
| 如何访问对象 | `r` | `*p` |
| 能改变目标吗 | 不能重新绑定 | 可令 `p = &y` |
| 能表示没有对象吗 | 没有合法空引用 | 可取 `nullptr` |
| 谁管理生命周期 | 普通引用不管理 | 普通指针也不管理 |

`&x` 在表达式里取地址，`int&` 在声明里表示引用，两种上下文不要混淆。`*p` 是解引用：访问地址所指的对象。引用并非天然安全，指向已销毁对象的引用和指针都可能悬空。

## 值传递、引用传递与存储方式

函数参数 `int x` 是副本，`int& x` 借用并可修改调用者的对象，`const int& x` 只读借用。对于小整数，按值通常最直接；对于不需要复制的大对象，常用只读引用。

特别注意你之前困惑的情况：参数是 `const std::string&`，成员是 `std::string`，那么初始化成员时会复制字符串。**传参方式不决定成员存储方式**。反过来，若成员真的声明成引用或指针，它会继续依赖外部对象的生命周期。

`const` 引用不是快照。不能通过它修改对象，不代表原对象不会被其他路径修改。`auto x = object` 通常得到值，`auto& x = object` 才明确要求引用。

## 完整示例

源文件：[03-initialization-references.cpp](../examples/03-initialization-references.cpp)。

```cpp
#include <cassert>
#include <iostream>
#include <string>

struct Label {
    std::string text;
    explicit Label(const std::string& input) : text(input) {}
};

void update(int x, int& y) {
    x += 10;
    y += x;
}

int make_value() {
    int local = 42;
    return local;
}

int& identity(int& value) {
    return value;
}

int main() {
    int a = 1;
    int b = 2;
    int copy = a;
    int& alias = b;
    alias = 6;
    update(a, b);
    assert(a == 1 && copy == 1 && b == 17);
    const int& read_only = b;
    b = 20;
    assert(read_only == 20);
    identity(a) = 9;
    assert(a == 9 && make_value() == 42);
    std::string title = "Database Systems";
    Label label{title};
    title = "Operating Systems";
    assert(label.text == "Database Systems");
    std::cout << label.text << '\n';
}
```

`update` 内的 `x` 从 1 变成 11，但 `a` 不变；`y` 引用 `b`，所以 6 加 11 得到 17。`make_value` 返回后局部 `local` 已销毁，返回值仍可初始化调用者自己的对象。`identity` 返回引用安全，是因为调用者的 `a` 仍然存在；不是因为“返回引用永远安全”。

## 生命周期的边界

下面是**错误示意，不要运行**：`int& bad() { int n = 42; return n; }`。返回局部变量引用后，`n` 已结束生命周期，读取该引用是未定义行为，不能通过“这次恰好输出 42”证明安全。

直接用临时值初始化某些局部 const 引用确实有生命周期延长规则，如 `const std::string& s = std::string{"hi"};`；但不能因此认为把任何引用传来传去都会继续延长生命。初学阶段优先返回值，不要依赖复杂的延长规则设计接口。

## 易错点与 BusTub 联系

- 把容器元素的地址保存起来之前，既检查容器寿命，也检查后续操作是否使地址失效。
- 按值返回对象不会自动悬空；现代 C++ 还可以消除许多不必要的复制。
- 数据库页面的裸指针可能只是借用，不能看到指针就 `delete`。由谁分配、谁释放必须看接口合同。

## 自测

1. `int a=1,b=2; int& r=a; r=b; b=9;` 后 `a` 和 `r` 是多少？
2. 为什么示例修改 `title` 不影响 `label.text`，虽然构造函数参数是引用？

<details>
<summary>参考答案</summary>

1. 都是 2。`r=b` 复制整数值给 `a`，没有改绑。
2. `text` 是拥有字符串值的成员。构造期间 `input` 借用 `title`，`text(input)` 复制其内容，之后两者独立。

</details>

## 来源

- [官方 Initialization & References](https://web.stanford.edu/class/cs106l/lectures/2026Spring-03-InitializationAndReferences.pdf)
- [对象生命周期](https://en.cppreference.com/w/cpp/language/lifetime)
- [来源与版本](sources.md)
