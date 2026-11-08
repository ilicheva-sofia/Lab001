#include <gtest/gtest.h>

#include "DomainError.hpp"
#include "PlayerId.hpp"

TEST(PlayerIdTest, ThrowsOnNonPositiveId) {
    EXPECT_THROW({ Values::PlayerId zero(0); }, Exceptions::InvalidPlayerId);
    EXPECT_THROW({ Values::PlayerId negative(-1); },
                 Exceptions::InvalidPlayerId);
}

TEST(PlayerIdTest, AcceptsPositiveId) {
    Values::PlayerId ok(42);
    EXPECT_EQ(ok.get_id(), 42);
}

TEST(PlayerIdTest, ComparesEqualityAndOrdering) {
    Values::PlayerId a(1);
    Values::PlayerId b(2);
    EXPECT_TRUE(a == Values::PlayerId(1));
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(a == b);
}
