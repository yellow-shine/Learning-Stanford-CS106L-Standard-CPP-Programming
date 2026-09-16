#include <cassert>
#include <iostream>
#include <string>

struct Label {
    std::string text;
    explicit Label(const std::string& input) : text(input) {}
};

void update(int x, int& y) {
    x += 10;
    y += x;
}

int make_value() {
    int local = 42;
    return local;
}

int& identity(int& value) {
    return value;
}

int main() {
    int a = 1;
    int b = 2;
    int copy = a;
    int& alias = b;
    alias = 6;
    update(a, b);
    assert(a == 1 && copy == 1 && b == 17);
    const int& read_only = b;
    b = 20;
    assert(read_only == 20);
    identity(a) = 9;
    assert(a == 9 && make_value() == 42);
    std::string title = "Database Systems";
    Label label{title};
    title = "Operating Systems";
    assert(label.text == "Database Systems");
    std::cout << label.text << '\n';
}
