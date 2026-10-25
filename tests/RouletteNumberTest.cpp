#include <gtest/gtest.h>

#include "DomainError.hpp"
#include "RouletteNumber.hpp"

// Проверка валидации выхода за границы рулетки (0-36)
TEST(RouletteNumberTest, ThrowsOnInvalidNumbers) {
    EXPECT_THROW(
        { Values::RouletteNumber bad_low(-1); }, Exceptions::InvalidNumber);

    EXPECT_THROW(
        { Values::RouletteNumber bad_high(37); }, Exceptions::InvalidNumber);
}

// Проверка правильности распределения цветов по номерам рулетки
TEST(RouletteNumberTest, VerifiesAllStandardColorGroups) {
    // Группа Зеро
    EXPECT_EQ(Values::RouletteNumber(0).color(), Values::Color::Green);

    // Группа 1 - 10: нечетные Красные, четные Черные
    EXPECT_EQ(Values::RouletteNumber(1).color(), Values::Color::Red);
    EXPECT_EQ(Values::RouletteNumber(2).color(), Values::Color::Black);

    // Группа 11 - 18: нечетные Черные, четные Красные
    EXPECT_EQ(Values::RouletteNumber(11).color(), Values::Color::Black);
    EXPECT_EQ(Values::RouletteNumber(12).color(), Values::Color::Red);

    // Группа 19 - 28: нечетные Красные, четные Черные
    EXPECT_EQ(Values::RouletteNumber(19).color(), Values::Color::Red);
    EXPECT_EQ(Values::RouletteNumber(20).color(), Values::Color::Black);

    // Группа 29 - 36: нечетные Черные, четные Красные
    EXPECT_EQ(Values::RouletteNumber(29).color(), Values::Color::Black);
    EXPECT_EQ(Values::RouletteNumber(30).color(), Values::Color::Red);
    EXPECT_EQ(Values::RouletteNumber(36).color(), Values::Color::Red);
}
