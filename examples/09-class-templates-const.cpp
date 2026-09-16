#include <cassert>
#include <iostream>
#include <string>

template <typename T>
class Slot {
public:
    explicit Slot(const T& value) : value_(value) {}
    T& value() { return value_; }
    const T& value() const { return value_; }
private:
    T value_;
};

int main() {
    Slot<int> number{7};
    number.value() = 9;
    const Slot<int>& view = number;
    assert(view.value() == 9);
    auto snapshot = view.value();
    number.value() = 10;
    assert(snapshot == 9 && view.value() == 10);
    std::string name = "Alice";
    Slot<std::string> text{name};
    name = "Bob";
    assert(text.value() == "Alice");
    int x = 1;
    int y = 2;
    const int* read_only = &x;
    read_only = &y;
    int* const fixed = &x;
    *fixed = 3;
    assert(*read_only == 2 && x == 3);
    std::cout << view.value() << ' ' << text.value() << '\n';
}
