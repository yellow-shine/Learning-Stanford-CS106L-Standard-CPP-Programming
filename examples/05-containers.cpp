#include <cassert>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> events{"read", "write", "read"};
    events.reserve(20);
    assert(events.size() == 3);
    std::map<std::string, int> counts;
    for (const auto& event : events) {
        ++counts[event];
    }
    assert(counts.at("read") == 2);
    const auto before = counts.size();
    assert(counts.find("missing") == counts.end());
    assert(counts.size() == before);
    std::set<std::string> kinds(events.begin(), events.end());
    assert(kinds.size() == 2);
    std::queue<int> pending;
    pending.push(7);
    pending.push(8);
    assert(pending.front() == 7);
    pending.pop();
    assert(pending.front() == 8);
    std::cout << counts.at("read") << ' ' << kinds.size() << '\n';
}
