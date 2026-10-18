TEST_CASE("RouletteNumber color test") {
    REQUIRE(RouletteNumber(0).color() == Color::Green);
    REQUIRE(RouletteNumber(1).color() == Color::Red);
    REQUIRE(RouletteNumber(2).color() == Color::Black);
    // ... остальные проверки
}
0 → Зелёный 1(нечётный в 1 - 10) → Красный 2(чётный в 1 - 10) → Чёрный
    11(нечётный в 11 - 18) → Чёрный 12(чётный в 11 - 18) → Красный
    19(нечётный в 19 - 28) → Красный 20(чётный в 19 - 28) → Чёрный
    29(нечётный в 29 - 36) → Чёрный 30(чётный в 29 - 36) → Красный 36 → Красный