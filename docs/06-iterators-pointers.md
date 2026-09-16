# 06 · 迭代器与指针：位置不等于所有权

[上一课](05-containers.md) · [目录](../README.md) · [下一课](07-classes.md)

## 先修与目标

理解引用与容器。本课要能读取 `[begin, end)` 范围，安全删除元素，并解释为何“容器仍活着”不能保证元素地址仍有效。之后进入 [A2](assignments.md)。

## 迭代器把容器和算法连接起来

迭代器表示序列中的位置；`*it` 访问元素，`++it` 前进一步，`it == end` 判断是否结束。`begin()` 指向首元素，`end()` 是尾后位置，不是最后一个元素，不能解引用。空容器的 begin 与 end 相等。

标准算法常接收左闭右开范围 `[first, last)`，不包含 last。这样空范围自然写成 first==last，多个相邻范围也容易拼接。

迭代器不一定是裸指针。vector 的迭代器支持随机访问 `it + n`；set 的迭代器只能按它提供的能力前后移动，不能写 `it + 3`。`std::find` 只要求输入迭代能力，`std::sort` 要求随机访问，因此不能直接对 set 调用 sort。

## 原始指针与失效

`int* p = &x` 只是保存地址，不会延长 `x` 的寿命。`new int{42}` 创建动态对象，`delete p` 销毁该对象并释放内存；它不会把其他别名指针自动置空。`new[]` 必须匹配 `delete[]`，但日常代码优先容器和智能指针。

vector 扩容重新分配时，旧元素的引用、指针和迭代器全部失效；即使没有扩容，插入/删除也可能使位置及其后的迭代器失效。不能靠地址打印结果碰巧没变来推导安全性。

set/map 的插入通常不使已有元素迭代器失效，删除会使被删元素的迭代器失效。unordered 容器 rehash 会使迭代器失效，但不使未删除元素的引用和指针失效；要查具体容器与操作，不要用一条口号覆盖所有情况。

## 完整示例

源文件：[06-iterators-pointers.cpp](../examples/06-iterators-pointers.cpp)。

```cpp
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> values{1, 2, 3, 4, 5};
    for (auto it = values.begin(); it != values.end();) {
        if (*it % 2 == 0) {
            it = values.erase(it);
        } else {
            ++it;
        }
    }
    assert((values == std::vector<int>{1, 3, 5}));
    const auto found = std::find(values.begin(), values.end(), 3);
    assert(found != values.end());
    assert(*found == 3);
    int number = 7;
    int* p = &number;
    int* q = p;
    *q = 9;
    assert(*p == 9);
    // p 和 q 借用局部对象，不允许对它们执行 delete。
    std::cout << values.size() << ' ' << number << '\n';
}
```

输出 `3 9`。删除时必须接收 `erase` 返回的下一个有效迭代器，而不是使用刚失效的旧迭代器。删除分支不再自增，否则会跳过元素；未删除分支才自增。查找失败会返回 end，解引用前先判断。

## 易错点与 BusTub 联系

- 范围 for 使用内部迭代器；边遍历 vector 边随意删元素仍然可能出错，看不到迭代器不代表没有迭代器。
- 裸指针非空不等于有效，`nullptr` 检查无法发现已释放对象。
- 数据库页面可能被驱逐或重新使用：内存地址存在，不一定代表你仍拥有访问该逻辑页面的权利。引用寿命、pin 状态和锁属于不同约束。
- 教学中的 `new/delete` 是理解底层规则，不是推荐手写每一个资源管理器。

## 自测

1. 为什么删除元素后不执行原来的 `++it`？
2. `p` 和 `q` 指向同一个动态对象，`delete p` 后只把 `p=nullptr`，能继续使用 `q` 吗？

<details>
<summary>参考答案</summary>

1. 原迭代器可能失效；`erase` 返回新位置，直接再自增还可能跳过下一项。
2. 不能。`q` 仍悬空，修改另一个指针变量不会复活对象或更新所有别名。

</details>

## 来源

- [官方 Iterators & Pointers](https://web.stanford.edu/class/cs106l/lectures/2026Spring-06-Iterators.pdf)
- [vector::erase 规则](https://en.cppreference.com/w/cpp/container/vector/erase)
- [来源与版本](sources.md)
