# std-move

这时候就出现 `std::move`：

```cpp
std::string a = "hello";

Foo(std::move(a));
```

这里：

```cpp
std::move(a)
```

相当于告诉编译器：

> 虽然 `a` 本来是一个 lvalue，但我现在愿意把它当作一个可以被消耗的 rvalue。

所以：

```cpp
Foo(std::move(a));
```

可以传给：

```cpp
void Foo(std::string &&s)
```

关系是：

```text
a
│
│ 原本是
▼
lvalue

std::move(a)
│
│ 类型转换
▼
rvalue-like expression

于是可以绑定到

std::string&&
```

---

## 转换不等于移动

这一点非常重要。

```cpp
std::move(a)
```

本身不会：

```text
复制内存
搬内存
清空 a
```

它基本只是在说：

> “请把 a 当成右值。”

真正“搬资源”的代码发生在接下来调用的 move constructor / move assignment 中。

例如：

```cpp
std::string b = std::move(a);
```

大概发生：

```text
std::move(a)
      ↓
把 a 当作 rvalue
      ↓
匹配 string(string&&)
      ↓
move constructor 执行
      ↓
b 接管 a 的资源
```

---

## 精确含义与常见误区

- 头文件：`<utility>`。
- `std::move(x)` 将表达式转换为 xvalue（将亡值），本身不转移资源。
- 是否移动取决于重载选择和被调用函数的实现。没有适用的移动操作时可能拷贝，也可能编译失败。
- `std::move` 不移除 `const`；对 const 对象使用它，通常不能绑定到普通的 `T&&` 移动构造参数。
- `T&&` 参数只提供移动的可能性，不会自动消耗传入对象。
- 按值返回对象时可能直接进行拷贝消除，不一定发生移动；不要为了“优化”而随意写 `return std::move(local);`。

## 相关知识

[左值与右值](../01-值与引用/左值与右值.md) · [移动构造函数](移动构造函数.md)

---

来源：01.md、02.md（重复内容已合并）。
