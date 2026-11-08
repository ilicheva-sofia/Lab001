#include <gtest/gtest.h>

#include "DomainError.hpp"
#include "Player.hpp"
#include "PlayerId.hpp"

TEST(PlayerTest, PayoutIncreasesBalance) {
    Domain::Player p(Values::PlayerId(1), Values::Money(1000'00));
    p.receive_payout(Values::Money(500'00));
    EXPECT_EQ(p.get_balance(), Values::Money(1500'00));
}

TEST(PlayerTest, SpendDecreasesBalance) {
    Domain::Player p(Values::PlayerId(1), Values::Money(1000'00));
    p.spend_money(Values::Money(300'00));
    EXPECT_EQ(p.get_balance(), Values::Money(700'00));
}

TEST(PlayerTest, SpendBeyondBalanceThrows) {
    Domain::Player p(Values::PlayerId(1), Values::Money(1000'00));
    EXPECT_THROW(p.spend_money(Values::Money(2000'00)),
                 Exceptions::InvalidMoneyOperation);
    // Баланс не должен измениться после исключения
    EXPECT_EQ(p.get_balance(), Values::Money(1000'00));
}
