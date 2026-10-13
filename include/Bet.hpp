#pragma once
#include "RouletteNumber.hpp"

namespace Bets {

    struct Bet {
        virtual Money amount() const = 0;
        virtual Money payout(const RouletteNumber& result) const = 0;
        virtual std::string describe()
            const = 0;  // "Red 1000" или "Number 17 500"
        virtual ~Bet() = default;
    };

    class ColorBet : public Bet {
        Money amount;
        Color color;

       public:
        ColorBet(Money amount, Color color) : amount(amount), color(color) {}
        Money amount() const override { return amount; }
        Money payout(const RouletteNumber& result) const override {
            if (result.color() == color)
                return amount + amount;  // 1:1 + возврат
            return Money{0};
        }
    };

    class SingleNumberBet : public Bet {
        Money amount;
        RouletteNumber number;

       public:
        SingleNumberBet(Money amount, RouletteNumber number)
            : amount(amount), number(number) {}
        Money amount() const override { return amount; }
        Money payout(const RouletteNumber& result) const override {
            if (result == number)
                return amount * 35 + amount;  // 35:1 + возврат
            return Money{0};
        }
    };

}  // namespace Bets