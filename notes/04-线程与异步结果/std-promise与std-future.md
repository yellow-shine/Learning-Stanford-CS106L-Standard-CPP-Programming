# std-promise与std-future

理解它们最简单的方法是：

> 一个线程未来会产生一个值，另一个线程等这个值。

比如：

```text
Worker Thread
      |
  计算结果
      |
 promise.set_value(42)
      |
      v

   shared state

      ^
      |
 future.get()
      |
 Main Thread
```

代码：

```cpp
std::promise<int> promise;

std::future<int> future = promise.get_future();

std::thread worker([&promise] {
  int result = 40 + 2;
  promise.set_value(result);
});

int result = future.get();

worker.join();
```

---

这里角色非常清楚：

```text
promise
    ↓
生产结果

future
    ↓
等待 / 获取结果
```

可以类比 Go：

```text
promise + future
≈
channel 的一种特殊场景
```

但它基本是：

```text
one-shot
```

只传一次结果。

---

## future.get()

```cpp
future.get();
```

如果结果还没出来：

```text
阻塞
```

直到：

```cpp
promise.set_value(...)
```

---

## 还可以传异常

worker：

```cpp
try {
  ...
} catch (...) {
  promise.set_exception(std::current_exception());
}
```

调用：

```cpp
future.get();
```

会重新抛出异常。

所以 future 不只是：

```text
future value
```

还可以理解为：

```text
future =
    future result
    +
    future exception
```

## 使用约束与异常路径

- 头文件：`<future>`；异常传递还涉及 `<exception>`。
- `promise.get_future()` 通常只能成功调用一次；共享状态只能设置一次结果或异常。
- 普通 `future.get()` 消费结果后不再有效；需要共享结果时考虑 `std::shared_future`。
- promise 未设置结果就被销毁，等待方会收到 `broken_promise` 对应的 `std::future_error`。
- `future.get()` 可能抛异常，必须保证异常路径也处理线程生命周期。本例可先 `worker.join()` 再 `future.get()`；C++20 也可由 `std::jthread` 管理。
- promise/future 传递一次性结果，本身不会创建线程。

## 相关知识

[std-thread](std-thread.md)

---

来源：02.md（重复内容已合并）。
