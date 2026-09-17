# std-optional

## 它解决什么问题？

表达：

> “这里可能有一个 `T`，也可能什么都没有。”

例如查 Hash Table：

```cpp
std::optional<int> Find(const std::string &key);
```

如果找到：

```cpp
return 42;
```

没找到：

```cpp
return std::nullopt;
```

调用：

```cpp
auto result = Find("foo");

if (result.has_value()) {
  std::cout << result.value();
}
```

更常见：

```cpp
if (result) {
  std::cout << *result;
}
```

---

## 为什么不直接返回 `nullptr`？

如果返回的是：

```cpp
Tuple *
```

那么 `nullptr` 可以表达不存在。

但如果你要返回：

```cpp
int
PageId
RID
Tuple
```

这些是值类型，没办法用 `nullptr`。

过去经常写：

```cpp
int Find(...); // -1 表示不存在
```

问题是 `-1` 本身可能是合法值。

所以：

```cpp
std::optional<int>
```

语义清晰得多。

---

## BusTub 典型场景

例如：

```cpp
std::optional<page_id_t> LookupPage(...);
std::optional<Tuple> GetTuple(...);
std::optional<FrameId> FindVictim(...);
```

这是非常常见的模式：

```text
成功
   ↓
optional<T>
   ↓
包含 T

失败 / 不存在
   ↓
nullopt
```

---

## 一个重要 API

```cpp
value_or()
```

例如：

```cpp
int value = result.value_or(0);
```

意思是：

```text
有值 -> 使用值
没值 -> 使用 0
```

## 注意事项

- `std::optional` 从 C++17 开始提供，头文件为 `<optional>`。
- 无值时调用 `value()` 会抛出 `std::bad_optional_access`；使用 `*result` 前必须确认有值。
- `nullopt` 表达“无值”，是否代表失败由接口约定；它不携带错误原因。

---

来源：02.md（重复内容已合并）。
