#include <gtest/gtest.h>

#include "Bet.hpp"
#include "Money.hpp"
#include "RouletteNumber.hpp"

TEST(BetDescribeTest, ColorBetDescribesAllThreeColors) {
    Bets::ColorBet red(Values::Money(100'00), Values::Color::Red);
    Bets::ColorBet black(Values::Money(100'00), Values::Color::Black);
    Bets::ColorBet green(Values::Money(100'00), Values::Color::Green);

    // Третий вариант ветки ternary: Green
    EXPECT_TRUE(red.describe().find("Red") != std::string::npos);
    EXPECT_TRUE(black.describe().find("Black") != std::string::npos);
    EXPECT_TRUE(green.describe().find("Green") != std::string::npos);

    EXPECT_TRUE(red.describe().find("100.00 RUB") != std::string::npos);
}
