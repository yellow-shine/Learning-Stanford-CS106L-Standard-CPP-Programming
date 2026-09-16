# 04 · Streams：把输入当成有状态的过程

[上一课](03-initialization-references.md) · [目录](../README.md) · [下一课](05-containers.md)

## 先修与目标

先读初始化与引用。学会分清按词读取、按行读取和错误状态，并写出不会在坏输入上死循环的读取代码。完成后可以开始 [A1](assignments.md)。

## 同一套接口，三种数据来源

`std::cin`/`std::cout` 处理标准输入输出；`std::ifstream`/`std::ofstream` 处理文件；`std::istringstream`/`std::ostringstream` 处理内存中的字符串。共同点是流维护读取位置和状态。

`in >> value` 按目标类型解析，通常跳过前导空白。读取整数时碰到不合法文字会设置失败状态，而不是凭空得到一个可靠数字。`std::getline(in, line)` 读到换行符，移除分隔符但保留行内空格，适合名字、标题等字段。

可靠循环是 `while (std::getline(in, line))` 或 `while (in >> number)`：先尝试读取，再检查是否成功。`while (!in.eof())` 把判断放在读取前，可能多处理一次旧值，因为 eof 状态往往在尝试越过结尾后才显现。

## 从原始输入到有效记录

先明确格式，再检查每一层：整行是否存在、字段是否可解析、数值是否合理、有没有额外内容。示例的格式是“传感器编号 空格 温度”，不是通用 CSV，温度范围也是这个小程序自定的合同。

## 完整示例

源文件：[04-streams.cpp](../examples/04-streams.cpp)。字符串流让检查不依赖键盘输入和工作目录。

```cpp
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

bool valid_reading(const std::string& line, int& id, double& temperature) {
    std::istringstream row{line};
    int parsed_id = 0;
    double parsed_temperature = 0;
    if (!(row >> parsed_id >> parsed_temperature)) {
        return false;
    }
    row >> std::ws;
    if (!row.eof() || parsed_id < 0 ||
        parsed_temperature < -100 || parsed_temperature > 100) {
        return false;
    }
    id = parsed_id;
    temperature = parsed_temperature;
    return true;
}

int main() {
    int id = -1;
    double temperature = 0;
    assert(valid_reading("7 21.5", id, temperature));
    assert(id == 7 && temperature == 21.5);
    assert(!valid_reading("7 warm", id, temperature));
    assert(!valid_reading("7 21.5 junk", id, temperature));
    assert(!valid_reading("", id, temperature));
    assert(id == 7 && temperature == 21.5);
    std::istringstream input{"first line\nsecond line\n"};
    std::string line;
    int lines = 0;
    while (std::getline(input, line)) {
        ++lines;
    }
    assert(lines == 2);
    std::cout << id << ' ' << temperature << '\n';
}
```

解析先写局部变量，全部检查通过后才修改输出参数，避免失败时留下“修改了一半”的结果。`std::ws` 消耗剩余空白；若还有非空白字符，就拒绝该行。输出 `7 21.5`。

## 文件与失败状态

文件流创建后必须检查是否打开成功：`std::ifstream input{"data.txt"}; if (!input) { ... }`。写入也可能因为磁盘满或权限失败而出错；重要数据写完后检查流状态，不能只检查打开。

若从交互输入恢复，`clear()` 仅重置失败标志，不会删除坏字符；通常还需要 `ignore(...)` 丢弃坏行。字符串流示例每次新建一个流，自然隔离了各行的错误状态。

混用 `>>` 与 `getline` 时，前一次读取可能留下换行符，让下一次得到空行。应根据输入格式选择“完整读行后解析”，或明确消耗分隔符；不要盲目调用 `std::ws`，它也会跳过你可能想保留的空行与前导空格。

## 易错点与 BusTub 联系

- 简单按逗号拆分不能正确处理所有 CSV 引号和转义。A1 按官方提供的数据与工具合同完成，不要宣称实现了通用 CSV 解析器。
- `std::endl` 除了换行还会刷新缓冲，普通日志换行通常用 `'\n'`。
- 数据库读盘失败需要显式处理；空数据、文件不存在与解析失败不应混为一谈。

## 自测

1. 为什么 `while (!input.eof())` 不如直接检查读取结果？
2. 本例错误输入后为什么保留上一次有效的 `id`？

<details>
<summary>参考答案</summary>

1. 结束状态可能在下一次读取失败后才出现，提前检查不能保证本次读取成功。
2. 函数先解析到局部变量，确认全部合法后才赋值给输出引用。这是本例明确选择的失败合同。

</details>

## 来源

- [官方 Streams](https://web.stanford.edu/class/cs106l/lectures/2026Spring-04-Streams.pdf)
- [输入输出库参考](https://en.cppreference.com/w/cpp/io)
- [来源与版本](sources.md)
