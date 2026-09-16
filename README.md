# CS106L 中文自学讲义：从恢复 C++ 到准备 BusTub

这是一套可以独立阅读的**完整课程配套讲义**，按 Stanford CS106L **Spring 2026 的 16 次主课**组织，另有第 17 次可选课入口、七份官方作业指南与 BusTub 衔接附录。不是官方讲义翻译，也不替代原课件。适合曾学过 C++、正在重新建立语言模型的读者。

目标不是背语法，而是能解释：对象是谁的、活到什么时候、何时复制、何时移动、哪些接口可以修改对象。数据库理论仍需另学。

## 怎么使用

每课按「先修 → 解释 → 完整示例 → 易错点 → 自测与折叠答案」阅读。不需要把答案发给任何人，不设周计划。示例均有独立 `.cpp` 文件与 `assert` 检查；先读再运行即可。每课的代码是讲义练习，**不是官方作业答案**。

本地例子统一使用 **C++17**。官方作业 A1–A5、A7 的命令使用 C++20；A6 使用 C++23。标准版本和标准库实现都要支持，不能把课程例子能编译等同于所有官方作业环境已就绪。

## 阅读顺序

| 课 | 主题 | 建议实践节点 |
| --- | --- | --- |
| [01](docs/01-welcome.md) | 课程、工具与编译 | 运行第一个检查 |
| [02](docs/02-types-structs.md) | 类型与结构体 | 组织一条记录 |
| [03](docs/03-initialization-references.md) | 初始化、引用与生命周期 | 分清副本和借用 |
| [04](docs/04-streams.md) | 流与输入输出 | A1 SimpleEnroll |
| [05](docs/05-containers.md) | 标准容器 | 选择存储方式 |
| [06](docs/06-iterators-pointers.md) | 迭代器与指针 | A2 Marriage Pact |
| [07](docs/07-classes.md) | 类、构造函数与接口 | A3，const 部分参看第 09 课 |
| [08](docs/08-inheritance.md) | 可选：继承与动态多态 | 可先跳过 |
| [09](docs/09-class-templates-const.md) | 类模板与 const 正确性 | 巩固 A3 |
| [10](docs/10-function-templates.md) | 函数模板与泛型算法 | 泛型约束 |
| [11](docs/11-functions-lambdas.md) | 函数、lambda 与算法 | A4 Ispell，含 C++20 补充 |
| [12](docs/12-operators.md) | 运算符重载 | 设计自然的接口 |
| [13](docs/13-special-members.md) | 特殊成员函数 | A5 Treebook |
| [14](docs/14-move.md) | 移动语义 | 区分移动表达式与移动操作 |
| [15](docs/15-optional.md) | optional 与类型安全 | A6 Explore Courses |
| [16](docs/16-raii-build.md) | RAII、智能指针与构建 | A7 Unique Pointer |
| [17](docs/17-optional-lecture.md) | 官方可选课入口 | 按兴趣延伸 |

- [七份官方作业：准备、接口、运行、验收](docs/assignments.md)
- [BusTub 衔接：工程工具、所有权、并发与学习边界](docs/bustub-bridge.md)
- [来源、固定版本与核实范围](docs/sources.md)

## 运行讲义示例

需要已有 C++17 编译器和 Bash；不需要安装第三方库。macOS/Linux 或具备同类工具的终端中，从仓库根目录运行：

```bash
bash scripts/check-examples.sh
# 选择具体编译器，而不是依赖 shell alias：
CXX=clang++ bash scripts/check-examples.sh
```

脚本在临时目录生成可执行文件、逐个执行、退出时清理；编译或断言失败会立即返回非零状态。不要加 `-DNDEBUG`，否则会关闭断言。`examples/compile_flags.txt` 同时为 clangd 等兼容编辑器指定 C++17，避免编辑器按旧标准误报 optional 不存在。

单独运行一课：

```bash
c++ -std=c++17 -Wall -Wextra -Wpedantic examples/02-types-structs.cpp -o /tmp/cs106l-example
/tmp/cs106l-example
```

脚本成功只证明这些教学例子通过检查，不证明你已掌握课程，也不证明官方作业或 BusTub 通过评测。

## 版本与诚信

官方课程：<https://web.stanford.edu/class/cs106l/>。本资料核对的是 Spring 2026；官网和 GitHub 默认分支可能变化。作业指南固定到已核实提交，不猜测未来要求。不要公开 BusTub 解答；即使是校外自学者，官方也要求保留实现私有。本仓库不包含任何官方作业核心实现。
