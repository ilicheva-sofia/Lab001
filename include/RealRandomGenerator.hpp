#pragma once
#include <random>

#include "RandomGenerator.hpp"
#include "RouletteNumber.hpp"

namespace Infrastructure {
    using Values::RouletteNumber;
    class RealRandomGenerator : public Ports::RandomGenerator {
       private:
        std::random_device rd;
        std::mt19937 gen;

       public:
        RealRandomGenerator() : gen(rd()) {}
        RouletteNumber next() override;
        void seed(unsigned int value) override;
    };
}  // namespace Infrastructure