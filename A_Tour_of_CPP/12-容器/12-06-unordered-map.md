# 12.6 unordered_map（unordered_map）

## 学习目标

理解 `std::unordered_map` 的哈希桶模型、平均复杂度和重哈希失效规则，并正确提供键的哈希与相等关系。

## 核心知识点

- **无序映射（unordered map）**：元素按哈希值分配到桶中，迭代顺序不代表键顺序且不应写入协议假设。
- **平均复杂度**：良好哈希分布下查找、插入和删除平均为常数时间，最坏情况可退化为线性。
- **哈希一致性**：若相等谓词认为两个键相等，它们的哈希值必须相同。
- **负载因子（load factor）**：元素数与桶数之比影响冲突；`reserve` 可为预期元素数准备桶并减少重哈希。
- **重哈希（rehash）**：会使迭代器失效，但指向元素的引用和指针仍保持有效；删除使被删元素相关句柄失效。
- **访问接口**：与 `map` 类似，`operator[]` 会在键缺失时插入默认值，C++20 可用 `contains` 查询。

## 最小示例

```cpp
#include <string>
#include <unordered_map>
int main() {
    std::unordered_map<std::string, int> m;
    m.reserve(10);
    m.insert_or_assign("cpp", 20);
    return m.contains("cpp") && m.at("cpp") == 20 ? 0 : 1;
}
```

## 常见陷阱

- 不要依赖遍历顺序在不同运行或实现间稳定。
- 糟糕或可被攻击者操控的哈希输入可能导致严重退化。
- 自定义 `hash` 与 `key_equal` 的等价定义必须一致。

← [上一节](12-05-map.md) | [目录](../README.md) | [下一节](12-07-分配器.md) →
