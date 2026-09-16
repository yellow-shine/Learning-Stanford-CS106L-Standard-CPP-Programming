#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

int main() {
    std::string source = "payload";
    std::string destination = std::move(source);
    assert(destination == "payload");
    // 不断言 source 为空：对 string 通常只保证有效但状态未指定。
    source = "reused";
    assert(source == "reused");
    auto owner = std::make_unique<int>(42);
    int* borrowed = owner.get();
    auto receiver = std::move(owner);
    assert(!owner && receiver && *receiver == 42);
    assert(borrowed == receiver.get());
    std::vector<std::unique_ptr<int>> values;
    values.push_back(std::move(receiver));
    assert(!receiver && *values.front() == 42);
    assert(*borrowed == 42);
    std::cout << destination << ' ' << *values.front() << '\n';
}
