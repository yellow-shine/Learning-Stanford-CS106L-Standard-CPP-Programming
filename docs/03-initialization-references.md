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



这三点围绕一个共同主题：

> **C++ 里“我手上有一个地址/引用/指针”不等于“我拥有这个对象”，也不等于“这个地址以后一直有效”。**

### 1. 容器还活着，不代表元素地址仍然有效

先保存一个容器元素的地址：

```cpp
std::vector<int> nums = {1, 2, 3};

int *p = &nums[0];
```

此时 `*p == 1` 没问题。但如果继续添加元素：

```cpp
nums.push_back(4);
nums.push_back(5);
```

`vector` 可能发现原来的内存不够，于是重新分配一块更大的连续内存：

```text
原来：

nums
  ↓
[1][2][3]
 ↑
 p


扩容后：

旧内存：
[?][?][?]   ← 已释放
 ↑
 p          ← 还指着这里，悬空

新内存：
[1][2][3][4][5]
```

于是 `*p` 可能已经是未定义行为。

所以即使 `nums` 这个容器本身还活着，也不能保证 `&nums[0]` 一直有效。这里要同时检查两件事：

1. 容器本身有没有销毁。
2. 容器后续操作会不会让元素地址失效。

`push_back()`、`emplace_back()`、`insert()`、`reserve()` 都可能触发重新分配（reallocation），使所有元素地址失效。即使没有重新分配，`insert()` 也会使插入位置及之后的元素地址失效。

这在 BusTub 里非常重要，因为你经常会看到：

```cpp
Page *page = ...
Tuple *tuple = ...
char *data = ...
```

如果这个指针其实指向某个容器、某个 page buffer、某个 frame 内部，那么你就必须知道：

> 后续操作会不会导致底层对象搬家、释放或者被复用。

### 2. 按值返回对象，不会自动悬空

下面的按值返回是安全的：

```cpp
std::string make_name() {
  std::string name = "Alice";
  return name;
}
```

局部变量在函数结束后不是会销毁吗？关键在于 `return name;` 是**按值返回**，不是返回引用：

```cpp
std::string x = make_name();
```

`x` 是调用者自己的 `std::string` 对象，不会因为函数返回而悬空。未发生 NRVO 时，局部 `name` 在函数退出时销毁；发生 NRVO 时，`name` 直接构造在返回结果的位置，不存在一个另行销毁的局部副本。

对比下面的**错误示例，不要运行**：

```cpp
std::string &make_name() {
  std::string name = "Alice";
  return name;   // ❌
}
```

这里返回的是 `name` 的别名，而不是独立对象。局部 `name` 销毁后，返回的引用就悬空了。

| 返回方式 | 调用者得到什么 | 函数返回后 |
| --- | --- | --- |
| `std::string` | 自己的字符串对象 | 仍然有效 |
| `std::string&`，引用局部变量 | 局部对象的别名 | 引用悬空 |

现代 C++ 还有 RVO / NRVO / move semantics，所以 `return name;` 通常并不会产生你想象中的“创建 name → 复制一次 → 再复制一次 → 再复制给调用者”，编译器经常可以直接把对象构造到调用者需要的位置。

所以初学时不要为了“避免复制”而过早返回 `const T&` 或 `T*`。很多时候，`T foo();` 反而是最安全、最简单的接口。

### 3. 页面指针只是借用，不要随意 delete

以下使用 BusTub 旧版 `FetchPage` / `UnpinPage` 接口说明；具体接口以所用版本为准。

比如你可能遇到：

```cpp
Page *page = buffer_pool_manager->FetchPage(page_id);
```

看到 `Page *`，不要直接推断“用完应该 `delete page`”。关键不是它是不是指针，而是**对象由谁创建、内存归谁所有、谁负责释放**。

假设 Buffer Pool 内部是这样：

```text
BufferPoolManager
       │
       │ owns
       ▼
+-------------------------+
| frame 0 : Page          |
| frame 1 : Page          |
| frame 2 : Page          |
| frame 3 : Page          |
+-------------------------+
            ↑
            │
         Page *page
```

**BufferPoolManager 拥有 Page，你的代码只是借用 `Page*`。** 接口没有把所有权转交给你，因此不能 `delete page`。

例如，Buffer Pool 可能一开始就整体分配页面数组，再返回其中一个元素的地址：

```cpp
pages_ = new Page[pool_size];
Page *page = &pages_[frame_id];
```

你拿到的是数组里的一个元素：

```text
pages_

+--------+--------+--------+
| Page 0 | Page 1 | Page 2 |
+--------+--------+--------+
           ↑
           page
```

这时候执行 `delete page;` 完全错误。正确的生命周期管理通常由 Buffer Pool 自己负责。

#### Pin / Unpin：结束借用，不是释放内存

BusTub 用 **pin/unpin** 表示页面是否仍在使用。获取一个 page：

```cpp
Page *page = bpm->FetchPage(page_id);
```

成功获取后，页面被 pin，表示“我暂时需要使用它，请先别把这个 frame 换出去”。当 `pin_count > 0` 时，frame 不允许被驱逐（eviction）。这并不转移所有权。

使用完后归还这次 pin：

```cpp
bpm->UnpinPage(page_id, is_dirty);
```

| 操作 | 含义 |
| --- | --- |
| `delete page` | 销毁对象并释放内存；这里不允许这样做 |
| `UnpinPage(...)` | 结束本次使用；对象仍归 Buffer Pool 所有 |

当 pin count 降为 0 后，frame 可能被驱逐或复用，不能再依赖原指针继续访问原页面。这就是所有权（ownership）与借用（borrowing）的区别。

#### reinterpret_cast：转换指针，不是创建对象

比如你以后在 BusTub 里可能看到类似：

```cpp
auto *page = reinterpret_cast<TablePage *>(raw_page->GetData());
```

这个 `TablePage *` 更不能随便 `delete page;`，因为它可能只是：

```text
raw_page->GetData()
       │
       ▼
+--------------------------------+
| Buffer Pool 中某个 frame       |
|                                |
| [ raw bytes ................. ]|
|   ↑                            |
|   reinterpret_cast<TablePage*> |
+--------------------------------+
```

这个 `TablePage *`：

- 没有自己分配内存；
- 没有创建新的 `TablePage`；
- 只是把已有内存“解释成 `TablePage`”。

所以 `delete page;` 完全错误。

### 小结：先问所有权，再问有效期

| 指针角色 | 含义 | 示例 | 释放责任 |
| --- | --- | --- | --- |
| Owning pointer（拥有） | 拥有对象，负责其生命周期 | `auto *p = new Page();` | 未转移所有权时，由当前代码负责 `delete p` |
| Borrowed pointer（借用） | 只访问对象，不负责释放 | `Page *p = bpm->FetchPage(id);` | 由 Buffer Pool 管理，调用者负责按约定 unpin |

这就是为什么 C++ API 的“合同”非常重要。光看到 `T*`，你其实不知道：

1. 谁拥有对象？
2. 指针的有效期有多长？
3. 能不能保存下来？
4. 能不能 `delete`？
5. 哪些操作会让它失效？

这些都必须由接口约定告诉你。

你前面学的 `T`、`T&`、`const T&`、`T*`、`const T*`，真正到了 BusTub 以后，其实就是不断在回答两个问题：

> **这个对象是谁的？这个对象还能活多久？**

这两个问题基本就是理解 BusTub 大量 C++ 指针代码的核心。

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
