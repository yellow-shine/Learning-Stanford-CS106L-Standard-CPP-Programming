#include <cassert>
#include <cstddef>
#include <iostream>
#include <list>
#include <string>
#include <vector>

template <typename T>
T smaller(T a, T b) {
    return b < a ? b : a;
}

template <typename InputIt, typename T>
std::size_t count_equal(InputIt first, InputIt last, const T& target) {
    std::size_t result = 0;
    for (; first != last; ++first) {
        if (*first == target) {
            ++result;
        }
    }
    return result;
}

int main() {
    assert(smaller(4, 7) == 4);
    assert(smaller(std::string{"b"}, std::string{"a"}) == "a");
    std::vector<int> values{1, 2, 1};
    std::list<int> linked{1, 3, 1, 1};
    assert(count_equal(values.begin(), values.end(), 1) == 2);
    assert(count_equal(linked.begin(), linked.end(), 1) == 3);
    assert(count_equal(values.begin(), values.begin(), 1) == 0);
    std::cout << count_equal(linked.begin(), linked.end(), 1) << '\n';
}
