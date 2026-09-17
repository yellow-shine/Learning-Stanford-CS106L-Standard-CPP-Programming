# 16.6 move() 与 forward()（move() and forward()）

## 学习目标

理解移动与完美转发基于值类别的机制，正确处理移动后对象和转发引用。

## 核心知识点

- **`std::move`**：把表达式转换为将亡值，本身不移动任何资源；后续构造或赋值决定是否移动。
- **移动后状态**：标准库对象通常仍有效但状态未指定，可销毁、重新赋值或执行明确允许的操作。
- **转发引用（forwarding reference）**：模板形参 `T&&` 在推导上下文中可绑定左值或右值。
- **`std::forward`**：根据推导出的 `T` 保留实参原有值类别，常用于包装器向下一层传参。
- **隐式移动**：返回局部对象通常应直接 `return x;`，复制消除或隐式移动优于手工 `move`。

## 最小示例

```cpp
#include <memory>
#include <utility>
template<class F, class T>
decltype(auto) call(F&& f, T&& x) {
    return std::forward<F>(f)(std::forward<T>(x));
}
int main() {
    auto p = std::make_unique<int>(7);
    auto q = std::move(p);
    return !p && *q == 7 && call([](int x){ return x + 1; }, 1) == 2 ? 0 : 1;
}
```

## 常见陷阱

- 对 `const` 对象使用 `move` 往往仍会复制，因为移动构造通常需要非 `const` 右值。
- 不要读取移动后对象的旧值，除非该类型明确保证。
- 返回局部变量时滥用 `std::move` 可能阻碍复制消除。

← [上一节](16-05-source-location.md) | [目录](../README.md) | [下一节](16-07-位操作.md) →
