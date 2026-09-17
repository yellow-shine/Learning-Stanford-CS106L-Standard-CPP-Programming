# std-atomic

这是并发编程里一个非常核心的概念。

假设两个线程同时执行：

```cpp
counter++;
```

你可能觉得 `counter++` 是一个操作。

实际上大概是：

```text
load counter
     ↓
counter + 1
     ↓
store counter
```

于是：

```text
Thread A              Thread B

load 10
                      load 10
+1
                      +1
store 11
                      store 11
```

最终：

```text
11
```

但实际上执行了两次 `++`，应该是：

```text
12
```

这是竞态的直观示意。更严格地说，多个线程无同步地读写普通 `int` 会产生 **data race（数据竞争）**，导致未定义行为，并非仅仅保证出现“少加一次”。

---

使用：

```cpp
std::atomic<int> counter{0};
```

然后：

```cpp
counter++;
```

这个操作是 atomic 的。

例如：

```cpp
#include <atomic>

std::atomic<int> count{0};

void worker() {
  count.fetch_add(1);
}
```

---

## 典型用途

非常适合：

```cpp
std::atomic<bool> stop{false};
std::atomic<int> reference_count{0};
std::atomic<uint64_t> sequence{0};
```

例如停止后台线程：

```cpp
std::atomic<bool> running{true};

void Worker() {
  while (running.load()) {
    // work
  }
}
```

另一个线程：

```cpp
running.store(false);
```

---

## atomic 不等于万能锁

这是非常重要的一点。

下面两个变量即使都是 atomic：

```cpp
std::atomic<int> balance;
std::atomic<int> version;
```

你想保证：

```text
balance 和 version 必须一起变化
```

`atomic` 做不到这种跨多个变量的 invariant。

例如：

```cpp
balance.store(100);
version.store(2);
```

另一个线程可能看到：

```text
balance = 100
version = 1
```

如果你需要：

> 多个状态一起保持一致

通常应该使用：

```cpp
std::mutex
```

因此可以粗略理解：

```text
atomic
  ↓
保护一个简单变量 / 原子状态转换

mutex
  ↓
保护一段逻辑 / 一组共享状态
```

## 操作边界

`counter++` / `fetch_add()` 是原子读改写；`counter = counter + 1` 即使变量是 atomic，也不是一个整体的原子读改写。默认内存序为 `std::memory_order_seq_cst`；atomic 不保证底层实现总是无锁。停止标志也不会自动唤醒正在阻塞等待的线程。

## 相关知识

[std-mutex](std-mutex.md)

---

来源：02.md（重复内容已合并）。
