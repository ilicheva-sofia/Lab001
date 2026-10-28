#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "Bet.hpp"
#include "SpinResult.hpp"
#include "Table.hpp"
#include "TestRandomGenerator.hpp"

// Проверка отката (Rollback) при нехватке средств у стола
TEST(TableAggregateTest, CancelledRoundOnCasinoBankruptcy) {
    Values::TableId tid(1);
    Values::Tablelimits limits(Values::Money(100'00), Values::Money(10000'00));

    // Создаем стол с маленькой кассой — 5 000 рублей
    Aggregates::Table table(tid, Values::Money(5000'00), limits);

    Values::PlayerId pid(1);
    // Крупная ставка на число — 1 000 рублей
    // (потенциальный выигрыш 36 000 рублей)
    table.accept_bet(pid,
                     std::make_unique<Bets::SingleNumberBet>(
                         Values::Money(1000'00), Values::RouletteNumber(17)));

    // Фиксируем, что выпадет именно число 17 (казино обязано выдать 36к)
    std::vector<Values::RouletteNumber> fixed_seq = {
        Values::RouletteNumber(17)};
    ::Test::TestRandomGenerator fake_rng(fixed_seq);

    // Запускаем спин агрегата стола
    Values::SpinResult result = table.spin(fake_rng);

    // Проверяем статус должен быть Cancelled
    EXPECT_EQ(result.get_status(), Values::SpinStatus::Cancelled);

    // Проверяем в payouts должен лежать возврат исходной ставки игрока (1 000
    // руб)
    ASSERT_EQ(result.get_payouts().size(), 1);
    EXPECT_EQ(result.get_payouts()[0].player.get_id(), pid.get_id());
    EXPECT_EQ(result.get_payouts()[0].amount, Values::Money(1000'00));

    // Баланс кассы казино должен откатиться в норму (исходные 5 000 рублей)
    EXPECT_EQ(table.get_balance(), Values::Money(5000'00));
}

// Проверка обычного сценария
TEST(TableAggregateTest, SuccessfulRoundWithPayouts) {
    Values::TableId tid(1);
    Values::Tablelimits limits(Values::Money(100'00), Values::Money(10000'00));

    // Создаем стол с огромной кассой — 100 000 рублей
    Aggregates::Table table(tid, Values::Money(100000'00), limits);

    Values::PlayerId pid(1);
    // Игрок делает ставку 1 000 рублей на Красное
    table.accept_bet(pid, std::make_unique<Bets::ColorBet>(
                              Values::Money(1000'00), Values::Color::Red));

    // Настраиваем генератор: выпадет число 1 (Красное) — ставка выиграла!
    std::vector<Values::RouletteNumber> fixed_seq = {Values::RouletteNumber(1)};
    ::Test::TestRandomGenerator fake_rng(fixed_seq);

    // Запускаем вращение
    Values::SpinResult result = table.spin(fake_rng);

    // Статус должен быть Completed
    EXPECT_EQ(result.get_status(), Values::SpinStatus::Completed);
    EXPECT_EQ(result.get_number().value(), 1);

    // Проверяем правильную выплату игроку (1000 * 2 = 2000 руб)
    ASSERT_EQ(result.get_payouts().size(), 1);
    EXPECT_EQ(result.get_payouts()[0].player.get_id(), pid.get_id());
    EXPECT_EQ(result.get_payouts()[0].amount, Values::Money(2000'00));

    // Проверяем баланс кассы стола после выигрыша
    EXPECT_EQ(table.get_balance(), Values::Money(99000'00));
}

TEST(TableAggregateTest, TableThrowsOnBetOutsideLimits) {
    Values::TableId tid(1);
    Values::Tablelimits limits(Values::Money(100'00),
                               Values::Money(5000'00));  // Лимит 100 - 5000 руб

    Aggregates::Table table(tid, Values::Money(100000'00), limits);
    Values::PlayerId pid(1);

    // Ставка меньше минимального лимита стола
    EXPECT_THROW(
        {
            table.accept_bet(
                pid, std::make_unique<Bets::ColorBet>(Values::Money(50'00),
                                                      Values::Color::Red));
        },
        Exceptions::BetOutsideLimits);

    // Ставка больше максимального лимита стола
    EXPECT_THROW(
        {
            table.accept_bet(
                pid, std::make_unique<Bets::ColorBet>(Values::Money(6000'00),
                                                      Values::Color::Black));
        },
        Exceptions::BetOutsideLimits);
}
