#pragma once
#include <vector>

#include "Bet.hpp"
#include "Money.hpp"
#include "PlayerId.hpp"
#include "RandomGenerator.hpp"
#include "SpinResult.hpp"
#include "Tablelimits.hpp"

namespace Aggregates {
    class Table {
       private:
        std::vector<Bets::Bet> bets;
        void log_bets();
        void roll_back();
        Money balance;
        Tablelimits limits;

       public:
        Table(Money balance, Tablelimits limits)
            : balance(balance), limits(limits) {}
        void accept_bet(PlayerId id, Bets::Bet bet);
        std::pair<PlayerId, Money> payout();
        SpinResult spin(Ports::RandomGenerator& rg);
    };
}  // namespace Aggregates
