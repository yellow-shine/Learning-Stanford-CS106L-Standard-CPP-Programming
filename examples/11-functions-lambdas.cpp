#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <vector>

int main() {
    std::vector<int> readings{12, 5, 20, 8};
    int threshold = 10;
    auto above = [threshold](int value) { return value > threshold; };
    auto live_above = [&threshold](int value) { return value > threshold; };
    threshold = 15;
    assert(above(12) && !live_above(12));
    std::vector<int> selected;
    std::copy_if(readings.begin(), readings.end(),
                 std::back_inserter(selected), above);
    assert((selected == std::vector<int>{12, 20}));
    std::vector<int> doubled;
    std::transform(selected.begin(), selected.end(),
                   std::back_inserter(doubled),
                   [](int value) { return value * 2; });
    std::sort(doubled.begin(), doubled.end(),
              [](int a, int b) { return a > b; });
    assert((doubled == std::vector<int>{40, 24}));
    std::cout << doubled.front() << '\n';
}
