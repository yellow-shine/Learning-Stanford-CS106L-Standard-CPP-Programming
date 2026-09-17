# std-thread

这是真正创建一个执行线程。

```cpp
#include <thread>

void Worker() {
  std::cout << "hello\n";
}

int main() {
  std::thread t(Worker);

  t.join();
}
```

执行过程：

```text
Main Thread
     |
     +------ create ------> Worker Thread
     |                        |
     |                        Worker()
     |
     +------ join ---------- 等它结束
```

`join()`：

> 等线程执行结束。

---

也可以 lambda：

```cpp
std::thread t([] {
  std::cout << "worker\n";
});

t.join();
```

传参数：

```cpp
void Worker(int id) {
}

std::thread t(Worker, 42);
```

---

## 一个非常重要的坑

如果：

```cpp
std::thread t(Worker);
```

`t` 析构时仍然是 joinable：

程序会：

```text
std::terminate()
```

所以一般必须：

```cpp
t.join();
```

或者：

```cpp
t.detach();
```

但绝大多数业务代码：

> 不要随便 detach。

因为生命周期很难管理。

如果项目使用 C++20，很多时候：

```cpp
std::jthread
```

比裸 `std::thread` 更安全，因为析构时会自动 join。

## 生命周期补充

工作函数结束后，尚未 join 或 detach 的线程对象仍然是 joinable。线程入口函数不能让异常逃逸，否则会调用 `std::terminate()`。线程参数默认按值保存，需要传引用时可使用 `std::ref`，但必须保证被引用对象活到线程结束。

`std::jthread` 析构时会请求停止并 join，但停止是协作式的，不会强制终止工作函数；工作函数不退出，析构仍可能一直等待。

## 相关知识

[std-promise与std-future](std-promise与std-future.md)

---

来源：02.md（重复内容已合并）。
