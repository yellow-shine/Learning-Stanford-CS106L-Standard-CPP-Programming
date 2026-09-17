# std-mutex

mutex = Mutual Exclusion。

意思是：

> 同一时刻，只允许一个线程进入 critical section。

例如：

```cpp
std::mutex mutex;
int balance = 0;

void AddMoney(int value) {
  mutex.lock();

  balance += value;

  mutex.unlock();
}
```

线程：

```text
Thread A
    |
    lock
    |
    v
 critical section
    |
   unlock

Thread B
    |
    +------ 等待
```

---

但上面的代码有一个严重问题：

```cpp
mutex.lock();

DoSomething();  // exception

mutex.unlock();
```

如果 `DoSomething()` 抛异常：

```cpp
unlock()
```

不会执行。

mutex 永远锁住。

这就引出：

## 相关知识

[std-lock_guard](std-lock_guard.md) · [std-atomic](std-atomic.md)

---

来源：02.md、03.md（重复内容已合并）。
