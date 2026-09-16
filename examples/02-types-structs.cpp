#include <cassert>
#include <iostream>
#include <string>

struct Book {
    std::string title;
    int pages{};
    bool borrowed{};
};

int main() {
    Book original{"Database Systems", 500, false};
    Book copy = original;
    copy.title = "Operating Systems";
    copy.borrowed = true;
    assert(original.title == "Database Systems");
    assert(!original.borrowed && copy.borrowed);
    Book empty{};
    assert(empty.pages == 0 && !empty.borrowed);
    auto pages = original.pages;
    pages += 1;
    assert(original.pages == 500 && pages == 501);
    std::cout << original.title << ": " << original.pages << '\n';
}
