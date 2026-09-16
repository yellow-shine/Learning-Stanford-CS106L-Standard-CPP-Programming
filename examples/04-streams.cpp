#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

bool valid_reading(const std::string& line, int& id, double& temperature) {
    std::istringstream row{line};
    int parsed_id = 0;
    double parsed_temperature = 0;
    if (!(row >> parsed_id >> parsed_temperature)) {
        return false;
    }
    row >> std::ws;
    if (!row.eof() || parsed_id < 0 ||
        parsed_temperature < -100 || parsed_temperature > 100) {
        return false;
    }
    id = parsed_id;
    temperature = parsed_temperature;
    return true;
}

int main() {
    int id = -1;
    double temperature = 0;
    assert(valid_reading("7 21.5", id, temperature));
    assert(id == 7 && temperature == 21.5);
    assert(!valid_reading("7 warm", id, temperature));
    assert(!valid_reading("7 21.5 junk", id, temperature));
    assert(!valid_reading("", id, temperature));
    assert(id == 7 && temperature == 21.5);
    std::istringstream input{"first line\nsecond line\n"};
    std::string line;
    int lines = 0;
    while (std::getline(input, line)) {
        ++lines;
    }
    assert(lines == 2);
    std::cout << id << ' ' << temperature << '\n';
}
