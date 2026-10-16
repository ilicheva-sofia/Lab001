#pragma once
#include <memory>
#include <vector>

#include "Bet.hpp"
#include "Money.hpp"
#include "PlayerId.hpp"
#include "RandomGenerator.hpp"
#include "SpinResult.hpp"
#include "Tablelimits.hpp"

namespace Aggregates {
    using Bets::Bet;
    using Ports::RandomGenerator;

    struct PlacedBet {
        PlayerId player;
        std::unique_ptr<Bet> bet;
    };

    class Table {
       private:
        std::vector<PlacedBet> bets;
        void log_bets();
        void roll_back();  // public  ??  how to restore balances ??
        Money balance;
        Tablelimits limits;

       public:
        Table(Money balance, Tablelimits limits)
            : balance(balance), limits(limits) {}
        void accept_bet(PlayerId id, std::unique_ptr<Bet> bet);
        SpinResult spin(RandomGenerator& rg);
    };
}  // namespace Aggregates
