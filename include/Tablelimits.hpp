#pragma once
#include <cstdint>

#include "Money.hpp"

namespace Values {
    class Tablelimits {
       private:
        Money min_money;
        Money max_money;

       public:
        constexpr Tablelimits(Money min_money, Money max_money)
            : min_money(min_money), max_money(max_money) {
            if (max_money < min_money)
                throw Exceptions::InvalidTablelimits(min_money, max_money);
        }
        bool accepts(Money amount) {
            if ((amount < min_money) && (amount > max_money)) return 1;
            return 0;
        }
    };
}  // namespace Values
