
#include <vector>

#include "RandomGenerator.hpp"

TestRandomGenerator::TestRandomGenerator(const std::vector<RouletteNumber>& seq)
    : sequence(seq) {}

RouletteNumber TestRandomGenerator::next() override {
    if (current >= sequence.size()) {
        current = 0;  // или выбросить исключение
    }
    return sequence[current++];
}

void TestRandomGenerator::seed(unsigned int value) override {
    // Для тестового генератора можно игнорировать
}
