#include <cassert>
#include <iostream>
#include <string>
#include <vector>

struct Notebook {
    std::vector<std::string> lines;
};

class ScopeMarker {
public:
    explicit ScopeMarker(int& alive) : alive_(alive) { ++alive_; }
    ~ScopeMarker() { --alive_; }
    ScopeMarker(const ScopeMarker&) = delete;
    ScopeMarker& operator=(const ScopeMarker&) = delete;
private:
    int& alive_;
};

int main() {
    Notebook a{{"first"}};
    Notebook b = a;
    b.lines.push_back("second");
    assert(a.lines.size() == 1 && b.lines.size() == 2);
    Notebook c;
    c = b;
    c.lines.front() = "changed";
    assert(b.lines.front() == "first");
    int alive = 0;
    {
        ScopeMarker outer{alive};
        assert(alive == 1);
        {
            ScopeMarker inner{alive};
            assert(alive == 2);
        }
        assert(alive == 1);
    }
    assert(alive == 0);
    std::cout << a.lines.size() << ' ' << b.lines.size() << '\n';
}
