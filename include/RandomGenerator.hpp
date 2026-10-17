#pragma once
#include "RouletteNumber.hpp"

namespace Ports {
    using Values::RouletteNumber;

    struct RandomGenerator {
        virtual RouletteNumber next() = 0;
        virtual void seed(unsigned int value) = 0;
        virtual ~RandomGenerator() = default;
    };
}  // namespace Ports
