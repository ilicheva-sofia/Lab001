#include <gtest/gtest.h>

#include "DomainError.hpp"
#include "Money.hpp"
#include "Tablelimits.hpp"

TEST(TableLimitsTest, ValidatesLimitsCorrectly) {
    Values::Tablelimits limits(Values::Money(100'00), Values::Money(5000'00));

    EXPECT_TRUE(limits.validate(Values::Money(500'00)));   // Внутри границ
    EXPECT_TRUE(limits.validate(Values::Money(100'00)));   // Точный минимум
    EXPECT_TRUE(limits.validate(Values::Money(5000'00)));  // Точный максимум

    EXPECT_FALSE(limits.validate(Values::Money(50'00)));     // Меньше минимума
    EXPECT_FALSE(limits.validate(Values::Money(10000'00)));  // Больше максимума
}

TEST(TableLimitsTest, ThrowsOnInvalidBoundaries) {
    EXPECT_THROW(
        {
            Values::Tablelimits bad_limits(Values::Money(500'00),
                                           Values::Money(100'00));
        },
        Exceptions::InvalidTablelimits);
}
