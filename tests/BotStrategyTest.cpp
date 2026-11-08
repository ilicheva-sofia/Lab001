#include <gtest/gtest.h>

#include "BotStrategy.hpp"
#include "Money.hpp"
#include "Player.hpp"
#include "PlayerId.hpp"
#include "SpinResult.hpp"
#include "SpinStatus.hpp"

namespace {
    Domain::Player make_player(Values::Money balance) {
        return Domain::Player(Values::PlayerId(1), balance);
    }
}  // namespace

// ============================ RedConstantBot ============================

TEST(RedConstantBotTest, PlaysWhenBalanceIsEnough) {
    Domain::RedConstantBot bot("bot", Values::Money(5000'00),
                               Values::Money(3000'00), Values::Money(10000'00),
                               Values::Money(200'00));
    auto decision =
        bot.make_decision(make_player(Values::Money(10000'00)));
    EXPECT_TRUE(decision.wants_to_play);
    ASSERT_NE(decision.bet, nullptr);
    EXPECT_EQ(decision.bet->get_amount(), Values::Money(200'00));
}

TEST(RedConstantBotTest, LeavesWhenNoMoneyForBet) {
    Domain::RedConstantBot bot("bot", Values::Money(5000'00),
                               Values::Money(3000'00), Values::Money(10000'00),
                               Values::Money(500'00));
    auto decision = bot.make_decision(make_player(Values::Money(400'00)));
    EXPECT_FALSE(decision.wants_to_play);
    EXPECT_EQ(decision.bet, nullptr);
}

TEST(RedConstantBotTest, LeavesWhenTakeProfitReached) {
    Domain::RedConstantBot bot("bot", Values::Money(5000'00),
                               Values::Money(3000'00), Values::Money(10000'00),
                               Values::Money(200'00));
    auto decision =
        bot.make_decision(make_player(Values::Money(15000'00)));
    EXPECT_FALSE(decision.wants_to_play);
    EXPECT_EQ(decision.bet, nullptr);
}

TEST(RedConstantBotTest, LeavesWhenStopLossReached) {
    Domain::RedConstantBot bot("bot", Values::Money(5000'00),
                               Values::Money(3000'00), Values::Money(10000'00),
                               Values::Money(200'00));
    auto decision = bot.make_decision(make_player(Values::Money(7000'00)));
    EXPECT_FALSE(decision.wants_to_play);
    EXPECT_EQ(decision.bet, nullptr);
}

// ============================ MartingaleBot ============================

TEST(MartingaleBotTest, DoublesAfterLoss) {
    Domain::MartingaleBot bot("m", Values::Money(5000'00),
                              Values::Money(5000'00), Values::Money(10000'00),
                              Values::Money(100'00));
    auto d1 = bot.make_decision(make_player(Values::Money(10000'00)));
    ASSERT_TRUE(d1.wants_to_play);
    EXPECT_EQ(d1.bet->get_amount(), Values::Money(100'00));

    // Проигрыш: выпало ЧЕРНОЕ, бот ставил на ЧЕРНОЕ... нет, ставил на Black,
    // а выпало Red -> проигрыш
    Values::SpinResult loss(Values::RouletteNumber(1),
                            Values::SpinStatus::Completed, {});
    bot.update_result(loss, Values::PlayerId(1));

    auto d2 = bot.make_decision(make_player(Values::Money(10000'00)));
    ASSERT_TRUE(d2.wants_to_play);
    EXPECT_EQ(d2.bet->get_amount(), Values::Money(200'00));
}

TEST(MartingaleBotTest, ResetsAfterWin) {
    Domain::MartingaleBot bot("m", Values::Money(5000'00),
                              Values::Money(5000'00), Values::Money(10000'00),
                              Values::Money(100'00));
    // Сначала удвоим ставку (проигрыш)
    Values::SpinResult loss(Values::RouletteNumber(1),
                            Values::SpinStatus::Completed, {});
    bot.update_result(loss, Values::PlayerId(1));

    // Теперь выигрыш: выплата игроку 1
    Values::PayoutEntry win(Values::PlayerId(1), Values::Money(400'00));
    Values::SpinResult win_result(Values::RouletteNumber(2),
                                  Values::SpinStatus::Completed,
                                  {std::move(win)});
    bot.update_result(win_result, Values::PlayerId(1));

    auto d = bot.make_decision(make_player(Values::Money(10000'00)));
    ASSERT_TRUE(d.wants_to_play);
    EXPECT_EQ(d.bet->get_amount(), Values::Money(100'00));
}

TEST(MartingaleBotTest, CancelledSpinDoesNotChangeBet) {
    Domain::MartingaleBot bot("m", Values::Money(5000'00),
                              Values::Money(5000'00), Values::Money(10000'00),
                              Values::Money(100'00));
    Values::SpinResult cancelled(Values::RouletteNumber(0),
                                 Values::SpinStatus::Cancelled, {});
    bot.update_result(cancelled, Values::PlayerId(1));

    auto d = bot.make_decision(make_player(Values::Money(10000'00)));
    ASSERT_TRUE(d.wants_to_play);
    EXPECT_EQ(d.bet->get_amount(), Values::Money(100'00));
}

TEST(MartingaleBotTest, LeavesWhenCannotAffordDoubledBet) {
    Domain::MartingaleBot bot("m", Values::Money(5000'00),
                              Values::Money(5000'00), Values::Money(10000'00),
                              Values::Money(100'00));
    // Два проигрыша -> текущая ставка 400
    Values::SpinResult loss(Values::RouletteNumber(1),
                            Values::SpinStatus::Completed, {});
    bot.update_result(loss, Values::PlayerId(1));
    bot.update_result(loss, Values::PlayerId(1));

    // Баланса не хватает на 400
    auto d = bot.make_decision(make_player(Values::Money(300'00)));
    EXPECT_FALSE(d.wants_to_play);
    EXPECT_EQ(d.bet, nullptr);
}
