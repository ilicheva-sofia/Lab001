#include <gtest/gtest.h>

#include "DomainError.hpp"
#include "Money.hpp"
#include "Tablelimits.hpp"

TEST(TableLimitsTest, ValidatesLimitsCorrectly) {
    Values::Tablelimits limits(Values::Money(100'00), Values::Money(5000'00));

    // Мягкая проверка границ (accepts)
    EXPECT_TRUE(limits.accepts(Values::Money(500'00)));
    EXPECT_FALSE(limits.accepts(Values::Money(50'00)));
    EXPECT_FALSE(limits.accepts(Values::Money(10000'00)));

    // Жесткая проверка с генерацией исключений (validate)
    EXPECT_NO_THROW(limits.validate(Values::Money(1000'00)));
    EXPECT_THROW(limits.validate(Values::Money(50'00)),
                 Exceptions::BetOutsideLimits);
    EXPECT_THROW(limits.validate(Values::Money(6000'00)),
                 Exceptions::BetOutsideLimits);

    EXPECT_THROW(
        {
            Values::Tablelimits bad_limits(Values::Money(500'00),
                                           Values::Money(100'00));
        },
        Exceptions::InvalidTablelimits);
}
