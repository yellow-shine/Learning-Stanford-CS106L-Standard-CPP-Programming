#include <cassert>
#include <iostream>
#include <set>
#include <sstream>

class RecordId {
public:
    explicit RecordId(int value) : value_(value) {}
    bool operator==(const RecordId& rhs) const {
        return value_ == rhs.value_;
    }
    bool operator<(const RecordId& rhs) const {
        return value_ < rhs.value_;
    }
    friend std::ostream& operator<<(std::ostream& out, const RecordId& id) {
        return out << "RecordId(" << id.value_ << ')';
    }
private:
    int value_;
};

int main() {
    RecordId a{2};
    RecordId b{5};
    assert(a < b && a == RecordId{2});
    std::set<RecordId> ids{b, a, a};
    assert(ids.size() == 2 && *ids.begin() == a);
    std::ostringstream out;
    out << a << ' ' << b;
    assert(out.str() == "RecordId(2) RecordId(5)");
    std::cout << out.str() << '\n';
}
