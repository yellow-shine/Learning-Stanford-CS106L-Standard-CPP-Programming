# 16.5 source_location（source_location）

## 学习目标

使用 C++20 `std::source_location` 在无需宏传参的情况下记录调用点，构建轻量诊断接口。

## 核心知识点

- **源位置（source location）**：保存文件名、函数名、行号和列号等实现提供的调用点信息。
- **`current()`**：当作为默认参数求值时，取得调用该函数的位置，而不是默认参数声明行。
- **轻量传递**：`source_location` 设计为可高效按值传递，适合日志、断言辅助和错误报告。
- **实现相关文本**：文件与函数名称的具体拼写、路径形式和列号精度不可移植地假定。
- **诊断用途**：这些信息适合给开发者定位代码，不应作为业务标识、协议字段或安全凭据。

## 最小示例

```cpp
#include <iostream>
#include <source_location>
void trace(std::source_location where = std::source_location::current()) {
    std::cout << where.file_name() << ':' << where.line() << '\n';
}
int main() { trace(); }
```

## 常见陷阱

- 在函数体内直接调用 `current()` 得到的是函数体位置，不是外部调用者位置。
- 构建路径可能泄露环境信息，对外日志应做策略化处理。
- 不要测试固定行号或完整函数名，它们会随编辑器和编译器变化。

← [上一节](16-04-类型函数.md) | [目录](../README.md) | [下一节](16-06-move与forward.md) →
