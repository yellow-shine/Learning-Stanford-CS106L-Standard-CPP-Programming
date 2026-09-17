# C++ 知识点索引

从原始 `01.md`、`02.md`、`03.md` 提取并按相近能力分组；知识点已合并整理，原文件已删除。各篇末尾的来源文件名仅用于记录出处。代码默认按 C++17 理解，C++20 特性单独标注。示例中的业务函数及类型为讲解占位。

## 01-值与引用

- [左值与右值](01-值与引用/左值与右值.md)
- [左值引用与右值引用](01-值与引用/左值引用与右值引用.md)
- [std-optional](01-值与引用/std-optional.md)

## 02-资源与移动语义

- [RAII](02-资源与移动语义/RAII.md)
- [移动语义与拷贝](02-资源与移动语义/移动语义与拷贝.md)
- [std-move](02-资源与移动语义/std-move.md)
- [移动构造函数](02-资源与移动语义/移动构造函数.md)
- [移动赋值运算符](02-资源与移动语义/移动赋值运算符.md)

## 03-并发同步

- [std-atomic](03-并发同步/std-atomic.md)
- [std-mutex](03-并发同步/std-mutex.md)
- [std-lock_guard](03-并发同步/std-lock_guard.md)
- [std-unique_lock](03-并发同步/std-unique_lock.md)
- [std-shared_mutex](03-并发同步/std-shared_mutex.md)
- [条件变量](03-并发同步/条件变量.md)

## 04-线程与异步结果

- [std-thread](04-线程与异步结果/std-thread.md)
- [std-promise与std-future](04-线程与异步结果/std-promise与std-future.md)

## 建议学习顺序

RAII → 左值、右值与引用 → 移动语义 → optional → mutex 与锁管理 → 条件变量与读写锁 → thread → atomic → promise/future。

## 组合使用

BusTub 风格的页面查询可组合 `optional`（可能找不到）、`mutex`（保护页表）和 `lock_guard`（自动解锁）；PageGuard 一类资源对象结合 RAII 与移动语义；后台工作线程结合 thread、同步原语和 promise/future。
