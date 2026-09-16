#include <cassert>
#include <future>
#include <iostream>
#include <mutex>

class Counter {
public:
    void increment() {
        std::lock_guard<std::mutex> lock{mutex_};
        ++value_;
    }
    int value() const {
        std::lock_guard<std::mutex> lock{mutex_};
        return value_;
    }
private:
    mutable std::mutex mutex_;
    int value_ = 0;
};

int main() {
    Counter counter;
    auto work = [&counter] {
        for (int i = 0; i < 1000; ++i) {
            counter.increment();
        }
    };
    auto first = std::async(std::launch::async, work);
    auto second = std::async(std::launch::async, work);
    first.get();
    second.get();
    assert(counter.value() == 2000);
    std::cout << counter.value() << '\n';
}
