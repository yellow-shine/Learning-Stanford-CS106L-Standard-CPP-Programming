# std-unique_lock

## 那 `unique_lock` 为什么存在？

因为有时候你不想：

```text
从进入 scope 开始
一直锁到 scope 结束
```

而是：

```text
lock

做一点事情

unlock

做一些不需要锁的事情

再 lock

继续访问共享数据
```

`lock_guard` 做不到这种事情。

但 `unique_lock` 可以。

例如：

```cpp
std::mutex m;

void Foo() {
    std::unique_lock<std::mutex> lock(m);

    // 此时 mutex 已经加锁
    ModifySharedData();

    lock.unlock();

    // 这里 mutex 已经释放
    DoSlowNetworkRequest();

    lock.lock();

    // 再次加锁
    ModifySharedDataAgain();
}
```

你可以把 `unique_lock` 想象成一个遥控器：

```text
unique_lock
    |
    | controls
    v
  mutex

lock.lock()
    ↓
锁住

lock.unlock()
    ↓
释放

lock.lock()
    ↓
再次锁住
```

而 `lock_guard` 没这个遥控能力。

---

## 一个现实例子

假设你有：

```cpp
std::mutex mutex;
std::vector<int> queue;
```

你需要：

1. 从 queue 拿一个任务
2. 做这个任务
3. 再更新 queue

错误写法：

```cpp
void Worker() {
    std::lock_guard lock(mutex);

    if (queue.empty()) {
        return;
    }
    int task = queue.back();
    queue.pop_back();

    ProcessTask(task);  // 可能执行 10 秒

    queue.push_back(task);  // 示意：放回任务，业务逻辑按需替换
}
```

这里最大的问题是：

```text
mutex 被锁了 10 秒
```

其他线程：

```text
Thread A:

lock
 |
取任务
 |
ProcessTask 10 秒
 |
更新 queue
 |
unlock


Thread B:

lock
 |
+---------------------- 等 10 秒
```

这非常不好。

因为：

```cpp
ProcessTask(task);
```

根本不需要访问共享的 `queue`。

更好的写法：

```cpp
void Worker() {
    std::unique_lock lock(mutex);

    if (queue.empty()) {
        return;
    }
    int task = queue.back();
    queue.pop_back();

    lock.unlock();

    ProcessTask(task);

    lock.lock();

    queue.push_back(task);  // 示意：放回任务，业务逻辑按需替换
}
```

这样：

```text
Thread A:

lock
 |
取任务
 |
unlock
 |
ProcessTask 10 秒
 |
lock
 |
更新 queue
 |
unlock
```

在 `ProcessTask()` 的 10 秒期间：

```text
Thread B 可以访问 queue
```

这就是 `unique_lock` 的核心用途之一。

---

## 那为什么不直接 `mutex.lock()` / `mutex.unlock()`？

你可能会问：

既然我要手动控制：

```cpp
lock.unlock();
lock.lock();
```

那干嘛不用：

```cpp
mutex.unlock();
mutex.lock();
```

关键区别在于：

> `unique_lock` 仍然拥有 RAII 的安全性。

例如：

```cpp
void Foo() {
    std::unique_lock lock(mutex);

    DoSomething();

    if (error) {
        return;
    }

    DoSomethingElse();
}
```

即使：

```cpp
return;
```

发生了，

`unique_lock` 析构时仍然会自动：

```cpp
mutex.unlock();
```

也就是说它兼具：

```text
RAII 安全
+
手动控制能力
```

这是它存在的核心原因。

---

## `lock_guard` vs `unique_lock`

你可以先只记这一张表：

|                    | `lock_guard` | `unique_lock` |
| ------------------ | ------------ | ------------- |
| 创建时自动加锁            | 是            | 是             |
| 析构时自动解锁            | 是            | 仅当拥有锁时     |
| 手动 `unlock()`      | 不行           | 可以            |
| 再次 `lock()`        | 不行           | 可以            |
| condition_variable | 不适合          | 常用            |
| 复杂度                | 简单           | 更灵活           |

所以选择原则特别简单：

```text
我只需要：

{
    lock
    ...
    ...
    unlock
}

→ lock_guard
```

如果需要：

```text
lock
...
unlock
...
lock
...
unlock
```

就用：

```text
unique_lock
```

---

## 为什么名字叫 `unique_lock`？

这里的 `unique` 容易误导。

不是说：

> 这个 mutex 只能有一个 `unique_lock` 对象。

它更接近：

> 这个 lock 对象独占地拥有 mutex 的锁控制权。

例如：

```cpp
std::unique_lock<std::mutex> lock(mutex);
```

`lock` 对象现在“拥有”这个 mutex 的锁。

可以问：

```cpp
lock.owns_lock()
```

例如：

```cpp
std::unique_lock<std::mutex> lock(mutex);

std::cout << lock.owns_lock();
// true

lock.unlock();

std::cout << lock.owns_lock();
// false

lock.lock();

std::cout << lock.owns_lock();
// true
```

mental model：

```text
unique_lock object
       |
       | owns
       v
     mutex
```

## 所有权与使用约束

- `mutex` 提供互斥；`unique_lock` 是管理锁所有权的 RAII 对象，不是第二把 mutex，也不拥有 mutex 对象的生命周期。
- 默认构造方式会加锁，但 `std::defer_lock` 等方式不会立即持锁。析构时仅在 `owns_lock()` 为真时解锁。
- 可以移动、不能复制；移动的是锁管理责任。
- 通过管理器执行 `lock()` / `unlock()`，不要绕过它直接操作底层 mutex，否则所有权状态可能失配。
- 解锁期间共享状态可能被其他线程改变；重新加锁后必须重新检查相关条件。
- 如果只是两个独立临界区，使用两个短作用域的 `lock_guard` 也可以。
- 示例中的 `ProcessTask`、`DoSomething` 等是业务占位函数。

## 相关知识

[条件变量](条件变量.md) · [std-lock_guard](std-lock_guard.md)

---

来源：02.md、03.md（重复内容已合并）。
