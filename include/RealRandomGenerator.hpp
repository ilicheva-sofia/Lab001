#pragma once
#include <random>

#include "RandomGenerator.hpp"

namespace Infrastructure {
    class RealRandomGenerator : public Ports::RandomGenerator {
       private:
        std::mt19937 gen;

       public:
        RealRandomGenerator();  //  real random

        // случайное  с  seed для воспроизводимости
        explicit RealRandomGenerator(std::uint32_t seed_value);

        Values::RouletteNumber next() override;
        void seed(std::uint32_t value) override;
    };
}  // namespace Infrastructure
