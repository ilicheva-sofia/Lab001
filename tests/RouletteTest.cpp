#include <gtest/gtest.h>

#include "RouletteNumber.hpp"
#include "TestRandomGenerator.hpp"

TEST(RouletteLogicTest, CheckSequenceLoop) {
    // Задаем строго фиксированную последовательность: 0 (Зеро), 17 (Черное), 32
    // (Красное)
    std::vector<Values::RouletteNumber> seq = {Values::RouletteNumber(0),
                                               Values::RouletteNumber(17),
                                               Values::RouletteNumber(32)};

    Test::TestRandomGenerator rng(seq);

    // Проверяем первые 3 шага
    EXPECT_EQ(rng.next().value(), 0);
    EXPECT_EQ(rng.next().value(), 17);
    EXPECT_EQ(rng.next().value(), 32);

    // Четвертый шаг — благодаря вашей логике current = 0, генератор пойдет по
    // кругу!
    EXPECT_EQ(rng.next().value(), 0);
}
