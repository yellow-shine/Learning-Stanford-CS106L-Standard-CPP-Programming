# 来源、版本与核实边界

[目录](../README.md) · [上一篇：BusTub 衔接](bustub-bridge.md)

## 本资料的性质

这套中文讲义是按官方课表主题编写的原创教学解释，不是逐页翻译、官方勘误或课程认证。每课链接对应官方 PDF，供深入阅读。已经核实官网课表与作业 README；没有逐页校对全部 16 份课件正文，因此不能宣称覆盖了每张幻灯片、每个课堂案例或口头说明。

核实时间：**2026-09-16**。网页与默认分支会继续变化，下面记录可复现的版本，避免把未来内容误认作本次依据。

## CS106L Spring 2026

- [课程主页](https://web.stanford.edu/class/cs106l/)
- [当季归档入口](https://web.stanford.edu/class/archive/cs/cs106l/cs106l.1266/)
- [官方课堂代码仓库](https://github.com/cs106l/cs106l-lecture-code)
- [C++ 标准库参考](https://en.cppreference.com/w/)

课表核实到 01 Welcome、02 Types & Structs、03 Initialization & References、04 Streams、05 Containers、06 Iterators & Pointers、07 Classes、08 可选 Inheritance Practice、09 Class Templates & Const Correctness、10 Function Templates、11 Functions & Lambdas、12 Operator Overloading、13 Special Member Functions、14 Move Semantics、15 optional & Type Safety、16 RAII/Smart Pointers/Building C++ Projects。

第 17 次仅在课表显示 Optional Lecture 与 PDF 链接，本次 PDF 下载超时，正文未核实；见[单独说明](17-optional-lecture.md)。不为它编造主题。

课件链接从官网取得。官方课堂代码仅作为延伸阅读入口，本仓库示例均为独立编写，没有复制它的实现，也没有固定或测试它的版本。

## 官方作业仓库

仓库：[cs106l/cs106l-assignments](https://github.com/cs106l/cs106l-assignments)

解析 main 得到固定提交：[`711bc45acca20e3d5a6be3487ed4fe73396c77c0`](https://github.com/cs106l/cs106l-assignments/commit/711bc45acca20e3d5a6be3487ed4fe73396c77c0)。通过 GitHub contents API 获取并读取该提交下的 `assignment-setup/README.md` 和 `assignment1` 至 `assignment7` 的 README。raw.githubusercontent.com 下载在当前环境超时，改用 API 成功，未把下载失败当成“资料不存在”。

| 作业 | README 名称 | README 主编译标准 |
| --- | --- | --- |
| A1 | SimpleEnroll | C++20 |
| A2 | Marriage Pact | C++20 |
| A3 | Make a Class | C++20 |
| A4 | Ispell | C++20，明确不使用 C++23 ranges::to |
| A5 | Treebook | C++20 |
| A6 | Explore Courses | C++23，使用 optional 单子操作 |
| A7 | Unique Pointer | C++20 |

全部文件名、命令、交付物和要求链接见[作业指南](assignments.md)。README 的说明有少量旧链接、目录简称和伪签名笔误，本讲义在对应位置说明，不把这些内容当成精确 C++ 声明。没有下载整个 starter、补齐作业、安装依赖或运行官方 autograder。

## BusTub

仓库：[cmu-db/bustub](https://github.com/cmu-db/bustub)

解析 master 得到固定提交：[`c0a5431985287e258a6f25f53d822f0d0d2e63e8`](https://github.com/cmu-db/bustub/commit/c0a5431985287e258a6f25f53d822f0d0d2e63e8)。已读取该提交的 README 和 CMakeLists.txt：

- README：推荐 Ubuntu 24.04、macOS 可用于开发、评测 Ubuntu 24.04、不支持 WSL、Debug 默认 AddressSanitizer、可配置 thread sanitizer、禁止公开作业解答。
- CMakeLists：C++17、CMake 最低版本 3.10、Clang 版本变量 15、项目版本字段 2025.8，启用 CTest。

这些记录不是替用户选定的学期。真正开始 BusTub 前，仍需按目标课程 handout 确定 starter 和工具链；没有构建 BusTub，没有验证其依赖安装、完整测试目标或成绩系统。

## 本地验证范围

讲义 `examples/*.cpp` 使用已有编译器执行 C++17 编译与 assert 检查；运行入口是 [check-examples.sh](../scripts/check-examples.sh)。示例不访问外部网络、不修改项目数据，也不执行故意无效的语法或悬空访问。

Markdown 本地链接可检查目标文件存在；外部课件与参考链接并未逐一下载测试，网络可用性与网页未来变化不在本地检查保证内。验证结果以交付报告为准，不把“链接已写入”描述为“所有外部资源可访问”。

## 标准版本速查

- **C++11**：auto、范围 for、lambda、移动、unique_ptr/shared_ptr 等基础。
- **C++14**：make_unique、泛型 lambda 等。
- **C++17**：optional、variant、scoped_lock、结构化绑定等；本地全部可运行例子的基线。
- **C++20**：ranges、concepts、contains、部分 erase_if、jthread 等。
- **C++23**：optional 的 and_then/transform/or_else、ranges::to、expected 等。

标准发布年份不等于你的标准库已经实现所有功能。发生编译错误时，同时核对标准选项、实际编译器与标准库实现版本。
