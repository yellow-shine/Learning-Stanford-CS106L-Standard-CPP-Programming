# 七份官方作业指南：读题、准备、运行与自验

[目录](../README.md) · [上一页](17-optional-lecture.md) · [下一篇：BusTub 衔接](bustub-bridge.md)

本页只解释任务合同与验证方法，**不提供核心函数、智能指针、链表或 Treebook 的实现**。先自己动手；官方 README 本身有较多提示，需要时再看。讲义例子的参考答案与官方作业解答是两回事。

## 版本与环境准备

以下要求核实自官方作业仓库提交 `711bc45acca20e3d5a6be3487ed4fe73396c77c0`。官网标题可能省略空格或带感叹号：A3 课表为 Make a Class!，README 为 Make a Class；A6 课表为 ExploreCourses，README 为 Explore Courses，指同一作业。

在本讲义目录之外准备作业工作区，避免混入课程文档：

```bash
# 在你选择的练习父目录运行，不要在本讲义源码中复制解答。
git clone https://github.com/cs106l/cs106l-assignments.git
cd cs106l-assignments
git checkout 711bc45acca20e3d5a6be3487ed4fe73396c77c0
python3 --version
g++ --version
```

checkout 固定提交会进入 detached HEAD，适合复现；开始保存自己的修改前可创建个人练习分支。不要在有未保存改动时直接切换版本或盲目 pull。这里没有执行克隆、安装或任何提交操作。

[官方 setup](https://github.com/cs106l/cs106l-assignments/blob/711bc45acca20e3d5a6be3487ed4fe73396c77c0/assignment-setup/README.md) 要求 Python **3.8+**，因为本地 autograder 使用 Python。Windows 检查命令通常是 `python --version`。主作业多用 C++20，A6 用 C++23；setup 中 assignment0 的主编译命令也为 C++23，而 Windows 注记有旧版本差异。确认 starter 实际特性，不要以旧注记证明新标准库可用。

macOS 的 `g++` 可能是 Apple Clang。直接指定已安装的真实编译器可比 alias 更可靠；不要为了本讲义自动安装或替换系统工具。C++23 optional 成员缺失时，检查标准库和编译器版本，单加 `-std=c++23` 未必足够。

下面每段命令都假设**从作业仓库根目录**开始。必须进入对应目录再运行，因为测试和数据使用相对路径。官方 A5/A7 README 中出现 `assign5/`、`assign7/` 的文字，但仓库路径是 `assignment5/`、`assignment7/`，下文使用实际路径。

校外读者可使用本地测试，不应假设拥有 Stanford Paperless、Ed 或课程反馈系统的提交权限。截止日期是当季教学安排，不是本自学路线的时间约束。

## A1 · SimpleEnroll

[固定版本官方说明](https://github.com/cs106l/cs106l-assignments/blob/711bc45acca20e3d5a6be3487ed4fe73396c77c0/assignment1/README.md)

**先修**：02 类型、03 引用、04 流；涉及 vector 删除时补读 05–06。

**输入与任务**：读取 `courses.csv`，三列是 Title、Number of Units、Quarter。第一行是表头，不是一条课程记录。补齐 `Course` 字段，检查函数参数如何传递课程集合，在 `main.cpp` 实现要求；`utils.cpp` 提供现成辅助功能，不需要修改。

**输出与后置条件**：

- Quarter 不等于字面字符串 `"null"` 的课程视为开设。
- 写入 `student_output/courses_offered.csv`，格式严格为 `<Title>,<Number of Units>,<Quarter>`，逗号旁不要加额外空格，并写出表头。
- `write_courses_offered` 结束后，传入集合中应仅保留未开设课程。这不仅是写文件，还会改变集合。
- `write_courses_not_offered` 把剩余课程写入 `student_output/courses_not_offered.csv`，同样按要求保留表头。

```bash
cd assignment1
g++ -std=c++20 main.cpp -o main
./main
```

**自己检查**：表头是否被当记录；输出格式是否完全一致；集合是否真的被修改而不只是改了副本；全部开设、全部未开设和空数据如何处理；文件打开失败有没有被发现。删除元素时注意失效规则，不要直接在范围循环中乱删。

**交付文件**：`main.cpp`。官方程序启动后运行内置 autograder；不在本页复制课程筛选的核心实现。

## A2 · Marriage Pact

[固定版本官方说明](https://github.com/cs106l/cs106l-assignments/blob/711bc45acca20e3d5a6be3487ed4fe73396c77c0/assignment2/README.md)

**先修**：04–06，尤其集合、队列与元素寿命。

**输入与任务**：设置 `kYourName` 为包含名和姓的名字；`students.txt` 每行一个虚构申请者姓名。`get_applicants` 读取到 set 或 unordered_set，若改用无序容器，要同步调整相关函数签名。

`find_matches` 根据与输入姓名相同的姓名首字母筛选，输出队列里保存的是**原集合中姓名的指针**，不是新复制的姓名。`get_match` 从候选队列选择一人，具体选择方式允许自行设计；无匹配时打印 `NO MATCHES FOUND.`。

```bash
cd assignment2
g++ -std=c++20 main.cpp -o main
./main
```

**自己检查**：一条姓名一行而不是一个词一条；重复姓名的集合语义；空队列不能调用 front/pop；指针目标是否真的是仍存活的集合元素，而不是循环变量副本；程序有没有依赖 unordered_set 的遍历顺序。

**简答与交付**：`main.cpp`、`short_answer.txt`。简答比较有序与无序集合、给出未在课堂展示的有效姓名哈希例子，并解释保存指针的动机与原集合销毁后的后果。本页不代写简答。

## A3 · Make a Class

[固定版本官方说明](https://github.com/cs106l/cs106l-assignments/blob/711bc45acca20e3d5a6be3487ed4fe73396c77c0/assignment3/README.md)

**先修**：07 类，09 const；类模板是可选加练。

这不是固定输入输出的解析任务，而是自行设计一个满足要求的类。必须包括：

1. 至少一个参数的自定义构造函数。
2. 无参默认构造函数。
3. 至少一个 private 数据成员。
4. 至少一个有实际意义的 private 成员函数。
5. 至少一个 public getter，且标记 const；按 README 命名要求提供对应 getter。
6. 至少一个 public setter。

声明放 `class.h`，定义放 `class.cpp`；在 `sandbox.cpp` 的 sandbox 函数中创建一个实例。不要以“程序能编译”替代检查所有六项要求。

```bash
cd assignment3
g++ -std=c++20 main.cpp class.cpp -o main
./main
```

**模板分支**：如果选择类模板，官方明确要求从编译命令移除 `class.cpp`，并按其头文件包含模板定义的布局调整；本页不替你搭建模板类。

**自己检查**：默认构造能创建有效对象；setter 不破坏自定不变量；const 对象可调用 getter；private helper 不是为凑数存在；声明与定义中的 const 一致。

**交付文件**：`class.h`、`class.cpp`、`sandbox.cpp`、`short_answer.txt`。简答分别解释 const-correctness 与自己类是否满足它，通常各 2–3 句话。

## A4 · Ispell

[固定版本官方说明](https://github.com/cs106l/cs106l-assignments/blob/711bc45acca20e3d5a6be3487ed4fe73396c77c0/assignment4/README.md)

**先修**：05–06、10–11；虽然课表在模板周发布，本作业用到 lambda 和 C++20 ranges，读完 11 再做更顺畅。

**输入与输出合同**：对文本分词，每个 Token 包含内容和在源文本中的偏移；词典提供合法词集合。拼写检查返回含建议的拼错词；建议与原词的 Damerau–Levenshtein 距离为 1；没有建议的拼错词不出现在最后结果。距离计算与外围程序已提供，不要求你重写编辑距离。

你在 `spellcheck.cpp` 完成 `tokenize` 与 `spellcheck`。明确限制：**不能使用 for/while 循环**；tokenize 用传统 STL 算法，spellcheck 用 ranges。不能用“更熟悉的循环”偷偷绕过练习目标。

```bash
cd assignment4
g++ -std=c++20 main.cpp spellcheck.cpp -o main
./main
./main "hello wrld"
./main --stdin < "examples/(marquez).txt"
```

`./main` 运行 autograder；提供文字参数可手动观察行为。还可使用 `--dict` 指定词典、`--unstyled` 去颜色、`--profile` 看时间，具体含义见官方说明。

**自己检查**：空文本、连续空白、首尾空白、标点、源偏移是否正确；捕获是否引用正确且仍存活的源字符串；空结果是否安全；只读词典不必大幅复制。读取自定义字符时注意 unsigned char 转换规则。

C++20 view 是惰性的，结果容器需要物化；`std::ranges::to` 是 C++23，官方明确要求这份作业不用它。官方 README 已给步骤，本页不重现核心管道或 lambda 实现。

**交付文件**：`spellcheck.cpp`。

## A5 · Treebook

[固定版本官方说明](https://github.com/cs106l/cs106l-assignments/blob/711bc45acca20e3d5a6be3487ed4fe73396c77c0/assignment5/README.md)

**先修**：07、09、12–13；移动概念可参看 14。

**任务合同**：已有 User 类通过原始指针数组存朋友名字，保留这种教学表示，不用 vector 擅自替换作业结构。修改 `user.h` 与 `user.cpp`：

- 非成员 friend `operator<<` 输出如 `User(name=Alice, friends=[Bob, Charlie])`，**不输出换行**。
- 提供析构、复制构造、复制赋值，复制结果应有独立资源。
- 明确删除移动构造与移动赋值；不要因为刚学了 move 就实现它们。
- 成员 `User& operator+=(User& rhs)` 表示双方建立朋友关系，并返回自身引用。
- 成员 `bool operator<(const User& rhs) const` 按名字字母顺序比较。

```bash
cd assignment5
g++ -std=c++20 main.cpp user.cpp -o main
./main
```

**自己检查**：零朋友、多个朋友的分隔符；复制后修改一个用户不影响另一用户；赋值目标原先已有资源的情况；自赋值；连续赋值；退出作用域后的资源释放。输出样子正确不能证明不存在双重释放。

**交付文件**：`user.h`、`user.cpp`。这里列出测试边界，不提供深复制或赋值的实现套路。

## A6 · Explore Courses

[固定版本官方说明](https://github.com/cs106l/cs106l-assignments/blob/711bc45acca20e3d5a6be3487ed4fe73396c77c0/assignment6/README.md)

**先修**：11 lambda、15 optional，支持 C++23 optional 单子成员的工具链。

**输入与输出合同**：使用与 A1 相同的 `courses.csv`；CourseDatabase 中的 `find_course` 根据课程标题查找，可能没有结果，需替换 starter 中的返回类型占位。主程序根据结果产生：

- 找到：`Found course: <title>,<number_of_units>,<quarter>`
- 未找到：`Course not found.`

主程序这部分要求使用 optional 的单子操作，**不能用 if 等条件语句替代所要求的链式处理**。这不意味着任何错误处理分支都被全局禁止，应以 README 指定范围为准。

```bash
cd assignment6
g++ -std=c++23 main.cpp -o main
./main
# 手动查询时，把实际 CSV 中的完整标题作为一个带引号的参数：
./main "A title copied from courses.csv"
```

最后一个参数是使用方式示意，不承诺该标题存在。不要编造课程名或把输出中额外空格当无关紧要。

**自己检查**：找到与找不到两条路径；标题包含空格；空 optional 不被无条件解引用；lambda 返回普通值还是 optional；fallback 类型是否与链前面的 optional 匹配。

官方说明中的 `std::end` 输出写法和 or_else 简化签名有笔误，详见[第 15 课](15-optional.md)。遵守输出合同，但不要照抄错字当标准 API。

**交付文件**：`main.cpp`。这里不指定作业最终选择哪两个操作或如何组合 lambda。

## A7 · Unique Pointer

[固定版本官方说明](https://github.com/cs106l/cs106l-assignments/blob/711bc45acca20e3d5a6be3487ed4fe73396c77c0/assignment7/README.md)

**先修**：09 模板、12 运算符、13–14 特殊成员与移动、16 RAII。

**任务合同**：在 `unique_ptr.h` 实现简化的独占智能指针，管理**单个 T 对象**，不要求数组版本，不使用 delete[]。接口包括构造、解引用、箭头访问、布尔判断及相应 const 版本；支持自动释放和移动，禁止复制。

在 `main.cpp` 的 `create_list` 中，把 vector 转为使用 **cs106l 命名空间的自制 unique_ptr** 的链表：保持元素顺序，空 vector 返回空指针。不能用 std::unique_ptr 替代待实现类型规避任务。README 提供构造步骤，这里不展开算法或核心代码。

```bash
cd assignment7
g++ -std=c++20 main.cpp -o main
./main
```

**自己检查**：空拥有者、单对象、移动构造、移动到原本已有资源的目标、源移动后状态、作用域释放；链表为空/单节点/多节点时顺序正确。逆向遍历时无符号下标会下溢，不要认为减到负数就一定退出。

本作业自制类型的 const 解引用接口由 starter 决定，与标准 unique_ptr 的所有细节不完全相同。先学合同，再写实现。

**简答与交付**：`unique_ptr.h`、`main.cpp`、`short_answer.txt`。四个问题围绕 RAII 好处、移动源清空的原因、递归释放长链表的栈深度问题，以及 std::move 在所有权转移中的作用。本页不提供可直接提交的简答答案。

## 完成作业后怎样解释自己的代码

不必发回审查，但建议自己回答：输入是什么，结果是什么，谁拥有每块资源，失败如何表示，哪个操作会复制或转移，边界测试如何失败。只需一段口头说明，不需要写一篇报告。

本资料编译运行的是讲义示例，**没有实现或运行官方七份作业的 autograder**。官方 starter、测试和依赖可能随版本变化；复现时优先使用本页固定提交，或重新核对你选择的新版本。
