#include "Bet.hpp"

#include <format>

namespace Bets {

    ColorBet::ColorBet(Values::Money amount, Values::Color color)
        : amount(amount), color(color) {}

    Values::Money ColorBet::get_amount() const { return amount; }

    Values::Money ColorBet::payout(const Values::RouletteNumber& result) const {
        if (result.color() == color) {
            return amount * 2;
        }
        return Values::Money::zero();
    }

    std::string ColorBet::describe() const {
        std::string colorStr = (color == Values::Color::Red)     ? "Red"
                               : (color == Values::Color::Black) ? "Black"
                                                                 : "Green";
        return std::format("Color Bet ({}) on {}", amount, colorStr);
    }

    SingleNumberBet::SingleNumberBet(Values::Money amount,
                                     Values::RouletteNumber number)
        : amount(amount), number(number) {}

    Values::Money SingleNumberBet::get_amount() const { return amount; }

    Values::Money SingleNumberBet::payout(
        const Values::RouletteNumber& result) const {
        if (result == number) {
            return amount * 35 + amount;
        }
        return Values::Money::zero();
    }

    std::string SingleNumberBet::describe() const {
        return std::format("Single Number Bet ({}) on №{}", amount,
                           number.value());
    }

}  // namespace Bets
