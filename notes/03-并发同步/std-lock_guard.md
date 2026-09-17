# std-lock_guard

正确方式一般是：

```cpp
void AddMoney(int value) {
  std::lock_guard<std::mutex> guard(mutex);

  balance += value;
}
```

它利用了 **RAII**：

```text
创建 guard
    ↓
mutex.lock()

作用域结束
    ↓
guard 析构
    ↓
mutex.unlock()
```

即使：

```cpp
throw
return
```

都没关系。

---

C++17 可以写：

```cpp
std::lock_guard guard(mutex);
```

不需要写：

```cpp
<std::mutex>
```

---

## 典型使用模式

你以后看到这种：

```cpp
{
  std::lock_guard guard(latch_);
  page_table_[page_id] = frame_id;
}
```

可以脑补成：

```text
进入作用域
↓
锁 mutex
↓
修改共享数据
↓
离开作用域
↓
自动 unlock
```

这在 BusTub 里会大量出现。

## 选择原则

只需在一个作用域内持锁时，优先使用 `lock_guard`。它不能复制或移动，也不提供手动 `lock()` / `unlock()`；需要这些控制能力或条件变量等待时使用 `unique_lock`。

## 相关知识

[RAII](../02-资源与移动语义/RAII.md) · [std-unique_lock](std-unique_lock.md)

---

来源：02.md、03.md（重复内容已合并）。
