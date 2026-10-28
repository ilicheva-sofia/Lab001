#pragma once
#include "Money.hpp"

namespace Values {
    class Tablelimits {
       private:
        Money min_money;
        Money max_money;

       public:
        Tablelimits(Money min_money, Money max_money);

        bool validate(Money amount) const;

        Money get_min_limits() const { return min_money; }

        Money get_max_limits() const { return max_money; }
    };
}  // namespace Values
