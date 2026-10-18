#include <vector>

#include "RandomGenerator.hpp"

namespace Infrastructure {
    using Values::RouletteNumber;
    class TestRandomGenerator : public Ports::RandomGenerator {
       private:
        std::vector<RouletteNumber> sequence;
        size_t current = 0;

       public:
        TestRandomGenerator(const std::vector<RouletteNumber>& seq);
        RouletteNumber next() override;
        void seed(unsigned int value) override;
    };
}  // namespace Infrastructure