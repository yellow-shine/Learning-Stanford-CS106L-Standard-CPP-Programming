# 12.4 forward_list（forward_list）

## 学习目标

掌握 `std::forward_list` 的单向遍历和“在某元素之后修改”的接口，并评估其精简节点结构的取舍。

## 核心知识点

- **单向链表（singly linked list）**：节点只保存后继链接，只能向前遍历，无法常数时间获得前驱。
- **前插接口**：`push_front` 与 `pop_front` 操作首元素；容器不提供 `push_back`。
- **after 操作**：`insert_after`、`erase_after`、`splice_after` 以“前一个位置”表达修改点，符合单链表成本模型。
- **哨兵位置**：`before_begin()` 返回首元素之前的特殊迭代器，用于在开头执行 after 操作，不能解引用。
- **空间特点**：每节点通常只需一个链接，可能比 `list` 节省空间，但仍有节点分配与较差局部性。
- **大小查询**：标准接口故意不提供 `size()`，因为维持或计算大小会改变其精简设计取舍。

## 最小示例

```cpp
#include <forward_list>
int main() {
    std::forward_list<int> xs{2, 3};
    xs.insert_after(xs.before_begin(), 1);
    auto first = xs.begin();
    xs.erase_after(first);
    return *std::next(xs.begin()) == 3 ? 0 : 1;
}
```

## 常见陷阱

- `erase_after(it)` 删除的是 `it` 后面的元素，不是 `it` 本身。
- 不要对 `before_begin()` 解引用。
- 若频繁需要尾插、反向遍历或元素个数，应选择其他容器。

← [上一节](12-03-list.md) | [目录](../README.md) | [下一节](12-05-map.md) →
