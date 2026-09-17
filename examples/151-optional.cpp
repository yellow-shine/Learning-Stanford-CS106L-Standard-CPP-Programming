#include <iostream>
#include <string>
#include <utility>

class MyClass {
public:
  // 普通构造函数
  explicit MyClass(std::string value) : member_(std::move(value)) {
    std::cout << "constructor\n";
  }

  // 拷贝构造函数
  MyClass(const MyClass &other) : member_(other.member_) {
    std::cout << "copy constructor\n";
  }

  // 移动构造函数
  MyClass(MyClass &&other) noexcept : member_(std::move(other.member_)) {
    std::cout << "move constructor\n";
  }

  void Print(const std::string &name) const {
    std::cout << name << ".member_ = \"" << member_ << "\"\n";
  }

private:
  std::string member_;
};

int main() {
  std::string value = "hello";
  std::string b(std::move(value));
  std::cout << "b = \"" << b << "\"\n";
  std::cout << "value = \"" << value << "\"\n";

  return 0;
}
