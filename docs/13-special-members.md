# 13 · 特殊成员函数：对象如何出生、复制和结束

[上一课](12-operators.md) · [目录](../README.md) · [下一课](14-move.md)

## 先修与目标

先理解构造、析构、初始化与赋值。目标是识别对象操作对应哪个特殊成员函数，并知道何时应该不写任何资源管理代码。读完可开始 [A5 Treebook](assignments.md)，但本课不提供该作业的资源管理实现。

## 六种关键操作

| 操作 | 典型声明 | 含义 |
| --- | --- | --- |
| 默认构造 | `T()` | 不提供参数创建对象 |
| 析构 | `~T()` | 对象生命周期结束时清理 |
| 复制构造 | `T(const T&)` | 从已有对象创建新对象 |
| 复制赋值 | `T& operator=(const T&)` | 改写已经存在的对象 |
| 移动构造 | `T(T&&)` | 从可移动源创建新对象 |
| 移动赋值 | `T& operator=(T&&)` | 让已有对象接收可移动源 |

`T b = a;` 是创建 b，调用复制构造，不是复制赋值；`b = a;` 才是赋值。存在其他用户声明构造函数后，编译器不一定再自动提供无参构造；声明析构、复制或移动函数也会影响其他特殊成员的隐式生成。不要背成“六个总会免费出现”。

## Rule of Zero：优先让成员管理资源

若成员是 string、vector、unique_ptr 等资源管理类型，组合起来的类通常不需要手写析构和复制逻辑。编译器生成的操作会逐成员执行对应操作，这叫 Rule of Zero 的设计方向。

如果类用裸指针拥有资源，默认复制只复制地址，两对象可能误认为都拥有同一块内存。析构时重复释放、复制赋值时泄漏旧资源，都是所有权合同不清导致的。传统 Rule of Three 提醒一起考虑析构、复制构造、复制赋值；加入移动后是 Rule of Five。它们是设计提醒，不是“必须无脑实现五个函数”。

## 完整示例

源文件：[13-special-members.cpp](../examples/13-special-members.cpp)。一部分观察值成员的自动复制，另一部分仅用计数观察析构，不手写拥有指针的容器。

```cpp
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

struct Notebook {
    std::vector<std::string> lines;
};

class ScopeMarker {
public:
    explicit ScopeMarker(int& alive) : alive_(alive) { ++alive_; }
    ~ScopeMarker() { --alive_; }
    ScopeMarker(const ScopeMarker&) = delete;
    ScopeMarker& operator=(const ScopeMarker&) = delete;
private:
    int& alive_;
};

int main() {
    Notebook a{{"first"}};
    Notebook b = a;
    b.lines.push_back("second");
    assert(a.lines.size() == 1 && b.lines.size() == 2);
    Notebook c;
    c = b;
    c.lines.front() = "changed";
    assert(b.lines.front() == "first");
    int alive = 0;
    {
        ScopeMarker outer{alive};
        assert(alive == 1);
        {
            ScopeMarker inner{alive};
            assert(alive == 2);
        }
        assert(alive == 1);
    }
    assert(alive == 0);
    std::cout << a.lines.size() << ' ' << b.lines.size() << '\n';
}
```

输出 `1 2`。Notebook 没有手写任何特殊成员，vector/string 已提供所需值语义。ScopeMarker 借用外部计数器，后者先创建、后销毁，因此析构中使用引用有效。删除复制操作避免把一个“登记的生命周期”无意复制成两个；它不是通用智能指针实现。

## default 与 delete

`= default` 请求编译器按规则生成操作，不保证该操作对所有成员都可用；成员不可复制时，默认复制也可能被删除。`= delete` 明确禁止操作，调用者会得到编译错误。

“没有移动函数”和“显式删除移动函数”也不同：前者某些情况下可以退回复制，后者可能成为最佳重载后直接导致编译失败。A5 明确要求删除移动操作，不要为了通用习惯擅自更改作业合同。

## 易错点与 BusTub 联系

- 析构函数通常不应让异常逃出，尤其在异常展开时再次抛异常可能终止程序。
- 复制赋值必须考虑目标原有资源、自赋值和失败后的状态；通过标准成员管理资源通常更可靠。
- 锁、文件、页面 guard 常不可复制；能不能复制取决于所有权含义，不是“编译器缺少一个函数”。

## 自测

1. 示例中 `Notebook b = a;` 与 `c = b;` 分别是哪种操作？
2. 把 Notebook 的成员换成拥有动态数组的裸指针，为什么不能期待相同的独立副本行为？

<details>
<summary>参考答案</summary>

1. 前者复制构造新对象，后者对已有对象复制赋值。
2. 指针默认只复制地址，不复制所指资源，也不记录谁负责释放。需要重新设计资源所有权或使用标准管理类型。

</details>

## 来源

- [官方 Special Member Functions](https://web.stanford.edu/class/cs106l/lectures/2026Spring-13-SpecialMemberFunctions.pdf)
- [Rule of Three/Five/Zero](https://en.cppreference.com/w/cpp/language/rule_of_three)
- [来源与版本](sources.md)
