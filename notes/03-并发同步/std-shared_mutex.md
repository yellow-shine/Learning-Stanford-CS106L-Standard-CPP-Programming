# std-shared_mutex

普通 mutex：

```text
reader A ─┐
reader B ─┼─ 不能同时进入
writer   ─┘
```

实际上：

> 即使两个线程都只是读，也必须互斥。

但很多数据结构是：

```text
读很多
写很少
```

例如 cache：

```cpp
std::unordered_map<int, Page> pages;
```

100 个线程：

```cpp
GetPage()
```

只有偶尔：

```cpp
InsertPage()
```

这时候可以用：

```cpp
std::shared_mutex
```

规则：

```text
多个 reader
    ↓
可以同时持锁

writer
    ↓
必须独占
```

---

读：

```cpp
std::shared_lock lock(mutex_);
auto it = map_.find(key);
// 检查 it != map_.end() 后再读取 it->second。
```

写：

```cpp
std::unique_lock lock(mutex_);
map_[key] = value;
```

结构：

```text
Reader 1 ─── shared lock ──┐
Reader 2 ─── shared lock ──┼── 同时执行
Reader 3 ─── shared lock ──┘

Writer   ─── unique lock ───── 必须等 reader 结束
```

注意这里通常搭配的是：

```cpp
std::shared_lock
```

和：

```cpp
std::unique_lock
```

## 注意事项

`std::shared_mutex` 从 C++17 开始提供，头文件为 `<shared_mutex>`。共享锁下只能进行不修改共享状态的操作：`map[key]` 可能插入缺失键，不能当作安全的只读查询。所有访问者必须遵守同一锁协议。读写锁不保证比普通 mutex 更快，也不保证公平性。

## 相关知识

[std-mutex](std-mutex.md) · [std-unique_lock](std-unique_lock.md)

---

来源：02.md（重复内容已合并）。
