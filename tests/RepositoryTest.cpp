#include <gtest/gtest.h>

#include <memory>

#include "InMemoryPlayerRepository.hpp"
#include "InMemoryTableRepository.hpp"
#include "Player.hpp"
#include "PlayerId.hpp"
#include "Table.hpp"
#include "TableId.hpp"
#include "Tablelimits.hpp"

// ===================== InMemoryPlayerRepository =====================

TEST(InMemoryPlayerRepositoryTest, SaveAndLoadExistingPlayer) {
    Repository::InMemoryPlayerRepository repo;
    auto p = std::make_shared<Domain::Player>(Values::PlayerId(5),
                                              Values::Money(1000'00));
    repo.save(p);
    auto loaded = repo.load(Values::PlayerId(5));
    ASSERT_NE(loaded, nullptr);
    EXPECT_EQ(loaded->get_balance(), Values::Money(1000'00));
}

TEST(InMemoryPlayerRepositoryTest, LoadMissingPlayerReturnsNull) {
    Repository::InMemoryPlayerRepository repo;
    EXPECT_EQ(repo.load(Values::PlayerId(99)), nullptr);
}

TEST(InMemoryPlayerRepositoryTest, SaveNullIsNoOp) {
    Repository::InMemoryPlayerRepository repo;
    repo.save(nullptr);  // не должно упасть
    EXPECT_EQ(repo.load(Values::PlayerId(1)), nullptr);
}

TEST(InMemoryPlayerRepositoryTest, SaveUpdatesExistingPlayer) {
    Repository::InMemoryPlayerRepository repo;
    repo.save(std::make_shared<Domain::Player>(Values::PlayerId(1),
                                               Values::Money(100'00)));
    // Обновляем баланс того же игрока
    auto p = std::make_shared<Domain::Player>(Values::PlayerId(1),
                                              Values::Money(999'00));
    repo.save(p);
    EXPECT_EQ(repo.load(Values::PlayerId(1))->get_balance(),
              Values::Money(999'00));
}

// ===================== InMemoryTableRepository =====================

TEST(InMemoryTableRepositoryTest, SaveAndLoadExistingTable) {
    Repository::InMemoryTableRepository repo;
    auto t = std::make_shared<Aggregates::Table>(
        Values::TableId(3), Values::Money(5000'00),
        Values::Tablelimits(Values::Money(100'00), Values::Money(5000'00)));
    repo.save(t);
    auto loaded = repo.load(Values::TableId(3));
    ASSERT_NE(loaded, nullptr);
    EXPECT_EQ(loaded->get_balance(), Values::Money(5000'00));
    EXPECT_EQ(loaded->get_limits().get_min_limits(), Values::Money(100'00));
    EXPECT_EQ(loaded->get_limits().get_max_limits(), Values::Money(5000'00));
}

TEST(InMemoryTableRepositoryTest, LoadMissingTableReturnsNull) {
    Repository::InMemoryTableRepository repo;
    EXPECT_EQ(repo.load(Values::TableId(99)), nullptr);
}

TEST(InMemoryTableRepositoryTest, SaveNullIsNoOp) {
    Repository::InMemoryTableRepository repo;
    repo.save(nullptr);  // не должно упасть
    EXPECT_EQ(repo.load(Values::TableId(1)), nullptr);
}
