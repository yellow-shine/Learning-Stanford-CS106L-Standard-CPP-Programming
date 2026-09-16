#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

class Score {
public:
    explicit Score(int initial, int maximum = 100,
                   const std::string& owner = "anonymous")
        : score_(initial), maximum_(maximum), owner_(owner) {
        if (maximum_ < 0 || score_ < 0 || score_ > maximum_) {
            throw std::invalid_argument("invalid score");
        }
    }

    int value() const { return score_; }
    const std::string& owner() const { return owner_; }

    void add(int points) {
        if (points < 0 || points > maximum_ - score_) {
            throw std::out_of_range("score exceeds limit");
        }
        score_ += points;
    }

private:
    int score_;
    int maximum_;
    std::string owner_;
};

int main() {
    Score a{10, 100, "Alice"};
    Score b(10);
    const Score& view = a;
    a.add(5);
    assert(view.value() == 15 && b.value() == 10);
    int result = view.value();
    result = 99;
    assert(view.value() == 15 && result == 99);
    bool rejected = false;
    try {
        a.add(100);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected && a.value() == 15);
    std::cout << view.owner() << ' ' << view.value() << '\n';
}
