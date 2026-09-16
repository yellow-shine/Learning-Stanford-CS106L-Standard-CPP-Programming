#include <cassert>
#include <iostream>
#include <limits>
#include <optional>

std::optional<int> safe_magnitude(int value) {
    if (value == std::numeric_limits<int>::min()) {
        return std::nullopt;
    }
    return value < 0 ? -value : value;
}

int main() {
    const auto present = safe_magnitude(-7);
    assert(present && *present == 7);
    const auto zero = safe_magnitude(0);
    assert(zero.has_value() && *zero == 0);
    const auto absent = safe_magnitude(std::numeric_limits<int>::min());
    assert(!absent && absent.value_or(99) == 99);
    bool threw = false;
    try {
        (void)absent.value();
    } catch (const std::bad_optional_access&) {
        threw = true;
    }
    assert(threw);
    std::cout << present.value_or(0) << '\n';
}
