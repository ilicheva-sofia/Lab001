#include "Tablelimits.hpp"

#include "DomainError.hpp"

namespace Values {
    Tablelimits::Tablelimits(Money min_money, Money max_money)
        : min_money(min_money), max_money(max_money) {
        if (max_money < min_money) {
            throw Exceptions::InvalidTablelimits(min_money, max_money);
        }
    }

    bool Tablelimits::validate(Money amount) const {
        return (amount > min_money || amount == min_money) &&
               (amount < max_money || amount == max_money);
    }

}  // namespace Values
