#pragma once
#include <cstddef>
#include <vector>

#include "RandomGenerator.hpp"
#include "RouletteNumber.hpp"

namespace Test {

    class TestRandomGenerator : public Ports::RandomGenerator {
       private:
        std::vector<Values::RouletteNumber> sequence;
        std::size_t current = 0;

       public:
        explicit TestRandomGenerator(
            const std::vector<Values::RouletteNumber>& seq);

        Values::RouletteNumber next() override;
        void seed(std::uint32_t value) override;
    };

}  // namespace Test
