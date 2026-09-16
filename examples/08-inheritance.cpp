#include <cassert>
#include <iostream>

class Expression {
public:
    virtual ~Expression() = default;
    virtual int evaluate() const = 0;
};

class Literal final : public Expression {
public:
    explicit Literal(int value) : value_(value) {}
    int evaluate() const override { return value_; }
private:
    int value_;
};

int read_expression(const Expression& expression) {
    return expression.evaluate();
}

int main() {
    Literal literal{42};
    const Expression& base = literal;
    assert(base.evaluate() == 42);
    assert(read_expression(literal) == 42);
    std::cout << read_expression(base) << '\n';
}
