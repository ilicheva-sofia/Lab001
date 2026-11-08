#include <gtest/gtest.h>

#include "DomainError.hpp"
#include "RouletteNumber.hpp"

// Проверка валидации выхода за границы рулетки (0-36)
TEST(RouletteNumberTest, ThrowsOnInvalidNumbers) {
    EXPECT_THROW({ Values::RouletteNumber bad_low(-1); },
                 Exceptions::InvalidNumber);

    EXPECT_THROW({ Values::RouletteNumber bad_high(37); },
                 Exceptions::InvalidNumber);
}

// Реальная цветовая карта европейской рулетки:
// Красные: 1,3,5,7,9, 12,14,16,18, 19,21,23,25,27, 30,32,34,36
// Чёрные:  2,4,6,8,10, 11,13,15,17, 20,22,24,26,28, 29,31,33,35
// Зеро:    0
TEST(RouletteNumberTest, VerifiesAllStandardColorGroups) {
    // Зеро
    EXPECT_EQ(Values::RouletteNumber(0).color(), Values::Color::Green);

    // Группа 1-10: нечетные Красные, четные Черные
    EXPECT_EQ(Values::RouletteNumber(1).color(), Values::Color::Red);
    EXPECT_EQ(Values::RouletteNumber(2).color(), Values::Color::Black);
    EXPECT_EQ(Values::RouletteNumber(9).color(), Values::Color::Red);
    EXPECT_EQ(Values::RouletteNumber(10).color(), Values::Color::Black);

    // Группа 11-18: четные Красные, нечетные Черные (ВНИМАНИЕ: наоборот!)
    EXPECT_EQ(Values::RouletteNumber(11).color(), Values::Color::Black);
    EXPECT_EQ(Values::RouletteNumber(12).color(), Values::Color::Red);
    EXPECT_EQ(Values::RouletteNumber(13).color(), Values::Color::Black);
    EXPECT_EQ(Values::RouletteNumber(14).color(), Values::Color::Red);
    EXPECT_EQ(Values::RouletteNumber(15).color(), Values::Color::Black);
    EXPECT_EQ(Values::RouletteNumber(16).color(), Values::Color::Red);
    EXPECT_EQ(Values::RouletteNumber(17).color(), Values::Color::Black);
    EXPECT_EQ(Values::RouletteNumber(18).color(), Values::Color::Red);

    // Группа 19-26: нечетные Красные, четные Черные
    EXPECT_EQ(Values::RouletteNumber(19).color(), Values::Color::Red);
    EXPECT_EQ(Values::RouletteNumber(20).color(), Values::Color::Black);
    EXPECT_EQ(Values::RouletteNumber(21).color(), Values::Color::Red);
    EXPECT_EQ(Values::RouletteNumber(26).color(), Values::Color::Black);

    // Группа 27-36: с 29 по 36 нечётные — Чёрные, чётные — Красные.
    // Исключения: 27 (нечётный, Красный) и 28 (чётный, Чёрный).
    EXPECT_EQ(Values::RouletteNumber(27).color(), Values::Color::Red);
    EXPECT_EQ(Values::RouletteNumber(28).color(), Values::Color::Black);
    EXPECT_EQ(Values::RouletteNumber(29).color(), Values::Color::Black);
    EXPECT_EQ(Values::RouletteNumber(30).color(), Values::Color::Red);
    EXPECT_EQ(Values::RouletteNumber(31).color(), Values::Color::Black);
    EXPECT_EQ(Values::RouletteNumber(32).color(), Values::Color::Red);
    EXPECT_EQ(Values::RouletteNumber(33).color(), Values::Color::Black);
    EXPECT_EQ(Values::RouletteNumber(34).color(), Values::Color::Red);

    // 35 — чёрное, 36 — красное
    EXPECT_EQ(Values::RouletteNumber(35).color(), Values::Color::Black);
    EXPECT_EQ(Values::RouletteNumber(36).color(), Values::Color::Red);
}

// Полный перебор: 18 красных, 18 черных, 1 зеро
TEST(RouletteNumberTest, CountsMatchStandardRoulette) {
    int red = 0, black = 0, green = 0;
    for (int i = 0; i <= 36; ++i) {
        auto c = Values::RouletteNumber(static_cast<std::int8_t>(i)).color();
        if (c == Values::Color::Red)
            ++red;
        else if (c == Values::Color::Black)
            ++black;
        else
            ++green;
    }
    EXPECT_EQ(red, 18);
    EXPECT_EQ(black, 18);
    EXPECT_EQ(green, 1);
}
