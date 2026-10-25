#include "TestRandomGenerator.hpp"

namespace Test {

    TestRandomGenerator::TestRandomGenerator(
        const std::vector<Values::RouletteNumber>& seq)
        : sequence(seq), current(0) {}

    Values::RouletteNumber TestRandomGenerator::next() {
        if (sequence.empty()) {
            return Values::RouletteNumber(0);
        }

        if (current >= sequence.size()) {
            current = 0;  // Зацикливаем последовательность по кругу
        }
        return sequence[current++];
    }

    void TestRandomGenerator::seed(std::uint32_t /*value*/) {
        // Тестовый генератор игнорирует сид
    }

}  // namespace Test
