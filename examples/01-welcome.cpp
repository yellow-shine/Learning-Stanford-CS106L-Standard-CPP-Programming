#include <cassert>
#include <iostream>

int main() {
    const int lessons = 16;
    assert(lessons > 0);
    std::cout << "Ready for " << lessons << " lessons\n";
}
