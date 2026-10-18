#include "RealRandomGenerator.hpp"

namespace Infrastructure {

    RealRandomGenerator::RealRandomGenerator() {
        std::random_device rd;
        gen.seed(rd());
    }

    RealRandomGenerator::RealRandomGenerator(std::uint32_t seed_value)
        : gen(seed_value) {}

    Values::RouletteNumber RealRandomGenerator::next() {
        std::uniform_int_distribution<int> dist(0, 36);
        // Создаем объект RouletteNumber, передавая туда выпавшее число
        return Values::RouletteNumber(static_cast<std::int8_t>(dist(gen)));
    }

    void RealRandomGenerator::seed(std::uint32_t value) { gen.seed(value); }
}  // namespace Infrastructure
