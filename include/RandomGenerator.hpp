#pragma once
#include "RouletteNumber.hpp"

namespace Ports {
    struct RandomGenerator {
        virtual Values::RouletteNumber next() = 0;
        virtual void seed(std::uint32_t value) = 0;
        virtual ~RandomGenerator() = default;
    };
}  // namespace Ports
