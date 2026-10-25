#include <gtest/gtest.h>

#include "Bet.hpp"
#include "Money.hpp"
#include "RouletteNumber.hpp"

// Тест выигрыша ставки на цвет (выплата 1:1 + возврат = Сумма * 2)
TEST(BetPayoutTest, ColorBetPayoutOnWin) {
    Bets::ColorBet red_bet(Values::Money(1000'00), Values::Color::Red);
    Values::RouletteNumber winning_red(1);  // 1 - это Red

    Values::Money expected_payout = Values::Money(2000'00);
    EXPECT_EQ(red_bet.payout(winning_red), expected_payout);
}

// Тест проигрыша ставки на цвет (выплата 0)
TEST(BetPayoutTest, ColorBetPayoutOnLoss) {
    Bets::ColorBet red_bet(Values::Money(1000'00), Values::Color::Red);
    Values::RouletteNumber winning_black(2);  // 2 - это Black

    EXPECT_EQ(red_bet.payout(winning_black), Values::Money::zero());
}

// Тест выигрыша ставки на число (выигрыш 35:1 + возврат = Сумма * 36)
TEST(BetPayoutTest, SingleNumberBetPayoutOnWin) {
    Bets::SingleNumberBet number_bet(Values::Money(100'00),
                                     Values::RouletteNumber(17));
    Values::RouletteNumber lucky_seventeen(17);

    Values::Money expected_payout = Values::Money(3600'00);  // 100 * 36
    EXPECT_EQ(number_bet.payout(lucky_seventeen), expected_payout);
}
// Тест проигрыша ставки на число
TEST(BetPayoutTest, SingleNumberBetPayoutOnLoss) {
    Bets::SingleNumberBet number_bet(Values::Money(100'00),
                                     Values::RouletteNumber(17));
    Values::RouletteNumber wrong_number(5);  // Выпало 5 вместо 17

    EXPECT_EQ(number_bet.payout(wrong_number), Values::Money::zero());
}

// Тест текстового описания ставок
TEST(BetPayoutTest, VerifiesBetDescriptions) {
    Bets::ColorBet red_bet(Values::Money(1000'00), Values::Color::Red);
    Bets::ColorBet black_bet(Values::Money(200'00), Values::Color::Black);
    Bets::SingleNumberBet num_bet(Values::Money(500'00),
                                  Values::RouletteNumber(7));

    // Проверяем генерацию строк (использует наш std::formatter)
    EXPECT_FALSE(red_bet.describe().empty());
    EXPECT_FALSE(black_bet.describe().empty());
    EXPECT_FALSE(num_bet.describe().empty());
}
