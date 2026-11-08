#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "FileLogger.hpp"
#include "FilePlayerRepository.hpp"
#include "FileTableRepository.hpp"
#include "InMemoryLogger.hpp"
#include "LoggingService.hpp"
#include "Player.hpp"
#include "PlayerId.hpp"
#include "RealRandomGenerator.hpp"
#include "RouletteNumber.hpp"
#include "Table.hpp"
#include "TableId.hpp"
#include "Tablelimits.hpp"
#include "TestRandomGenerator.hpp"

namespace {
    // Уникальный путь в /tmp, чтобы тесты не конфликтовали
    std::string tmp_path(const std::string& name) {
        std::string p = "/tmp/lab001_test_" + name;
        std::remove(p.c_str());
        return p;
    }
}  // namespace

// =========================== FileLogger ===========================

TEST(FileLoggerTest, WritesLinesToFile) {
    auto path = tmp_path("log.txt");
    {
        Infrastructure::FileLogger logger(path);
        logger.log("first");
        logger.log("second");
    }
    std::ifstream in(path);
    std::string line1, line2;
    std::getline(in, line1);
    std::getline(in, line2);
    EXPECT_NE(line1.find("first"), std::string::npos);
    EXPECT_NE(line2.find("second"), std::string::npos);
    std::remove(path.c_str());
}

TEST(FileLoggerTest, EmptyMessageIsIgnored) {
    auto path = tmp_path("log_empty.txt");
    {
        Infrastructure::FileLogger logger(path);
        logger.log("");  // пустая строка не должна записываться
    }
    std::ifstream in(path);
    std::string line;
    EXPECT_FALSE(std::getline(in, line));  // файл пуст
    std::remove(path.c_str());
}

// =========================== MemoryLogger ===========================

TEST(MemoryLoggerTest, AcceptsAndClearsMessages) {
    Infrastructure::MemoryLogger logger;
    logger.log("hello");  // не должно упасть; деструктор очистит вектор
    SUCCEED();
}

// ======================== LoggingService ========================

TEST(LoggingServiceTest, EmitsAllKindsOfLogs) {
    Infrastructure::MemoryLogger raw;
    Infrastructure::LoggingService service(raw);

    Values::PlayerId pid(1);
    Domain::Player player(pid, Values::Money(1000'00));
    Bets::ColorBet bet(Values::Money(500'00), Values::Color::Red);
    Aggregates::Table table(Values::TableId(1), Values::Money(10000'00),
                            Values::Tablelimits(Values::Money(100'00),
                                                Values::Money(5000'00)));

    service.log("plain");
    service.log_payout(pid, Values::Money(2000'00));
    service.log_balance(player);
    service.log_bet(pid, bet);
    service.log_exceptions("boom");
    service.log_newTable(table);
    service.log_newPlayer(player);
    // Красное (1), Черное (2), Зеро (0)
    service.log_spin_result(Values::RouletteNumber(1));
    service.log_spin_result(Values::RouletteNumber(2));
    service.log_spin_result(Values::RouletteNumber(0));
    SUCCEED();
}

// ====================== FilePlayerRepository ======================

TEST(FilePlayerRepositoryTest, SaveThenLoadRoundTrip) {
    auto path = tmp_path("players.txt");
    {
        Repository::FilePlayerRepository repo(path);
        repo.save(std::make_shared<Domain::Player>(Values::PlayerId(1),
                                                   Values::Money(1000'00)));
        repo.save(std::make_shared<Domain::Player>(Values::PlayerId(2),
                                                   Values::Money(2000'00)));
    }
    {
        Repository::FilePlayerRepository repo(path);
        auto p1 = repo.load(Values::PlayerId(1));
        auto p2 = repo.load(Values::PlayerId(2));
        ASSERT_NE(p1, nullptr);
        ASSERT_NE(p2, nullptr);
        EXPECT_EQ(p1->get_balance(), Values::Money(1000'00));
        EXPECT_EQ(p2->get_balance(), Values::Money(2000'00));
        EXPECT_EQ(repo.load(Values::PlayerId(3)), nullptr);
    }
    std::remove(path.c_str());
}

TEST(FilePlayerRepositoryTest, SaveNullIsNoOp) {
    auto path = tmp_path("players_null.txt");
    Repository::FilePlayerRepository repo(path);
    repo.save(nullptr);  // не должно упасть
    EXPECT_EQ(repo.load(Values::PlayerId(1)), nullptr);
    std::remove(path.c_str());
}

TEST(FilePlayerRepositoryTest, LoadMissingFileReturnsNull) {
    Repository::FilePlayerRepository repo("/tmp/lab001_nonexistent_players.txt");
    EXPECT_EQ(repo.load(Values::PlayerId(1)), nullptr);
}

// ====================== FileTableRepository ======================

TEST(FileTableRepositoryTest, SaveThenLoadRoundTrip) {
    auto path = tmp_path("tables.txt");
    {
        Repository::FileTableRepository repo(path);
        auto t = std::make_shared<Aggregates::Table>(
            Values::TableId(7), Values::Money(9999'00),
            Values::Tablelimits(Values::Money(100'00),
                                Values::Money(5000'00)));
        repo.save(t);
    }
    {
        Repository::FileTableRepository repo(path);
        auto t = repo.load(Values::TableId(7));
        ASSERT_NE(t, nullptr);
        EXPECT_EQ(t->get_balance(), Values::Money(9999'00));
        EXPECT_EQ(t->get_limits().get_min_limits(), Values::Money(100'00));
        EXPECT_EQ(t->get_limits().get_max_limits(), Values::Money(5000'00));
        EXPECT_EQ(repo.load(Values::TableId(8)), nullptr);
    }
    std::remove(path.c_str());
}

TEST(FileTableRepositoryTest, SaveNullIsNoOp) {
    auto path = tmp_path("tables_null.txt");
    Repository::FileTableRepository repo(path);
    repo.save(nullptr);  // не должно упасть
    EXPECT_EQ(repo.load(Values::TableId(1)), nullptr);
    std::remove(path.c_str());
}

TEST(FileTableRepositoryTest, PendingBetsSurviveReloadAndArePaid) {
    auto path = tmp_path("tables_with_bets.txt");
    {
        Repository::FileTableRepository repo(path);
        auto table = std::make_shared<Aggregates::Table>(
            Values::TableId(1), Values::Money(100000'00),
            Values::Tablelimits(Values::Money(100'00),
                                Values::Money(10000'00)));
        table->accept_bet(
            Values::PlayerId(1),
            std::make_unique<Bets::ColorBet>(Values::Money(200'00),
                                             Values::Color::Red));
        repo.save(table);
    }
    {
        Repository::FileTableRepository repo(path);
        auto table = repo.load(Values::TableId(1));
        ASSERT_NE(table, nullptr);
        table->accept_bet(
            Values::PlayerId(2),
            std::make_unique<Bets::ColorBet>(Values::Money(100'00),
                                             Values::Color::Black));
        repo.save(table);
    }
    {
        Repository::FileTableRepository repo(path);
        auto table = repo.load(Values::TableId(1));
        ASSERT_NE(table, nullptr);
        std::vector<Values::RouletteNumber> sequence{
            Values::RouletteNumber(13)};
        ::Test::TestRandomGenerator rng(sequence);
        auto result = table->spin(rng);

        ASSERT_EQ(result.get_payouts().size(), 2);
        EXPECT_EQ(result.get_payouts()[0].get_player(), Values::PlayerId(1));
        EXPECT_EQ(result.get_payouts()[0].get_amount(), Values::Money::zero());
        EXPECT_EQ(result.get_payouts()[1].get_player(), Values::PlayerId(2));
        EXPECT_EQ(result.get_payouts()[1].get_amount(), Values::Money(200'00));
        EXPECT_EQ(table->get_balance(), Values::Money(100100'00));
    }
    std::remove(path.c_str());
}

// ====================== RealRandomGenerator ======================

TEST(RealRandomGeneratorTest, NextReturnsValidRouletteNumbers) {
    Infrastructure::RealRandomGenerator rng(12345);
    for (int i = 0; i < 1000; ++i) {
        Values::RouletteNumber n = rng.next();
        EXPECT_GE(n.value(), 0);
        EXPECT_LE(n.value(), 36);
    }
}

TEST(RealRandomGeneratorTest, SameSeedProducesSameSequence) {
    Infrastructure::RealRandomGenerator a(42);
    Infrastructure::RealRandomGenerator b(42);
    for (int i = 0; i < 20; ++i) {
        EXPECT_EQ(a.next().value(), b.next().value());
    }
}

TEST(RealRandomGeneratorTest, ReSeedReproducesSequence) {
    Infrastructure::RealRandomGenerator rng(7);
    std::vector<std::int8_t> first;
    for (int i = 0; i < 10; ++i) first.push_back(rng.next().value());

    rng.seed(7);
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(rng.next().value(), first[i]);
    }
}
