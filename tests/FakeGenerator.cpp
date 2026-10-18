#include <gtest/gtest.h>

#include "TestRandomGenerator.hpp"
namespace Adapters {

    class FakeGenerator : public RandomGenerator {
        std::vector<RouletteNumber> sequence;
        size_t index = 0;

       public:
        FakeGenerator(std::vector<int> seq) {
            for (int v : seq) sequence.emplace_back(v);
        }
        RouletteNumber next() override {
            if (index_ >= sequence.size())
                throw std::runtime_error("FakeGenerator exhausted");
            return sequence_[index_++];
        }
        void seed(unsigned int) override {}  // no-op для фейка
    };
}

TEST_CASE("Color bet wins on red") {
    FakeGenerator rng{17};  // 17 — красное
    RouletteTable table {
        TableLimits{Money{100}, Money{10000}, Money{1000000}};
        table.accept_bet(player1,
                         std::make_unique<ColorBet>(Money{1000}, Color::Red));

        SpinResult result = table.spin(rng);

        REQUIRE(result.number().value() == 17);
        REQUIRE(result.payouts().at(0).amount == Money{2000});  // 1:1 + возврат
    }

    TEST_CASE("Same seed produces same sequence") {
        Mt19937Generator rng1;
        Mt19937Generator rng2;

        rng1.seed(42);
        rng2.seed(42);

        for (int i = 0; i < 100; ++i) {
            REQUIRE(rng1.next() == rng2.next());
        }
    }

    TEST(RouletteTest, TestFixedSequence) {
        std::vector<RouletteNumber> sequence = {1, 2, 3, 0, 27};
        Test::TestRandomGenerator rng(sequence);

        // Здесь идут тесты
    }