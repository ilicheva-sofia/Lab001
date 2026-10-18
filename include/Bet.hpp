#pragma once
#include <string>

#include "Money.hpp"
#include "RouletteNumber.hpp"

namespace Bets {

    struct Bet {
        virtual Values::Money get_amount() const = 0;
        virtual Values::Money payout(
            const Values::RouletteNumber& result) const = 0;
        virtual std::string describe() const = 0;
        virtual ~Bet() = default;
    };

    class ColorBet : public Bet {
       private:
        Values::Money amount;
        Values::Color color;

       public:
        ColorBet(Values::Money amount, Values::Color color);

        Values::Money get_amount() const override;
        Values::Money payout(
            const Values::RouletteNumber& result) const override;
        std::string describe() const override;
    };

    class SingleNumberBet : public Bet {
       private:
        Values::Money amount;
        Values::RouletteNumber number;

       public:
        SingleNumberBet(Values::Money amount, Values::RouletteNumber number);

        Values::Money get_amount() const override;
        Values::Money payout(
            const Values::RouletteNumber& result) const override;
        std::string describe() const override;
    };

}  // namespace Bets
