# BusTub 衔接附录：语言基础之后还需要什么

[目录](../README.md) · [上一篇：作业指南](assignments.md) · [下一篇：来源](sources.md)

**这是本讲义的补充，不是 CS106L 正课，也不是 CMU 项目解答。** 目标是读懂项目代码、构建测试、分析所有权和并发边界，不能用它替代数据库原理课。

## 先选定版本，不要追着 master 写作业

本页检查了 BusTub 提交 `c0a5431985287e258a6f25f53d822f0d0d2e63e8` 的 README 与 CMakeLists。后者设置 C++17、CMake 最低 3.10、推荐 Clang 15，项目版本字段为 2025.8。这些是该提交的证据，不是对所有 CMU 15-445 年份的要求。

README 推荐 Ubuntu 24.04，macOS 可用于开发，评测环境是 Ubuntu 24.04，并明确不支持 WSL。不同平台的 mutex 等行为可能造成差异，官方建议用 Linux 环境复现测试问题。不要把这条当前说明强行套到旧学期上。

开始实际项目时，先确定学期、项目 handout、starter 提交和工具链。项目编号、数据结构、函数签名、可修改文件、公开测试范围会变化。本讲义不预设某年的 Project 0 一定是什么，也不承诺 main/master 能搭配任意旧作业说明。

## 语言知识到项目阅读的映射

| 语言知识 | 在项目中问什么 | 复习 |
| --- | --- | --- |
| 引用、指针、迭代器 | 是拥有还是借用？何时失效？ | 03、06 |
| const 与类接口 | 只读访问保证什么？是否仍可通过其他路径修改？ | 07、09 |
| 容器与模板 | 键类型要求、比较规则、复杂度、失效规则是什么？ | 05、10、12 |
| 复制和移动 | guard 能复制吗？移动后谁释放资源？ | 13、14 |
| RAII | return/异常路径是否仍解锁、释放 pin？ | 16 |
| optional 与错误 | 没匹配、资源耗尽和 I/O 错误是否区分？ | 04、15 |
| 虚函数 | 调用静态类型与实际派生类型是什么？ | 08 |
| Lambda | 回调什么时候执行？捕获还活着吗？ | 11 |

不要试图把这张表变成 BusTub 的具体实现设计。源码注释和当前 handout 才是接口约束来源。

## 构建：先复现，再修改

官方 README 提供 `build_support/packages.sh` 安装依赖。本次没有运行它，也不建议不阅读脚本就以管理员权限执行。先核对已安装环境；依赖不足时按所选版本官方说明处理。

以下在 **BusTub 工作区根目录**运行，不是在本讲义仓库：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 2
ctest --test-dir build -N
ctest --test-dir build --output-on-failure
```

这是官方 `mkdir build; cd build; cmake ..; make` 的独立构建目录形式。先用低并行度避免内存不足；不是所有环境都提供 Linux 的 nproc，所以这里不给 macOS 照搬 `make -j$(nproc)`。

`ctest -N` 先列出已注册测试；输出 0 tests 不代表通过。若所选版本的测试目标不在默认 all 中，先查看 `cmake --build build --target help` 与该版测试说明，再构建真实目标。不要根据网上旧文章猜目标名。

Debug 更适合看变量和调用栈；Release 优化会改变调试可见性，也常关闭 assert。构建失败先确认是编译、链接、依赖还是资源限制，再处理代码。

## 测试：从一个失败缩小问题

1. 记录失败测试名、输入、期望和实际结果，不先修改测试让它变绿。
2. 用 `ctest --test-dir build -R '实际测试名' --output-on-failure` 只跑相关测试。此处字符串必须替换成 -N 列出的名称。
3. 若程序使用 GoogleTest，可对真实测试可执行文件使用 `--gtest_filter=Suite.Case`；名称以当前程序列出的测试为准。
4. 修复后跑相关测试和受影响范围，再跑完整集合。公开测试通过不代表隐藏边界正确。

空集合、满容量、重复键、资源不足、异常/提前返回、并发交错，都可能是边界。哪些情况需要支持，应由项目合同决定，不擅自添加或删减行为。

## 调试器与 Sanitizer

LLDB 的最小工作流是 `lldb 实际测试可执行文件`，设置断点 `breakpoint set --name 函数名`，`run`，暂停后 `bt` 看调用栈、`frame variable` 看当前变量。GDB 有对应的 `break`、`run`、`bt`、`print`。尽量在第一次异常状态出现处停下，而不是只看最后崩溃位置。

已核实 BusTub README：Debug 默认启用 AddressSanitizer，可通过 `-DBUSTUB_SANITIZER=thread` 选择 ThreadSanitizer。建议不同配置用不同目录：

```bash
# 是否支持 ThreadSanitizer 仍受平台和工具链限制。
cmake -S . -B build-tsan -DCMAKE_BUILD_TYPE=Debug -DBUSTUB_SANITIZER=thread
cmake --build build-tsan --parallel 2
ctest --test-dir build-tsan --output-on-failure
```

ASan 主要帮助发现越界、释放后使用等内存问题；TSan 发现数据竞争；UBSan 检查部分未定义行为。没有报告不是正确性的数学证明，也不能把 ASan 与 TSan 随意混在同一构建中。这里没有构建或验证 BusTub，只给出已核实配置与使用原则。

## 所有权、pin 与锁：三个不同问题

- **所有权**：谁负责销毁对象或释放资源？
- **pin/有效期**：页面是否允许被驱逐或复用？
- **同步**：其他线程能否同时访问冲突状态？

一个 shared_ptr 可能延长对象寿命，却不保证对象内数据不被并发修改。一个 mutex 保护临界区，却不一定代表页面仍 pin 住。一个非空裸指针更不能保证以上三件事。读 guard 的构造、移动和析构时，应分别追踪每项责任。

## 独立示例：用 RAII 锁保护计数器

先修为第 16 课。源码：[bustub-locks.cpp](../examples/bustub-locks.cpp)。这不是页面管理器实现，只展示锁对象的作用域。

```cpp
#include <cassert>
#include <future>
#include <iostream>
#include <mutex>

class Counter {
public:
    void increment() {
        std::lock_guard<std::mutex> lock{mutex_};
        ++value_;
    }
    int value() const {
        std::lock_guard<std::mutex> lock{mutex_};
        return value_;
    }
private:
    mutable std::mutex mutex_;
    int value_ = 0;
};

int main() {
    Counter counter;
    auto work = [&counter] {
        for (int i = 0; i < 1000; ++i) {
            counter.increment();
        }
    };
    auto first = std::async(std::launch::async, work);
    auto second = std::async(std::launch::async, work);
    first.get();
    second.get();
    assert(counter.value() == 2000);
    std::cout << counter.value() << '\n';
}
```

lock_guard 构造时加锁，离开作用域时解锁，两个线程对 value_ 的读写遵守同一 mutex 协议。value 是逻辑只读操作，但仍需锁定 mutex，所以 mutex 使用 mutable；这不是允许随意改动 value_。get 等待工作完成并传播任务异常，保证主线程检查前工作结束。由 async 创建的这些 future 在必要时会在析构中等待任务，因此即使启动另一任务失败，counter 也不会先于已启动任务销毁。

这只是单计数器的最小同步例子，不是完整线程池或高吞吐数据库设计。若直接使用 std::thread，必须保证所有已启动且可 join 的线程在销毁前被妥善 join；C++20 的 jthread 可简化这种生命周期管理。本例选用 C++17 async/future，把重点保留在临界区。

多个锁可能死锁：两个线程分别持有 A 等 B、持有 B 等 A。固定锁顺序或在适用时用 C++17 scoped_lock 同时获取多个锁，不要以“用了 RAII 就不会死锁”解释代码。耗时 I/O 放在锁内是否合理，需要结合项目合同和性能要求分析，不能统一一刀切。

## 数据库理论仍需单独学习

CS106L 不教授磁盘页布局、缓冲池替换、B+ 树、查询执行、事务隔离、并发控制或恢复算法。它让你能实现和解释 C++ 代码，不替你决定算法正确性。建议先理解对应数据库课，再读项目 handout，最后实现并测试，不以模板技巧代替不变量证明。

## 自测与答案

1. shared_ptr 能否自动保护两个线程对其指向对象的同时写入？
2. 为什么 getter 是 const，示例却仍要加锁？

<details>
<summary>参考答案</summary>

1. 不能。所有权控制与对象内容同步是不同问题；需要单独设计同步协议。
2. const 只约束这条访问路径，其他线程仍可能修改。读取共享非原子数据也必须与写入遵守同步规则。

</details>

## 诚信与来源

BusTub README 明确要求在校与校外学习者都不要公开项目解答；实际实现保留私有。本仓库没有 BusTub 解法，也不执行任何创建远端、镜像或推送操作。

- [固定提交 README](https://github.com/cmu-db/bustub/blob/c0a5431985287e258a6f25f53d822f0d0d2e63e8/README.md)
- [固定提交 CMakeLists](https://github.com/cmu-db/bustub/blob/c0a5431985287e258a6f25f53d822f0d0d2e63e8/CMakeLists.txt)
- [CMU 15-445](https://15445.courses.cs.cmu.edu/)
- [std::lock_guard](https://en.cppreference.com/w/cpp/thread/lock_guard)
