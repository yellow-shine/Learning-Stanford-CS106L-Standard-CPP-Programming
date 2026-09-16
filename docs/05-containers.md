# 05 · Containers：按访问需求选择容器

[上一课](04-streams.md) · [目录](../README.md) · [下一课](06-iterators-pointers.md)

## 先修与目标

理解对象复制、范围循环和基本输入输出。目标不是背全所有容器，而是能解释为什么选择序列、集合、映射或队列，以及这种选择对复杂度和生命周期的影响。

## 从问题到数据结构

| 需求 | 常用容器 | 关键性质 |
| --- | --- | --- |
| 按位置保存一串元素 | `std::vector<T>` | 连续存储，随机访问 O(1)，尾部追加摊还 O(1) |
| 两端增删 | `std::deque<T>` | 随机访问 O(1)，不是整体连续存储 |
| 排序且去重 | `std::set<T>` | 查找、插入通常 O(log n)，按比较规则遍历 |
| 按键找值且有序 | `std::map<K,V>` | O(log n)，不是下标连续的数组 |
| 无序集合或映射 | `unordered_set` / `unordered_map` | 平均 O(1) 查找，最坏 O(n)，遍历顺序不保证 |
| 先进先出 / 后进先出 | `queue` / `stack` | 容器适配器，只暴露受限接口 |

哈希容器的“平均 O(1)”不是最坏情况保证，也不代表永远比有序容器快。元素少、需要排序、哈希计算昂贵或内存局部性不同时，选择可能不同。

`vector.size()` 是已经存在的元素数，`capacity()` 是无需重新分配即可容纳的容量。`reserve(100)` 只是准备空间，不会创建 100 个可访问元素；`resize(100)` 才改变元素数。访问不存在的 `v[50]` 是错误，不能因为 capacity 足够就这样做。需要运行时边界检查可用 `at()`，越界会抛异常。

## 完整示例

源文件：[05-containers.cpp](../examples/05-containers.cpp)。

```cpp
#include <cassert>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> events{"read", "write", "read"};
    events.reserve(20);
    assert(events.size() == 3);
    std::map<std::string, int> counts;
    for (const auto& event : events) {
        ++counts[event];
    }
    assert(counts.at("read") == 2);
    const auto before = counts.size();
    assert(counts.find("missing") == counts.end());
    assert(counts.size() == before);
    std::set<std::string> kinds(events.begin(), events.end());
    assert(kinds.size() == 2);
    std::queue<int> pending;
    pending.push(7);
    pending.push(8);
    assert(pending.front() == 7);
    pending.pop();
    assert(pending.front() == 8);
    std::cout << counts.at("read") << ' ' << kinds.size() << '\n';
}
```

输出 `2 2`。`counts[event]` 在键不存在时插入一个值初始化的整数 0，再加 1，因此适合计数；只想查询时用 `find`，避免无意修改数据库式映射。`at` 不插入，但找不到键会抛出异常。`set` 根据比较规则把等价元素归为一类，因此重复的 `read` 只占一项。

范围 for 中，`auto x` 每轮复制元素，`auto& x` 修改原元素，`const auto& x` 只读借用。对字符串只读遍历通常不必每轮复制。

## 接口习惯

容器一般自己管理元素，但不会替原始指针成员决定所有权：`vector<int*>` 被销毁会销毁各个指针值，不会自动 `delete` 所指对象。反之，`vector<string>` 会正常析构每个字符串。

`queue.pop()` 只删除队首，不返回值；想读取先调用 `front()`。空队列上调用 `front()`/`pop()` 不合法，先检查 `empty()`。`set` 的元素不能随意原地修改排序关键字，否则会破坏容器不变量。

## 易错点与 BusTub 联系

- `unordered_map` 遍历顺序不确定，测试不应依赖它碰巧出现的顺序。
- 在循环中插入或删除可能使迭代器、引用、指针失效，下一课专门讨论。
- 缓存索引需要快速按键定位，待处理任务需要顺序调度；这是选择不同容器的理由，不是“数据库都应该用 map”。

## 自测

1. `vector<int> v; v.reserve(10);` 后 `v.size()` 是多少，可以读 `v[0]` 吗？
2. 只想检查名字是否在映射中，为什么不推荐 `counts[name]`？

<details>
<summary>参考答案</summary>

1. 是 0，不能读取。容量不等于已构造元素数。
2. `operator[]` 可能插入缺失键，改变程序状态。C++17 用 `find`；`contains` 是 C++20 功能。

</details>

## 来源

- [官方 Containers](https://web.stanford.edu/class/cs106l/lectures/2026Spring-05-Containers.pdf)
- [容器库与失效规则表](https://en.cppreference.com/w/cpp/container)
- [来源与版本](sources.md)
