#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> values{1, 2, 3, 4, 5};
    for (auto it = values.begin(); it != values.end();) {
        if (*it % 2 == 0) {
            it = values.erase(it);
        } else {
            ++it;
        }
    }
    assert((values == std::vector<int>{1, 3, 5}));
    const auto found = std::find(values.begin(), values.end(), 3);
    assert(found != values.end());
    assert(*found == 3);
    int number = 7;
    int* p = &number;
    int* q = p;
    *q = 9;
    assert(*p == 9);
    // p 和 q 借用局部对象，不允许对它们执行 delete。
    std::cout << values.size() << ' ' << number << '\n';
}
