#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "Bet.hpp"
#include "GameService.hpp"
#include "InMemoryLogger.hpp"
#include "InMemoryPlayerRepository.hpp"
#include "InMemoryTableRepository.hpp"
#include "LoggingService.hpp"
#include "Player.hpp"
#include "PlayerId.hpp"
#include "SpinResult.hpp"
#include "SpinStatus.hpp"
#include "Table.hpp"
#include "TableId.hpp"
#include "Tablelimits.hpp"
#include "TestRandomGenerator.hpp"

namespace {
    struct Harness {
        Repository::InMemoryPlayerRepository players;
        Repository::InMemoryTableRepository tables;
        Infrastructure::MemoryLogger logger;
        Infrastructure::LoggingService logging{logger};
        Domain::GameService service{players, tables, logging};

        Harness()
            : logging(logger)
            , service(players, tables, logging) {}

        void setup_table(Values::TableId tid, Values::Money balance,
                         Values::Tablelimits limits) {
            tables.save(std::make_shared<Aggregates::Table>(tid, balance,
                                                            limits));
        }
        void setup_player(Values::PlayerId pid, Values::Money balance) {
            players.save(std::make_shared<Domain::Player>(pid, balance));
        }
    };
}  // namespace

// ========================= place_bet =========================

TEST(GameServiceTest, PlaceBetDeductsPlayerAndCreditedTable) {
    Harness h;
    h.setup_table(Values::TableId(1), Values::Money(100000'00),
                  Values::Tablelimits(Values::Money(100'00),
                                      Values::Money(10000'00)));
    h.setup_player(Values::PlayerId(1), Values::Money(1000'00));

    h.service.place_bet(Values::PlayerId(1), Values::TableId(1),
                        std::make_unique<Bets::ColorBet>(Values::Money(500'00),
                                                         Values::Color::Red));

    EXPECT_EQ(h.players.load(Values::PlayerId(1))->get_balance(),
              Values::Money(500'00));
    EXPECT_EQ(h.tables.load(Values::TableId(1))->get_balance(),
              Values::Money(100500'00));
}

TEST(GameServiceTest, PlaceBetOutsideLimitsRefundsPlayer) {
    Harness h;
    // Лимит 100..5000
    h.setup_table(Values::TableId(1), Values::Money(100000'00),
                  Values::Tablelimits(Values::Money(100'00),
                                      Values::Money(5000'00)));
    h.setup_player(Values::PlayerId(1), Values::Money(1000'00));

    // 6000 > max -> стол бросит BetOutsideLimits, деньги должны вернуться
    h.service.place_bet(Values::PlayerId(1), Values::TableId(1),
                        std::make_unique<Bets::ColorBet>(Values::Money(6000'00),
                                                         Values::Color::Red));

    // Баланс игрока не должен измениться
    EXPECT_EQ(h.players.load(Values::PlayerId(1))->get_balance(),
              Values::Money(1000'00));
    // Касса стола не должна измениться
    EXPECT_EQ(h.tables.load(Values::TableId(1))->get_balance(),
              Values::Money(100000'00));
}

TEST(GameServiceTest, PlaceBetWithInsufficientPlayerBalanceIsNotTaken) {
    Harness h;
    h.setup_table(Values::TableId(1), Values::Money(100000'00),
                  Values::Tablelimits(Values::Money(100'00),
                                      Values::Money(10000'00)));
    h.setup_player(Values::PlayerId(1), Values::Money(300'00));

    // Ставка 500 > баланса 300 -> списание бросит InvalidMoneyOperation
    h.service.place_bet(Values::PlayerId(1), Values::TableId(1),
                        std::make_unique<Bets::ColorBet>(Values::Money(500'00),
                                                         Values::Color::Red));

    // Игрок ничего не потерял, стол ничего не получил
    EXPECT_EQ(h.players.load(Values::PlayerId(1))->get_balance(),
              Values::Money(300'00));
    EXPECT_EQ(h.tables.load(Values::TableId(1))->get_balance(),
              Values::Money(100000'00));
}

TEST(GameServiceTest, PlaceBetWithMissingEntitiesIsNoOp) {
    Harness h;
    // Таблица и игрок не созданы
    h.service.place_bet(Values::PlayerId(99), Values::TableId(99),
                        std::make_unique<Bets::ColorBet>(Values::Money(500'00),
                                                         Values::Color::Red));
    // Не должно ни бросить, ни что-то создать
    EXPECT_EQ(h.players.load(Values::PlayerId(99)), nullptr);
    EXPECT_EQ(h.tables.load(Values::TableId(99)), nullptr);
}

// ============================ spin ============================

TEST(GameServiceTest, SpinPaysOutWinningBets) {
    Harness h;
    h.setup_table(Values::TableId(1), Values::Money(100000'00),
                  Values::Tablelimits(Values::Money(100'00),
                                      Values::Money(10000'00)));
    h.setup_player(Values::PlayerId(1), Values::Money(1000'00));

    h.service.place_bet(Values::PlayerId(1), Values::TableId(1),
                        std::make_unique<Bets::ColorBet>(Values::Money(500'00),
                                                         Values::Color::Red));

    // Выпадает 1 (Red) -> ставка выигрывает: выплата = 500*2 = 1000
    std::vector<Values::RouletteNumber> seq{Values::RouletteNumber(1)};
    ::Test::TestRandomGenerator rng(seq);

    Values::SpinResult result = h.service.spin(Values::TableId(1), rng);

    EXPECT_EQ(result.get_status(), Values::SpinStatus::Completed);
    // Игрок: 500 (остаток) + 1000 (выплата) = 1500
    EXPECT_EQ(h.players.load(Values::PlayerId(1))->get_balance(),
              Values::Money(1500'00));
    // Стол: 100500 (после ставки) - 1000 (выплата) = 99500
    EXPECT_EQ(h.tables.load(Values::TableId(1))->get_balance(),
              Values::Money(99500'00));
}

TEST(GameServiceTest, SpinLosingBetKeepsPlayersMoneyAtTable) {
    Harness h;
    h.setup_table(Values::TableId(1), Values::Money(100000'00),
                  Values::Tablelimits(Values::Money(100'00),
                                      Values::Money(10000'00)));
    h.setup_player(Values::PlayerId(1), Values::Money(1000'00));

    h.service.place_bet(Values::PlayerId(1), Values::TableId(1),
                        std::make_unique<Bets::ColorBet>(Values::Money(500'00),
                                                         Values::Color::Red));

    // Выпадает 2 (Black) -> ставка проигрывает, выплата 0
    std::vector<Values::RouletteNumber> seq{Values::RouletteNumber(2)};
    ::Test::TestRandomGenerator rng(seq);

    Values::SpinResult result = h.service.spin(Values::TableId(1), rng);

    EXPECT_EQ(result.get_status(), Values::SpinStatus::Completed);
    // Игрок: 500 (остаток), ничего не получил
    EXPECT_EQ(h.players.load(Values::PlayerId(1))->get_balance(),
              Values::Money(500'00));
    // Стол: 100500 (после ставки), ничего не выплатил
    EXPECT_EQ(h.tables.load(Values::TableId(1))->get_balance(),
              Values::Money(100500'00));
}

TEST(GameServiceTest, SpinOnBankruptTableRefundsBets) {
    Harness h;
    // Касса стола слишком мала для выплаты по ставке на число
    h.setup_table(Values::TableId(1), Values::Money(1000'00),
                  Values::Tablelimits(Values::Money(100'00),
                                      Values::Money(10000'00)));
    h.setup_player(Values::PlayerId(1), Values::Money(100000'00));

    // Ставка 500 на число 17: при выигрыше стол должен выплатить 500*36=18000
    h.service.place_bet(Values::PlayerId(1), Values::TableId(1),
                        std::make_unique<Bets::SingleNumberBet>(
                            Values::Money(500'00),
                            Values::RouletteNumber(17)));

    // Выпадает 17 -> стол должен выплатить 18000, но у него только 1500
    std::vector<Values::RouletteNumber> seq{Values::RouletteNumber(17)};
    ::Test::TestRandomGenerator rng(seq);

    Values::SpinResult result = h.service.spin(Values::TableId(1), rng);

    EXPECT_EQ(result.get_status(), Values::SpinStatus::Cancelled);
    // Игрок получил свою ставку обратно: 99500 (остаток) + 500 = 100000
    EXPECT_EQ(h.players.load(Values::PlayerId(1))->get_balance(),
              Values::Money(100000'00));
    // Стол вернулся к исходному балансу 1000
    EXPECT_EQ(h.tables.load(Values::TableId(1))->get_balance(),
              Values::Money(1000'00));
}

TEST(GameServiceTest, SpinOnMissingTableThrowsDomainError) {
    Harness h;
    std::vector<Values::RouletteNumber> seq{Values::RouletteNumber(1)};
    ::Test::TestRandomGenerator rng(seq);

    EXPECT_THROW(h.service.spin(Values::TableId(42), rng),
                 Exceptions::DomainError);
}

// =========================== startGame ===========================

TEST(GameServiceTest, StartGameRunsUntilAllBotsLeave) {
    Harness h;
    h.setup_table(Values::TableId(1), Values::Money(1000000'00),
                  Values::Tablelimits(Values::Money(100'00),
                                      Values::Money(10000'00)));
    h.setup_player(Values::PlayerId(1), Values::Money(10000'00));

    // Бот с очень маленьким stop_loss: после первой же потери он уйдёт
    auto bot = std::make_shared<Domain::RedConstantBot>(
        "test-bot", Values::Money(5000'00), Values::Money(0'00),
        Values::Money(10000'00), Values::Money(100'00));
    std::vector<std::shared_ptr<Domain::BotStrategy>> bots{bot};

    // Выпадают Red-номера (1, 3, 5, ...) -> бот выигрывает до take-profit
    // или до stop-loss. Возьмем Red-последовательность.
    std::vector<Values::RouletteNumber> seq;
    for (int i = 1; i <= 36; i += 2) {
        seq.push_back(Values::RouletteNumber(static_cast<std::int8_t>(i)));
    }
    ::Test::TestRandomGenerator rng(seq);

    // Не должно упасть; бот уйдёт, когда достигнет лимита
    h.service.startGame(bots, Values::TableId(1), rng, 50);
    // Баланс игрока после игры должен быть >= 0
    EXPECT_GT(h.players.load(Values::PlayerId(1))->get_balance(),
              Values::Money::zero());
}

TEST(GameServiceTest, StartGameStopsWhenAllBotsRefuseToPlay) {
    Harness h;
    h.setup_table(Values::TableId(1), Values::Money(1000000'00),
                  Values::Tablelimits(Values::Money(100'00),
                                      Values::Money(10000'00)));
    // Баланс игрока меньше базовой ставки бота -> бот сразу уходит
    h.setup_player(Values::PlayerId(1), Values::Money(50'00));

    auto bot = std::make_shared<Domain::RedConstantBot>(
        "test-bot", Values::Money(5000'00), Values::Money(3000'00),
        Values::Money(10000'00), Values::Money(100'00));
    std::vector<std::shared_ptr<Domain::BotStrategy>> bots{bot};

    std::vector<Values::RouletteNumber> seq{Values::RouletteNumber(1)};
    ::Test::TestRandomGenerator rng(seq);

    // Бот не сможет сделать ставку (50 < 100) -> игра остановится после 1 раунда
    h.service.startGame(bots, Values::TableId(1), rng, 100);

    // Игрок не потратил ничего
    EXPECT_EQ(h.players.load(Values::PlayerId(1))->get_balance(),
              Values::Money(50'00));
}
