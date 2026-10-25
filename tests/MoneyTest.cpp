#include <gtest/gtest.h>

#include "DomainError.hpp"
#include "Money.hpp"

// Проверка защиты от отрицательных денег
TEST(MoneyTest, ThrowsOnNegativeAmount) {
    EXPECT_THROW(
        { Values::Money bad_money(static_cast<std::int64_t>(-500'00)); },
        Exceptions::InvalidMoney);
}

// Проверка ухода баланса в минус при вычитании
TEST(MoneyTest, ThrowsOnInsufficientFunds) {
    Values::Money wallet(500'00);  // 500 руб
    Values::Money price(600'00);   // 600 руб

    EXPECT_THROW(
        { Values::Money result = wallet - price; },
        Exceptions::InvalidMoneyOperation);
}

// Проверка работы оператора умножения на коэффициент
TEST(MoneyTest, MultipliesCorrectly) {
    Values::Money bet(100'00);
    Values::Money payout = bet * 35;
    EXPECT_EQ(payout.get_kopecks(), 3500'00);
}
