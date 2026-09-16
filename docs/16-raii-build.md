# 16 · RAII、智能指针与构建项目

[上一课](15-optional.md) · [目录](../README.md) · [下一篇：可选课入口](17-optional-lecture.md)

## 先修与目标

先理解生命周期、析构、移动和模板。本课把它们串起来：让资源的有效期跟对象作用域绑定，并把单文件练习推广到可编译、可测试的多文件项目。完成后进入 [A7](assignments.md) 与 [BusTub 衔接](bustub-bridge.md)。

## RAII 不只管理内存

Resource Acquisition Is Initialization：把资源获取和对象初始化联系起来，把释放责任放在析构中。正常返回、提前 return、异常展开时，已构造的局部对象都会按规则析构，因此不用在每个出口手写重复清理。

资源可以是堆内存、文件句柄、互斥锁或页面 pin。RAII 不会修复未定义行为，也不能保证进程被强制终止时执行析构；它解决的是正常 C++ 控制流中的释放责任。

`std::unique_ptr<T>` 表示独占所有权，不可复制但可移动。`std::make_unique<T>(args...)` 创建受管对象。`get()` 返回借用指针，不转移所有权；`reset()` 释放旧资源并可接管新资源；`release()` 放弃管理并交出裸指针，此后清理由调用者负责，不能把它当普通读取函数。

`shared_ptr` 表示共享所有权，最后一个所有者释放对象。`weak_ptr` 不增加强引用计数，用 lock 尝试取得临时所有者，适合观察或打破强引用环。不要为避免思考谁拥有对象，就把所有指针换成 shared_ptr；共享所有权也不代表所指对象线程安全。

## 完整示例

源文件：[16-raii-build.cpp](../examples/16-raii-build.cpp)。异常分支验证释放责任，不包含自制 unique_ptr 的实现。

```cpp
#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>

class Resource {
public:
    explicit Resource(int& alive) : alive_(alive) { ++alive_; }
    ~Resource() { --alive_; }
    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;
private:
    int& alive_;
};

void fail_after_acquire(int& alive) {
    auto owner = std::make_unique<Resource>(alive);
    assert(alive == 1);
    throw std::runtime_error("operation failed");
}

int main() {
    int alive = 0;
    bool caught = false;
    try {
        fail_after_acquire(alive);
    } catch (const std::runtime_error&) {
        caught = true;
    }
    assert(caught && alive == 0);
    std::weak_ptr<int> observer;
    {
        auto shared = std::make_shared<int>(7);
        observer = shared;
        const auto temporary_owner = observer.lock();
        assert(temporary_owner && *temporary_owner == 7);
    }
    assert(observer.expired());
    std::cout << "resources alive: " << alive << '\n';
}
```

输出 `resources alive: 0`。throw 发生时 owner 已成功构造，展开栈会销毁 unique_ptr，继而销毁 Resource。weak_ptr 不延长 int 的寿命；lock 返回的 shared_ptr 才暂时持有资源，使用前检查是否成功。

## 编译多个文件

假设一个小程序由 `main.cpp`、`score.cpp` 和 `score.h` 组成：main 包含声明，score.cpp 提供实现。应把两个实现文件交给编译器：

```bash
# 示意命令：针对你自己的多文件工程，不是本仓库已有文件。
c++ -std=c++17 -Wall -Wextra main.cpp score.cpp -o app
```

头文件通常不独立列为一个编译单元；不要 include 普通 `.cpp` 来解决链接错误，这可能造成重复定义。模板是实现可见性有特殊要求的场景，不是随意包含实现文件的通行证。

大型项目使用 CMake 描述目标、源文件和依赖，再由构建工具执行。生成与构建分开：

```bash
# 通用示意，需要当前目录存在 CMakeLists.txt。
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

本讲义没有必要为独立示例引入 CMake；统一使用[示例脚本](../scripts/check-examples.sh)。BusTub 已有 CMake 配置，应复用它，而不是再生成一套。

## 易错点与 BusTub 联系

- 不要从同一个裸地址分别构造两个独立 shared_ptr，这会产生两个所有权控制块，可能重复释放。
- `unique_ptr` 本身 const 与 `unique_ptr<const T>` 不同：前者通常限制换所有者，不保证 T 不可写。A7 自制类的 const 接口有自己的作业合同。
- shared_ptr 引用环不会因为离开局部作用域就自动消失；选一条非拥有关系用 weak_ptr 或重新设计。
- 原始 mutex 的 lock/unlock 容易漏解锁，RAII 锁在附录演示；多个锁仍需考虑死锁顺序。

## 自测

1. 示例抛异常后，为什么计数仍回到 0？
2. `get()` 与 `release()` 返回裸指针，意义是否相同？

<details>
<summary>参考答案</summary>

1. 已构造的 owner 在异常展开中析构，自动删除 Resource，Resource 析构减少计数。
2. 不同。get 仅借用，所有权仍在智能指针；release 解除管理，清理责任交给调用者。

</details>

## 来源

- [官方 RAII, Smart Pointers, & Building C++ Projects](https://web.stanford.edu/class/cs106l/lectures/2026Spring-16-RAII-SmartPointers.pdf)
- [智能指针参考](https://en.cppreference.com/w/cpp/memory)
- [来源与版本](sources.md)
