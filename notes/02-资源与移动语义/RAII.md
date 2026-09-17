# RAII

这是你学习 C++ 时应该真正吃透的概念。

RAII：

> Resource Acquisition Is Initialization

名字不好理解。

真正含义是：

> **把资源的生命周期绑定到 C++ 对象生命周期。**

例如：

```cpp
{
  std::lock_guard lock(mutex);

  ...
}
```

对象：

```cpp
lock
```

创建时：

```text
获得 mutex
```

析构时：

```text
释放 mutex
```

---

RAII 不只是 mutex。

例如：

```cpp
std::unique_ptr<Foo>
```

构造：

```text
拥有 Foo
```

析构：

```text
delete Foo
```

文件：

```cpp
std::fstream file("data.txt");
```

析构：

```text
close file
```

锁：

```cpp
std::lock_guard
```

析构：

```text
unlock
```

因此 C++ 的核心模式是：

```text
Resource
   │
   ▼
Object
   │
   ▼
scope
   │
   ▼
destructor
   │
   ▼
release resource
```

---

这也是为什么 C++ 里：

```cpp
{
    ...
}
```

这个作用域特别重要。

你其实是在控制：

```text
resource lifetime
```

## 边界

正常退出作用域和异常栈展开会析构已经构造完成的自动对象；进程被强制终止等情况不保证执行析构。RAII 管理资源，不自动保证共享数据的线程安全。

## 相关知识

[std-lock_guard](../03-并发同步/std-lock_guard.md) · [std-unique_lock](../03-并发同步/std-unique_lock.md)

---

来源：02.md、03.md（重复内容已合并）。
