#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>

class Resource {
public:
    explicit Resource(int& alive) : alive_(alive) { ++alive_; }
    ~Resource() { --alive_; }
    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;
private:
    int& alive_;
};

void fail_after_acquire(int& alive) {
    auto owner = std::make_unique<Resource>(alive);
    assert(alive == 1);
    throw std::runtime_error("operation failed");
}

int main() {
    int alive = 0;
    bool caught = false;
    try {
        fail_after_acquire(alive);
    } catch (const std::runtime_error&) {
        caught = true;
    }
    assert(caught && alive == 0);
    std::weak_ptr<int> observer;
    {
        auto shared = std::make_shared<int>(7);
        observer = shared;
        const auto temporary_owner = observer.lock();
        assert(temporary_owner && *temporary_owner == 7);
    }
    assert(observer.expired());
    std::cout << "resources alive: " << alive << '\n';
}
